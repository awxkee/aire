//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include "ToneMapper.h"
#include <fast_math-inl.h>

namespace aire {
    template<typename D>
    class LogarithmicToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        TFromD<D> den;
        TFromD<D> exposure;

    public:
        LogarithmicToneMapper(const TFromD<D> lumaCoefficients[3], const TFromD<D> exposure = 1.0f) : exposure(exposure), ToneMapper<D>() {
            std::copy(lumaCoefficients, lumaCoefficients + 3, this->lumaCoefficients);
            this->lumaCoefficients[3] = 0.0f;
            TFromD<D> Lmax = 1;
            den = static_cast<TFromD<D>>(1) / log(static_cast<TFromD<D>>(1 + Lmax * exposure));
        }

        ~LogarithmicToneMapper() override = default;

        HWY_FAST_MATH_INLINE void Execute(V &R, V &G, V &B) {
            const V mExposure = Set(df_, exposure);
            const V lumaR = Set(df_, lumaCoefficients[0]);
            const V lumaG = Set(df_, lumaCoefficients[1]);
            const V lumaB = Set(df_, lumaCoefficients[2]);
            const V denom = Set(df_, den);

            R = Mul(R,  mExposure);
            G = Mul(G,  mExposure);
            B = Mul(B,  mExposure);

            V rLuma = Mul(R, lumaR);
            V gLuma = Mul(G, lumaG);
            V bLuma = Mul(B, lumaB);

            const V ones = Set(df_, static_cast<TFromD<D>>(1.0f));
            const V zeros = Zero(df_);

            V Lin = Add(Add(rLuma, gLuma), bLuma);
            V Lout = Mul(aire::HWY_NAMESPACE::Lognf(df_, Abs(Add(Lin, ones))), denom);

            Lin = IfThenElse(Lin == zeros, ones, Lin);

            V scales = Div(Lout, Lin);
            R = Mul(R, scales);
            G = Mul(G, scales);
            B = Mul(B, scales);
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) {
            r *= exposure;
            g *= exposure;
            b *= exposure;
            const float Lin =
                    r * lumaCoefficients[0] + g * lumaCoefficients[1] + b * lumaCoefficients[2];
            if (Lin == 0) {
                return;
            }
            const TFromD<D> Lout = log(abs(1 + Lin)) * den;
            const auto shScale = Lout / Lin;
            r = r * shScale;
            g = g * shScale;
            b = b * shScale;
        }

    private:
        TFromD<D> lumaCoefficients[4];
    };
}