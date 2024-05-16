/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 17/03/24, 6:13 PM
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

#if defined(SPARKYUV__YUV_INL_H) == defined(HWY_TARGET_TOGGLE)
#ifdef SPARKYUV__YUV_INL_H
#undef SPARKYUV__YUV_INL_H
#else
#define SPARKYUV__YUV_INL_H
#endif

#include "hwy/highway.h"

#define SPARKYUV_INLINE __attribute__((flatten)) inline
#define SPARKYUV_RESTRICT __restrict__

HWY_BEFORE_NAMESPACE();
namespace sparkyuv::HWY_NAMESPACE {
using namespace hwy;
using namespace hwy::HWY_NAMESPACE;

template<typename D>
SPARKYUV_INLINE void YUVToRGBKrKb(D d,
                                  const Vec<D> Y, const Vec<D> Cr, const Vec<D> Cb,
                                  Vec<D> &R, Vec<D> &G, Vec<D> &B,
                                  const Vec<D> kr, const Vec<D> kb, const Vec<D> kg) {
  const auto ones = Set(d, static_cast<TFromD<D>>(1.0f));
  const auto twos = Set(d, static_cast<TFromD<D>>(2.0f));
  const auto oneMKr = Sub(ones, kr);
  const auto oneMKb = Sub(ones, kb);
  const auto twiceOneMKr = Mul(twos, oneMKr);
  const auto twiceOneMKb = Mul(twos, oneMKb);
  R = MulAdd(twiceOneMKr, Cr, Y);
  B = MulAdd(twiceOneMKb, Cb, Y);

  const auto revertKg = ApproximateReciprocal(kg);

  const auto p1 = MulAdd(oneMKr, Mul(Cr, kr), Mul(Mul(oneMKb, Cb), kb));
  const auto p2 = Mul(p1, twos);
  G = NegMulAdd(revertKg, p2, Y);
}

template<typename D>
SPARKYUV_INLINE void YUVToRGBLimited(D d,
                                     const Vec<D> Y, const Vec<D> U, const Vec<D> V,
                                     Vec<D> &R, Vec<D> &G, Vec<D> &B,
                                     const Vec<D> lumaCoeff,
                                     const Vec<D> crCoeff, const Vec<D> cbCoeff,
                                     const Vec<D> gCoeff1, const Vec<D> gCoeff2) {
  const auto luma = Mul(Y, lumaCoeff);
  R = MulAdd(crCoeff, V, luma);
  B = MulAdd(cbCoeff, U, luma);
  G = NegMulAdd(gCoeff2, U, NegMulAdd(gCoeff1, V, luma));
}

void computeTransform(const float kr,
                      const float kb,
                      const float rangeHigh,
                      const float rangeLow,
                      float &CrCoeff,
                      float &CbCoeff,
                      float &GCoeff1,
                      float &GCoeff2) {
  const float range = rangeHigh / rangeLow;
  CrCoeff = (2.f * (1.f - kr)) * range;
  CbCoeff = (2.f * (1.f - kb)) * range;
  const float kg = 1.0f - kr - kb;
  if (kg == 0.f) {
    throw std::runtime_error("1.0f - kr - kg must not be 0");
  }
  GCoeff1 = (2 * ((1 - kr) * kr / kg)) * range;
  GCoeff2 = (2 * ((1 - kb) * kb / kg)) * range;
}

}
HWY_AFTER_NAMESPACE();

#endif