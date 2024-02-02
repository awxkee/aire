//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Dilation.h"
#include <vector>

namespace aire {

    template<class T>
    void dilateRGBA(T *pixels, T *destination, int stride, int width, int height,
                    std::vector<std::vector<int>> &kernel) {
        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(pixels) + y * stride);
            for (int x = 0; x < width; ++x) {
                int mSize = kernel.size() / 2;
                int pos = x * 4;
                if (src[pos] > 0 && src[pos + 1] > 0 && src[pos + 2] > 0) {
                    for (int m = -mSize; m < mSize; ++m) {
                        std::vector<int> sub = kernel[m + mSize];
                        int nSize = sub.size() / 2;
                        for (int n = -nSize; n < nSize; ++n) {
                            if (sub[n + nSize] > 0) {
                                int newX = x + m;
                                int newY = y + n;
                                auto dst = reinterpret_cast<T *>(
                                        reinterpret_cast<uint8_t *>(destination) + newY * stride);
                                if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                                    dst[newX * 4] = src[x * 4];
                                    dst[newX * 4 + 1] = src[x * 4 + 1];
                                    dst[newX * 4 + 2] = src[x * 4 + 2];
                                }
                            }
                        }
                    }
                }

                auto dst = reinterpret_cast<T *>(
                        reinterpret_cast<uint8_t *>(destination) + y * stride);
                dst[x * 4 + 3] = 255;
            }
        }
    }

    template<class T>
    void dilate(T *pixels, T *destination, int width, int height,
                std::vector<std::vector<int>> &kernel) {
        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(pixels) + y * width);
            for (int x = 0; x < width; ++x) {
                int mSize = kernel.size() / 2;
                if (src[x] > 0) {
                    for (int m = -mSize; m < mSize; ++m) {
                        std::vector<int> sub = kernel[m + mSize];
                        int nSize = sub.size() / 2;
                        for (int n = -nSize; n < nSize; ++n) {
                            if (sub[n + nSize] > 0) {
                                int newX = x + m;
                                int newY = y + n;
                                auto dst = reinterpret_cast<T *>(
                                        reinterpret_cast<uint8_t *>(destination) + newY * width);
                                if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                                    dst[newX] = 255;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    std::vector<std::vector<int>> getDilateKernel(int size) {
        std::vector<std::vector<int>> kernel(size, std::vector<int>(size, 1));
        return std::move(kernel);
    }

    template void
    dilate(uint8_t *pixels, uint8_t *destination, int width, int height,
           std::vector<std::vector<int>> &kernel);

    template
    void dilateRGBA(uint8_t *pixels, uint8_t *destination, int stride, int width, int height,
                    std::vector<std::vector<int>> &kernel);
}