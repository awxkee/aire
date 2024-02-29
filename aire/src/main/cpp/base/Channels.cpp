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

#include "Channels.h"
#include "concurrency.hpp"
#include <iostream>

namespace aire {

    template<class T>
    void split(T *pixels, T *r, T *g, T *b, T *a, int stride, int width, int height) {
        concurrency::parallel_for(2, height, [&](int y) {
            auto src = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(pixels) + y * stride);
            T *rDst = nullptr;
            if (r != nullptr) {
                rDst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(r) + y * width);
            }
            T *gDst = nullptr;
            if (r != nullptr) {
                gDst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(g) + y * width);
            }
            T *bDst = nullptr;
            if (r != nullptr) {
                bDst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(b) + y * width);
            }
            T *aDst = nullptr;
            if (r != nullptr) {
                aDst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(a) + y * width);
            }
            for (int x = 0; x < width; ++x) {
                if (rDst) {
                    rDst[0] = src[0];
                    rDst += 1;
                }
                if (gDst) {
                    gDst[0] = src[1];
                    gDst += 1;
                }
                if (bDst) {
                    bDst[0] = src[2];
                    bDst += 1;
                }
                if (aDst) {
                    aDst[0] = src[3];
                    aDst += 1;
                }
                src += 4;
            }
        });
    }

    template<class T>
    void merge(T *destination, T *r, T *g, T *b, T *a, int stride, int width, int height) {
        concurrency::parallel_for(2, height, [&](int y) {
            auto dst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(destination) + y * stride);
            auto rSrc = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(r) + y * width);
            auto gSrc = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(g) + y * width);
            auto bSrc = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(b) + y * width);
            auto aSrc = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(a) + y * width);
            for (int x = 0; x < width; ++x) {
                dst[0] = rSrc[0];
                dst[1] = gSrc[0];
                dst[2] = bSrc[0];
                dst[3] = aSrc[0];
                dst += 4;
                rSrc += 1;
                gSrc += 1;
                bSrc += 1;
                aSrc += 1;
            }
        });
    }

    template void
    split(uint8_t *pixels, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a, int stride, int width,
          int height);

    template
    void merge(uint8_t *destination, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a, int stride,
               int width, int height);
}