//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "TentBlur.h"
#include <vector>
#include "jni/JNIUtils.h"
#include <thread>
#include <algorithm>

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "blur/TentBlur.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"

using namespace std;

HWY_BEFORE_NAMESPACE();
namespace aire::HWY_NAMESPACE {

    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    template<class V>
    void
    tentBlurPass(V *data, V *transient, int y, int stride, int width, int height, int radius,
                 std::vector<std::vector<float>> &kernel, float scale) {

        const FixedTag<uint8_t, 4> du8;
        const FixedTag<float32_t, 4> df32;
        const FixedTag<uint32_t, 4> du32;
        using VU = Vec<decltype(du8)>;
        using VF = Vec<decltype(df32)>;

        auto dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient) +
                                         y * stride);

        const VF vScale = Set(df32, scale);
        const VF revertScale = ApproximateReciprocal(vScale);

        int halfKernel = kernel.size() / 2;

        const VF zeros = Zero(df32);

        for (int x = 0; x < width; ++x) {
            VF store = zeros;

            for (int j = -halfKernel; j <= halfKernel; ++j) {
                int py = clamp(y + j, 0, height - 1);

                for (int i = -halfKernel; i <= halfKernel; ++i) {
                    int px = clamp(x + i, 0, width - 1);
                    auto mSrc = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data) +
                                                      py * stride);

                    int srcX = px * 4;

                    VU vuPixels = LoadU(du8, &mSrc[srcX]);

                    float weight = kernel[j + halfKernel][i + halfKernel];
                    store = Add(store, Mul(Mul(ConvertTo(df32, PromoteTo(du32, vuPixels)), vScale),
                                           Set(df32, weight)));
                }
            }

            store = Mul(store, revertScale);
            VU pixels = DemoteTo(du8, ConvertTo(du32, store));
            StoreU(pixels, du8, dst);
            dst += 4;
        }
    }

    template<class V>
    void
    tentBlurPass1D(V *data, V *transient, int y, int stride, int width, int height, int radius,
                   std::vector<float> &kernel, const float scale, const bool vertical) {

        const FixedTag<uint8_t, 4> du8;
        const FixedTag<float32_t, 4> df32;
        const FixedTag<uint32_t, 4> du32;
        using VU = Vec<decltype(du8)>;
        using VF = Vec<decltype(df32)>;

        auto dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient) +
                                         y * stride);

        const VF vScale = Set(df32, scale);
        const VF revertScale = ApproximateReciprocal(vScale);

        int halfKernel = kernel.size() / 2;

        for (int x = 0; x < width; ++x) {
            VF store = Zero(df32);

            for (int i = -halfKernel; i <= halfKernel; ++i) {
                int py;
                if (vertical) {
                    py = clamp(y + i, 0, height - 1);
                } else {
                    py = y;
                }
                int px;
                if (!vertical) {
                    px = clamp(x + i, 0, width - 1);
                } else {
                    px = x;
                }
                auto mSrc = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data) +
                                                  py * stride);

                int srcX = px * 4;

                VU vuPixels = LoadU(du8, &mSrc[srcX]);

                float weight = kernel[i + halfKernel];
                store = Add(store, Mul(Mul(ConvertTo(df32, PromoteTo(du32, vuPixels)), vScale),
                                       Set(df32, weight)));
            }

            store = Mul(store, revertScale);
            VU pixels = DemoteTo(du8, ConvertTo(du32, store));
            StoreU(pixels, du8, dst);
            dst += 4;
        }
    }

    void tentBlurHWY(uint8_t *data, int stride, int width, int height, int radius, float scale) {
        int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    height * width / (256 * 256)), 1, 12);
        vector<thread> workers;

        int segmentHeight = height / threadCount;
        auto kernel = generateTentFilterNormalized(radius);

        std::vector<uint8_t> transient(stride * height);

        for (int i = 0; i < threadCount; i++) {
            int start = i * segmentHeight;
            int end = (i + 1) * segmentHeight;
            if (i == threadCount - 1) {
                end = height;
            }
            workers.emplace_back(
                    [start, end, width, height, stride, data, radius, &transient, &kernel, scale]() {
                        for (int y = start; y < end; ++y) {
                            tentBlurPass(data, transient.data(), y,
                                           stride, width, height, radius, kernel, scale);
                        }
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }

        workers.clear();

        std::copy(transient.begin(), transient.end(), data);
    }

    template
    void
    tentBlurPass(uint8_t *data, uint8_t *transient, int y, int stride, int width, int height,
                 int radius,
                 std::vector<std::vector<float>> &kernel, float scale);

    template
    void
    tentBlurPass1D(uint8_t *data, uint8_t *transient, int y, int stride, int width, int height,
                   int radius,
                   std::vector<float> &kernel, const float scale, const bool vertical);
}

HWY_AFTER_NAMESPACE();

#if HWY_ONCE

namespace aire {

    std::vector<std::vector<float>> generateTentFilterNormalized(int N) {
        auto kernel = generateTentFilter(N);
        float kernelSum = 0.f;
        for (const auto &row: kernel) {
            for (float value: row) {
                kernelSum += value;
            }
        }
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                kernel[i][j] /= kernelSum;
            }
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
        for (int i = 0; i < size; i++) {
            kernel[i] /= sum;
        }
        return kernel;
    }

    std::vector<std::vector<float>> generateTentFilter(int N) {
        if (N % 2 == 0 || N < 1) {
            throw AireError(
                    "Invalid filter size. Please use an odd positive integer for N, but received: " +
                    std::to_string(N));
        }

        std::vector<std::vector<float>> tentFilter(N, std::vector<float>(N, 0.0f));
        float peakValue = 1.0f / ((N / 2) + 1);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                float distanceToCenter = std::min({i, j, N - 1 - i, N - 1 - j});
                tentFilter[i][j] = peakValue * (1.0f - (distanceToCenter / (N / 2.0f)));
            }
        }

        return tentFilter;
    }

    HWY_EXPORT(tentBlurHWY);

    void tentBlur(uint8_t *data, int stride, int width, int height, int radius) {
        HWY_DYNAMIC_DISPATCH(tentBlurHWY)(data, stride, width, height, radius,
                                          float(1.f / (pow(2.0f, 8.0f) - 1)));
    }
}

#endif