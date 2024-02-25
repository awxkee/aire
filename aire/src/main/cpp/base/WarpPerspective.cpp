/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 15/02/24, 6:13 PM
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

#include "WarpPerspective.h"
#include "EigenUtils.h"
#include "MathUtils.hpp"

namespace aire {

    WarpResult WarpPerspective::apply() {
        auto boundingBox = computeBoundingBox(perspective, width, height);

        double minX = boundingBox.col(0).minCoeff();
        double minY = boundingBox.col(1).minCoeff();
        double maxX = boundingBox.col(0).maxCoeff();
        double maxY = boundingBox.col(1).maxCoeff();

        int newWidth = static_cast<int>(std::ceil(maxX - minX));
        int newHeight = static_cast<int>(std::ceil(maxY - minY));
        int dstStride = computeStride(newWidth, sizeof(uint8_t), 4);

        std::vector<uint8_t > dst(dstStride * newHeight);

        return {
            .result = dst,
            .stride = dstStride,
            .width = newWidth,
            .height = newHeight
        };
    }

} // aire