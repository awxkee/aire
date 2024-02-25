/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 12/02/24, 6:13 PM
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

#ifndef _COVER_TREE_POINT_H
#define _COVER_TREE_POINT_H

#include <vector>
#include "Eigen/Eigen"

/**
 * A simple point class containing a vector of doubles and a single char name.
 */
class CoverTreePoint {
private:
    Eigen::Vector4i mColor;
public:
    CoverTreePoint(Eigen::Vector4i v) : mColor(v) {}

    double distance(const CoverTreePoint &p) const {
        Eigen::Vector4i diff = mColor - p.mColor;
        int mask = (mColor[0] < 128) ? 3 : 2;
        Eigen::Array4i weights;
        weights << 2, 4, mask, 1;
        Eigen::Array4i squaredDiff = diff.array().square() * weights;
        return squaredDiff.sum();
    }

    Eigen::Vector4i getColor() {
        return mColor;
    }

    void print() const {

    }

    bool operator==(const CoverTreePoint &p) const {
        return p.mColor == mColor;
    }
};

#endif // _COVER_TREE_POINT_H

