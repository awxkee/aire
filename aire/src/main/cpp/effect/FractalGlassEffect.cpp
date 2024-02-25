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

#include "FractalGlassEffect.h"
#include <vector>
#include "MathUtils.hpp"

namespace aire {
    void fractalGlassEffect(uint8_t *data, int stride, int width, int height, float glassSize, float amplitude) {
        std::vector<uint8_t> transient(stride * height);
        std::vector<uint8_t> displacement(width * height);
        int displacementWidth = width * glassSize;
        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(displacement.data()) +
                    y * width);
            int currentDisplacementWidth = 0;
            for (int x = 0; x < width; ++x, ++currentDisplacementWidth) {
                float px = float(currentDisplacementWidth) / float(displacementWidth);
                float intensity = px * 255.f;
                dst[x] = static_cast<uint8_t >(intensity);
                if (currentDisplacementWidth >= displacementWidth) {
                    currentDisplacementWidth = 0;
                }
            }
        }
        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(transient.data()) +
                    y * stride);
            auto displacementSource = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(displacement.data()) +
                    y * width);
            for (int x = 0; x < width; ++x) {
                float displ = displacementSource[x] / 255.f * (width * amplitude);

                int newX = clamp(static_cast<int>((displ + x)) % width, 0, width - 1);
                int newY = y;

                auto src = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(data) +
                        newY * stride);

                dst[0] = src[newX * 4];
                dst[1] = src[newX * 4 + 1];
                dst[2] = src[newX * 4 + 2];
                dst[3] = src[newX * 4 + 3];
                dst += 4;
            }
        }

        std::copy(transient.begin(), transient.end(), data);
    }
}