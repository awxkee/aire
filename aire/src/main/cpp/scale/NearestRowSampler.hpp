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

#if defined(AIRE_NEAREST_ROW_SAMPLER) == defined(HWY_TARGET_TOGGLE)
#ifdef AIRE_NEAREST_ROW_SAMPLER
#undef AIRE_NEAREST_ROW_SAMPLER
#else
#define AIRE_NEAREST_ROW_SAMPLER
#endif

#include <hwy/highway.h>
#include "ScaleRowSampler.hpp"
#include <cstdint>
#include <algorithm>

namespace aire::HWY_NAMESPACE {

    class NearestRowSampler : public ScaleRowSampler<uint8_t> {
    public:
        NearestRowSampler(const uint8_t *mSource,
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

        }

        void sample(const int row) {
            auto dst = reinterpret_cast<uint8_t *>(mDestination + row * dstStride);
            if (components == 4) {
                for (int x = 0; x < outputWidth; ++x) {
                    const float srcX = (float) x * xScale;
                    const float srcY = (float) row * yScale;

                    const int x1 = std::clamp(static_cast<int>(std::floor(srcX)), 0, inputWidth - 1);
                    const int y1 = std::clamp(static_cast<int>(std::floor(srcY)), 0, inputHeight - 1);
                    auto srcRow = reinterpret_cast<const uint8_t *>(mSource + y1 * srcStride);
                    reinterpret_cast<uint32_t *>(dst)[x] = reinterpret_cast<const uint32_t *>(srcRow)[x1];
                }
            } else {
                for (int x = 0; x < outputWidth; ++x) {
                    const float srcX = (float) x * xScale;
                    const float srcY = (float) row * yScale;

                    const int x1 = std::clamp(static_cast<int>(std::floor(srcX)), 0, inputWidth - 1);
                    const int y1 = std::clamp(static_cast<int>(std::floor(srcY)), 0, inputHeight - 1);
                    auto srcRow = reinterpret_cast<const uint8_t *>(mSource + y1 * srcStride);
                    auto srcPtr = &srcRow[x1 * components];
                    std::copy(srcPtr, srcPtr + sizeof(uint8_t) * components, &dst[x * components]);
                }
            }
        }

        ~NearestRowSampler() {

        }

    private:
        const float maxColors = std::powf(2.0f, (float) 8.f) - 1.0f;
    };

    class NearestRowSampler16Bit : public ScaleRowSampler<uint16_t> {
    public:
        NearestRowSampler16Bit(const uint16_t *mSource,
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

        }

        void sample(const int row) {
            auto dst = reinterpret_cast<uint16_t *>(reinterpret_cast<uint8_t *>(mDestination) + row * dstStride);
            for (int x = 0; x < outputWidth; ++x) {
                const float srcX = (float) x * xScale;
                const float srcY = (float) row * yScale;

                const int x1 = std::clamp(static_cast<int>(std::floor(srcX)), 0, inputWidth - 1);
                const int y1 = std::clamp(static_cast<int>(std::floor(srcY)), 0, inputHeight - 1);
                auto srcRow = reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(mSource) + y1 * srcStride);
                auto srcPtr = &srcRow[x1 * components];
                std::copy(srcPtr, srcPtr + sizeof(uint8_t) * components, &dst[x * components]);
            }
        }

        ~NearestRowSampler16Bit() {

        }

    };

    class NearestRowSampler10Bit : public ScaleRowSampler<uint32_t> {
    public:
        NearestRowSampler10Bit(const uint32_t *mSource,
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

        }

        void sample(const int row) {
            auto dst = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(mDestination) + row * dstStride);
            for (int x = 0; x < outputWidth; ++x) {
                const float srcX = (float) x * xScale;
                const float srcY = (float) row * yScale;

                const int x1 = std::clamp(static_cast<int>(std::floor(srcX)), 0, inputWidth - 1);
                const int y1 = std::clamp(static_cast<int>(std::floor(srcY)), 0, inputHeight - 1);
                auto srcRow = reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(mSource) + y1 * srcStride);
                dst[x] = srcRow[x1];
            }
        }

        ~NearestRowSampler10Bit() {

        }

    private:
    };

} // aire

#endif //AIREDESKTOP_NEARESTROWSAMPLER_HPP
