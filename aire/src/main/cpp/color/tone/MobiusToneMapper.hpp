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
    class MobiusToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        TFromD<D> exposure;
        TFromD<D> transition;
        TFromD<D> peak;

        HWY_FAST_MATH_INLINE TFromD<D> mobius(const TFromD<D> x) {
            const TFromD<D> in = x * exposure;
            const TFromD<D> j = transition;
            if (in <= j)
                return in;
            const TFromD<D> a = -j * j * (peak - 1.0f) / (j * j - 2.0f * j + peak);
            const TFromD<D> b = (j * j - 2.0f * j * peak + peak) / std::max(peak - 1.0f, 1e-6f);
            return (b * b + 2.0f * b * j + j * j) / (b - a) * (in + a) / (in + b);
        }

        HWY_FAST_MATH_INLINE V mobius(const V x) {
            const V vExp = Set(df_, exposure);
            const V in = Mul(x, vExp);
            const V j = Set(df_, transition);
            const V vPeak = Set(df_, peak);
            const auto lowerMask = in <= j;
            const V ones = Set(df_, static_cast<TFromD<D>>(1.0));
            const V twos = Set(df_, static_cast<TFromD<D>>(1.0));
            const V highLimit = Set(df_, static_cast<TFromD<D>>(1e-6f));
            auto r = Mul(Mul(Neg(j), j), Sub(vPeak, ones));
            auto d = Add(MulSub(j, j, Mul(twos, j)), vPeak);
            auto a = Div(r, d);
            auto num1 = Add(MulSub(j, j, Mul(Mul(twos, j), vPeak)), vPeak);
            auto den1 = Max(Sub(vPeak, ones), highLimit);
            auto b = Div(num1, den1);
            auto rnum = MulAdd(j, j, MulAdd(b, b, Mul(Mul(twos, b), j)));
            rnum = Div(rnum, Sub(b, a));
            rnum = Mul(rnum, Add(in ,a));
            auto res = Div(rnum, Add(in, b));
            auto masked = IfThenElse(lowerMask, in, res);
            return masked;
        }

    public:
        MobiusToneMapper(const TFromD<D> exposure = 1.0f,
                         const TFromD<D> transition = 0.9f,
                         const TFromD<D> peak = 1.0f) :
                exposure(exposure),
                transition(transition),
                peak(peak),
                ToneMapper<D>() {
        }

        ~MobiusToneMapper() override = default;

        HWY_FAST_MATH_INLINE void Execute(V &R, V &G, V &B) override {
            R = mobius(R);
            G = mobius(G);
            B = mobius(B);
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) override {
            r = mobius(r);
            g = mobius(g);
            b = mobius(b);
        }
    };
}