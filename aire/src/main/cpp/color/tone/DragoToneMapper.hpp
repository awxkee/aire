//
// Created by Radzivon Bartoshyk on 17/02/2024.
//

#pragma once

#include "ToneMapper.h"
#include <fast_math-inl.h>
#include "sleef-hwy.h"

namespace aire {
    template<typename D>
    class DragoToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        TFromD<D> exposure;
        TFromD<D> maxLd;
        //Low avg across image considering static
        const TFromD<D> Lwa = 1.0f; // log average across image
        const TFromD<D> bias = 0.85f;

    public:
        DragoToneMapper(const TFromD<D> lumaCoefficients[3],
                        const TFromD<D> exposure = 1.0f,
                        const TFromD<D> maxLd = 250.f) : exposure(exposure),
                                                         maxLd(maxLd),
                                                         ToneMapper<D>() {
            std::copy(lumaCoefficients, lumaCoefficients + 3, this->lumaCoefficients);
            this->lumaCoefficients[3] = 0.0f;
        }

        ~DragoToneMapper() override = default;

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

            const TFromD<D> Lmax = this->maxLd;
            const auto LwaP = Set(df_, Lwa / std::pow(1.f + bias - 0.85f, 5.f));
            const auto LmaxP = Div(Set(df_, Lmax), LwaP);
            const auto LinP = Div(Lin, LwaP);

            const auto exponent = Set(df_, std::log(bias) / std::log(0.5f));
            const auto c1High = Set(df_, 0.01f * maxLd);
            const auto c1 = Div(c1High, hwy::HWY_NAMESPACE::sleef::Log10f(df_, Add(ones, LmaxP)));

            const auto twos = Set(df_, static_cast<TFromD<D>>(2.f));
            const auto eights = Set(df_, static_cast<TFromD<D>>(8.f));

            const auto c2 = Div(hwy::HWY_NAMESPACE::sleef::LogFast(df_, Add(ones, LinP)),
                                hwy::HWY_NAMESPACE::sleef::LogFast(df_,
                                        MulAdd(eights, aire::HWY_NAMESPACE::Pow(df_, Div(LinP, LmaxP), exponent), twos)
                                ));

            Lin = IfThenElse(Lin == zeros, ones, Lin);

            const auto Lout = Mul(c1, c2);

            V scales = Div(Lout, Lin);
            R = Mul(R, scales);
            G = Mul(G, scales);
            B = Mul(B, scales);
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) override {
            r *= exposure;
            g *= exposure;
            b *= exposure;
            float Lin = Luma(r, g, b);

            // Apply exposure scale to parameters
            const float Lmax = this->maxLd * exposure;

            // Bias the world adaptation and scale other parameters accordingly
            float LwaP = Lwa / std::pow(1.f + bias - 0.85f, 5.f),
                    LmaxP = Lmax / LwaP,
                    LinP = Lin / LwaP;

            // Apply tonemapping curve to luminance
            float exponent = std::log(bias) / std::log(0.5f),
                    c1 = (0.01f * maxLd) / std::log10(1.f + LmaxP),
                    c2 = std::log(1.f + LinP) / std::log(2.f + 8.f * std::pow(LinP / LmaxP, exponent)),
                    Lout = c1 * c2;

            const float scale = Lout / Lin;
            if (scale == 1) {
                return;
            }

            r = r * scale;
            g = g * scale;
            b = b * scale;
        }

    private:

        float Luma(const float r, const float g, const float b) {
            return r * lumaCoefficients[0] + g * lumaCoefficients[1] + b * lumaCoefficients[2];
        }

        TFromD<D> lumaCoefficients[4];
    };
}