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
#include "base/Convolve2D.h"
#include "Eigen/Eigen"

using namespace std;

namespace aire {

    static Eigen::MatrixXf generate2DGaussianKernel(int size, double sigma) {
        Eigen::MatrixXf kernel2d(2 * size + 1, 2 * size + 1);
        for (int row = 0; row < kernel2d.rows(); row++) {
            for (int col = 0; col < kernel2d.cols(); col++) {
                double x = exp(-(row * row + col * col) / (2 * sigma * sigma));
                kernel2d(row, col) = x;
            }
        }
        float sum = kernel2d.sum();
        if (sum != 0.f) {
            kernel2d /= sum;
        }
        return kernel2d;
    }

    void gaussBlurU8(uint8_t *data, int stride, int width, int height, int radius, float sigma) {
        vector<float> kernel = compute1DGaussianKernel(radius * 2 + 1, sigma);
        convolve1D(data, stride, width, height, kernel, kernel);
    }

    void gaussBlurF16(uint16_t *data, int stride, int width, int height, int radius, float sigma) {
        vector<float> kernel = compute1DGaussianKernel(radius * 2 + 1, sigma);
        Convolve1Db16 convolution(kernel, kernel);
        convolution.convolve(data, stride, width, height);
    }

}
