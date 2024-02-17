//
// Created by Radzivon Bartoshyk on 31/01/2024.
//


#include "algo/support-inl.h"
#include "base/Convolve1Db16.h"
#include "base/Convolve2D.h"
#include "BoxBlur.h"
#include "Eigen/Eigen"
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
        convolve1D(data, stride, width, height, kernel, kernel);
    }

    void boxBlurF16(uint16_t *data, int stride, int width, int height, int radius) {
        const auto kernel = generateBoxKernel(radius);
        Convolve1Db16 convolution(kernel, kernel);
        convolution.convolve(data, stride, width, height);
    }

    std::vector<float> generateBoxKernel(int radius) {
        if (radius < 0) {
            std::string err = "Radius must be a non-negative integer but received " + std::to_string(radius);
            throw AireError(err);
        }
        int kernelSize = 2 * radius + 1;
        std::vector<float> boxKernel(kernelSize, 1.0 / float(kernelSize));
        return std::move(boxKernel);
    }

    Eigen::MatrixXf generateBoxKernel2D(const int radius) {
        if (radius < 0) {
            std::string err = "Radius must be a non-negative integer but received " + std::to_string(radius);
            throw AireError(err);
        }
        const int kernelSize = 2 * radius + 1;
        Eigen::MatrixXf kernel = Eigen::MatrixXf::Constant(kernelSize, kernelSize, 1.0f / static_cast<float>(kernelSize));
        return kernel;
    }

}