/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 07/02/24, 6:13 PM
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

#include "LUT8.h"
#include <algorithm>
#include "concurrency.hpp"

namespace aire {

    using namespace std;

    void LUT8::apply(uint8_t *data, int stride, int width, int height) const {
        concurrency::parallel_for(2, height, [&](int y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(data) + y * stride);
            for (int x = 0; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(data) + y * stride);
                int px = x * 4;
                dst[px] = this->table[clamp(src[px], uint8_t(0), uint8_t(255))];
                dst[px + 1] = this->table[clamp(src[px + 1], uint8_t(0), uint8_t(255))];
                dst[px + 2] = this->table[clamp(src[px + 2], uint8_t(0), uint8_t(255))];
            }
        });
    }
}