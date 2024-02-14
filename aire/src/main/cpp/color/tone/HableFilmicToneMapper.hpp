//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include "ToneMapper.h"
#include <fast_math-inl.h>
#include <algorithm>
#include "Eigen/Eigen"

namespace aire {
    template<typename D>
    class HableFilmicToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        TFromD<D> exposure;

        HWY_FAST_MATH_INLINE TFromD<D> hable(TFromD<D> x) {
            const float A = 0.15, B = 0.50, C = 0.10, D1 = 0.20, E = 0.02, F = 0.30;
            return ((x * (A * x + (C * B)) + (D1 * E)) / (x * (A * x + B) + (D1 * F))) - E / F;
        }

        HWY_FAST_MATH_INLINE Eigen::Vector3f hable(Eigen::Vector3f x) {
            const float A = 0.15, B = 0.50, C = 0.10, D1 = 0.20, E = 0.02, F = 0.30;
            return ((x.array() * (A * x.array() + (C * B)) + (D1 * E)) / (x.array() * (A * x.array() + B) + (D1 * F))) - E / F;
        }

        HWY_FAST_MATH_INLINE V hable(V v) {
            const auto A = Set(df_, 0.15), B = Set(df_, 0.50), C = Set(df_, 0.10),
                D1 = Set(df_, 0.20), E = Set(df_, 0.02), F = Set(df_, 0.30);

            auto num = MulAdd(v, MulAdd(A, v, Mul(C, B)), Mul(D1, E));
            auto den = MulAdd(v, MulAdd(v, A, B), Mul(D1, F));
            auto sub = Div(E, F);
            return Sub(Div(num, den), sub);
        }

    public:
        HableFilmicToneMapper(const TFromD<D> exposure = 1.0f) : exposure(exposure), ToneMapper<D>() {
        }

        ~HableFilmicToneMapper() override = default;

        HWY_FAST_MATH_INLINE void Execute(V &R, V &G, V &B) override {
            const auto vExpo = Set(df_, exposure);
            R = hable(Mul(R, vExpo));
            G = hable(Mul(G, vExpo));
            B = hable(Mul(B, vExpo));
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) override {
            Eigen::Vector3f vec = {r, g, b};
            vec = hable(vec * exposure);
            r = vec.x();
            g = vec.y();
            b = vec.z();
        }

    };
}