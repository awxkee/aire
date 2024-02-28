/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 27/02/24, 6:13 PM
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

namespace aire {

    typedef float (*ScaleWeightSampler)(float);

    template<typename T>
    class ScaleRowSampler {
    public:
        ScaleRowSampler(const T *mSource,
                        const int srcStride,
                        const int inputWidth,
                        const int inputHeight,
                        T *mDestination,
                        const int dstStride,
                        const int outputWidth,
                        const int outputHeight,
                        const int components) : mSource(mSource),
                                                srcStride(srcStride),
                                                inputWidth(inputWidth),
                                                inputHeight(inputHeight),
                                                mDestination(mDestination),
                                                dstStride(dstStride),
                                                outputWidth(outputWidth),
                                                outputHeight(outputHeight),
                                                components(components) {
            xScale = static_cast<float>(inputWidth) / static_cast<float>(outputWidth);
            yScale = static_cast<float>(inputHeight) / static_cast<float>(outputHeight);
        }

        virtual void sample(const int row) = 0;

        virtual ~ScaleRowSampler() {

        };

    protected:
        const T *mSource;
        const int srcStride;
        const int inputWidth;
        const int inputHeight;
        T *mDestination;
        const int dstStride;
        const int outputWidth;
        const int outputHeight;
        const int components;

        float xScale;
        float yScale;
    };
}