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
    class UchimuraToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        TFromD<D> exposure;

        HWY_FAST_MATH_INLINE V smoothstepV(V edge0, V edge1, V x) {
            const V zeros = Zero(df_);
            const V ones = Set(df_, static_cast<TFromD<D>>(1.0));
            auto t = Clamp(Div(Sub(x, edge0), Sub(edge1, edge0)), zeros, ones);
            const V twos = Set(df_, static_cast<TFromD<D>>(2.0));
            const V threes = Set(df_, static_cast<TFromD<D>>(3.0));
            return Mul(Mul(t, t), Sub(threes, Mul(twos, t)));
        }

        HWY_FAST_MATH_INLINE V stepV(V edge, V x) {
            return smoothstepV(edge, edge, x);
        }

        HWY_FAST_MATH_INLINE TFromD<D> smoothstep(TFromD<D> edge0, TFromD<D> edge1, TFromD<D> x) {
            TFromD<D> t = clamp((x - edge0) / (edge1 - edge0), TFromD<D>(0), TFromD<D>(1));
            return t * t * (TFromD<D>(3) - TFromD<D>(2) * t);
        }

        HWY_FAST_MATH_INLINE TFromD<D> step(TFromD<D> edge, TFromD<D> x) {
            // https://docs.gl/sl4/step
            return smoothstep(edge, edge, x);
        }

        V uchimura(const V v) {
            V P = Set(df_, static_cast<TFromD<D>>(1.0)),
                    a = Set(df_, static_cast<TFromD<D>>(1.0)),
                    m = Set(df_, static_cast<TFromD<D>>(0.22)),
                    l = Set(df_, static_cast<TFromD<D>>(0.4)),
                    c = Set(df_, static_cast<TFromD<D>>(1.33)),
                    b = Zero(df_);

            const V vZeros = Zero(df_);
            const V vExposure = Set(df_, exposure);
            // Fetch color
            const V Cin = Mul(v, vExposure);

            // Apply curve directly on color input
            const V l0 = Div(Mul(Sub(P, m), l), a),
                    S0 = Add(m, l0),
                    S1 = MulAdd(a, l0, m),
                    C2 = Div(Mul(a, P), Sub(P, S1)),
                    CP = Div(Neg(C2), P);

            const V ones = Set(df_, static_cast<TFromD<D>>(1.0));

            V w0 = Sub(ones, smoothstepV(vZeros, m, Cin)),
                    w2 = stepV(Add(m, l0), Cin),
                    w1 = Sub(Sub(ones, w0), w2);

            V T = MulAdd(m, aire::HWY_NAMESPACE::Pow(df_, Div(Cin, m), c), b), // toe
            L = MulAdd(Sub(Cin, m), a, m),           // linear
            S = NegMulAdd(Sub(P, S1), hwy::HWY_NAMESPACE::sleef::Exp(df_, Mul(CP, Sub(Cin, S0))), P);  // shoulder

            const V Cout = MulAdd(T, w0, MulAdd(L, w1, Mul(S, w2)));
            return Cout;
        }

        TFromD<D> uchimura(const TFromD<D> v) {
            TFromD<D> P = 1,
                    a = 1,
                    m = 0.22f,
                    l = 0.4f,
                    c = 1.33f,
                    b = 0;

            // Fetch color
            TFromD<D> Cin = exposure * v;

            // Apply curve directly on color input
            TFromD<D> l0 = ((P - m) * l) / a,
                    S0 = m + l0,
                    S1 = m + a * l0,
                    C2 = (a * P) / (P - S1),
                    CP = -C2 / P;

            TFromD<D> w0 = (1.f) - smoothstep(TFromD<D>(0.f), TFromD<D>(m), Cin),
                    w2 = step(TFromD<D>(m + l0), Cin),
                    w1 = TFromD<D>(1.f) - w0 - w2;

            TFromD<D> T = m * pow(Cin / m, c) + b,                       // toe
            L = TFromD<D>(m) + a * (Cin - TFromD<D>(m)),           // linear
            S = TFromD<D>(P) - (P - S1) * exp(CP * (Cin - S0));  // shoulder

            TFromD<D> Cout = T * w0 + L * w1 + S * w2;
            return Cout;
        }

    public:
        UchimuraToneMapper(const TFromD<D> exposure = 1.0f) :
                exposure(exposure),
                ToneMapper<D>() {
        }

        ~UchimuraToneMapper() override = default;

        HWY_FAST_MATH_INLINE void Execute(V &R, V &G, V &B) override {
            R = uchimura(R);
            G = uchimura(G);
            B = uchimura(B);
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) override {
            r = uchimura(r);
            g = uchimura(g);
            b = uchimura(b);
        }
    };
}