/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 31/01/24, 6:13 PM
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

#pragma once

#include <cstdint>
#include "Eigen/Eigen"

namespace aire {
    void boxBlurU8(uint8_t* data, int stride, int width, int height, int radius);
    void boxBlurF16(uint16_t *data, int stride, int width, int height, int radius);
    std::vector<float> generateBoxKernel(int radius);
    Eigen::MatrixXf generateBoxKernel2D(const int radius);

    Eigen::MatrixXf generateTentFilter(int N);

    Eigen::MatrixXf generateTentFilterNormalized(int N);

    std::vector<float> generate1DTentFilterKernelNormalized(int size);

    void tentBlur(uint8_t *data, int stride, int width, int height, const int size);

    void tentBlurF16(uint16_t *data, int stride, int width, int height, const int size);
}
