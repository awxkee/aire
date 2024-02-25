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

#include "Gamut.h"
#include "eotf-inl.h"
#include "concurrency.hpp"

namespace aire {
    using namespace aire::HWY_NAMESPACE;
    void bitmapToXYZ(uint8_t *data, int stride, float *xyzBitmap, int xyzStride, int width, int height, TransferFunction function,
                     Eigen::Matrix3f conversionMatrix) {
        concurrency::parallel_for(2, height, [&](int y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto dst = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(xyzBitmap) + xyzStride * y);
            for (int x = 0; x < width; ++x) {
                float r, g, b;
                r = src[0];
                g = src[1];
                b = src[2];
                if (function == TRANSFER_SRGB) {
                    r = SRGBToLinear(r / 255.f);
                    g = SRGBToLinear(g / 255.f);
                    b = SRGBToLinear(b / 255.f);
                }
                Eigen::Vector3f vec3 = {r, g, b};
                auto result = conversionMatrix * vec3;
                dst[0] = result.x();
                dst[1] = result.y();
                dst[2] = result.z();
                dst += 3;
                src += 4;
            }
        });
    }

    void xyzToBitmap(uint8_t *data, int stride, float *xyzBitmap, int xyzStride, int width, int height, TransferFunction function,
                     Eigen::Matrix3f conversionMatrix) {
        concurrency::parallel_for(2, height, [&](int y) {
            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(xyzBitmap) + xyzStride * y);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + stride * y);
            for (int x = 0; x < width; ++x) {
                float r, g, b;
                r = src[0];
                g = src[1];
                b = src[2];
                if (function == TRANSFER_SRGB) {
                    r = LinearSRGBTosRGB(r);
                    g = LinearSRGBTosRGB(g);
                    b = LinearSRGBTosRGB(b);
                }
                Eigen::Vector3f vec3 = {r, g, b};
                auto result = (conversionMatrix * vec3 * 255.f).array().max(0.f).min(255.f);
                dst[0] = result.x();
                dst[1] = result.y();
                dst[2] = result.z();
                src += 3;
                dst += 4;
            }
        });
    }
}