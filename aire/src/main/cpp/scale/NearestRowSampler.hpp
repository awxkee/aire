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

    template<class T>
    class NearestRowSampler : public ScaleRowSampler<T> {
    public:
        NearestRowSampler(const T *mSource,
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

        }

        void sample(const int row) override {
            auto dst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(this->mDestination) + row * this->dstStride);
            if (this->components == 4 && std::is_same<T, uint8_t>::value) {
                for (int x = 0; x < this->outputWidth; ++x) {
                    const float srcX = (float) x * this->xScale;
                    const float srcY = (float) row * this->yScale;

                    const int x1 = std::clamp(static_cast<int>(std::floor(srcX)), 0, this->inputWidth - 1);
                    const int y1 = std::clamp(static_cast<int>(std::floor(srcY)), 0, this->inputHeight - 1);
                    auto srcRow = reinterpret_cast<const T *>(reinterpret_cast<const uint8_t *>(this->mSource) + y1 * this->srcStride);
                    reinterpret_cast<uint32_t *>(dst)[x] = reinterpret_cast<const uint32_t *>(srcRow)[x1];
                }
            } else {
                for (int x = 0; x < this->outputWidth; ++x) {
                    const float srcX = (float) x * this->xScale;
                    const float srcY = (float) row * this->yScale;

                    const int x1 = std::clamp(static_cast<int>(std::floor(srcX)), 0, this->inputWidth - 1);
                    const int y1 = std::clamp(static_cast<int>(std::floor(srcY)), 0, this->inputHeight - 1);
                    auto srcRow = reinterpret_cast<const T *>(reinterpret_cast<const uint8_t *>(this->mSource) + y1 * this->srcStride);
                    auto srcPtr = &srcRow[x1 * this->components];
                    std::copy(srcPtr, srcPtr + sizeof(uint8_t) * this->components, &dst[x * this->components]);
                }
            }
        }

        ~NearestRowSampler() override = default;

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

        void sample(const int row) override {
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

        ~NearestRowSampler16Bit() override = default;
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

        void sample(const int row) override {
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

        ~NearestRowSampler10Bit() override = default;

    private:
    };

} // aire

#endif //AIREDESKTOP_NEARESTROWSAMPLER_HPP
