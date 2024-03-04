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
        WEIGHTED_ROW4_MITCHELL
    };

    class WeightedWindow4RowSampler : public ScaleRowSampler<uint8_t> {
    public:
        WeightedWindow4RowSampler(const WeightedRow4Operation op,
                                  const uint8_t *mSource,
                                  const int srcStride,
                                  const int inputWidth,
                                  const int inputHeight,
                                  uint8_t *mDestination,
                                  const int dstStride,
                                  const int outputWidth,
                                  const int outputHeight,
                                  const int components) :
                ScaleRowSampler<uint8_t>(mSource,
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
                    samplerHWY = CubicHermiteV;
                }
                    break;
                case WEIGHTED_ROW4_CATMULL_ROM: {
                    sampler = CatmullRom;
                    samplerHWY = CatmullRomV;
                }
                    break;
                case WEIGHTED_ROW4_BSPLINE: {
                    sampler = BSpline;
                    samplerHWY = CubicBSplineV;
                }
                    break;
                case WEIGHTED_ROW4_CUBIC: {
                    sampler = SimpleCubic;
                    samplerHWY = SimpleCubicV;
                }
                    break;
                case WEIGHTED_ROW4_BICUBIC: {
                    sampler = BiCubicSpline;
                    samplerHWY = BiCubicSplineV;
                }
                    break;
                case WEIGHTED_ROW4_MITCHELL: {
                    sampler = MitchellNetravalli;
                    samplerHWY = MitchellNetravaliV;
                }
                    break;
            }
        }

        void sample(const int y) {
            const FixedTag<float32_t, 4> dfx4;
            const FixedTag<int32_t, 4> dix4;
            const FixedTag<uint32_t, 4> dux4;
            const FixedTag<uint8_t, 4> du8x4;
            using VI4 = Vec<decltype(dix4)>;
            using VF4 = Vec<decltype(dfx4)>;
            using VU8x4 = Vec<decltype(du8x4)>;

            const VF4 vfZeros = Zero(dfx4);
            const VF4 maxColorsV = Set(dfx4, maxColors);

            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(mDestination) + y * dstStride);

            for (int x = 0; x < outputWidth; ++x) {
                float srcX = (float) x * xScale;
                float srcY = (float) y * yScale;

                if (components == 4 && x + 8 < outputWidth) {
                    // only kernel with size 3 is supported
                    constexpr int kernelSize = 2;

                    float kx1 = std::floor(srcX);
                    float ky1 = std::floor(srcY);

                    VF4 color = Set(dfx4, 0);

                    const int a = kernelSize;
                    const int mMaxWidth = inputWidth - 1;

                    const int appendixLow[4] = {-1, 0, 1, 2};

                    VF4 srcXV = Set(dfx4, srcX);
                    VI4 kx1V = Set(dix4, kx1);
                    const VI4 appendixLowV = LoadU(dix4, appendixLow);

                    for (int j = -a + 1; j <= a; j++) {
                        int yj = (int) ky1 + j;
                        float dy = float(srcY) - (float(ky1) + (float) j);
                        float yWeight = sampler(dy);
                        auto row = reinterpret_cast<const uint8_t *>(mSource + clamp(yj, 0, inputHeight - 1) * srcStride);
                        VF4 yWeightV = Set(dfx4, yWeight);
                        VI4 xi = Add(kx1V, appendixLowV);
                        VF4 dx = Sub(srcXV, ConvertTo(dfx4, xi));
                        VF4 weights = Mul(samplerHWY(dfx4, dx), yWeightV);
                        for (int i = 0; i < components; ++i) {
                            int sizeXPos = std::clamp(ExtractLane(xi, i), 0, mMaxWidth) * components;
                            VU8x4 u81 = LoadU(du8x4, reinterpret_cast<const uint8_t *>(&row[sizeXPos]));
                            VF4 fr1 = ConvertTo(dfx4, PromoteTo(dix4, u81));
                            fr1 = Mul(fr1, Set(dfx4, ExtractLane(weights, i)));
                            color = Add(color, fr1);
                        }
                    }

                    color = ClampRound(dfx4, color, vfZeros, maxColorsV);
                    VU8x4 u8Color = DemoteTo(du8x4, ConvertTo(dux4, color));
                    StoreU(u8Color, du8x4, reinterpret_cast<uint8_t *>(&dst[x * components]));
                } else {
                    float kx1 = floor(srcX);
                    float ky1 = floor(srcY);

                    int a = 2;

                    float rgb[components];
                    std::fill(rgb, rgb + components, 0.0f);

                    for (int j = -a + 1; j <= a; j++) {
                        int yj = (int) ky1 + j;
                        float dy = float(srcY) - (float(ky1) + (float) j);
                        float yWeight = sampler(dy);

                        auto row = reinterpret_cast<const uint8_t *>(mSource + std::clamp(yj, 0, inputHeight - 1) * srcStride);

                        for (int i = -a + 1; i <= a; i++) {
                            int xi = (int) kx1 + i;
                            float dx = float(srcX) - (float(kx1) + (float) i);
                            float weight = sampler(dx) * yWeight;

                            const int px = std::clamp(xi, 0, inputWidth - 1) * components;

                            for (int c = 0; c < components; ++c) {
                                auto clrf = static_cast<float>(row[px + c]);
                                float clr = clrf * weight;
                                rgb[c] += clr;
                            }
                        }
                    }

                    const int px = x * components;

                    for (int c = 0; c < components; ++c) {
                        dst[px + c] = static_cast<uint8_t>(std::clamp(std::round(rgb[c]), 0.f, maxColors));
                    }
                }
            }
        }

        ~WeightedWindow4RowSampler() {

        }

    private:
        typedef Vec<FixedTag<float32_t, 4>> (*ScaleWeightSamplerHWY)(FixedTag<float32_t, 4>, Vec<FixedTag<float32_t, 4>>);

        const float maxColors = std::powf(2.0f, (float) 8.f) - 1.0f;
        ScaleWeightSampler sampler;
        ScaleWeightSamplerHWY samplerHWY;
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
            }
        }

        void sample(const int y) {
            auto dst = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(mDestination) + y * dstStride);

            for (int x = 0; x < outputWidth; ++x) {
                float srcX = (float) x * xScale;
                float srcY = (float) y * yScale;

                float kx1 = floor(srcX);
                float ky1 = floor(srcY);

                const int a = 2;

                float rgb[4] = {0, 0, 0, 0};

                for (int j = -a + 1; j <= a; j++) {
                    int yj = (int) ky1 + j;
                    float dy = float(srcY) - (float(ky1) + (float) j);
                    float yWeight = sampler(dy);

                    auto row = reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(mSource) +
                                                                  std::clamp(yj, 0, inputHeight - 1) * srcStride);

                    for (int i = -a + 1; i <= a; i++) {
                        int xi = (int) kx1 + i;
                        float dx = float(srcX) - (float(kx1) + (float) i);
                        float weight = sampler(dx) * yWeight;

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

                auto R10 = static_cast<uint32_t >(std::clamp(std::round(rgb[0] * maxColors), 0.0f, (float) maxColors));
                auto G10 = static_cast<uint32_t >(std::clamp(std::round(rgb[1] * maxColors), 0.0f, (float) maxColors));
                auto B10 = static_cast<uint32_t >(std::clamp(std::round(rgb[2] * maxColors), 0.0f, (float) maxColors));
                auto A10 = static_cast<uint32_t >(std::clamp(std::round(rgb[3] * 3.f), 0.0f, 3.0f));

                dst[x] = (A10 << 30) | (R10 << 20) | (G10 << 10) | B10;
            }
        }

        ~WeightedWindow4RowSampler10Bit() {

        }

    private:
        typedef Vec<FixedTag<float32_t, 4>> (*ScaleWeightSamplerHWY)(FixedTag<float32_t, 4>, Vec<FixedTag<float32_t, 4>>);

        const float maxColors = std::powf(2.0f, (float) 10.f) - 1.0f;
        ScaleWeightSampler sampler;

        inline void parseToFloat(const uint32_t rgba1010102, float &r, float &g, float &b, float &a) {
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
                    samplerHWY = CubicHermiteV;
                }
                    break;
                case WEIGHTED_ROW4_CATMULL_ROM: {
                    sampler = CatmullRom;
                    samplerHWY = CatmullRomV;
                }
                    break;
                case WEIGHTED_ROW4_BSPLINE: {
                    sampler = BSpline;
                    samplerHWY = CubicBSplineV;
                }
                    break;
                case WEIGHTED_ROW4_CUBIC: {
                    sampler = SimpleCubic;
                    samplerHWY = SimpleCubicV;
                }
                    break;
                case WEIGHTED_ROW4_BICUBIC: {
                    sampler = BiCubicSpline;
                    samplerHWY = BiCubicSplineV;
                }
                    break;
                case WEIGHTED_ROW4_MITCHELL: {
                    sampler = MitchellNetravalli;
                    samplerHWY = MitchellNetravaliV;
                }
                    break;
            }
        }

        void sample(const int y) {
            const FixedTag<float32_t, 4> dfx4;
            const FixedTag<int32_t, 4> dix4;
            const FixedTag<hwy::float16_t, 4> df16x4;
            using VI4 = Vec<decltype(dix4)>;
            using VF4 = Vec<decltype(dfx4)>;
            using VF16x4 = Vec<decltype(df16x4)>;

            const VF4 xScaleV = Set(dfx4, xScale);
            const VF4 yScaleV = Set(dfx4, yScale);
            const VF4 fOneV = Set(dfx4, 1.0f);
            const VF4 vfZeros = Zero(dfx4);
            const int mMaxWidth = inputWidth - 1;

            const auto src8 = reinterpret_cast<const uint8_t *>(mSource);
            auto dst16 = reinterpret_cast<uint16_t *>(reinterpret_cast<uint8_t *>(mDestination) + y * dstStride);

            for (int x = 0; x < outputWidth; ++x) {
                float srcX = (float) x * xScale;
                float srcY = (float) y * yScale;

                if (components == 4 && x + 8 < outputWidth) {
                    const int a = 2;
                    float rgb[components];
                    fill(rgb, rgb + components, 0.0f);

                    float kx1 = std::floor(srcX);
                    float ky1 = std::floor(srcY);

                    VF4 color = Set(dfx4, 0);

                    const int appendixLow[4] = {-1, 0, 1, 2};

                    const VF4 aVector = Set(dfx4, a);
                    VF4 srcXV = Set(dfx4, srcX);
                    VI4 kx1V = Set(dix4, kx1);
                    const VI4 appendixLowV = LoadU(dix4, appendixLow);

                    for (int j = -a + 1; j <= a; j++) {
                        int yj = (int) ky1 + j;
                        float dy = float(srcY) - (float(ky1) + (float) j);
                        float yWeight = sampler(dy);
                        auto row = reinterpret_cast<const uint16_t *>(src8 + std::clamp(yj, 0, inputHeight - 1) * srcStride);
                        VF4 yWeightV = Set(dfx4, yWeight);
                        VI4 xi = Add(kx1V, appendixLowV);
                        VF4 dx = Sub(srcXV, ConvertTo(dfx4, xi));
                        VF4 weights = Mul(samplerHWY(dfx4, dx), yWeightV);

                        for (int i = 0; i < 4; ++i) {
                            int sizeXPos = clamp(ExtractLane(xi, i), 0, mMaxWidth) * components;
                            VF16x4 r1 = LoadU(df16x4, reinterpret_cast<const hwy::float16_t *>(&row[sizeXPos]));
                            VF4 fr1 = PromoteTo(dfx4, r1);
                            fr1 = Mul(fr1, Set(dfx4, ExtractLane(weights, i)));
                            color = Add(color, fr1);
                        }
                    }

                    VF16x4 f16Color = DemoteTo(df16x4, color);
                    StoreU(f16Color, df16x4, reinterpret_cast<hwy::float16_t *>(&dst16[x * components]));
                } else {
                    const int a = 2;
                    float rgb[components];
                    std::fill(rgb, rgb + components, 0.0f);

                    float kx1 = std::floor(srcX);
                    float ky1 = std::floor(srcY);

                    for (int j = -a + 1; j <= a; j++) {
                        int yj = (int) ky1 + j;
                        float dy = float(srcY) - (float(ky1) + (float) j);
                        float yWeight = sampler(dy);

                        for (int i = -a + 1; i <= a; i++) {
                            int xi = (int) kx1 + i;
                            float dx = float(srcX) - (float(kx1) + (float) i);
                            float weight = sampler(dx) * yWeight;

                            const uint16_t *row = reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(src8) +
                                                                                     std::clamp(yj, 0, inputHeight - 1) * srcStride);

                            const int px = std::clamp(xi, 0, inputWidth - 1) * components;

                            for (int c = 0; c < components; ++c) {
                                float clrf = hwy::F32FromF16(hwy::float16_t::FromBits(row[px + c]));
                                float clr = (float) clrf * weight;
                                rgb[c] += clr;
                            }
                        }
                    }

                    int px = x * components;

                    for (int c = 0; c < components; ++c) {
                        float newColor = rgb[c];
                        dst16[px + c] = hwy::F16FromF32(newColor).bits;
                    }
                }
            }
        }

        ~WeightedWindow4RowSampler16Bit() {

        }

    private:
        typedef Vec<FixedTag<float32_t, 4>> (*ScaleWeightSamplerHWY)(FixedTag<float32_t, 4>, Vec<FixedTag<float32_t, 4>>);

        ScaleWeightSampler sampler;
        ScaleWeightSamplerHWY samplerHWY;
    };

} // aire


#endif
