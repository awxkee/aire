//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Erosion.h"
#include <vector>
#include <algorithm>
#include <thread>

using namespace std;

namespace aire {

    template<class T>
    void erodeRGBA(T *pixels, T *destination, int stride, int width, int height,
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
                        for (int y = start; y < end; ++y) {
                            for (int x = 0; x < width; ++x) {
                                int mSize = kernel.size() / 2;

                                auto srcLocal = reinterpret_cast<uint32_t *>(
                                        reinterpret_cast<uint8_t *>(pixels) +
                                        y * stride);

                                long min = srcLocal[x];

                                for (int m = -mSize; m < mSize; ++m) {
                                    std::vector<int> sub = kernel[m + mSize];
                                    int nSize = sub.size() / 2;
                                    for (int n = -nSize; n < nSize; ++n) {
                                        int newX = x + m;
                                        int newY = y + n;
                                        if (newX >= 0 && newX < width && newY >= 0 &&
                                            newY < height) {
                                            auto src = reinterpret_cast<uint32_t *>(
                                                    reinterpret_cast<uint8_t *>(pixels) +
                                                    newY * stride);
                                            const uint32_t item = src[newX] * sub[n + nSize];
                                            if (item < min) {
                                                min = item;
                                            }
                                        }
                                    }
                                }

                                auto dst = reinterpret_cast<uint32_t *>(
                                        reinterpret_cast<uint8_t *>(destination) + y * stride);
                                dst[x] = min;
                            }
                        }
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }
    }

    template<class T>
    void erode(T *pixels, T *destination, int width, int height,
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
                                        y * height);

                                T min = srcLocal[x];

                                for (int m = -mSize; m < mSize; ++m) {
                                    std::vector<int> sub = kernel[m + mSize];
                                    int nSize = sub.size() / 2;
                                    for (int n = -nSize; n < nSize; ++n) {
                                        float kernelItem = sub[n + nSize];
                                        int newX = x + m;
                                        int newY = y + n;
                                        if (newX >= 0 && newX < width && newY >= 0 &&
                                            newY < height) {
                                            auto src = reinterpret_cast<T *>(
                                                    reinterpret_cast<uint8_t *>(pixels) +
                                                    newY * width);
                                            T vl = src[newX] * kernelItem;
                                            if (vl > min) {
                                                min = vl;
                                            }
                                        }
                                    }
                                }

                                dst[x] = min;
                            }
                        }
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }
    }

    template void
    erode(uint8_t *pixels, uint8_t *destination, int width, int height,
          std::vector<std::vector<int>> &kernel);

    template
    void erodeRGBA(uint8_t *pixels, uint8_t *destination, int stride, int width, int height,
                   std::vector<std::vector<int>> &kernel);
}