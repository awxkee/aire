/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 02/02/24, 6:13 PM
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

#include "Erosion.h"
#include <vector>
#include <algorithm>
#include <thread>
#include "concurrency.hpp"

using namespace std;

namespace aire {

    template<class T>
    void erodeRGBA(T *pixels, T *destination, int stride, int width, int height,
                   Eigen::MatrixXi &kernel) {
        concurrency::parallel_for(8, height, [&](int y) {
            for (int x = 0; x < width; ++x) {
                int mSize = kernel.rows() / 2;

                auto srcLocal = reinterpret_cast<uint32_t *>(
                        reinterpret_cast<uint8_t *>(pixels) +
                        y * stride);

                long min = srcLocal[x];

                for (int m = -mSize; m < mSize; ++m) {
                    int nSize = kernel.cols() / 2;
                    for (int n = -nSize; n < nSize; ++n) {
                        int newX = x + m;
                        int newY = y + n;
                        if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                            auto src = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(pixels) + newY * stride);
                            const uint32_t item = src[newX] * kernel(m + mSize, n + nSize);
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
        });
    }

    template<class T>
    void erode(T *pixels, T *destination, int width, int height,
               Eigen::MatrixXi &kernel) {
        concurrency::parallel_for(8, height, [&](int y) {
            auto dst = reinterpret_cast<T *>(
                    reinterpret_cast<uint8_t *>(destination) + y * width);
            for (int x = 0; x < width; ++x) {
                int mSize = kernel.rows() / 2;

                auto srcLocal = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(pixels) + y * height);

                T min = srcLocal[x];

                for (int m = -mSize; m < mSize; ++m) {
                    int nSize = kernel.cols() / 2;
                    for (int n = -nSize; n < nSize; ++n) {
                        int newX = x + m;
                        int newY = y + n;
                        if (newX >= 0 && newX < width && newY >= 0 &&
                            newY < height) {
                            auto src = reinterpret_cast<T *>(
                                    reinterpret_cast<uint8_t *>(pixels) +
                                    newY * width);
                            T vl = src[newX] * kernel(m + mSize, n + nSize);
                            if (vl > min) {
                                min = vl;
                            }
                        }
                    }
                }

                dst[x] = min;
            }
        });
    }

    template void
    erode(uint8_t *pixels, uint8_t *destination, int width, int height,
          Eigen::MatrixXi &kernel);

    template
    void erodeRGBA(uint8_t *pixels, uint8_t *destination, int stride, int width, int height,
                   Eigen::MatrixXi &kernel);
}