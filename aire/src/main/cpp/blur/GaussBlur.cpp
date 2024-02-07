//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#include "GaussBlur.h"
#include <vector>
#include <thread>

#include <algorithm>
#include "MathUtils.hpp"
#include "base/Convolve1D.h"
#include "base/Convolve1Db16.h"

using namespace std;

namespace aire {

    void gaussBlurU8(uint8_t *data, int stride, int width, int height, float radius, float sigma) {
        vector<float> kernel = compute1DGaussianKernel(radius * 2 + 1, sigma);
        convolve1D(data, stride, width, height, kernel, kernel);
    }

    void gaussBlurF16(uint16_t *data, int stride, int width, int height, float radius, float sigma) {
        vector<float> kernel = compute1DGaussianKernel(radius * 2 + 1, sigma);
        Convolve1Db16 convolution(kernel, kernel);
        convolution.convolve(data, stride, width, height);
    }

}
