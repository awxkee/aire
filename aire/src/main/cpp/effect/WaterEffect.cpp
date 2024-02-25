/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 04/02/24, 6:13 PM
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

#include "WaterEffect.h"
#include <vector>
#include <algorithm>
#include "concurrency.hpp"

namespace aire {
    void waterEffect(uint8_t *data, int stride, int width, int height, float fractionSize,
                     float frequencyX, float amplitudeX, float frequencyY, float amplitudeY) {
        std::vector<uint8_t> transient(stride * height);
        int xMove = width * fractionSize;
        int yMove = height * fractionSize;

        concurrency::parallel_for(2, height, [&](int y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(transient.data()) +
                    y * stride);
            for (int x = 0; x < width; ++x) {
                float dx = xMove * sin(2 * M_PI * y / height * frequencyX);
                float dy = yMove * cos(2 * M_PI * x / width * frequencyY);
                if (amplitudeY != 0) {
                    dy *= amplitudeY;
                } else {
                    dy = 0;
                }
                if (amplitudeX != 0) {
                    dx *= amplitudeX;
                } else {
                    dx = 0;
                }

                int newX = std::clamp(static_cast<int>(x + dx), 0, width - 1);
                int newY = std::clamp(static_cast<int>(y + dy), 0, height - 1);

                auto src = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(data) +
                        newY * stride);

                dst[0] = src[newX * 4];
                dst[1] = src[newX * 4 + 1];
                dst[2] = src[newX * 4 + 2];
                dst[3] = src[newX * 4 + 3];
                dst += 4;
            }
        });

        std::copy(transient.begin(), transient.end(), data);
    }
}