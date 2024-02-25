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

#include "Adjustments.h"
#include "Eigen/Eigen"
#include "color/Gamut.h"
#include "color/Blend.h"
#include "color/eotf-inl.h"
#include "concurrency.hpp"

namespace aire {

    using namespace aire::HWY_NAMESPACE;

    void colorMatrix(uint8_t *data, int stride, int width, int height, const Eigen::Matrix3f matrix) {
        concurrency::parallel_for(4, height, [&](int y) {
            auto pixels = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;

            for (; x < width; ++x) {
                Eigen::Vector3f rgb;
                rgb << pixels[0], pixels[1], pixels[2];
                rgb /= 255.f;

                rgb = (matrix * rgb * 255.f).array().max(0.f).min(255.f);

                pixels[0] = rgb.x();
                pixels[1] = rgb.y();
                pixels[2] = rgb.z();

                pixels += 4;
            }
        });
    }

    void adjustment(uint8_t *data, int stride, int width, int height, float gain, float bias) {
        const Eigen::Vector3f fBias = {bias, bias, bias};
        const Eigen::Vector3f balance = {0.5f, 0.5f, 0.5f};
        concurrency::parallel_for(4, height, [&](int y) {
            auto pixels = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;

            for (; x < width; ++x) {
                Eigen::Vector3f rgb;
                rgb << pixels[0], pixels[1], pixels[2];
                rgb /= 255.f;

                rgb = gain * (rgb - balance) + balance + fBias;
                rgb = (rgb * 255.f).array().max(0.f).min(255.f);

                pixels[0] = rgb.x();
                pixels[1] = rgb.y();
                pixels[2] = rgb.z();

                pixels += 4;
            }
        });
    }

    void saturation(uint8_t *data, int stride, int width, int height, float saturation) {
        const Eigen::Vector3f lumaPrimaries = {0.2125, 0.7154, 0.0721};
        concurrency::parallel_for(4, height, [&](int y) {
            auto pixels = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;

            for (; x < width; ++x) {
                Eigen::Vector3f rgb;
                rgb << pixels[0], pixels[1], pixels[2];
                rgb /= 255.f;

                Eigen::Vector3f letf = {SRGBToLinear(rgb[0]), SRGBToLinear(rgb[1]), SRGBToLinear(rgb[2])};

                float luma = LinearSRGBTosRGB(letf.dot(lumaPrimaries));
                Eigen::Vector3f grayscale = {luma, luma, luma};
                rgb = (mix(grayscale, rgb, saturation) * 255.f).array().max(0.f).min(255.f);

                pixels[0] = rgb.x();
                pixels[1] = rgb.y();
                pixels[2] = rgb.z();

                pixels += 4;
            }
        });
    }

}