/*
 * MIT License
 *
 * Copyright (c) 2023 Radzivon Bartoshyk
 * aire [https://github.com/awxkee/aire]
 *
 * Created by Radzivon Bartoshyk on 29/09/2023
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "XScaler.h"
#include "half.hpp"
#include <thread>
#include <vector>
#include "sampler.h"
#include "concurrency.hpp"
#include "aire_filters.h"

#if defined(__clang__)
#pragma clang fp contract(fast) exceptions(ignore) reassociate(on)
#endif

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "scale/XScaler.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"
#include "sampler-inl.h"
#include "algo/math-inl.h"
#include "NearestRowSampler.hpp"
#include "WeightedRowSampler.hpp"

using namespace half_float;
using namespace std;

HWY_BEFORE_NAMESPACE();

namespace aire::HWY_NAMESPACE {

namespace an = aire::HWY_NAMESPACE;

void scaleImageRGBA1010102HWY(const uint32_t *input,
                              int srcStride,
                              int inputWidth, int inputHeight,
                              uint32_t *output,
                              int dstStride,
                              int outputWidth,
                              int outputHeight,
                              XSampler option) {
  auto source = reinterpret_cast<const uint32_t *>(input);

  const int threadCount = std::clamp(std::min(static_cast<int>(std::thread::hardware_concurrency()),
                                              outputHeight * outputWidth / (256 * 256)), 1, 12);

  std::unique_ptr<ScaleRowSampler<uint32_t>> sampler;
  switch (option) {
    case hermite: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler10Bit>(WEIGHTED_ROW4_HERMITE,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight);
    }
      break;
    case catmullRom: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler10Bit>(WEIGHTED_ROW4_CATMULL_ROM,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight);
    }
      break;
    case bSpline: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler10Bit>(WEIGHTED_ROW4_BSPLINE,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight);
    }
      break;
    case cubic: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler10Bit>(WEIGHTED_ROW4_CUBIC,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight);
    }
      break;
    case bicubic: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler10Bit>(WEIGHTED_ROW4_BICUBIC,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight);
    }
      break;
    case mitchell: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler10Bit>(WEIGHTED_ROW4_MITCHELL,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight);
    }
      break;
    case bilinear: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler10Bit>(WEIGHTED_ROW4_BILINEAR, source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight);
    }
      break;
    case hann: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler10Bit>(WEIGHTED_ROW4_HANN, source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight);
    }
      break;
    case lanczos: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler10Bit>(WEIGHTED_ROW4_LANCZOS,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight);
    }
      break;
    default: {
      sampler = std::make_unique<an::NearestRowSampler10Bit>(source,
                                                             srcStride,
                                                             inputWidth,
                                                             inputHeight,
                                                             output,
                                                             dstStride,
                                                             outputWidth,
                                                             outputHeight);
    }
      break;
  }

  concurrency::parallel_for(threadCount,
                            outputHeight,
                            [&](int iterationId) {
                              sampler->sample(iterationId);
                            });
}

void scaleImageFloat16HWY(const uint16_t *input,
                          int srcStride,
                          int inputWidth, int inputHeight,
                          uint16_t *output,
                          int dstStride,
                          int outputWidth, int outputHeight,
                          int components,
                          XSampler option) {
  auto source = reinterpret_cast<const uint16_t *>(input);

  const int threadCount = std::clamp(std::min(static_cast<int>(std::thread::hardware_concurrency()),
                                              outputHeight * outputWidth / (256 * 256)), 1, 12);

  std::unique_ptr<ScaleRowSampler<uint16_t>> sampler;
  switch (option) {
    case hermite: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler16Bit>(WEIGHTED_ROW4_HERMITE,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight,
                                                                     components);
    }
      break;
    case catmullRom: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler16Bit>(WEIGHTED_ROW4_CATMULL_ROM,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight,
                                                                     components);
    }
      break;
    case bSpline: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler16Bit>(WEIGHTED_ROW4_BSPLINE,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight,
                                                                     components);
    }
      break;
    case cubic: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler16Bit>(WEIGHTED_ROW4_CUBIC,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight,
                                                                     components);
    }
      break;
    case bicubic: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler16Bit>(WEIGHTED_ROW4_BICUBIC,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight,
                                                                     components);
    }
      break;
    case mitchell: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler16Bit>(WEIGHTED_ROW4_MITCHELL,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight,
                                                                     components);
    }
      break;
    case bilinear: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler16Bit>(WEIGHTED_ROW4_BILINEAR,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight,
                                                                     components);
    }
      break;
    case hann: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler16Bit>(WEIGHTED_ROW4_HANN, source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight,
                                                                     components);
    }
      break;
    case lanczos: {
      sampler = std::make_unique<an::WeightedWindow4RowSampler16Bit>(WEIGHTED_ROW4_LANCZOS,
                                                                     source,
                                                                     srcStride,
                                                                     inputWidth,
                                                                     inputHeight,
                                                                     output,
                                                                     dstStride,
                                                                     outputWidth,
                                                                     outputHeight,
                                                                     components);
    }
      break;
    default: {
      sampler = std::make_unique<an::NearestRowSampler16Bit>(source,
                                                             srcStride,
                                                             inputWidth,
                                                             inputHeight,
                                                             output,
                                                             dstStride,
                                                             outputWidth,
                                                             outputHeight,
                                                             components);
    }
      break;
  }

  concurrency::parallel_for(threadCount,
                            outputHeight,
                            [&](int iterationId) {
                              sampler->sample(iterationId);
                            });
}

void scaleImageUseTransient(const uint8_t *input,
                            const int srcStride,
                            int inputWidth,
                            int inputHeight,
                            uint8_t *output,
                            const int dstStride,
                            int outputWidth,
                            int outputHeight,
                            const int components,
                            const int depth,
                            const XSampler option,
                            const XScalingColorSpace space) {

  uint32_t transientRescaledStride = outputWidth * 4 * sizeof(float);
  std::vector<float> transient(outputWidth * 4 * outputHeight);

  std::vector<float> pixels(inputWidth * 4 * inputHeight);
  uint32_t linearStride = inputWidth * 4 * sizeof(float);
  if (space == LINEAR) {
    aire_reformat_surface_u8_to_linear(input, srcStride,
                                       pixels.data(), linearStride,
                                       inputWidth, inputHeight);
  } else if (space == LAB) {
    aire_reformat_surface_u8_to_laba(input, srcStride,
                                     pixels.data(), linearStride,
                                     inputWidth, inputHeight);
  }

  auto src8 = reinterpret_cast<const uint8_t *>(input);

  const int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    outputHeight * outputWidth / (256 * 256)), 1, 12);

  std::unique_ptr<ScaleRowSampler<float>> sampler;
  switch (option) {
    case hermite: {
      sampler = std::make_unique<an::WeightedRowSampler<float>>(WEIGHTED_ROW4_HERMITE,
                                                                pixels.data(),
                                                                linearStride,
                                                                inputWidth,
                                                                inputHeight,
                                                                transient.data(),
                                                                transientRescaledStride,
                                                                outputWidth,
                                                                outputHeight,
                                                                components);
    }
      break;
    case catmullRom: {
      sampler = std::make_unique<an::WeightedRowSampler<float>>(WEIGHTED_ROW4_CATMULL_ROM,
                                                                pixels.data(),
                                                                linearStride,
                                                                inputWidth,
                                                                inputHeight,
                                                                transient.data(),
                                                                transientRescaledStride,
                                                                outputWidth,
                                                                outputHeight,
                                                                components);
    }
      break;
    case bSpline: {
      sampler = std::make_unique<an::WeightedRowSampler<float>>(WEIGHTED_ROW4_BSPLINE,
                                                                pixels.data(),
                                                                linearStride,
                                                                inputWidth,
                                                                inputHeight,
                                                                transient.data(),
                                                                transientRescaledStride,
                                                                outputWidth,
                                                                outputHeight,
                                                                components);
    }
      break;
    case cubic: {
      sampler = std::make_unique<an::WeightedRowSampler<float>>(WEIGHTED_ROW4_CUBIC,
                                                                pixels.data(),
                                                                linearStride,
                                                                inputWidth,
                                                                inputHeight,
                                                                transient.data(),
                                                                transientRescaledStride,
                                                                outputWidth,
                                                                outputHeight,
                                                                components);
    }
      break;
    case bicubic: {
      sampler = std::make_unique<an::WeightedRowSampler<float>>(WEIGHTED_ROW4_BICUBIC,
                                                                pixels.data(),
                                                                linearStride,
                                                                inputWidth,
                                                                inputHeight,
                                                                transient.data(),
                                                                transientRescaledStride,
                                                                outputWidth,
                                                                outputHeight,
                                                                components);
    }
      break;
    case mitchell: {
      sampler = std::make_unique<an::WeightedRowSampler<float>>(WEIGHTED_ROW4_MITCHELL,
                                                                pixels.data(),
                                                                linearStride,
                                                                inputWidth,
                                                                inputHeight,
                                                                transient.data(),
                                                                transientRescaledStride,
                                                                outputWidth,
                                                                outputHeight,
                                                                components);
    }
      break;
    case lanczos: {
      sampler = std::make_unique<an::WeightedRowSampler<float>>(WEIGHTED_ROW4_LANCZOS,
                                                                pixels.data(),
                                                                linearStride,
                                                                inputWidth,
                                                                inputHeight,
                                                                transient.data(),
                                                                transientRescaledStride,
                                                                outputWidth,
                                                                outputHeight,
                                                                components);
    }
      break;
    case hann: {
      sampler = std::make_unique<an::WeightedRowSampler<float>>(WEIGHTED_ROW4_HANN,
                                                                pixels.data(),
                                                                linearStride,
                                                                inputWidth,
                                                                inputHeight,
                                                                transient.data(),
                                                                transientRescaledStride,
                                                                outputWidth,
                                                                outputHeight,
                                                                components);
    }
      break;
    case bilinear: {
      sampler = std::make_unique<an::WeightedRowSampler<float>>(WEIGHTED_ROW4_BILINEAR,
                                                                pixels.data(),
                                                                linearStride,
                                                                inputWidth,
                                                                inputHeight,
                                                                transient.data(),
                                                                transientRescaledStride,
                                                                outputWidth,
                                                                outputHeight,
                                                                components);
    }
      break;
    default: {
      sampler = std::make_unique<an::NearestRowSampler<float>>(pixels.data(),
                                                               linearStride,
                                                               inputWidth,
                                                               inputHeight,
                                                               transient.data(),
                                                               transientRescaledStride,
                                                               outputWidth,
                                                               outputHeight,
                                                               components);
    }
      break;
  }

  concurrency::parallel_for(threadCount,
                            outputHeight,
                            [&](int iterationId) {
                              sampler->sample(iterationId);
                            });

  if (space == LINEAR) {
    aire_reformat_surface_linear_to_u8(transient.data(), transientRescaledStride,
                                       output, dstStride,
                                       outputWidth, outputHeight);
  } else if (space == LAB) {
    aire_reformat_surface_laba_to_u8(transient.data(), transientRescaledStride,
                                     output, dstStride,
                                     outputWidth, outputHeight);
  }
}

void scaleImageU8HWY(const uint8_t *input,
                     const int srcStride,
                     int inputWidth,
                     int inputHeight,
                     uint8_t *output,
                     const int dstStride,
                     int outputWidth,
                     int outputHeight,
                     const int components,
                     const int depth,
                     const XSampler option,
                     const XScalingColorSpace colorSpace) {
  if (colorSpace != sRGB) {
    scaleImageUseTransient(input, srcStride, inputWidth, inputHeight, output, dstStride, outputWidth, outputHeight, components, depth, option, colorSpace);
    return;
  }

  auto src8 = reinterpret_cast<const uint8_t *>(input);

  const int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    outputHeight * outputWidth / (256 * 256)), 1, 12);

  std::unique_ptr<ScaleRowSampler<uint8_t>>
      sampler;
  switch (option) {
    case hermite: {
      sampler = std::make_unique<an::WeightedRowSampler<uint8_t>>
          (WEIGHTED_ROW4_HERMITE,
           src8,
           srcStride,
           inputWidth,
           inputHeight,
           output,
           dstStride,
           outputWidth,
           outputHeight,
           components);
    }
      break;
    case catmullRom: {
      sampler = std::make_unique<an::WeightedRowSampler<uint8_t>>
          (WEIGHTED_ROW4_CATMULL_ROM,
           src8,
           srcStride,
           inputWidth,
           inputHeight,
           output,
           dstStride,
           outputWidth,
           outputHeight,
           components);
    }
      break;
    case bSpline: {
      sampler = std::make_unique<an::WeightedRowSampler<uint8_t>>
          (WEIGHTED_ROW4_BSPLINE,
           src8,
           srcStride,
           inputWidth,
           inputHeight,
           output,
           dstStride,
           outputWidth,
           outputHeight,
           components);
    }
      break;
    case cubic: {
      sampler = std::make_unique<an::WeightedRowSampler<uint8_t>>
          (WEIGHTED_ROW4_CUBIC,
           src8,
           srcStride,
           inputWidth,
           inputHeight,
           output,
           dstStride,
           outputWidth,
           outputHeight,
           components);
    }
      break;
    case bicubic: {
      sampler = std::make_unique<an::WeightedRowSampler<uint8_t>>
          (WEIGHTED_ROW4_BICUBIC,
           src8,
           srcStride,
           inputWidth,
           inputHeight,
           output,
           dstStride,
           outputWidth,
           outputHeight,
           components);
    }
      break;
    case mitchell: {
      sampler = std::make_unique<an::WeightedRowSampler<uint8_t>>
          (WEIGHTED_ROW4_MITCHELL,
           src8,
           srcStride,
           inputWidth,
           inputHeight,
           output,
           dstStride,
           outputWidth,
           outputHeight,
           components);
    }
      break;
    case lanczos: {
      sampler = std::make_unique<an::WeightedRowSampler<uint8_t>>
          (WEIGHTED_ROW4_LANCZOS,
           src8,
           srcStride,
           inputWidth,
           inputHeight,
           output,
           dstStride,
           outputWidth,
           outputHeight,
           components);
    }
      break;
    case hann: {
      sampler = std::make_unique<an::WeightedRowSampler<uint8_t>>
          (WEIGHTED_ROW4_HANN, src8,
           srcStride,
           inputWidth,
           inputHeight,
           output,
           dstStride,
           outputWidth,
           outputHeight,
           components);
    }
      break;
    case bilinear: {
      sampler = std::make_unique<an::WeightedRowSampler<uint8_t>>
          (WEIGHTED_ROW4_BILINEAR, src8,
           srcStride,
           inputWidth,
           inputHeight,
           output,
           dstStride,
           outputWidth,
           outputHeight,
           components);
    }
      break;
    default: {
      sampler = std::make_unique<an::NearestRowSampler<uint8_t>>(src8, srcStride,
                                                                 inputWidth,
                                                                 inputHeight,
                                                                 output,
                                                                 dstStride,
                                                                 outputWidth,
                                                                 outputHeight,
                                                                 components);
    }
      break;
  }

  concurrency::parallel_for(threadCount,
                            outputHeight,
                            [&](int iterationId) {
                              sampler->sample(iterationId);
                            });
}
}

HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace aire {
HWY_EXPORT(scaleImageFloat16HWY);

void scaleImageFloat16(const uint16_t *input,
                       int srcStride,
                       int inputWidth, int inputHeight,
                       uint16_t *output,
                       int dstStride,
                       int outputWidth, int outputHeight,
                       int components,
                       XSampler option) {
  HWY_DYNAMIC_DISPATCH(scaleImageFloat16HWY)(input, srcStride, inputWidth, inputHeight,
                                             output, dstStride, outputWidth, outputHeight,
                                             components, option);
}

HWY_EXPORT(scaleImageU8HWY);

void scaleImageU8(const uint8_t *input,
                  int srcStride,
                  int inputWidth, int inputHeight,
                  uint8_t *output,
                  int dstStride,
                  int outputWidth, int outputHeight,
                  int components,
                  int depth,
                  XSampler option,
                  XScalingColorSpace colorSpace) {
  HWY_DYNAMIC_DISPATCH(scaleImageU8HWY)(input, srcStride, inputWidth, inputHeight, output,
                                        dstStride, outputWidth, outputHeight, components,
                                        depth, option, colorSpace);
}

HWY_EXPORT(scaleImageRGBA1010102HWY);

void scaleImageRGBA1010102(const uint32_t *input,
                           int srcStride,
                           int inputWidth, int inputHeight,
                           uint32_t *output,
                           int dstStride,
                           int outputWidth,
                           int outputHeight,
                           XSampler option) {
  HWY_DYNAMIC_DISPATCH(scaleImageRGBA1010102HWY)(input, srcStride, inputWidth, inputHeight, output,
                                                 dstStride, outputWidth, outputHeight, option);
}
}
#endif