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

static const Eigen::Vector2f IlluminantD65 = {0.31272, 0.32903};

static const Eigen::Matrix<float, 3, 2> SRGBPrimaries({{0.640f, 0.330f},
                                                       {0.300f, 0.600f},
                                                       {0.150f, 0.060f}});

static Eigen::Matrix<float, 3, 2> getSRGBPrimaries() {
    Eigen::Matrix<float, 3, 2> m;
    Eigen::Vector2f rPrimary = {0.640f, 0.330f};
    Eigen::Vector2f gPrimary = {0.300f, 0.600f};
    Eigen::Vector2f bPrimary = {0.150f, 0.060f};
    m << rPrimary, gPrimary, bPrimary;
    return m;
}

static Eigen::Vector3f XyToXYZ(const float x, const float y) {
    Eigen::Vector3f ret(0.f, 0.f, 0.f);
    ret[0] = x / y;
    ret[1] = 1.0f;
    ret[2] = (1.0 - x - y) / y;
    return ret;
}

static const Eigen::Matrix3f getPrimariesXYZ(const Eigen::Matrix<float, 3, 2> primariesXy) {
    Eigen::Matrix3f ret;
    ret << XyToXYZ(primariesXy(0, 0), primariesXy(0, 1)),
            XyToXYZ(primariesXy(1, 0), primariesXy(1, 1)),
            XyToXYZ(primariesXy(2, 0), primariesXy(2, 1));
    return ret;
}

static const Eigen::Vector3f getWhitePoint(const Eigen::Vector2f whitePoint) {
    return XyToXYZ(whitePoint[0], whitePoint[1]);
}

static Eigen::Matrix3f
GamutRgbToXYZ(const Eigen::Matrix<float, 3, 2> primariesXy, const Eigen::Vector2f whitePoint) {
    Eigen::Matrix3f xyZMatrix = getPrimariesXYZ(primariesXy);
    const Eigen::Vector3f whiteXyz = getWhitePoint(whitePoint);
    const Eigen::Matrix3f inverted = xyZMatrix.inverse();
    const Eigen::Vector3f s = inverted * whiteXyz;
    Eigen::Matrix3f transposed = xyZMatrix;
    auto row1 = transposed.row(0) * s;
    auto row2 = transposed.row(1) * s;
    auto row3 = transposed.row(2) * s;
    Eigen::Matrix3f conversion = xyZMatrix.array().rowwise() * s.transpose().array();
    return conversion;
}

enum TransferFunction {
    TRANSFER_SRGB = 1
};

namespace aire {
    void bitmapToXYZ(uint8_t *data, int stride, float *xyzBitmap, int xyzStride, int width, int height, TransferFunction function,
                     Eigen::Matrix3f conversionMatrix);

    void xyzToBitmap(uint8_t *data, int stride, float *xyzBitmap, int xyzStride, int width, int height, TransferFunction function,
                     Eigen::Matrix3f conversionMatrix);
}