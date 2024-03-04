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

#if defined(AIRE_BILINEAR_ROW_SAMPLER) == defined(HWY_TARGET_TOGGLE)
#ifdef AIRE_BILINEAR_ROW_SAMPLER
#undef AIRE_BILINEAR_ROW_SAMPLER
#else
#define AIRE_BILINEAR_ROW_SAMPLER
#endif

#include <hwy/highway.h>
#include "ScaleRowSampler.hpp"
#include <cstdint>
#include <algorithm>

HWY_BEFORE_NAMESPACE();
namespace aire::HWY_NAMESPACE {

    class BilinearRowSampler4Chan8Bit : public ScaleRowSampler<uint8_t> {
    public:
        BilinearRowSampler4Chan8Bit(const uint8_t *mSource,
                                    const int srcStride,
                                    const int inputWidth,
                                    const int inputHeight,
                                    uint8_t *mDestination,
                                    const int dstStride,
                                    const int outputWidth,
                                    const int outputHeight) :
                ScaleRowSampler<uint8_t>(mSource,
                                         srcStride,
                                         inputWidth,
                                         inputHeight,
                                         mDestination,
                                         dstStride,
                                         outputWidth,
                                         outputHeight,
                                         4) {

        }

        ~BilinearRowSampler4Chan8Bit() {

        }

        void sample(const int row);

    private:
        const float maxColors = std::powf(2.0f, (float) 8.f) - 1.0f;
    };

    class BilinearRowSampler16Bit : public ScaleRowSampler<uint16_t> {
    public:
        BilinearRowSampler16Bit(const uint16_t *mSource,
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

        ~BilinearRowSampler16Bit() {

        }

        void sample(const int row);

    private:
        const float maxColors = std::powf(2.0f, (float) 8.f) - 1.0f;
    };

    class BilinearRowSampler8Bit : public ScaleRowSampler<uint8_t> {
    public:
        BilinearRowSampler8Bit(const uint8_t *mSource,
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

        ~BilinearRowSampler8Bit() {

        }

        void sample(const int row);

    private:
        const float maxColors = std::powf(2.0f, (float) 8.f) - 1.0f;
    };

    class BilinearRowSampler10Bit : public ScaleRowSampler<uint32_t> {
    public:
        BilinearRowSampler10Bit(const uint32_t *mSource,
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

        ~BilinearRowSampler10Bit() {

        }

        void sample(const int row);

    private:
        const float maxColors = std::powf(2.0f, (float) 10.f) - 1.0f;

        inline void parseToFloat(const uint32_t rgba1010102, float &r, float& g, float& b, float &a) {
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

} // aire
HWY_AFTER_NAMESPACE();

#endif //AIRE_BILINEARROWSAMPLER_HPP
