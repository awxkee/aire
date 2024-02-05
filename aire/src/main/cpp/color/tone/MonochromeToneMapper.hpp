//
// Created by Radzivon Bartoshyk on 05/02/2024.
//

#include "ToneMapper.h"
#include "color/Blend.h"
#include <fast_math-inl.h>
#include <algorithm>

namespace aire {
    template<typename D>
    class MonochromeToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        TFromD<D> exposure;
        TFromD<D> lumaCoefficients[4];
        TFromD<D> color[4];

        HWY_FAST_MATH_INLINE V applyMono(const V v, const float clr, const V Lin) {
            const auto ones = Set(df_, 1.0f);
            const auto twoF = Set(df_, 2.0f);
            const auto halFofOnes = Set(df_, 0.5f);
            const auto vColor = Set(df_, clr);
            const auto positive = Mul(Mul(twoF, vColor), Lin);
            const auto negPart = NegMulAdd(Mul(Sub(ones, Lin), twoF), Sub(ones, vColor), ones);
            const auto decision = Lin < halFofOnes;
            return IfThenElse(decision, positive, negPart);
        }

    public:
        MonochromeToneMapper(const TFromD<D> color[4], const TFromD<D> lumaCoefficients[3], const TFromD<D> exposure = 1.0f) :
                exposure(exposure),
                ToneMapper<D>() {
            std::copy(lumaCoefficients, lumaCoefficients + 3, this->lumaCoefficients);
            std::copy(color, color + 4, this->color);
            this->lumaCoefficients[3] = 0.0f;
        }

        ~MonochromeToneMapper() override = default;

        HWY_FAST_MATH_INLINE void Execute(V &R, V &G, V &B) override {
            const V mExposure = Set(df_, exposure);
            const V lumaR = Set(df_, lumaCoefficients[0]);
            const V lumaG = Set(df_, lumaCoefficients[1]);
            const V lumaB = Set(df_, lumaCoefficients[2]);

            R = Mul(R, mExposure);
            G = Mul(G, mExposure);
            B = Mul(B, mExposure);

            V rLuma = Mul(R, lumaR);
            V gLuma = Mul(G, lumaG);
            V bLuma = Mul(B, lumaB);

            const V ones = Set(df_, static_cast<TFromD<D>>(1.0f));
            const V zeros = Zero(df_);

            V Lin = Add(Add(rLuma, gLuma), bLuma);

            auto newR = applyMono(R, color[0], Lin);
            auto newG = applyMono(G, color[1], Lin);
            auto newB = applyMono(B, color[2], Lin);

            const auto vAlpha = Set(df_, color[3]);

            R = Clamp(MulAdd(Sub(ones, vAlpha), R, Mul(newR, vAlpha)), zeros, ones);
            G = Clamp(MulAdd(Sub(ones, vAlpha), G, Mul(newG, vAlpha)), zeros, ones);
            B = Clamp(MulAdd(Sub(ones, vAlpha), G, Mul(newG, vAlpha)), zeros, ones);
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) override {
            r *= exposure;
            g *= exposure;
            b *= exposure;

            const float Lin =
                    r * lumaCoefficients[0] + g * lumaCoefficients[1] + b * lumaCoefficients[2];

            TFromD<D> newR = (Lin < 0.5f) ? (2.f * Lin * color[0]) : (1.f - 2.f * (1.f - Lin) * (1.f - color[0]));
            TFromD<D> newG = (Lin < 0.5f) ? (2.f * Lin * color[1]) : (1.f - 2.f * (1.f - Lin) * (1.f - color[1]));
            TFromD<D> newB = (Lin < 0.5f) ? (2.f * Lin * color[2]) : (1.f - 2.f * (1.f - Lin) * (1.f - color[2]));

            r = std::clamp(blendColor(r, newR, color[3]), 0.0f, 1.0f);
            g = std::clamp(blendColor(g, newG, color[3]), 0.0f, 1.0f);
            b = std::clamp(blendColor(b, newB, color[3]), 0.0f, 1.0f);
        }

    };
}