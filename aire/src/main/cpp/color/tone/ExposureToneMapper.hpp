//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include "ToneMapper.h"
#include <fast_math-inl.h>

namespace aire {
    template<typename D>
    class ExposureToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        TFromD<D> exposure;

    public:
        ExposureToneMapper(const TFromD<D> exposure = 1.0f) : exposure(exposure), ToneMapper<D>() {
        }

        ~ExposureToneMapper() override = default;

        HWY_FAST_MATH_INLINE void Execute(V &R, V &G, V &B) {
            const V mExposure = Set(df_, exposure);
            R = Mul(R, mExposure);
            G = Mul(G, mExposure);
            B = Mul(B, mExposure);
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) {
            r = r * exposure;
            g = g * exposure;
            b = b * exposure;
        }
    };
}