//
// Created by Radzivon Bartoshyk on 06/02/2024.
//

#pragma once

#include "FF2DWorkspace.h"
#include "Eigen/Eigen"

namespace aire {
    class Convolve2D {
    public:
        Convolve2D(Eigen::MatrixXf matrix) : matrix(matrix) {

        }

        void convolve(uint8_t *data, int stride, int width, int height);

    private:
        const Eigen::MatrixXf matrix;

//        void applyChannel(FF2DWorkspace *workspace, uint8_t *data, const int stride, const int chanIndex, const int width,
//                          const int height);

//        void fftConvolve(uint8_t *data, int stride, int width, int height);
        void bruteForceConvolve(uint8_t *data, int stride, int width, int height);
    };
}
