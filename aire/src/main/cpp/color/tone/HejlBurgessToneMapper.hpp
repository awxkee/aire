//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include "ToneMapper.h"
#include <fast_math-inl.h>
#include <algorithm>

namespace aire {
    template<typename D>
    class HejlBurgessToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        TFromD<D> exposure;

        HWY_FAST_MATH_INLINE TFromD<D> hejlBurgess(TFromD<D> v) {
            const float Cin = v * exposure;
            float x = std::max(float(0.f), Cin - 0.004f),
                    Cout = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
            return pow(Cout, 2.4f);
        }

        HWY_FAST_MATH_INLINE V hejlBurgess(V v) {
            const auto vExposure = Set(df_, exposure);
            const auto zeros = Zero(df_);
            const auto z0004 = Set(df_, 0.0004);
            const auto Cin = Mul(v, vExposure);
            const auto x = Max(zeros, Sub(Cin, z0004));
            const auto six2p6 = Set(df_, 6.2f);
            const auto halfOf1 = Set(df_, 0.5f);
            const auto one7f = Set(df_, 1.7f);
            const auto z006 = Set(df_, 0.06f);
            const auto Cout = Div(Mul(x, MulAdd(x, six2p6, halfOf1)), MulAdd(x, MulAdd(x, six2p6, one7f), z006));

            const auto gEotf = Set(df_, 2.4f);

            return aire::HWY_NAMESPACE::Pow(df_,Cout, gEotf);
        }

    public:
        HejlBurgessToneMapper(const TFromD<D> exposure = 1.0f) : exposure(exposure), ToneMapper<D>() {
        }

        ~HejlBurgessToneMapper() override = default;

        HWY_FAST_MATH_INLINE void Execute(V &R, V &G, V &B) override {
            R = hejlBurgess(R);
            G = hejlBurgess(G);
            B = hejlBurgess(B);
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) override {
            r = hejlBurgess(r);
            g = hejlBurgess(g);
            b = hejlBurgess(b);
        }

    };
}