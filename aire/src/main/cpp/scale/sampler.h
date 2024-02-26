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

// P Found using maxima
//
// y(x) := 4 * x * (%pi-x) / (%pi^2) ;
// z(x) := (1-p)*y(x) + p * y(x)^2;
// e(x) := z(x) - sin(x);
// solve( diff( integrate( e(x)^2, x, 0, %pi/2 ), p ) = 0, p ),numer;
//
// [p = .2248391013559941]
template<typename T>
static inline T fastSin1(T x) {
    constexpr T A = T(4.0) / (T(M_PI) * T(M_PI));
    constexpr T P = 0.2248391013559941;
    T y = A * x * (T(M_PI) - x);
    return y * ((1 - P) + y * P);
}

// P and Q found using maxima
//
// y(x) := 4 * x * (%pi-x) / (%pi^2) ;
// zz(x) := (1-p-q)*y(x) + p * y(x)^2 + q * y(x)^3
// ee(x) := zz(x) - sin(x)
// solve( [ integrate( diff(ee(x)^2, p ), x, 0, %pi/2 ) = 0, integrate( diff(ee(x)^2,q), x, 0, %pi/2 ) = 0 ] , [p,q] ),numer;
//
// [[p = .1952403377008734, q = .01915214119105392]]
template<typename T>
static inline T fastSin2(T x) {
    constexpr T A = T(4.0) / (T(M_PI) * T(M_PI));
    constexpr T P = 0.1952403377008734;
    constexpr T Q = 0.01915214119105392;

    T y = A * x * (T(M_PI) - x);

    return y * ((1 - P - Q) + y * (P + y * Q));
}

static inline half_float::half castU16(uint16_t t) {
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
static inline T BiCubicSpline(T x, const T a = -0.5) {
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
static inline T HannWindow(const T n, const T length) {
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

static float J1(float x)
{
    static const float
            Pone[] = {
            0.581199354001606143928050809e+21,
            -0.6672106568924916298020941484e+20,
            0.2316433580634002297931815435e+19,
            -0.3588817569910106050743641413e+17,
            0.2908795263834775409737601689e+15,
            -0.1322983480332126453125473247e+13,
            0.3413234182301700539091292655e+10,
            -0.4695753530642995859767162166e+7,
            0.270112271089232341485679099e+4
    },
            Qone[] = {
            0.11623987080032122878585294e+22,
            0.1185770712190320999837113348e+20,
            0.6092061398917521746105196863e+17,
            0.2081661221307607351240184229e+15,
            0.5243710262167649715406728642e+12,
            0.1013863514358673989967045588e+10,
            0.1501793594998585505921097578e+7,
            0.1606931573481487801970916749e+4,
            0.1e+1
    };

    float p = Pone[8];
    float q = Qone[8];

    const float dX = x*x;

    for (int i = 7; i >= 0; i--){
        p = p*dX + Pone[i];
        q = q*dX + Qone[i];
    }

    return p/q;
}

static float P1(float x)
{
    static const float
            Pone[] = {
            0.352246649133679798341724373e+5,
            0.62758845247161281269005675e+5,
            0.313539631109159574238669888e+5,
            0.49854832060594338434500455e+4,
            0.2111529182853962382105718e+3,
            0.12571716929145341558495e+1
    },
            Qone[] = {
            0.352246649133679798068390431e+5,
            0.626943469593560511888833731e+5,
            0.312404063819041039923015703e+5,
            0.4930396490181088979386097e+4,
            0.2030775189134759322293574e+3,
            0.1e+1
    };

    float p = Pone[5];
    float q = Qone[5];

    const float dX = (8.0/x)*(8.0/x);

    for (int i = 4; i >= 0; i--) {
        p = p*dX + Pone[i];
        q = q*dX + Qone[i];
    }

    return p/q;
}

static float Q1(float x)
{
    static const float
            Pone[] = {
            0.3511751914303552822533318e+3,
            0.7210391804904475039280863e+3,
            0.4259873011654442389886993e+3,
            0.831898957673850827325226e+2,
            0.45681716295512267064405e+1,
            0.3532840052740123642735e-1
    },
            Qone[] = {
            0.74917374171809127714519505e+4,
            0.154141773392650970499848051e+5,
            0.91522317015169922705904727e+4,
            0.18111867005523513506724158e+4,
            0.1038187585462133728776636e+3,
            0.1e+1
    };

    float p = Pone[5];
    float q = Qone[5];

    const float dX = (8.0/x)*(8.0/x);

    for (int i = 4; i >= 0; i--) {
        p = p*dX + Pone[i];
        q = q*dX + Qone[i];
    }

    return p/q;
}

static float BesselOrderOne(float x)
{
    if (x < 1e-8)
        return 0.0;

    float p = x;

    if (x < 0.0)
        x = -x;

    if (x < 8.0)
        return p * J1(x);

    float q = (
            sqrt((float)(2.0/(M_PI*x)))
            * (P1(x)*(1.0/sqrt(2.0)*(sin((float)x)-cos((float)x)))-8.0/x*Q1(x)*(-1.0/sqrt(2.0)*(sin((float) x)+cos((float) x))))
    );

    if (p < 0.0)
        q = -q;

    return q;
}


static inline float jinc(float x) {
    if (x < 1e-8)
        return 0.5*M_PI;
    return BesselOrderOne(M_PI*x)/x;
}

template<typename T>
static inline T LanczosJinc(T x, const T a) {
    if (abs(x) < a) {
        return jinc(T(M_PI) * x) * jinc(T(M_PI) * x / a);
    }
    return T(0.0);
}

#endif //JXLCODER_SAMPLER_H
