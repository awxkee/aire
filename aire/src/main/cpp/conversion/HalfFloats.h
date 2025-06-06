/*
 * MIT License
 *
 * Copyright (c) 2023 Radzivon Bartoshyk
 * aire [https://github.com/awxkee/aire]
 *
 * Created by Radzivon Bartoshyk on 04/09/2023
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef JXLCODER_HALFFLOATS_H
#define JXLCODER_HALFFLOATS_H

#include <cstdint>
#include "half.hpp"

#if HAVE_NEON
void RgbaF32ToF16Neon(const float *src, int srcStride, uint16_t *dst, int dstStride,
                      int width, int height) ;
#endif

namespace coder {
    void RgbaF32ToF16(const float *src, int srcStride, uint16_t *dst, int dstStride, int width,
                      int height);
}

float half_to_float(const uint16_t x);

uint16_t float_to_half(const float x);

uint as_uint(const float x);

float as_float(const uint x);

#endif //JXLCODER_HALFFLOATS_H
