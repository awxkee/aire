//
// Created by Radzivon Bartoshyk on 31/01/2024.
//


#include "algo/support-inl.h"

#include "BoxBlur.h"
#include <vector>
#include <algorithm>
#include <math.h>
#include <thread>
#include "base/Convolve1D.h"
#include "jni/JNIUtils.h"

using namespace std;

namespace aire {

    void boxBlurU8(uint8_t *data, int stride, int width, int height, int radius) {
        const auto kernel = generateBoxKernel(radius);
        convolve1D(data, stride, width, height, kernel);
    }

    std::vector<float> generateBoxKernel(int radius) {
        if (radius < 0) {
            std::string err = "Radius must be a non-negative integer.";
            throw AireError(err);
        }
        int kernelSize = 2 * radius + 1;
        std::vector<float> boxKernel(kernelSize, 1.0 / float(kernelSize));
        return std::move(boxKernel);
    }


}