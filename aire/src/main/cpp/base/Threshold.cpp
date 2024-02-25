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

#include "Threshold.h"
#include "hwy/highway.h"
#include "algo/support-inl.h"
#include "concurrency.hpp"

namespace aire {

    using namespace std;
    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    template<class V>
    void threshold(V *pixels, int width, int height, V thresholdLevel, V max, V min) {
        const ScalableTag<uint8_t> du;
        using VU = Vec<decltype(du)>;
        const int lanes = du.MaxLanes();

        const auto vThresholdLevel = Set(du, thresholdLevel);
        const auto vMax = Set(du, max);
        const auto vMin = Set(du, min);

        concurrency::parallel_for(2, height, [&](int y) {
            auto data = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(pixels) + y * width);
            int x = 0;

            for (; x + lanes < width; x += lanes) {
                auto lane = LoadU(du, data);
                auto mask = lane > vThresholdLevel;
                lane = IfThenElse(mask, vMax, vMin);
                StoreU(lane, du, data);
                data += lanes;
            }
            for (; x < width; ++x) {
                if (data[0] > thresholdLevel) {
                    data[0] = max;
                } else {
                    data[0] = min;
                }
                data += 1;
            }
        });
    }

    template
    void threshold(uint8_t *pixels, int width, int height, uint8_t thresholdLevel, uint8_t max,
                   uint8_t min);
}