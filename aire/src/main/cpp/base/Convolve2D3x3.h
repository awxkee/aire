//
// Created by Radzivon Bartoshyk on 06/02/2024.
//

#pragma once

#include "Eigen/Eigen"

namespace aire {
    class Convolve2D3x3 {
    public:
        Convolve2D3x3(Eigen::Matrix3f matrix): matrix(matrix) {

        }
        void convolve(uint8_t *data, int stride, int width, int height);

    private:
        const Eigen::Matrix3f matrix;
    };
}
