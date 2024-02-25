/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 01/02/24, 6:13 PM
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

#if defined(AIRE_RATIONAL_POLYNOMIAL_INL_H_) == \
    defined(HWY_TARGET_TOGGLE)
#ifdef AIRE_RATIONAL_POLYNOMIAL_INL_H_
#undef AIRE_RATIONAL_POLYNOMIAL_INL_H_
#else
#define AIRE_RATIONAL_POLYNOMIAL_INL_H_
#endif

#include <stddef.h>

#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();

namespace aire {
    namespace HWY_NAMESPACE {
        namespace {

// These templates are not found via ADL.
            using hwy::HWY_NAMESPACE::Div;
            using hwy::HWY_NAMESPACE::MulAdd;

// Primary template: default to actual division.
            template<typename T, class V>
            struct FastDivision {
                HWY_INLINE V

                operator()(const V n, const V d) const { return n / d; }
            };

// Partial specialization for float vectors.
            template<class V>
            struct FastDivision<float, V> {
                // One Newton-Raphson iteration.
                static HWY_INLINE V

                ReciprocalNR(const V x) {
                    const auto rcp = ApproximateReciprocal(x);
                    const auto sum = Add(rcp, rcp);
                    const auto x_rcp = Mul(x, rcp);
                    return NegMulAdd(x_rcp, rcp, sum);
                }

                V operator()(const V n, const V d) const {
#if 1  // Faster on SKX
                    return Div(n, d);
#else
                    return n * ReciprocalNR(d);
#endif
                }
            };

// Approximates smooth functions via rational polynomials (i.e. dividing two
// polynomials). Evaluates polynomials via Horner's scheme, which is faster than
// Clenshaw recurrence for Chebyshev polynomials. LoadDup128 allows us to
// specify constants (replicated 4x) independently of the lane count.
            template<size_t NP, size_t NQ, class D, class V, typename T>
            HWY_INLINE HWY_MAYBE_UNUSED

            V EvalRationalPolynomial(const D d, const V x,
                                     const T (&p)[NP],
                                     const T (&q)[NQ]) {
                constexpr
                size_t kDegP = NP / 4 - 1;
                constexpr
                size_t kDegQ = NQ / 4 - 1;
                auto yp = LoadDup128(d, &p[kDegP * 4]);
                auto yq = LoadDup128(d, &q[kDegQ * 4]);
                // We use pointer arithmetic to refer to &p[(kDegP - n) * 4] to avoid a
                // compiler warning that the index is out of bounds since we are already
                // checking that it is not out of bounds with (kDegP >= n) and the access
                // will be optimized away. Similarly with q and kDegQ.
                HWY_FENCE;
                if (kDegP >= 1) yp = MulAdd(yp, x, LoadDup128(d, p + ((kDegP - 1) * 4)));
                if (kDegQ >= 1) yq = MulAdd(yq, x, LoadDup128(d, q + ((kDegQ - 1) * 4)));
                HWY_FENCE;
                if (kDegP >= 2) yp = MulAdd(yp, x, LoadDup128(d, p + ((kDegP - 2) * 4)));
                if (kDegQ >= 2) yq = MulAdd(yq, x, LoadDup128(d, q + ((kDegQ - 2) * 4)));
                HWY_FENCE;
                if (kDegP >= 3) yp = MulAdd(yp, x, LoadDup128(d, p + ((kDegP - 3) * 4)));
                if (kDegQ >= 3) yq = MulAdd(yq, x, LoadDup128(d, q + ((kDegQ - 3) * 4)));
                HWY_FENCE;
                if (kDegP >= 4) yp = MulAdd(yp, x, LoadDup128(d, p + ((kDegP - 4) * 4)));
                if (kDegQ >= 4) yq = MulAdd(yq, x, LoadDup128(d, q + ((kDegQ - 4) * 4)));
                HWY_FENCE;
                if (kDegP >= 5) yp = MulAdd(yp, x, LoadDup128(d, p + ((kDegP - 5) * 4)));
                if (kDegQ >= 5) yq = MulAdd(yq, x, LoadDup128(d, q + ((kDegQ - 5) * 4)));
                HWY_FENCE;
                if (kDegP >= 6) yp = MulAdd(yp, x, LoadDup128(d, p + ((kDegP - 6) * 4)));
                if (kDegQ >= 6) yq = MulAdd(yq, x, LoadDup128(d, q + ((kDegQ - 6) * 4)));
                HWY_FENCE;
                if (kDegP >= 7) yp = MulAdd(yp, x, LoadDup128(d, p + ((kDegP - 7) * 4)));
                if (kDegQ >= 7) yq = MulAdd(yq, x, LoadDup128(d, q + ((kDegQ - 7) * 4)));

                static_assert(kDegP < 8, "Polynomial degree is too high");
                static_assert(kDegQ < 8, "Polynomial degree is too high");

                return FastDivision<T, V>()(yp, yq);
            }

        }  // namespace
// NOLINTNEXTLINE(google-readability-namespace-comments)
    }  // namespace HWY_NAMESPACE
}  // namespace jxl
HWY_AFTER_NAMESPACE();

#endif  // AIRE_RATIONAL_POLYNOMIAL_INL_H_
