//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#include "MedianBlur.h"
#include <vector>
#include <thread>
#include "algo/median/QuickSelect.h"
#include "algo/median/Wirth.h"

using namespace std;

namespace aire {
    inline __attribute__((flatten))
    uint32_t getMedian(const std::vector<uint32_t> &data) {
        std::vector<uint32_t> copy = data;
        std::nth_element(copy.begin(), copy.begin() + copy.size() / 2, copy.end());
        return copy[copy.size() / 2];
    }

    inline __attribute__((flatten))
    uint8_t getMedian(const std::vector<uint8_t> &data) {
        std::vector<uint8_t> copy = data;
        std::nth_element(copy.begin(), copy.begin() + copy.size() / 2, copy.end());
        return copy[copy.size() / 2];
    }

    void medianBlurU8Runner(std::vector<uint8_t> &transient, uint8_t *data, int stride, int width,
                            int y, int radius, int height, const MedianSelector selector) {
        uint32_t *dst = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(transient.data()) +
                                                     y * stride);
        const int length = (2 * radius + 1);
        const int N = (2 * radius + 1) * (2 * radius + 1);

        uint32_t *mStore = reinterpret_cast<uint32_t *>(malloc(sizeof(uint32_t) * N));

        for (int x = 0; x < width; ++x) {
            if (selector == MEDIAN_QUICK_SELECT || selector == MEDIAN_WIRTH) {

                for (int j = -radius; j <= radius; ++j) {
                    for (int i = -radius; i <= radius; ++i) {
                        int *src = reinterpret_cast<int *>(reinterpret_cast<uint8_t *>(data) +
                                                           clamp(y + j, 0, height - 1) * stride);
                        int pos = clamp((x + i), 0, width - 1);
                        mStore[(i + radius) * length + (j + radius)] = src[pos];
                    }
                }

                if (selector == MEDIAN_QUICK_SELECT) {
                    reinterpret_cast<uint32_t *>(dst)[0] = QuickSelect(mStore, N);
                } else {
                    reinterpret_cast<uint32_t *>(dst)[0] = wirthMedian(mStore, N);
                }
            } else {
                std::vector<uint32_t> store(0);

                for (int j = -radius; j <= radius; ++j) {
                    for (int i = -radius; i <= radius; ++i) {
                        int *src = reinterpret_cast<int *>(reinterpret_cast<uint8_t *>(data) +
                                                           clamp(y + j, 0, height - 1) * stride);
                        int pos = clamp((x + i), 0, width - 1);
                        store.insert(store.end(), src[pos]);
                    }
                }
                reinterpret_cast<uint32_t *>(dst)[0] = getMedian(store);
            }

            dst += 1;
        }

        free(mStore);
    }

    void medianBlurChannel(uint8_t *data, const int width, const int height, const int radius,
                           const MedianSelector selector) {
        std::vector<uint8_t> transient(width * height);

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
                    [start, end, width, height, data, radius, &transient, selector]() {
                        const int length = (2 * radius + 1);
                        const int N = (2 * radius + 1) * (2 * radius + 1);

                        uint8_t *mStore = reinterpret_cast<uint8_t *>(malloc(sizeof(uint8_t) * N));

                        for (int y = start; y < end; ++y) {
                            uint8_t *dst = reinterpret_cast<uint8_t *>(
                                    reinterpret_cast<uint8_t *>(transient.data()) + y);
                            for (int x = 0; x < width; ++x) {
                                if (selector == MEDIAN_QUICK_SELECT || selector == MEDIAN_WIRTH) {
                                    for (int j = -radius; j <= radius; ++j) {
                                        for (int i = -radius; i <= radius; ++i) {
                                            uint8_t *src = reinterpret_cast<uint8_t *>(
                                                    reinterpret_cast<uint8_t *>(data) +
                                                    clamp(y + j, 0, height - 1) * width);
                                            int pos = clamp((x + i), 0, width - 1);
                                            mStore[(i + radius) * length + (j + radius)] = src[pos];
                                        }
                                    }

                                    if (selector == MEDIAN_QUICK_SELECT) {
                                        reinterpret_cast<uint8_t *>(dst)[0] = QuickSelect(mStore, N);
                                    } else if (selector == MEDIAN_WIRTH) {
                                        reinterpret_cast<uint8_t *>(dst)[0] = wirthMedian(mStore, N);
                                    }
                                } else {
                                    std::vector<uint8_t> store(N);

                                    for (int j = -radius; j <= radius; ++j) {
                                        for (int i = -radius; i <= radius; ++i) {
                                            uint8_t *src = reinterpret_cast<uint8_t *>(
                                                    reinterpret_cast<uint8_t *>(data) +
                                                    clamp(y + j, 0, height - 1) * width);
                                            int pos = clamp((x + i), 0, width - 1);
                                            store[(i + radius) * length + (j + radius)] = src[pos];
                                        }
                                    }
                                    reinterpret_cast<uint8_t *>(dst)[0] = getMedian(store);
                                }

                                dst += 1;
                            }
                        }

                        free(mStore);
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }
    }

    void
    medianBlur(uint8_t *data, const int stride, const int width, const int height, const int radius,
               const MedianSelector selector) {
        std::vector<uint8_t> transient(stride * height);
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
                    [start, end, width, height, stride, data, radius, &transient, selector]() {
                        for (int y = start; y < end; ++y) {
                            medianBlurU8Runner(transient, data, stride, width, y, radius, height,
                                               selector);
                        }
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }

        std::copy(transient.begin(), transient.end(), data);
    }
}