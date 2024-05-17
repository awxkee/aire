/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 26/02/24, 6:13 PM
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

#if defined(AIRE_WEIGHTED4_ROW_SAMPLER) == defined(HWY_TARGET_TOGGLE)
#ifdef AIRE_WEIGHTED4_ROW_SAMPLER
#undef AIRE_WEIGHTED4_ROW_SAMPLER
#else
#define AIRE_WEIGHTED4_ROW_SAMPLER
#endif

#include <hwy/highway.h>
#include "ScaleRowSampler.hpp"
#include <cstdint>
#include <algorithm>
#include "sampler.h"
#include "sampler-inl.h"
#include "algo/math-inl.h"
#include "HalfFloats.h"

namespace aire::HWY_NAMESPACE {

using namespace hwy;
using namespace hwy::HWY_NAMESPACE;

enum WeightedRow4Operation {
  WEIGHTED_ROW4_HERMITE,
  WEIGHTED_ROW4_CATMULL_ROM,
  WEIGHTED_ROW4_BSPLINE,
  WEIGHTED_ROW4_CUBIC,
  WEIGHTED_ROW4_BICUBIC,
  WEIGHTED_ROW4_MITCHELL,
  WEIGHTED_ROW4_LANCZOS,
  WEIGHTED_ROW4_HANN,
  WEIGHTED_ROW4_BILINEAR
};

template<typename T>
class WeightedRowSampler : public ScaleRowSampler<T> {
 public:
  WeightedRowSampler(const WeightedRow4Operation op,
                     const T *mSource,
                     const int srcStride,
                     const int inputWidth,
                     const int inputHeight,
                     T *mDestination,
                     const int dstStride,
                     const int outputWidth,
                     const int outputHeight,
                     const int components) :
      ScaleRowSampler<T>(mSource,
                         srcStride,
                         inputWidth,
                         inputHeight,
                         mDestination,
                         dstStride,
                         outputWidth,
                         outputHeight,
                         components) {
    switch (op) {
      case WEIGHTED_ROW4_HERMITE: {
        sampler = CubicHermite;
      }
        break;
      case WEIGHTED_ROW4_CATMULL_ROM: {
        sampler = CatmullRom;
      }
        break;
      case WEIGHTED_ROW4_BSPLINE: {
        sampler = BSpline;
      }
        break;
      case WEIGHTED_ROW4_CUBIC: {
        sampler = SimpleCubic;
      }
        break;
      case WEIGHTED_ROW4_BICUBIC: {
        sampler = BiCubicSpline;
      }
        break;
      case WEIGHTED_ROW4_MITCHELL: {
        sampler = MitchellNetravalli;
      }
        break;
      case WEIGHTED_ROW4_BILINEAR: {
        sampler = BilinearFilter;
      }
        break;
      case WEIGHTED_ROW4_LANCZOS: {
        sampler = Lanczos3Sinc;
        filterBase = 3;
      }
        break;
      case WEIGHTED_ROW4_HANN: {
        sampler = HannWindow;
        filterBase = 3;
      }
        break;
    }
  }

  void sample(const int y) override {
    auto dst8 = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(this->mDestination) + y * this->dstStride);
    auto dst = reinterpret_cast<T *>(dst8);

    const float scaleX = static_cast<float>(this->inputWidth) / static_cast<float>(this->outputWidth);
    const float scaleY = static_cast<float>(this->inputHeight) / static_cast<float >(this->outputHeight);

    const float filterScale = std::max(std::max(scaleX, scaleY), 1.0f);
    const int filterLength = filterBase * std::max(static_cast<int>(filterScale), 1) + 1;
    const float iFilterScale = 1.0f / static_cast<float>(filterScale);

    const float totalIterations = 1.f / (static_cast<float>(filterLength) * 2.f + 1.f) * (static_cast<float>(filterLength) * 2.f + 1.f);

    for (int x = 0; x < this->outputWidth; ++x) {
      float centerX = static_cast<float>(x) * this->xScale;
      float centerY = static_cast<float>(y) * this->yScale;

      float rgb[4];
      std::fill(rgb, rgb + 4, 0.0f);
      float weightSum = 0.f;
      float alphaStore = 0.f;

      const auto sourceX = static_cast<uint32_t>(::floorf(centerX));
      const auto sourceY = static_cast<uint32_t>(::floorf(centerY));

      for (int j = -filterLength; j <= filterLength; j++) {
        auto yj = static_cast<uint32_t >(static_cast<int64_t >(sourceY) + static_cast<int64_t >(j));
        float dy = static_cast<float>(yj) - centerY;
        float yWeight = sampler(dy * iFilterScale);
        for (int i = -filterLength; i <= filterLength; i++) {
          auto xi = static_cast<uint32_t >(static_cast<int64_t >(sourceX) + static_cast<int64_t >(i));
          float dx = static_cast<float>(xi) - centerX;
          float xWeight = sampler(dx * iFilterScale);
          float weight = xWeight * yWeight;
          weightSum += weight;

          auto row = reinterpret_cast<const T *>(reinterpret_cast<const uint8_t *>(this->mSource) +
              std::clamp(yj, static_cast<uint32_t >(0),
                         static_cast<uint32_t>(this->inputHeight - 1)) *
                  this->srcStride);

          const uint32_t px = std::clamp(xi,
                                         static_cast<uint32_t >(0),
                                         static_cast<uint32_t>(this->inputWidth - 1)) * this->components;
          for (int c = 0; c < std::min(this->components, 3); ++c) {
            auto clrf = static_cast<float>(row[px + c]);
            float clr = clrf * weight;
            rgb[c] += clr;
          }

          if (this->components == 4) {
            alphaStore += row[px + 3];
          }
        }
      }

      const int px = x * this->components;

      if (std::is_same<T, uint8_t>::value) {
        for (int c = 0; c < std::min(this->components, 3); ++c) {
          if (weightSum != 0.f) {
            dst[px + c] = static_cast<uint8_t>(std::clamp(::roundf(rgb[c] / weightSum), 0.0f, maxColors));
          } else {
            dst[px + c] = 0;
          }
        }
        if (this->components == 4) {
          dst[px + 3] = std::clamp(static_cast<float>(alphaStore) * totalIterations, 0.f, maxColors);
        }
      } else if (std::is_same<T, float>::value) {
        for (int c = 0; c < std::min(this->components, 3); ++c) {
          if (weightSum != 0.f) {
            dst[px + c] = rgb[c] / weightSum;
          } else {
            dst[px + c] = 0.f;
          }
        }
        if (this->components == 4) {
          dst[px + 3] = static_cast<float>(alphaStore) * totalIterations;
        }
      }
    }
  }

  ~WeightedRowSampler() override = default;

 private:
  int filterBase = 2;
  const float maxColors = std::powf(2.0f, (float) 8.f) - 1.0f;
  ScaleWeightSampler sampler;
};

class WeightedWindow4RowSampler10Bit : public ScaleRowSampler<uint32_t> {
 public:
  WeightedWindow4RowSampler10Bit(const WeightedRow4Operation op,
                                 const uint32_t *mSource,
                                 const int srcStride,
                                 const int inputWidth,
                                 const int inputHeight,
                                 uint32_t *mDestination,
                                 const int dstStride,
                                 const int outputWidth,
                                 const int outputHeight) :
      ScaleRowSampler<uint32_t>(mSource,
                                srcStride,
                                inputWidth,
                                inputHeight,
                                mDestination,
                                dstStride,
                                outputWidth,
                                outputHeight,
                                4) {
    switch (op) {
      case WEIGHTED_ROW4_HERMITE: {
        sampler = CubicHermite;
      }
        break;
      case WEIGHTED_ROW4_CATMULL_ROM: {
        sampler = CatmullRom;
      }
        break;
      case WEIGHTED_ROW4_BSPLINE: {
        sampler = BSpline;
      }
        break;
      case WEIGHTED_ROW4_CUBIC: {
        sampler = SimpleCubic;
      }
        break;
      case WEIGHTED_ROW4_BICUBIC: {
        sampler = BiCubicSpline;
      }
        break;
      case WEIGHTED_ROW4_MITCHELL: {
        sampler = MitchellNetravalli;
      }
        break;
      case WEIGHTED_ROW4_LANCZOS: {
        sampler = Lanczos3Sinc;
        filterBase = 3;
      }
        break;
      case WEIGHTED_ROW4_HANN: {
        sampler = HannWindow;
        filterBase = 3;
      }
        break;
      case WEIGHTED_ROW4_BILINEAR: {
        sampler = BilinearFilter;
      }
        break;
    }
  }

  void sample(const int y) override {
    auto dst = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(mDestination) + y * dstStride);

    const float scaleX = static_cast<float>(inputWidth) / static_cast<float>(outputWidth);
    const float scaleY = static_cast<float>(inputHeight) / static_cast<float >(outputHeight);

    const float filterScale = std::max(std::max(scaleX, scaleY), 1.0f);
    const int filterLength = filterBase * std::max(static_cast<int>(filterScale), 1) + 1;
    const float iFilterScale = 1.0f / static_cast<float>(filterScale);

    for (int x = 0; x < outputWidth; ++x) {
      const float srcX = (float) x * xScale;
      const float srcY = (float) y * yScale;

      const float kx1 = ::floorf(srcX);
      const float ky1 = ::floorf(srcY);

      float rgb[4] = {0, 0, 0, 0};

      float filterWeight = 0.f;

      for (int j = -filterLength; j <= filterLength; j++) {
        int yj = (int) ky1 + j;
        float dy = float(srcY) - (float(ky1) + (float) j);
        float yWeight = sampler(dy);

        auto row = reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(mSource) +
            std::clamp(yj, 0, inputHeight - 1) * srcStride);

        for (int i = -filterLength; i <= filterLength; i++) {
          int xi = (int) kx1 + i;
          float dx = float(srcX) - (float(kx1) + (float) i);
          float weight = sampler(dx) * yWeight;

          filterWeight += weight;

          const int px = std::clamp(xi, 0, inputWidth - 1);

          uint32_t color = row[px];

          float r = 0, g = 0, b = 0, aAlpha = 0;
          parseToFloat(color, r, g, b, aAlpha);

          rgb[0] += r * weight;
          rgb[1] += g * weight;
          rgb[2] += b * weight;
          rgb[3] += aAlpha * weight;
        }
      }

      if (filterWeight != 0.f) {
        filterWeight = 1.f / filterWeight;
      }

      auto R10 = static_cast<uint32_t >(std::clamp(::roundf(rgb[0] * maxColors * filterWeight), 0.0f, (float) maxColors));
      auto G10 = static_cast<uint32_t >(std::clamp(::roundf(rgb[1] * maxColors * filterWeight), 0.0f, (float) maxColors));
      auto B10 = static_cast<uint32_t >(std::clamp(::roundf(rgb[2] * maxColors * filterWeight), 0.0f, (float) maxColors));
      auto A10 = static_cast<uint32_t >(std::clamp(::roundf(rgb[3] * 3.f), 0.0f, 3.0f));

      dst[x] = (A10 << 30) | (R10 << 20) | (G10 << 10) | B10;
    }
  }

  ~WeightedWindow4RowSampler10Bit() override = default;

 private:
  const float maxColors = std::powf(2.0f, (float) 10.f) - 1.0f;
  int filterBase = 2;
  ScaleWeightSampler sampler;

  static inline void parseToFloat(const uint32_t rgba1010102, float &r, float &g, float &b, float &a) {
    const uint32_t scalarMask = (1u << 10u) - 1u;
    uint32_t b1 = (rgba1010102) & scalarMask;
    uint32_t g1 = (rgba1010102 >> 10) & scalarMask;
    uint32_t r1 = (rgba1010102 >> 20) & scalarMask;
    uint32_t a1 = (rgba1010102 >> 30) * 3;
    float rFloat = static_cast<float>(r1) / 1023.0f;
    float gFloat = static_cast<float>(g1) / 1023.0f;
    float bFloat = static_cast<float>(b1) / 1023.0f;
    float aFloat = static_cast<float>(a1) / 3.0f;

    r = rFloat;
    g = gFloat;
    b = bFloat;
    a = aFloat;
  }
};

class WeightedWindow4RowSampler16Bit : public ScaleRowSampler<uint16_t> {
 public:
  WeightedWindow4RowSampler16Bit(const WeightedRow4Operation op,
                                 const uint16_t *mSource,
                                 const int srcStride,
                                 const int inputWidth,
                                 const int inputHeight,
                                 uint16_t *mDestination,
                                 const int dstStride,
                                 const int outputWidth,
                                 const int outputHeight,
                                 const int components) :
      ScaleRowSampler<uint16_t>(mSource,
                                srcStride,
                                inputWidth,
                                inputHeight,
                                mDestination,
                                dstStride,
                                outputWidth,
                                outputHeight,
                                components) {
    switch (op) {
      case WEIGHTED_ROW4_HERMITE: {
        sampler = CubicHermite;
      }
        break;
      case WEIGHTED_ROW4_CATMULL_ROM: {
        sampler = CatmullRom;
      }
        break;
      case WEIGHTED_ROW4_BSPLINE: {
        sampler = BSpline;
      }
        break;
      case WEIGHTED_ROW4_CUBIC: {
        sampler = SimpleCubic;
      }
        break;
      case WEIGHTED_ROW4_BICUBIC: {
        sampler = BiCubicSpline;
      }
        break;
      case WEIGHTED_ROW4_MITCHELL: {
        sampler = MitchellNetravalli;
      }
        break;
      case WEIGHTED_ROW4_LANCZOS: {
        sampler = Lanczos3Sinc;
        filterBase = 3;
      }
        break;
      case WEIGHTED_ROW4_HANN: {
        sampler = HannWindow;
        filterBase = 3;
      }
        break;
      case WEIGHTED_ROW4_BILINEAR: {
        sampler = BilinearFilter;
      }
        break;
    }
  }

  void sample(const int y) override {
    const float scaleX = static_cast<float>(inputWidth) / static_cast<float>(outputWidth);
    const float scaleY = static_cast<float>(inputHeight) / static_cast<float >(outputHeight);

    const float filterScale = std::max(std::max(scaleX, scaleY), 1.0f);
    const int filterLength = filterBase * std::max(static_cast<int>(filterScale), 1) + 1;
    const float iFilterScale = 1.0f / static_cast<float>(filterScale);

    const float totalIterations = 1.f / (static_cast<float>(filterLength) * 2.f + 1.f) * (static_cast<float>(filterLength) * 2.f + 1.f);

    const int mMaxWidth = inputWidth - 1;

    const auto src8 = reinterpret_cast<const uint8_t *>(mSource);
    auto dst16 = reinterpret_cast<uint16_t *>(reinterpret_cast<uint8_t *>(mDestination) + y * dstStride);

    for (int x = 0; x < outputWidth; ++x) {
      float centerX = static_cast<float>(x) * this->xScale;
      float centerY = static_cast<float>(y) * this->yScale;

      float rgb[4];
      std::fill(rgb, rgb + 4, 0.0f);

      float weightSum = 0.f;

      float alphaStore = 0.f;

      const auto sourceX = static_cast<uint32_t>(::floorf(centerX));
      const auto sourceY = static_cast<uint32_t>(::floorf(centerY));

      for (int j = -filterLength; j <= filterLength; j++) {
        auto yj = static_cast<uint32_t >(static_cast<int64_t >(sourceY) + static_cast<int64_t >(j));
        float dy = static_cast<float>(yj) - centerY;
        float yWeight = sampler(dy);

        for (int i = -filterLength; i <= filterLength; i++) {
          auto xi = static_cast<uint32_t >(static_cast<int64_t >(sourceX) + static_cast<int64_t >(i));
          float dx = static_cast<float>(xi) - centerX;
          float weight = sampler(dx) * yWeight;
          weightSum += weight;

          const uint16_t *row = reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(src8) +
              std::clamp(yj, static_cast<uint32_t >(0), static_cast<uint32_t>(inputHeight - 1)) * srcStride);

          const uint32_t px = std::clamp(xi, static_cast<uint32_t >(0), static_cast<uint32_t>(inputWidth - 1)) * components;

          for (int c = 0; c < std::min(components, 3); ++c) {
            float clrf = hwy::F32FromF16(hwy::float16_t::FromBits(row[px + c]));
            float clr = (float) clrf * weight;
            rgb[c] += clr;
          }

          if (components == 4) {
            float clrf = hwy::F32FromF16(hwy::float16_t::FromBits(row[px + 3]));
            alphaStore += clrf;
          }
        }
      }

      int px = x * components;

      for (int c = 0; c < std::min(components, 3); ++c) {
        float newColor = rgb[c];
        if (weightSum != 0.f) {
          dst16[px + c] = hwy::F16FromF32(newColor / weightSum).bits;
        } else {
          dst16[px + c] = hwy::F16FromF32(0.f).bits;
        }
      }

      if (this->components == 4) {
        dst16[px + 3] = hwy::F16FromF32(static_cast<float>(alphaStore) * totalIterations).bits;
      }

    }
  }

  ~WeightedWindow4RowSampler16Bit() override = default;

 private:
  ScaleWeightSampler sampler;
  int filterBase = 2;
};

} // aire


#endif
