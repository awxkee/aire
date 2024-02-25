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

#include "TiltShift.h"
#include <algorithm>
#include <vector>
#include "color/Blend.h"
#include "jni/JNIUtils.h"
#include <thread>
#include "concurrency.hpp"

namespace aire {

    void horizontalTiltShift(uint8_t *data, uint8_t *source, std::vector<uint8_t> &blurred, int stride,
                             int width, int height,
                             float anchorX, float anchorY,
                             float radius, float angle) {
        const int newWidth = std::abs(width * std::cos(angle)) + std::abs(height * std::sin(angle));
        const int newHeight = std::abs(width * std::sin(angle)) + std::abs(height * std::cos(angle));
        float availableDistance;

        const float rotationAngle = std::fmod(rotationAngle + M_PI, 2 * M_PI) - M_PI;

        const float horizontalRangeMin = -M_PI / 4.0;  // -45 degrees in radians
        const float horizontalRangeMax = M_PI / 4.0;   // +45 degrees in radians

        if (rotationAngle >= horizontalRangeMin && rotationAngle <= horizontalRangeMax) {
            availableDistance = std::sqrt(newWidth * newWidth);
        } else {
            availableDistance = std::sqrt(newHeight * newHeight);
        }
        const float minDistance = availableDistance * radius;

        const int fAnchorX = width * anchorX;
        const int fAnchorY = height * anchorY;

        const int centerX = fAnchorX;
        const int centerY = fAnchorY;

        concurrency::parallel_for(4, height, [&](int y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(source) + y * stride);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto blurredSource = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(blurred.data()) + y * stride);
            for (int x = 0; x < width; ++x) {

                const int newX = (x - fAnchorX) * std::cos(angle) - (y - fAnchorY) * std::sin(angle) + fAnchorX;
                const int newY = (x - fAnchorX) * std::sin(angle) + (y - fAnchorY) * std::cos(angle) + fAnchorY;

                const float dx = newX - centerX;
                const float dy = newY - centerY;

                float currentDistance;
                if (rotationAngle >= horizontalRangeMin && rotationAngle <= horizontalRangeMax) {
                    currentDistance = std::sqrt(dx * dx);
                } else {
                    currentDistance = std::sqrt(dy * dy);
                }

                auto fr = currentDistance / minDistance;
                auto fraction = std::clamp(fr * fr * fr, 0.0f, 1.0f);
                auto blurredA = blurredSource[3];
                auto sourceA = src[3];
                auto newA = blurredA * fraction;

                Eigen::Vector3f mSource = {src[0], src[1], src[2]};
                mSource /= 255.f;
                Eigen::Vector3f blurSource = {blurredSource[0], blurredSource[1], blurredSource[2]};
                blurSource /= 255.f;
                auto blended = mix(mSource, blurSource, newA / 255.f);
                auto cmp = (blended * 255.f).array().min(255.f).max(0.f);

                dst[0] = cmp.x();
                dst[1] = cmp.y();
                dst[2] = cmp.z();
                dst[3] = sourceA;
                src += 4;
                dst += 4;
                blurredSource += 4;
            }
        });
    }

    void tiltShift(uint8_t *data, uint8_t *source, std::vector<uint8_t> &blurred, int stride, int width, int height,
                   float anchorX, float anchorY, float radius) {
        const float availableDistance = std::sqrt(height * height + width * width);
        const float minDistance = availableDistance * radius;
        const int centerX = width * anchorX;
        const int centerY = height * anchorY;

        concurrency::parallel_for(4, height, [&](int y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(source) + y * stride);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto blurredSource = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(blurred.data()) + y * stride);
            for (int x = 0; x < width; ++x) {
                const float dx = x - centerX;
                const float dy = y - centerY;
                float currentDistance = std::sqrt(dx * dx + dy * dy);

                auto fr = currentDistance / minDistance;
                auto fraction = std::clamp(fr * fr * fr, 0.0f, 1.0f);
                auto blurredA = blurredSource[3];
                auto sourceA = src[3];
                auto newA = blurredA * fraction;

                Eigen::Vector3f mSource = {src[0], src[1], src[2]};
                mSource /= 255.f;
                Eigen::Vector3f blurSource = {blurredSource[0], blurredSource[1], blurredSource[2]};
                blurSource /= 255.f;
                auto blended = mix(mSource, blurSource, newA / 255.f);
                auto cmp = (blended * 255.f).array().min(255.f).max(0.f);

                dst[0] = cmp.x();
                dst[1] = cmp.y();
                dst[2] = cmp.z();
                dst[3] = sourceA;
                src += 4;
                dst += 4;
                blurredSource += 4;
            }
        });
    }
}