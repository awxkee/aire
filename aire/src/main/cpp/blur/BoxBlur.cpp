/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 31/01/24, 6:13 PM
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


#include "hwy/foreach_target.h"
#include "hwy/highway.h"
#include "algo/support-inl.h"
#include "base/Convolve1Db16.h"
#include "base/Convolve2D.h"
#include "BoxBlur.h"
#include "Eigen/Eigen"
#include <vector>
#include <algorithm>
#include <math.h>
#include <thread>
#include "base/Convolve1D.h"
#include "jni/JNIUtils.h"
#include "concurrency.hpp"

using namespace std;

namespace aire {

    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    class BoxBlur {
    public:
        BoxBlur(uint8_t *data, int stride, int width, int height, int radius) :
                data(data), stride(stride), width(width), height(height), radius(radius) {

        }

        void convolve() {
            std::vector<uint8_t> transient(stride *height);
            horizontalPass(data, transient.data());
            verticalPass(transient.data(), data);
        }

    private:
        uint8_t *data;
        const int stride;
        const int width;
        const int height;
        const int radius;

        void horizontalPass(uint8_t *source, uint8_t *destination) {
            const FixedTag<uint8_t, 4> du8;
            const FixedTag<uint32_t, 4> du32x4;
            const FixedTag<float32_t, 4> dfx4;
            using VF = Vec<decltype(dfx4)>;
            using VU = Vec<decltype(du8)>;
            const auto max255 = Set(dfx4, 255.0f);
            const VF zeros = Zero(dfx4);

            const int halfOfKernel = radius / 2;
            const bool isEven = radius % 2 == 0;
            const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

            const VF mKernelScale = Set(dfx4, 1.f / radius);

            const int threadCount = std::clamp(std::min(static_cast<int>(std::thread::hardware_concurrency()),
                                                        width * height / (256 * 256)), 1, 12);

            concurrency::parallel_for(threadCount, height, [&](int y) {
                VF store = zeros;

                auto dst = reinterpret_cast<uint8_t *>(destination + y * stride);

                for (int j = -halfOfKernel; j <= maxKernel; ++j) {
                    auto src = reinterpret_cast<uint8_t *>(source + y * stride);
                    int pos = std::clamp(j, 0, width - 1) * 4;
                    VU pixels = LoadU(du8, &src[pos]);
                    store = Add(store, ConvertTo(dfx4, PromoteTo(du32x4, pixels)));
                }

                auto src = reinterpret_cast<uint8_t *>(source + y * stride);

                for (int x = 0; x < width; ++x) {
                    int pos = std::clamp(x - halfOfKernel, 0, width - 1) * 4;
                    VU pixels = LoadU(du8, &src[pos]);
                    store = Sub(store, ConvertTo(dfx4, PromoteTo(du32x4, pixels)));
                    pos = std::clamp(x + halfOfKernel, 0, width - 1) * 4;
                    pixels = LoadU(du8, &src[pos]);
                    store = Add(store, ConvertTo(dfx4, PromoteTo(du32x4, pixels)));
                    VF mPixel = Max(Min(Round(Mul(store, mKernelScale)), max255), zeros);
                    VU pixelU = DemoteTo(du8, ConvertTo(du32x4, mPixel));

                    StoreU(pixelU, du8, dst);

                    dst += 4;
                }
            });

        }

        void verticalPass(uint8_t *source, uint8_t *destination) {
            const FixedTag<uint8_t, 4> du8;
            const FixedTag<uint32_t, 4> du32x4;
            const FixedTag<float32_t, 4> dfx4;
            using VF = Vec<decltype(dfx4)>;
            using VU = Vec<decltype(du8)>;
            const auto max255 = Set(dfx4, 255.0f);
            const VF zeros = Zero(dfx4);

            const int halfOfKernel = radius / 2;
            const bool isEven = radius % 2 == 0;
            const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

            const VF mKernelScale = Set(dfx4, 1.f / radius);

            const int threadCount = std::clamp(std::min(static_cast<int>(std::thread::hardware_concurrency()),
                                                        width * height / (256 * 256)), 1, 12);
            concurrency::parallel_for(threadCount, width, [&](int x) {
                VF store = zeros;

                for (int j = -halfOfKernel; j <= maxKernel; ++j) {
                    auto src = reinterpret_cast<uint8_t *>(source + std::clamp(j, 0, height - 1) * stride);
                    int pos = x*4;
                    VU pixels = LoadU(du8, &src[pos]);
                    store = Add(store, ConvertTo(dfx4, PromoteTo(du32x4, pixels)));
                }

                for (int y = 0; y < height; ++y) {
                    auto dst = reinterpret_cast<uint8_t *>(destination + y * stride);

                    auto oldSrc = reinterpret_cast<uint8_t *>(source + std::clamp(y - halfOfKernel, 0, height - 1) * stride);

                    int pos = x * 4;
                    VU pixels = LoadU(du8, &oldSrc[pos]);
                    store = Sub(store, ConvertTo(dfx4, PromoteTo(du32x4, pixels)));

                    auto newSrc = reinterpret_cast<uint8_t *>(source + std::clamp(y + halfOfKernel, 0, height - 1) * stride);

                    pixels = LoadU(du8, &newSrc[pos]);
                    store = Add(store, ConvertTo(dfx4, PromoteTo(du32x4, pixels)));
                    VF mPixel = Max(Min(Round(Mul(store, mKernelScale)), max255), zeros);
                    VU pixelU = DemoteTo(du8, ConvertTo(du32x4, mPixel));

                    StoreU(pixelU, du8, &dst[pos]);
                }
            });
        }
    };

    void boxBlurU8(uint8_t *data, int stride, int width, int height, int radius) {
        BoxBlur boxBlur(data, stride, width, height, radius);
        boxBlur.convolve();
    }

    void boxBlurF16(uint16_t *data, int stride, int width, int height, int radius) {
        const auto kernel = generateBoxKernel(radius);
        Convolve1Db16 convolution(kernel, kernel);
        convolution.convolve(data, stride, width, height);
    }

    std::vector<float> generateBoxKernel(int size) {
        if (size < 0) {
            std::string err = "Radius must be a non-negative integer but received " + std::to_string(size);
            throw AireError(err);
        }
        const int kernelSize = size;
        std::vector<float> boxKernel(kernelSize, 1.0 / float(kernelSize));
        return std::move(boxKernel);
    }

    Eigen::MatrixXf generateBoxKernel2D(const int size) {
        if (size <= 0) {
            std::string err = "Radius must be a non-negative integer but received " + std::to_string(size);
            throw AireError(err);
        }
        const int kernelSize = size;
        Eigen::MatrixXf kernel = Eigen::MatrixXf::Constant(kernelSize, kernelSize, 1.0f / static_cast<float>(kernelSize));
        return kernel;
    }

}