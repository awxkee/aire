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

#include "Sharpness.h"
#include "Eigen/Eigen"
#include "concurrency.hpp"

namespace aire {
    using namespace std;
    using namespace Eigen;

    void applySharp(uint8_t *src, uint8_t *sharpenMask, int stride, int width, int height, const float intensity) {
        concurrency::parallel_for(3, height, [&](int y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(src) + y * stride);
            for (int x = 0; x < width; ++x) {
                auto mask = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(sharpenMask) + y * stride);
                int px = x * 4;
                Eigen::Vector4f color = {dst[px], dst[px + 1], dst[px + 2], dst[px + 3]};
                Eigen::Vector4f maskColor = {mask[px], mask[px + 1], mask[px + 2], mask[px + 3]};
                color /= 255.f;
                maskColor /= 255.f;
                Eigen::Vector4f diff = (color - maskColor) * intensity;
                Eigen::Array4i final = ((color - diff) * 255.f).array().max(0.f).min(255.f).cast<int>();
                dst[px] = final.x();
                dst[px + 1] = final.y();
                dst[px + 2] = final.z();
                dst[px + 3] = final.w();
            }
        });
    }

    void applyUnsharp(uint8_t *src, uint8_t *sharpenMask, int stride, int width, int height, const float intensity) {
        concurrency::parallel_for(3, height, [&](int y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(src) + y * stride);
            for (int x = 0; x < width; ++x) {
                auto mask = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(sharpenMask) + y * stride);
                int px = x * 4;
                Eigen::Vector4f color = {dst[px], dst[px + 1], dst[px + 2], dst[px + 3]};
                Eigen::Vector4f maskColor = {mask[px], mask[px + 1], mask[px + 2], mask[px + 3]};
                color /= 255.f;
                maskColor /= 255.f;
                Eigen::Vector4f diff = (color - maskColor) * intensity;
                Eigen::Array4i final = ((color - diff) * 255.f).array().max(0.f).min(255.f).cast<int>();
                dst[px] = final.x();
                dst[px + 1] = final.y();
                dst[px + 2] = final.z();
                dst[px + 3] = final.w();
            }
        });
    }

    Eigen::Matrix3f generateSharpenKernel() {
        Eigen::Matrix3f kernel;
        kernel << 0, -1, 0, -1, 5, -1, 0, -1, 0;
        kernel /= kernel.sum();
        return kernel;
    }

}