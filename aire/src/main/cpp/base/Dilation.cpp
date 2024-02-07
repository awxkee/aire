//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Dilation.h"
#include <vector>
#include <algorithm>
#include <thread>
#include "hwy/highway.h"
#include "algo/support-inl.h"

using namespace std;

namespace aire {

    using namespace hwy;
    using namespace std;
    using namespace hwy::HWY_NAMESPACE;

    template<class T>
    void dilateRGBA(T *pixels, T *destination, int stride, int width, int height,
                    std::vector<std::vector<int>> &kernel) {

        int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    height * width / (256 * 256)), 1, 12);
        vector<thread> workers;

        int segmentHeight = height / threadCount;

        for (int i = 0; i < threadCount; i++) {
            int start = i * segmentHeight;
            int end = (i + 1) * segmentHeight;
            if (i == threadCount - 1) {
                end = height;
            }
            workers.emplace_back(
                    [start, end, width, height, pixels, destination, &kernel, stride]() {
                        const FixedTag<uint8_t, 16> du8x16;
                        const FixedTag<uint8_t, 8> du8x8;
                        const FixedTag<uint8_t, 4> du8x4;
                        const FixedTag<int16_t, 8> di16x8;
                        const FixedTag<int16_t, 4> di16x4;
                        const FixedTag<int32_t, 4> di32x4;
                        const FixedTag<int8_t, 4> di8x4;
                        using VU8x16 = Vec<decltype(du8x16)>;
                        using VU8x4 = Vec<decltype(du8x4)>;

                        for (int y = start; y < end; ++y) {
                            for (int x = 0; x < width; ++x) {
                                int mSize = kernel.size() / 2;

                                auto srcLocal = reinterpret_cast<uint8_t *>(
                                        reinterpret_cast<uint8_t *>(pixels) +
                                        y * stride);

                                int px = x * 4;
                                int maxR = srcLocal[px];
                                int maxG = srcLocal[px + 1];
                                int maxB = srcLocal[px + 2];
                                int maxA = srcLocal[px + 3];

                                for (int j = -mSize; j < mSize; ++j) {
                                    std::vector<int> sub = kernel[j + mSize];
                                    int nSize = sub.size() / 2;
                                    int i = -nSize;
                                    int newY = y + j;
                                    auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(pixels) + newY * stride);
                                    if (newY >= 0 && newY < height) {
                                        for (; i + 16 < nSize && x + i + 16 < width; i += 16) {
                                            VU8x16 r, g, b, a;
                                            int newX = (clamp(x + i, 0, width - 1)) * 4;
                                            LoadInterleaved4(du8x16, &src[newX], r, g, b, a);
                                            auto lowR = PromoteLowerTo(di16x8, r);

                                            auto vKernelHigh = LoadU(di32x4, &sub[i + nSize]);
                                            auto vKernelLow = LoadU(di32x4, &sub[i + nSize + 4]);

                                            auto vKernel = Combine(di16x8, DemoteTo(di16x4, vKernelHigh), DemoteTo(di16x4, vKernelLow));

                                            maxR = std::max(int(maxR), (int) ExtractLane(
                                                    MaxOfLanes(di16x8, Mul(PromoteTo(di16x8, UpperHalf(du8x8, r)), vKernel)), 0));
                                            maxG = std::max(int(maxG), (int) ExtractLane(
                                                    MaxOfLanes(di16x8, Mul(PromoteTo(di16x8, UpperHalf(du8x8, g)), vKernel)), 0));
                                            maxB = std::max(int(maxB), (int) ExtractLane(
                                                    MaxOfLanes(di16x8, Mul(PromoteTo(di16x8, UpperHalf(du8x8, b)), vKernel)), 0));
                                            maxA = std::max(int(maxA), (int) ExtractLane(
                                                    MaxOfLanes(di16x8, Mul(PromoteTo(di16x8, UpperHalf(du8x8, a)), vKernel)), 0));

                                            vKernelHigh = LoadU(di32x4, &sub[i + nSize + 8]);
                                            vKernelLow = LoadU(di32x4, &sub[i + nSize + 12]);

                                            vKernel = Combine(di16x8, DemoteTo(di16x4, vKernelHigh), DemoteTo(di16x4, vKernelLow));

                                            maxR = std::max(int(maxR), (int) ExtractLane(
                                                    MaxOfLanes(di16x8, Mul(PromoteTo(di16x8, LowerHalf(r)), vKernel)), 0));
                                            maxG = std::max(int(maxG), (int) ExtractLane(
                                                    MaxOfLanes(di16x8, Mul(PromoteTo(di16x8, LowerHalf(g)), vKernel)), 0));
                                            maxB = std::max(int(maxB), (int) ExtractLane(
                                                    MaxOfLanes(di16x8, Mul(PromoteTo(di16x8, LowerHalf(b)), vKernel)), 0));
                                            maxA = std::max(int(maxA), (int) ExtractLane(
                                                    MaxOfLanes(di16x8, Mul(PromoteTo(di16x8, LowerHalf(a)), vKernel)), 0));
                                        }

                                        for (; i + 4 < nSize && x + i + 4 < width; i += 4) {
                                            VU8x4 r, g, b, a;
                                            int newX = (clamp(x + i, 0, width - 1)) * 4;
                                            auto vKernel = LoadU(di32x4, &sub[i + nSize]);
                                            LoadInterleaved4(du8x4, reinterpret_cast<uint8_t *>(src) + newX, r, g, b, a);
                                            maxR = std::max(int(maxR), ExtractLane(
                                                    MaxOfLanes(di32x4, Mul(PromoteTo(di32x4, r), vKernel)), 0));
                                            maxG = std::max(int(maxG), ExtractLane(
                                                    MaxOfLanes(di32x4, Mul(PromoteTo(di32x4, g), vKernel)), 0));
                                            maxB = std::max(int(maxB), ExtractLane(
                                                    MaxOfLanes(di32x4, Mul(PromoteTo(di32x4, b), vKernel)), 0));
                                            maxA = std::max(int(maxA), ExtractLane(
                                                    MaxOfLanes(di32x4, Mul(PromoteTo(di32x4, a), vKernel)), 0));
                                        }

                                        for (; i < nSize; ++i) {
                                            int newX = x + i;
                                            if (newX >= 0 && newX < width) {
                                                newX *= 4;
                                                const uint8_t itemR = src[newX] * sub[i + nSize];
                                                if (itemR > maxR) {
                                                    maxR = itemR;
                                                }
                                                const uint8_t itemG = src[newX + 1] * sub[i + nSize];
                                                if (itemG > maxG) {
                                                    maxG = itemG;
                                                }
                                                const uint8_t itemB = src[newX + 2] * sub[i + nSize];
                                                if (itemB > maxB) {
                                                    maxB = itemB;
                                                }
                                                const uint8_t itemA = src[newX + 3] * sub[i + nSize];
                                                if (itemA > maxA) {
                                                    maxA = itemA;
                                                }
                                            }
                                        }
                                    }
                                }

                                auto dst = reinterpret_cast<uint8_t *>(
                                        reinterpret_cast<uint8_t *>(destination) + y * stride);
                                dst[px] = maxR;
                                dst[px + 1] = maxG;
                                dst[px + 2] = maxB;
                                dst[px + 3] = maxA;
                            }
                        }
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }
    }

    template<class T>
    void dilate(T *pixels, T *destination, int width, int height,
                std::vector<std::vector<int>> &kernel) {
        int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    height * width / (256 * 256)), 1, 12);
        vector<thread> workers;

        int segmentHeight = height / threadCount;

        for (int i = 0; i < threadCount; i++) {
            int start = i * segmentHeight;
            int end = (i + 1) * segmentHeight;
            if (i == threadCount - 1) {
                end = height;
            }
            workers.emplace_back(
                    [start, end, width, height, pixels, destination, &kernel]() {
                        for (int y = start; y < end; ++y) {
                            auto dst = reinterpret_cast<T *>(
                                    reinterpret_cast<uint8_t *>(destination) + y * width);
                            for (int x = 0; x < width; ++x) {
                                int mSize = kernel.size() / 2;

                                auto srcLocal = reinterpret_cast<T *>(
                                        reinterpret_cast<uint8_t *>(pixels) +
                                        y * width);

                                T max = srcLocal[x];

                                for (int n = -mSize; n <= mSize; ++n) {
                                    std::vector<int> sub = kernel[n + mSize];
                                    int nSize = sub.size() / 2;

                                    int m = -nSize;

                                    for (; m <= nSize; ++m) {
                                        float kernelItem = sub[n + nSize];
                                        int newX = x + m;
                                        int newY = y + n;
                                        if (newX >= 0 && newX < width && newY >= 0 &&
                                            newY < height) {
                                            auto src = reinterpret_cast<T *>(
                                                    reinterpret_cast<uint8_t *>(pixels) +
                                                    newY * width);
                                            T vl = src[newX] * kernelItem;
                                            if (vl > max) {
                                                max = vl;
                                            }
                                        }
                                    }
                                }

                                dst[x] = max;
                            }
                        }
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }
    }

    template void
    dilate(uint8_t *pixels, uint8_t *destination, int width, int height,
           std::vector<std::vector<int>> &kernel);

    template
    void dilateRGBA(uint8_t *pixels, uint8_t *destination, int stride, int width, int height,
                    std::vector<std::vector<int>> &kernel);
}