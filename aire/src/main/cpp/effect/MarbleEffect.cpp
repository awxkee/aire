/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 03/02/24, 6:13 PM
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

#include "MarbleEffect.h"
#include "algo/PerlinNoise.hpp"
#include <algorithm>
#include <chrono>

using namespace std;

namespace aire {

    void marbleEffect(uint8_t *data, int stride, int width, int height, float intensity, float turbulence, float amplitude) {
        const siv::PerlinNoise::seed_type seed = std::chrono::system_clock::now().time_since_epoch().count();

        const siv::PerlinNoise perlin{ seed };

        float sinTable[256];
        float cosTable[256];
        for (int i = 0; i < 256; i++) {
            float angle = 2 * M_PI * i / 256.f * turbulence;
            sinTable[i] = (float) (-sin(angle)) * -(intensity * width);
            cosTable[i] = (float) (cos(angle)) * (intensity * width);
        }

        std::vector<uint8_t > output(stride * height);

        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(output.data()) +
                                                   y * stride);
            for (int x = 0; x < width; ++x) {
                int px = x * 4;

                float noise = perlin.octave2D_01(float(x) * 0.01f, float(y) * 0.01f, 4) * amplitude;
                int displacement = clamp((int) (127 * (1 + noise)), 0, 255);

                int sourceX = clamp(static_cast<int>(floor(x + sinTable[displacement])), 0,
                                    width - 1);
                int sourceY = clamp(static_cast<int>(floor(y + cosTable[displacement])), 0,
                                    height - 1);

                auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) +
                        sourceY * stride);

                int sc = sourceX * 4;

                dst[px] = src[sc];
                dst[px + 1] = src[sc + 1];
                dst[px + 2] = src[sc + 2];
                dst[px + 3] = src[px + 3];
            }
        }

        std::copy(output.begin(), output.end(), data);
    }
}