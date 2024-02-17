//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#pragma once

#include <cstdint>
#include <vector>
#include "Eigen/Eigen"

namespace aire {
    template<class T>
    void dilate(T *pixels, T *destination, int width, int height,
                std::vector<std::vector<int>> &kernel);

    template<class T>
    void dilateRGBA(T *pixels, T *destination, int stride, int width, int height,
                    Eigen::MatrixXi &kernel);

}