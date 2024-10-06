/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 04/02/24, 6:13 PM
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

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "base/Convolve1D.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"

#include "Convolve1D.h"
#include "hwy/highway.h"
#include "jni/JNIUtils.h"
#include <thread>
#include "algo/support-inl.h"
#include "concurrency.hpp"
#include "Eigen/Eigen"

HWY_BEFORE_NAMESPACE();
namespace aire::HWY_NAMESPACE {

using namespace hwy;
using namespace std;
using namespace hwy::HWY_NAMESPACE;

void
convolve1DHorizontalPass(std::vector<uint8_t> &transient,
                         uint8_t *data, int stride,
                         int y, int width,
                         int height,
                         const Eigen::VectorXf &kernel) {

  auto src = reinterpret_cast<uint8_t *>(data + y * stride);
  auto dst = reinterpret_cast<uint8_t *>(transient.data() + y * stride);

  const FixedTag<uint8_t, 4> du8;
  const FixedTag<uint32_t, 4> du32x4;
  const FixedTag<float32_t, 4> dfx4;
  using VF = Vec<decltype(dfx4)>;
  using VU = Vec<decltype(du8)>;
  const auto max255 = Set(dfx4, 255.0f);
  const VF zeros = Zero(dfx4);
  const FixedTag<uint8_t, 16> du8x16;

  // Preheat kernel memory to stack
  VF kernelCache[kernel.size()];
  for (int j = 0; j < kernel.size(); ++j) {
    kernelCache[j] = Set(dfx4, kernel[j]);
  }

  const int kernelSize = kernel.size();
  const int halfOfKernel = kernelSize / 2;
  const bool isEven = kernelSize % 2 == 0;
  const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

  for (int x = 0; x < width; ++x) {
    VF store = zeros;

    int r = -halfOfKernel;

    if (kernelSize == 3) {
      int pos = clamp(x - 1, 0, width - 1) * 4;
      VF dWeight = kernelCache[0];
      VU pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x, 0, width - 1) * 4;
      dWeight = kernelCache[1];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 1, 0, width - 1) * 4;
      dWeight = kernelCache[2];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
    } else if (kernelSize == 5) {
      int pos = clamp(x - 2, 0, width - 1) * 4;
      VF dWeight = kernelCache[0];
      VU pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x - 1, 0, width - 1) * 4;
      dWeight = kernelCache[1];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x, 0, width - 1) * 4;
      dWeight = kernelCache[2];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 1, 0, width - 1) * 4;
      dWeight = kernelCache[3];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 2, 0, width - 1) * 4;
      dWeight = kernelCache[4];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
    } else if (kernelSize == 7) {
      int pos = clamp(x - 3, 0, width - 1) * 4;
      VF dWeight = kernelCache[0];
      VU pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x - 2, 0, width - 1) * 4;
      dWeight = kernelCache[1];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x - 1, 0, width - 1) * 4;
      dWeight = kernelCache[2];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x, 0, width - 1) * 4;
      dWeight = kernelCache[3];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 1, 0, width - 1) * 4;
      dWeight = kernelCache[4];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 2, 0, width - 1) * 4;
      dWeight = kernelCache[5];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 3, 0, width - 1) * 4;
      dWeight = kernelCache[6];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
    } else if (kernelSize == 9) {
      int pos = clamp(x - 4, 0, width - 1) * 4;
      VF dWeight = kernelCache[0];
      VU pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x - 3, 0, width - 1) * 4;
      dWeight = kernelCache[1];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x - 2, 0, width - 1) * 4;
      dWeight = kernelCache[2];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x - 1, 0, width - 1) * 4;
      dWeight = kernelCache[3];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x, 0, width - 1) * 4;
      dWeight = kernelCache[4];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 1, 0, width - 1) * 4;
      dWeight = kernelCache[5];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 2, 0, width - 1) * 4;
      dWeight = kernelCache[6];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 3, 0, width - 1) * 4;
      dWeight = kernelCache[7];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

      pos = clamp(x + 4, 0, width - 1) * 4;
      dWeight = kernelCache[8];
      pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
    } else {
      for (; r + 4 <= maxKernel && x + r + 4 < width; r += 4) {
        int pos = clamp((x + r), 0, width - 1) * 4;

        VF v1, v2, v3, v4;
        auto pu = LoadU(du8x16, &src[pos]);
        ConvertToFloatVec16(du8x16, pu, v1, v2, v3, v4);

        int pf = r + halfOfKernel;

        VF dWeight = kernelCache[pf];
        store = Add(store, Mul(v1, dWeight));
        dWeight = kernelCache[pf + 1];
        store = Add(store, Mul(v2, dWeight));
        dWeight = kernelCache[pf + 2];
        store = Add(store, Mul(v3, dWeight));
        dWeight = kernelCache[pf + 3];
        store = Add(store, Mul(v4, dWeight));
      }

      for (; r <= maxKernel; ++r) {
        int pos = clamp((x + r), 0, width - 1) * 4;
        VF dWeight = kernelCache[r + halfOfKernel];
        VU pixels = LoadU(du8, &src[pos]);
        store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
      }
    }

    store = Max(Min(Round(store), max255), zeros);
    VU pixelU = DemoteTo(du8, ConvertTo(du32x4, store));
    StoreU(pixelU, du8, dst);

    dst += 4;
  }
}

void
convolve1DVerticalPass(std::vector<uint8_t> &transient, uint8_t *data, int stride,
                       int y, int width, int height,
                       const Eigen::VectorXf &kernel) {
  const FixedTag<uint8_t, 4> du8;
  const FixedTag<uint32_t, 4> du32x4;
  const FixedTag<float32_t, 4> dfx4;
  using VF = Vec<decltype(dfx4)>;
  using VU = Vec<decltype(du8)>;
  const auto max255 = Set(dfx4, 255.0f);
  const VF zeros = Zero(dfx4);

  // Preheat kernel memory to stack
  VF kernelCache[kernel.size()];
  for (int j = 0; j < kernel.size(); ++j) {
    kernelCache[j] = Set(dfx4, kernel[j]);
  }

  const int halfOfKernel = kernel.size() / 2;
  const bool isEven = kernel.size() % 2 == 0;
  const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

  auto dst = reinterpret_cast<uint8_t *>(data + y * stride);
  for (int x = 0; x < width; ++x) {
    VF store = zeros;

    int r = -halfOfKernel;

    for (; r <= maxKernel; ++r) {
      auto src = reinterpret_cast<uint8_t *>(transient.data() +
          clamp((r + y), 0, height - 1) * stride);
      int pos = clamp(x, 0, width - 1) * 4;
      VF dWeight = kernelCache[r + halfOfKernel];
      VU pixels = LoadU(du8, &src[pos]);
      store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
    }

    store = Max(Min(Round(store), max255), zeros);
    VU pixelU = DemoteTo(du8, ConvertTo(du32x4, store));

    StoreU(pixelU, du8, dst);

    dst += 4;
  }
}

}
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace aire {
HWY_EXPORT(convolve1DHorizontalPass);
HWY_EXPORT(convolve1DVerticalPass);

void convolve1D(uint8_t *data, int stride, int width, int height, const std::vector<float> &horizontal, const std::vector<float> &vertical) {
  std::vector<uint8_t> transient(stride * height);

  Eigen::VectorXf horizontalKernel(horizontal.size());
  for (int i = 0; i < horizontal.size(); ++i) {
    horizontalKernel(i) = horizontal[i];
  }

  Eigen::VectorXf verticalKernel(vertical.size());
  for (int i = 0; i < vertical.size(); ++i) {
    verticalKernel(i) = vertical[i];
  }

  const int threadCount = std::clamp(std::min(static_cast<int>(std::thread::hardware_concurrency()),
                                              height * width / (256 * 256)), 1, 12);
  concurrency::parallel_for(threadCount, height, [&](int y) {
    HWY_DYNAMIC_DISPATCH(convolve1DHorizontalPass)(transient, data, stride, y, width, height, horizontalKernel);
  });

  concurrency::parallel_for(threadCount, height, [&](int y) {
    HWY_DYNAMIC_DISPATCH(convolve1DVerticalPass)(transient, data, stride, y, width, height, verticalKernel);
  });
}
}
#endif