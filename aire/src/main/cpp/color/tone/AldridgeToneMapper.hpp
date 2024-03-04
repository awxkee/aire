//
// Created by Radzivon Bartoshyk on 16/02/2024.
//

#pragma once

#include "ToneMapper.h"
#include <fast_math-inl.h>
#include <algorithm>
#include "Eigen/Eigen"

namespace aire {
    template<typename D>
    class AldridgeToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        const TFromD<D> exposure;
        const TFromD<D> cutoff;

        HWY_FAST_MATH_INLINE TFromD<D> aldridge(const TFromD<D> v) {
            const TFromD<D> Cin = exposure * v;

            // Apply curve directly on color input
            const TFromD<D> tmp = TFromD<D>(2.f * cutoff);
            const TFromD<D> x = Cin + (tmp - Cin) * std::clamp(tmp - Cin, 0.f, 1.f) * (0.25f / cutoff) - cutoff;
            const TFromD<D> Cout = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
            return std::powf(Cout, 2.4f);
        }

        HWY_FAST_MATH_INLINE V aldridge(const V v) {
            const V vExposure = Set(df_, exposure);
            const V Cin = Mul(v, vExposure);
            const V vCutoff = Set(df_, cutoff);

            const V tmp = Mul(Set(df_, static_cast<TFromD<D>>(2.f)), vCutoff);

            const V zeros = Zero(df_);
            const V ones = Set(df_, static_cast<TFromD<D>>(1.f));

            const auto x = Sub(MulAdd(Sub(tmp, Cin), Mul(Clamp(Sub(tmp, Cin), zeros, ones),
                                                         Div(Set(df_, 0.25f), vCutoff)), Cin), vCutoff);

            const auto sixP2 = Set(df_, static_cast<TFromD<D>>(6.2));

            auto num = Mul(MulAdd(sixP2, x, Set(df_, static_cast<TFromD<D>>(0.5))), x);
            auto den = Add(Mul(MulAdd(sixP2, x, Set(df_, static_cast<TFromD<D>>(1.7))), x),
                           Set(df_, static_cast<TFromD<D>>(0.06)));

            const V Cout = Div(num, den);
            const V twoP4 = Set(df_, static_cast<TFromD<D>>(2.4));
            return aire::HWY_NAMESPACE::Pow(df_, Cout, twoP4);
        }

    public:
        AldridgeToneMapper(const TFromD<D> exposure = 1.0f, const TFromD<D> cutoff = 0.025f) :
                exposure(exposure),
                cutoff(cutoff),
                ToneMapper<D>() {
        }

        ~AldridgeToneMapper() override = default;

        HWY_FAST_MATH_INLINE void Execute(V &R, V &G, V &B) override {
            R = aldridge(R);
            G = aldridge(G);
            B = aldridge(B);
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) override {
            r = aldridge(r);
            g = aldridge(g);
            b = aldridge(b);
        }
    };
}