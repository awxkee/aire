/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 29/09/23, 6:13 PM
 *  *
 *  * Permission is hereby granted, free of charge, to any person obtaining a copy
 *  * of this software and associated documentation files (the "Software"), to deal
 *  * in the Software without restriction, including without limitation the rights
 *  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  * copies of the Software, and to permit persons to whom the Software is
 *  * furnished to do so, subject to the following conditions:
 *  *
 *  * The above copyright notice and this permission notice shall be included in all
 *  * copies or substantial portions of the Software.
 *  *
 *  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  * SOFTWARE.
 *  *
 *
 */

#ifndef JXLCODER_SAMPLER_H
#define JXLCODER_SAMPLER_H

#include "half.hpp"
#include <algorithm>

using namespace std;
using namespace half_float;

static inline half_float::half castU16(const uint16_t t) {
    half_float::half result;
    result.data_ = t;
    return result;
}

template<typename D, typename T>
static inline D PromoteTo(T t, float maxColors) {
    D result = static_cast<D>((float) t / maxColors);
    return result;
}

template<typename D, typename T>
static inline D DemoteTo(T t, float maxColors) {
    return (D) clamp(((float) t * (float) maxColors), 0.0f, (float) maxColors);
}

template<typename T>
static inline float BCSpline(T x, const T B, const T C) {
    if (x < 0.0f) x = -x;

    const T dp = x * x;
    const T tp = dp * x;

    if (x < 1.0f)
        return ((12 - 9 * B - 6 * C) * tp + (-18 + 12 * B + 6 * C) * dp + (6 - 2 * B)) *
               (T(1) / T(6));
    else if (x < 2.0f)
        return ((-B - 6 * C) * tp + (6 * B + 30 * C) * dp + (-12 * B - 48 * C) * x +
                (8 * B + 24 * C)) * (T(1) / T(6));

    return (0.0f);
}

template<typename T>
static inline T SimpleCubic(T x) {
    if (x < 0.0f) x = -x;

    if (x < 1.0f)
        return (4.0f + x * x * (3.0f * x - 6.0f)) / 6.0f;
    else if (x < 2.0f)
        return (8.0f + x * (-12.0f + x * (6.0f - x))) / 6.0f;

    return (0.0f);
}

template<typename T>
static inline T BiCubicSpline(T x) {
    const T a = -0.5;
    const T modulo = abs(x);
    if (modulo >= 2) {
        return 0;
    }
    const T floatd = modulo * modulo;
    const T triplet = floatd * modulo;
    if (modulo <= 1) {
        return (a + T(2.0))*triplet - (a + T(3.0)) * floatd + T(1.0);
    }
    return a * triplet - T(5.0) * a * floatd + T(8.0) * a * modulo - T(4.0) * a;
}

template<typename T>
static inline T CubicHermite(T x) {
    constexpr T C = T(0.0);
    constexpr T B = T(0.0);
    return BCSpline(x, B, C);
}

template<typename T>
static inline float BSpline(T x) {
    constexpr T C = T(0.0);
    constexpr T B = T(1.0);
    return BCSpline(x, B, C);
}

template<typename T>
static inline float MitchellNetravalli(T x) {
    constexpr T B = 1.0f / 3.0f;
    constexpr T C = 1.0f / 3.0f;
    return BCSpline(x, B, C);
}

template<typename T>
static inline T sinc(T x) {
    if (x == 0.0) {
        return T(1.0);
    } else {
        return sin(x) / x;
    }
}

template<typename T>
static inline T LanczosWindow(T x, const T a) {
    if (abs(x) < a) {
        return sinc(T(M_PI) * x) * sinc(T(M_PI) * x / a);
    }
    return T(0.0);
}

template<typename T>
static inline T fastCos(T x) {
    constexpr T C0 = 0.99940307;
    constexpr T C1 = -0.49558072;
    constexpr T C2 = 0.03679168;
    constexpr T C3 = -0.00434102;

    while (x < -2 * M_PI) {
        x += 2.0 * M_PI;
    }
    while (x > 2 * M_PI) {
        x -= 2.0 * M_PI;
    }

    // Calculate cos(x) using Chebyshev polynomial approximation
    T x2 = x * x;
    T result = C0 + x2 * (C1 + x2 * (C2 + x2 * C3));
    return result;
}

template<typename T>
static inline T CatmullRom(T x) {
    return BCSpline(x, 0.0f, 0.5f);
}

template<typename T>
static inline T HannWindow(const T n) {
    const T length = 3.0f;
    const T size = length * 2;
    const T part = M_PI / size;
    if (abs(n) > length) {
        return 0;
    }
    T r = cos(n * part);
    r = r * r;
    return r / size;
}

template<typename T>
static inline T blerp(T c00, T c10, T c01, T c11, T tx, T ty) {
    return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

template<typename T>
static inline T Lanczos3Sinc(T x) {
    return LanczosWindow(x, 3.0f);
}

template<typename T>
static inline T BilinearFilter(T x) {
    x = ::abs(x);
    if (x < static_cast<T>(1.0f)) {
        return static_cast<T>(1.0f) - x;
    } else {
        return static_cast<T>(0.0f);
    }
}

#endif //JXLCODER_SAMPLER_H
