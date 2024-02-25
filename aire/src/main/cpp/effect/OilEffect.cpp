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

#include "OilEffect.h"
#include "hwy/highway.h"
#include "MathUtils.hpp"
#include "jni/JNIUtils.h"
#include "algo/support-inl.h"

namespace aire {

    using namespace std;
    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    void oilEffect(uint8_t *data, int stride, int width, int height, int radius, float levels) {
        if (radius < 1) {
            std::string msg("Radius must be > 0 but received: " + std::to_string(radius));
            throw AireError(msg);
        }
        if (levels < 0 || levels > 255) {
            std::string msg("Levels must be in 0..255 but received: " + std::to_string(levels));
            throw AireError(msg);
        }
        const FixedTag<uint8_t, 4> du;
        using VU = Vec<decltype(du)>;
        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        std::vector<uint8_t> transient(stride * height);
        std::vector<uint8_t> intensities(pow(2 * radius + 1, 2));
        std::vector<uint8_t> rStore(pow(2 * radius + 1, 2));
        std::vector<uint8_t> gStore(pow(2 * radius + 1, 2));
        std::vector<uint8_t> bStore(pow(2 * radius + 1, 2));
        std::vector<uint8_t> aStore(pow(2 * radius + 1, 2));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int intensityIteration = 0;
                auto dst = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(transient.data()) +
                        y * stride);
                for (int j = -radius; j <= radius; ++j) {
                    for (int i = -radius; i <= radius; ++i) {
                        int posX = clamp(x + i, 0, width - 1) * 4;
                        int posY = clamp(y + i, 0, height - 1);
                        auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + posY * stride);
                        uint8_t r = src[posX];
                        uint8_t g = src[posX + 1];
                        uint8_t b = src[posX + 2];
                        int intensity = clamp((r + g + b) / 3 * levels, 0, 255);
                        rStore[intensityIteration] = r;
                        gStore[intensityIteration] = g;
                        bStore[intensityIteration] = b;
                        aStore[intensityIteration] = src[posX + 3];
                        intensities[intensityIteration] = intensity;
                        intensityIteration += 1;
                    }
                }

                auto maxElementIterator = std::max_element(intensities.begin(), intensities.end());

                if (maxElementIterator == intensities.end()) {
                    std::string msg("Invalid intensities array");
                    throw AireError(msg);
                }
                std::size_t position = std::distance(intensities.begin(), maxElementIterator);
                int px = x * 4;
                dst[px] = rStore[position];
                dst[px + 1] = gStore[position];
                dst[px + 2] = bStore[position];
                dst[px + 3] = aStore[position];
            }
        }

        std::copy(transient.begin(), transient.end(), data);
    }
}