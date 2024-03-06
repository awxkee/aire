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
#include "base/Convolve1D.h"
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

    template<class D>
    class BoxBlur {
    public:
        BoxBlur(D d, TFromD<D> *data, int stride, int width, int height, int radius) :
                d(d), data(data), stride(stride), width(width), height(height), radius(radius) {

        }

        void convolve() {
            std::vector<uint8_t> transient(stride *height);
            horizontalPass(reinterpret_cast<TFromD<D> *>(data), reinterpret_cast<TFromD<D> *>(transient.data()));
            verticalPass(reinterpret_cast<TFromD<D> *>(transient.data()), reinterpret_cast<TFromD<D> *>(data));
        }

    private:
        const D d;
        TFromD<D> *data;
        const int stride;
        const int width;
        const int height;
        const int radius;

        void horizontalPass(TFromD<D> *source, TFromD<D> *destination) {
            const Rebind<float32_t, decltype(d)> dfx4;
            using VF = Vec<decltype(dfx4)>;
            using VU = VFromD<decltype(d)>;
            const auto max255 = Set(dfx4, 255.0f);
            const VF zeros = Zero(dfx4);

            const int halfOfKernel = radius / 2;
            const bool isEven = radius % 2 == 0;
            const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

            const VF mKernelScale = Set(dfx4, 1.f / static_cast<float>(radius));

            const int lanes = Lanes(d);

            const int threadCount = std::clamp(std::min(static_cast<int>(std::thread::hardware_concurrency()),
                                                        width * height / (256 * 256)), 1, 12);
            concurrency::parallel_for(threadCount, height, [&](int y) {

                auto dst = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(destination) + y * stride);
                auto src = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(source) + y * stride);

                VF store = Mul(PromoteTo(dfx4, LoadU(d, &src[0])),
                               Set(dfx4, static_cast<float>(halfOfKernel + 1)));

                for (int j = 1; j <= maxKernel; ++j) {
                    int pos = std::clamp(j, 0, width - 1) * 4;
                    VU pixels = LoadU(d, &src[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                }

                for (int x = 0; x < width; ++x) {
                    int pos = std::clamp(x - halfOfKernel, 0, width - 1) * 4;
                    VU pixels = LoadU(d, &src[pos]);
                    store = Sub(store, PromoteTo(dfx4, pixels));
                    pos = std::clamp(x + maxKernel + 1, 0, width - 1) * 4;
                    pixels = LoadU(d, &src[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                    VF mPixel;
                    if (std::is_same<TFromD<decltype(d)>, uint8_t>::value) {
                        mPixel = Max(Min(Round(Mul(store, mKernelScale)), max255), zeros);
                    } else {
                        mPixel = Mul(store, mKernelScale);
                    }
                    VU pixelU = DemoteTo(d, mPixel);

                    StoreU(pixelU, d, dst);
                    dst += lanes;
                }
            });
        }

        void verticalPass(TFromD<decltype(d)> *source, TFromD<decltype(d)> *destination) {
            const Rebind<float32_t, decltype(d)> dfx4;
            using VF = Vec<decltype(dfx4)>;
            using VU = Vec<decltype(d)>;
            const auto max255 = Set(dfx4, 255.0f);
            const VF zeros = Zero(dfx4);

            const int halfOfKernel = radius / 2;
            const bool isEven = radius % 2 == 0;
            const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

            const VF mKernelScale = Set(dfx4, 1.f / static_cast<float>(radius));

            const int threadCount = std::clamp(std::min(static_cast<int>(std::thread::hardware_concurrency()),
                                                        width * height / (256 * 256)), 1, 12);
            concurrency::parallel_for(threadCount, width, [&](int x) {

                int pos = x * 4;

                auto src1 = reinterpret_cast<TFromD<decltype(d)> *>(source);
                VF store = Mul(PromoteTo(dfx4, LoadU(d, &src1[pos])), Set(dfx4, static_cast<float>(halfOfKernel + 1)));

                for (int j = 1; j <= maxKernel; ++j) {
                    auto src = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(source) + std::clamp(j, 0, height - 1) * stride);
                    VU pixels = LoadU(d, &src[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                }

                for (int y = 0; y < height; ++y) {
                    auto dst = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(destination) + y * stride);

                    auto oldSrc = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(source) +
                                                                          std::clamp(y - halfOfKernel, 0, height - 1) * stride);

                    VU pixels = LoadU(d, &oldSrc[pos]);
                    store = Sub(store, PromoteTo(dfx4, pixels));

                    auto newSrc = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(source) +
                                                                          std::clamp(y + maxKernel + 1, 0, height - 1) * stride);

                    pixels = LoadU(d, &newSrc[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                    VF mPixel;
                    if (std::is_same<TFromD<decltype(d)>, uint8_t>::value) {
                        mPixel = Max(Min(Round(Mul(store, mKernelScale)), max255), zeros);
                    } else {
                        mPixel = Mul(store, mKernelScale);
                    }
                    VU pixelU = DemoteTo(d, mPixel);

                    StoreU(pixelU, d, &dst[pos]);
                }
            });
        }
    };

    template<class D>
    class TentBlur {
    public:
        TentBlur(D d, TFromD<D> *data, int stride, int width, int height, int radius) :
                d(d), data(data), stride(stride), width(width), height(height), radius(radius) {

        }

        void convolve() {
            std::vector<uint8_t> transient(stride *height);
            horizontalPass(reinterpret_cast<TFromD<D> *>(data), reinterpret_cast<TFromD<D> *>(transient.data()));
            verticalPass(reinterpret_cast<TFromD<D> *>(data), reinterpret_cast<TFromD<D> *>(transient.data()));
        }

    private:
        const D d;
        TFromD<D> *data;
        const int stride;
        const int width;
        const int height;
        const int radius;

        void horizontalPass(TFromD<D> *source, TFromD<D> *transient) {
            const Rebind<float32_t, decltype(d)> dfx4;
            using VF = Vec<decltype(dfx4)>;
            using VU = VFromD<decltype(d)>;
            const auto max255 = Set(dfx4, 255.0f);
            const VF zeros = Zero(dfx4);

            const int halfOfKernel = radius / 2;
            const bool isEven = radius % 2 == 0;
            const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

            const VF mKernelScale = Set(dfx4, 1.f / static_cast<float>(radius));

            const size_t lanes = Lanes(d);

            const int threadCount = std::clamp(std::min(static_cast<int>(std::thread::hardware_concurrency()),
                                                        width * height / (256 * 256)), 1, 12);
            concurrency::parallel_for(threadCount, height, [&](int y) {

                auto dst = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(transient) + y * stride);
                auto src = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(source) + y * stride);

                VF store = Mul(PromoteTo(dfx4,LoadU(d, &src[0])),
                               Set(dfx4, static_cast<float>(halfOfKernel + 1)));

                for (int j = 1; j <= maxKernel; ++j) {
                    int pos = std::clamp(j, 0, width - 1) * 4;
                    VU pixels = LoadU(d, &src[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                }

                for (int x = 0; x < width; ++x) {
                    int pos = std::clamp(x - halfOfKernel, 0, width - 1) * 4;
                    VU pixels = LoadU(d, &src[pos]);
                    store = Sub(store, PromoteTo(dfx4, pixels));
                    pos = std::clamp(x + maxKernel + 1, 0, width - 1) * 4;
                    pixels = LoadU(d, &src[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                    VF mPixel;
                    if (std::is_same<TFromD<decltype(d)>, uint8_t>::value) {
                        mPixel = Max(Min(Round(Mul(store, mKernelScale)), max255), zeros);
                    } else {
                        mPixel = Mul(store, mKernelScale);
                    }
                    VU pixelU = DemoteTo(d, mPixel);

                    StoreU(pixelU, d, dst);
                    dst += lanes;
                }

                dst = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(source) + y * stride);
                src = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(transient) + y * stride);

                dst = &dst[(width - 1) * 4];

                store = Mul(PromoteTo(dfx4, LoadU(d, &src[(width - 1) * 4])),
                            Set(dfx4, static_cast<float>(halfOfKernel + 1)));

                for (int j = 1; j <= maxKernel; ++j) {
                    int pos = std::clamp(width - 1 - j, 0, width - 1) * 4;
                    VU pixels = LoadU(d, &src[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                }

                for (int x = width - 1; x >= 0; --x) {
                    int pos = std::clamp(x + halfOfKernel, 0, width - 1) * 4;
                    VU pixels = LoadU(d, &src[pos]);
                    store = Sub(store, PromoteTo(dfx4, pixels));
                    pos = std::clamp(x - maxKernel - 1, 0, width - 1) * 4;
                    pixels = LoadU(d, &src[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                    VF mPixel;
                    if (std::is_same<TFromD<decltype(d)>, uint8_t>::value) {
                        mPixel = Max(Min(Round(Mul(store, mKernelScale)), max255), zeros);
                    } else {
                        mPixel = Mul(store, mKernelScale);
                    }
                    VU pixelU = DemoteTo(d, mPixel);

                    StoreU(pixelU, d, dst);
                    dst -= lanes;
                }
            });
        }

        void verticalPass(TFromD<D> *source, TFromD<D> *transient) {
            const Rebind<float32_t, decltype(d)> dfx4;
            using VF = Vec<decltype(dfx4)>;
            using VU = Vec<decltype(d)>;
            const auto max255 = Set(dfx4, 255.0f);
            const VF zeros = Zero(dfx4);

            const int halfOfKernel = radius / 2;
            const bool isEven = radius % 2 == 0;
            const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

            const VF mKernelScale = Set(dfx4, 1.f / static_cast<float>(radius));

            const int threadCount = std::clamp(std::min(static_cast<int>(std::thread::hardware_concurrency()),
                                                        width * height / (256 * 256)), 1, 12);
            concurrency::parallel_for(threadCount, width, [&](int x) {

                int pos = x * 4;

                auto src1 = reinterpret_cast<TFromD<decltype(d)> *>(source);
                VF store = Mul(PromoteTo(dfx4, LoadU(d, &src1[pos])), Set(dfx4, static_cast<float>(halfOfKernel + 1)));

                for (int j = 1; j <= maxKernel; ++j) {
                    auto src = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(source) + std::clamp(j, 0, height - 1) * stride);
                    auto pixels = LoadU(d, &src[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                }

                for (int y = 0; y < height; ++y) {
                    auto dst = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(transient) + y * stride);

                    auto oldSrc = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(source) +
                                                              std::clamp(y - halfOfKernel, 0, height - 1) * stride);

                    VU pixels = LoadU(d, &oldSrc[pos]);
                    store = Sub(store, PromoteTo(dfx4, pixels));

                    auto newSrc = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(source) +
                                                              std::clamp(y + maxKernel + 1, 0, height - 1) * stride);

                    pixels = LoadU(d, &newSrc[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                    VF mPixel;
                    if (std::is_same<TFromD<decltype(d)>, uint8_t>::value) {
                        mPixel = Max(Min(Round(Mul(store, mKernelScale)), max255), zeros);
                    } else {
                        mPixel = Mul(store, mKernelScale);
                    }
                    VU pixelU = DemoteTo(d, mPixel);

                    StoreU(pixelU, d, &dst[pos]);
                }

                auto srcPtr = transient;
                auto transientPtr = source;

                src1 = reinterpret_cast<TFromD<decltype(d)> *>(srcPtr);
                store = Mul(PromoteTo(dfx4, LoadU(d, &src1[pos])), Set(dfx4, static_cast<float>(halfOfKernel + 1)));

                for (int j = 1; j <= maxKernel; ++j) {
                    auto src = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(srcPtr) + std::clamp(j, 0, height - 1) * stride);
                    VU pixels = LoadU(d, &src[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                }

                for (int y = 0; y < height; ++y) {
                    auto dst = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(transientPtr) + y * stride);

                    auto oldSrc = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(srcPtr) +
                                                                          std::clamp(y - halfOfKernel, 0, height - 1) * stride);

                    VU pixels = LoadU(d, &oldSrc[pos]);
                    store = Sub(store, PromoteTo(dfx4, pixels));

                    auto newSrc = reinterpret_cast<TFromD<decltype(d)> *>(reinterpret_cast<uint8_t *>(srcPtr) +
                                                                          std::clamp(y + maxKernel + 1, 0, height - 1) * stride);

                    pixels = LoadU(d, &newSrc[pos]);
                    store = Add(store, PromoteTo(dfx4, pixels));
                    VF mPixel;
                    if (std::is_same<TFromD<decltype(d)>, uint8_t>::value) {
                        mPixel = Max(Min(Round(Mul(store, mKernelScale)), max255), zeros);
                    } else {
                        mPixel = Mul(store, mKernelScale);
                    }
                    VU pixelU = DemoteTo(d, mPixel);

                    StoreU(pixelU, d, &dst[pos]);
                }

            });
        }
    };

    void boxBlurU8(uint8_t *data, int stride, int width, int height, int radius) {
        const FixedTag<uint8_t, 4> du8;
        BoxBlur boxBlur(du8, data, stride, width, height, radius);
        boxBlur.convolve();
    }

    void boxBlurF16(uint16_t *data, int stride, int width, int height, int radius) {
        const FixedTag<hwy::float16_t, 4> df16x4;
        BoxBlur boxBlur(df16x4, reinterpret_cast<hwy::float16_t *>(data), stride, width, height, radius);
        boxBlur.convolve();
    }

    std::vector<float> generateBoxKernel(int size) {
        if (size < 0) {
            std::string err = "Radius must be a non-negative integer but received " + std::to_string(size);
            throw AireError(err);
        }
        const int kernelSize = size;
        std::vector<float> boxKernel(kernelSize, 1.0 / float(kernelSize));
        return boxKernel;
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


    Eigen::MatrixXf generateTentFilterNormalized(int N) {
        auto kernel = generateTentFilter(N);
        float kernelSum = kernel.sum();
        if (kernelSum != 0.f) {
            kernel /= kernelSum;
        }
        return kernel;
    }

    std::vector<float> generate1DTentFilterKernelNormalized(int size) {
        if (size % 2 == 0 || size < 1) {
            throw AireError(
                    "Invalid filter size. Please use an odd positive integer for N, but received: " +
                    std::to_string(size));
        }

        std::vector<float> kernel(size, 0.0f);
        int center = size / 2;
        for (int i = 0; i < size; i++) {
            kernel[i] = 1.0f - std::abs(i - center) / static_cast<float>(center);
            kernel[i] = std::max(0.0f, kernel[i]);
        }
        float sum = 0.0f;
        for (int i = 0; i < size; i++) {
            sum += kernel[i];
        }
        if (sum != 0.f) {
            for (int i = 0; i < size; i++) {
                kernel[i] /= sum;
            }
        }
        return kernel;
    }

    Eigen::MatrixXf generateTentFilter(int N) {
        int padding = N > 4 ? N * 0.2 : 0;
        int newSize = (N - padding) / 2;
        float maxDistance = newSize;

        Eigen::MatrixXf tentFilter(N, N);

        float peakValue = static_cast<float>(N);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                int dx = j - N / 2;
                int dy = i - N / 2;
                float distance = std::sqrt(dx * dx + dy * dy);
                if (distance > maxDistance) {
                    tentFilter(i, j) = 0;
                } else {
                    float distanceToCenter = std::min({i, j, N - 1 - i, N - 1 - j});
                    tentFilter(i, j) = peakValue * distanceToCenter / (N / 2.0f);
                }
            }
        }

        const float sum = tentFilter.sum();

        if (sum != 0.f) {
            tentFilter /= sum;
        }

        return tentFilter;
    }

    void tentBlur(uint8_t *data, int stride, int width, int height, const int size) {
        const FixedTag<uint8_t, 4> du8;
        TentBlur tentBlur(du8, data, stride, width, height, size);
        tentBlur.convolve();
    }

    void tentBlurF16(uint16_t *data, int stride, int width, int height, const int size) {
        const FixedTag<hwy::float16_t, 4> df16x4;
        TentBlur tentBlur(df16x4, reinterpret_cast<hwy::float16_t*>(data), stride, width, height, size);
        tentBlur.convolve();
    }

}