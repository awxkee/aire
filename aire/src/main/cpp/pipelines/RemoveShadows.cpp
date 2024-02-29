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

#include "RemoveShadows.h"
#include "base/Grayscale.h"
#include "base/Threshold.h"
#include "base/Channels.h"
#include "base/Dilation.h"
#include "blur/MedianBlur.h"
#include <algorithm>
#include "base/Arithmetics.h"
#include <thread>
#include "MathUtils.hpp"
#include "EigenUtils.h"

using namespace std;

namespace aire {

    void removeProcessChannel(uint8_t *src, int width, int height, int kernelSize) {
        std::vector<uint8_t> chan(width * height);
        auto kernel = getStructuringKernel(kernelSize);
        dilate(src, chan.data(), width, height, kernel);
        medianBlurChannel(chan.data(), width, height, 21);

        std::vector<uint8_t> dstDiff(width * height);
        absDiff(dstDiff.data(), chan.data(), src, width, height);
        diff(chan.data(), 255, dstDiff.data(), width, height);
        normalize(reinterpret_cast<uint8_t *>(chan.data()), width, height, uint8_t(0), uint8_t(255));
        std::copy(chan.begin(), chan.end(), src);
    }

    void removeShadows(uint8_t *src, int stride, int width, int height, int kernelSize) {
        std::vector<uint8_t> rChan(width * height);
        std::vector<uint8_t> gChan(width * height);
        std::vector<uint8_t> bChan(width * height);
        std::vector<uint8_t> aChan(width * height);

        split(src,
              rChan.data(), gChan.data(), bChan.data(), aChan.data(),
              stride, width, height);

        removeProcessChannel(rChan.data(), width, height, kernelSize);

        removeProcessChannel(gChan.data(), width, height, kernelSize);

        removeProcessChannel(bChan.data(), width, height, kernelSize);

        merge(src, rChan.data(), gChan.data(), bChan.data(), aChan.data(), stride, width, height);
    }
}