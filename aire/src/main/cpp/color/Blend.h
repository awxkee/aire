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

#pragma once

#include "Eigen/Eigen"

inline __attribute__((flatten))
static float blendColor(const float c, const float c1, const float a) {
    float r = c1 * a + c * (1.0f - a);
    return r;
}

inline __attribute__((flatten))
static Eigen::Vector3f blendColor(const Eigen::Vector3f c, const Eigen::Vector3f c1, const Eigen::Vector3f a) {
    const Eigen::Vector3f ones = {1.0f, 1.0f, 1.0f};
    Eigen::Vector3f r = c1.array() * a.array() + c.array() * (ones - a).array();
    return r;
}

inline __attribute__((flatten))
static Eigen::Vector3f mix(const Eigen::Vector3f& x, const Eigen::Vector3f& y, float a) {
//    a = std::max(0.0f, std::min(1.0f, a));
    return x * (1.0f - a) + y * a;
}

inline __attribute__((flatten))
static Eigen::Vector4f mix(const Eigen::Vector4f& x, const Eigen::Vector4f& y, float a) {
//    a = std::max(0.0f, std::min(1.0f, a));
    return x * (1.0f - a) + y * a;
}

inline __attribute__((flatten))
static float mix(const float x, const float y, float a) {
    a = std::max(0.0f, std::min(1.0f, a));
    return x * (1.0f - a) + y * a;
}

static uint8_t blendColor(const uint8_t fg, const uint8_t bg, const uint8_t a) {
    return (fg * a + bg * (255 - a) + 127) / 255;
}

static uint8_t mergeAlpha(const uint8_t fg, const uint8_t bg) {
    return fg + (bg * (255 - fg) + 127) / 255;
}

static float mergeAlpha(const float fg, const float bg) {
    return fg + bg * (1.f - fg);
}

inline __attribute__((flatten))
static float blendColor(const float c1, const float c, const float ca, const float cb) {
    float r = c1 * ca + c * cb * (1.0f - ca);
    return r;
}