/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 01/02/24, 6:13 PM
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

#include "Halftone.h"

namespace aire {
    template<class V>
    void
    generateHalftoneHorizontalLines(std::vector<V> &data, int stride, int width, int height,
                                    int lineSize, int lineSpacing, V lineColor, V alpha) {
        for (int y = 0; y < height; y += lineSize + lineSpacing) {
            for (int j = y; j < lineSize + y && j < height; ++j) {
                auto dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data.data()) + j * stride);
                for (int x = 0; x < width; ++x) {
                    int ps = x * 4;
                    dst[ps] = lineColor;
                    dst[ps + 1] = lineColor;
                    dst[ps + 2] = lineColor;
                    dst[ps + 3] = alpha;
                }
            }
        }
    }

    template void
    generateHalftoneHorizontalLines(std::vector<uint8_t> &data, int stride, int width, int height,
                                    int lineSize, int lineSpacing, uint8_t lineColor,
                                    uint8_t alpha);

}