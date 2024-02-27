//
// Created by Radzivon Bartoshyk on 27/02/2024.
//

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