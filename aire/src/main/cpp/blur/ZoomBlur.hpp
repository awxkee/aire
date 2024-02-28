/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 28/02/24, 6:13 PM
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

#include <algorithm>
#include "Eigen/Eigen"
#include "MathUtils.hpp"
#include "concurrency.hpp"

namespace aire {
    class ZoomBlur {
    public:
        ZoomBlur(int kernelSize,
                 float sigma,
                 float centerX,
                 float centerY,
                 float strength) :
                centerX(centerX),
                centerY(centerY),
                strength(strength),
                sigma(sigma),
                kernelSize(kernelSize) {
        }

        void apply(uint8_t *data, int stride, int width, int height) {
            std::vector<uint8_t> transient(stride * height, 0);
            const float cx = std::floor(static_cast<float>(width) * centerX);
            const float cy = std::floor(static_cast<float>(height) * centerY);

            Eigen::Vector2f centerPoint;
            centerPoint << cx, cy;

            const auto gaussian = compute1DGaussianKernel(kernelSize, sigma);

            const bool isKernelEven = gaussian.size() % 2 == 0;
            const int halfOfKernel = gaussian.size() / 2;
            const int jMax = isKernelEven ? halfOfKernel - 1 : halfOfKernel;

            concurrency::parallel_for(5, height, [&](int y) {
                auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(transient.data()) + y * stride);
                for (int x = 0; x < width; ++x) {
                    Eigen::Vector2f point = {x, y};
                    Eigen::Vector2f offset = (1.0f / 100.0f * (centerPoint - point) * strength);

                    Eigen::Vector4f accumulator = {0, 0, 0, 0};

                    int multiplier = -halfOfKernel;

                    for (int j = 0; j < gaussian.size(); ++j, ++multiplier) {
                        accumulator += loadPoint(data, (point + multiplier * offset).cast<int>(), stride, width, height) * gaussian[j];
                    }

                    dst[0] = std::clamp(static_cast<int >(accumulator.x()), 0, 255);
                    dst[1] = std::clamp(static_cast<int >(accumulator.y()), 0, 255);
                    dst[2] = std::clamp(static_cast<int >(accumulator.z()), 0, 255);
                    dst[3] = std::clamp(static_cast<int >(accumulator.w()), 0, 255);

                    dst += 4;
                }
            });

            std::copy(transient.begin(), transient.end(), data);
        }

    private:
        const float centerX;
        const float centerY;
        const float strength;
        const float sigma;
        const int kernelSize;

        inline Eigen::Vector4f loadPoint(uint8_t *data, Eigen::Vector2i px, const int stride, const int width, const int height) {
            px.x() = std::clamp(px.x(), 0, width - 1);
            px.y() = std::clamp(px.y(), 0, height - 1);
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + px.y() * stride);
            const int p = px.x() * 4;
            return {src[p], src[p + 1], src[p + 2], src[p + 3]};
        }
    };
}