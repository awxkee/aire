/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 06/02/24, 6:13 PM
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

#include "Grain.h"
#include <random>
#include "concurrency.hpp"

namespace aire {

    using namespace std;

    void grain(uint8_t *data, int stride, int width, int height, float intensity) {
        default_random_engine generator;
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
        normal_distribution<float> distribution(0, 127.f * intensity);

        concurrency::parallel_for(2, height, [&](int y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(data) + y * stride);
            for (int x = 0; x < width; ++x) {
                int grain = distribution(generator);
                const float mGrain = static_cast<float>(grain);

                int px = x * 4;
                dst[px] = clamp(dst[px] + mGrain, 0.f, 255.f);
                dst[px + 1] = clamp(dst[px + 1] + mGrain, 0.f, 255.f);
                dst[px + 2] = clamp(dst[px + 2] + mGrain, 0.f, 255.f);
            }
        });
    }
}