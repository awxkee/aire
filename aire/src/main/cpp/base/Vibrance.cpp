/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 05/02/24, 6:13 PM
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

#include "Vibrance.h"
#include <algorithm>
#include "fast_math-inl.h"
#include "MathUtils.hpp"
#include "concurrency.hpp"

namespace aire {
    void vibrance(uint8_t *pixels, int stride, int width, int height, float vibrance) {
        concurrency::parallel_for(2, height, [&](int y) {
            auto data = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(pixels) + y * stride);
            int x = 0;

            for (; x < width; ++x) {
                int red = data[0];
                int green = data[1];
                int blue = data[2];

                int avgIntensity = (red + green + blue) / 3;
                int mx = max3(red, green, blue);
                int vibranceBoost = std::clamp((float(mx) - avgIntensity) * vibrance, -255.f, 255.f);

                data[0] = std::clamp(red + vibranceBoost, 0, 255);
                data[1] = std::clamp(green + vibranceBoost, 0, 255);
                data[2] = std::clamp(blue + vibranceBoost, 0, 255);
                data += 4;
            }
        });
    }
}