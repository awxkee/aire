//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#pragma once

#include <vector>
#include "Eigen/Eigen"

namespace aire {
    Eigen::MatrixXf generateTentFilter(int N);
    Eigen::MatrixXf generateTentFilterNormalized(int N);
    std::vector<float> generate1DTentFilterKernelNormalized(int size);
    void tentBlur(uint8_t *data, int stride, int width, int height, int radius);
    void tentBlurF16(uint16_t *data, int stride, int width, int height, int radius);
}
