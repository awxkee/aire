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
#include "TentBlur.h"
#include "base/Convolve1D.h"
#include "base/Convolve1Db16.h"
#include <vector>
#include "jni/JNIUtils.h"
#include "Eigen/Eigen"
#include <thread>
#include <algorithm>
#include "base/Convolve2D.h"

namespace aire {

    Eigen::MatrixXf generateTentFilterNormalized(int N) {
        auto kernel = generateTentFilter(N);
        float kernelSum = kernel.sum();
        if (kernelSum != 0.f) {
            kernel /= kernelSum;
        }
        return kernel;
    }

    std::vector<float> generate1DTentFilterKernelNormalized(int size) {
        if (size % 2 == 0 || size < 1) {
            throw AireError(
                    "Invalid filter size. Please use an odd positive integer for N, but received: " +
                    std::to_string(size));
        }

        std::vector<float> kernel(size, 0.0f);
        int center = size / 2;
        for (int i = 0; i < size; i++) {
            kernel[i] = 1.0f - std::abs(i - center) / static_cast<float>(center);
            kernel[i] = std::max(0.0f, kernel[i]);
        }
        float sum = 0.0f;
        for (int i = 0; i < size; i++) {
            sum += kernel[i];
        }
        if (sum != 0.f) {
            for (int i = 0; i < size; i++) {
                kernel[i] /= sum;
            }
        }
        return kernel;
    }

    Eigen::MatrixXf generateTentFilter(int N) {
        if (N % 2 == 0 || N < 1) {
            throw AireError(
                    "Invalid filter size. Please use an odd positive integer for N, but received: " +
                    std::to_string(N));
        }

        Eigen::MatrixXf tentFilter(N, N);
        float peakValue = 1.0f / ((N / 2) + 1);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                float distanceToCenter = std::min({i, j, N - 1 - i, N - 1 - j});
                tentFilter(i, j) = peakValue * (1.0f - (distanceToCenter / (N / 2.0f)));
            }
        }

        return tentFilter;
    }

    void tentBlur(uint8_t *data, int stride, int width, int height, const int size) {
        auto gen1DKernel = generate1DTentFilterKernelNormalized(size);
        convolve1D(data, stride, width, height, gen1DKernel, gen1DKernel);
    }

    void tentBlurF16(uint16_t *data, int stride, int width, int height, const int size) {
        auto gen1DKernel = generate1DTentFilterKernelNormalized(size);
        Convolve1Db16 convolution(gen1DKernel, gen1DKernel);
        convolution.convolve(data, stride, width, height);
    }
}
