//
// Created by Radzivon Bartoshyk on 06/02/2024.
//

#pragma once

#include <cstdint>
#include "Eigen/Eigen"

namespace aire {
    Eigen::Matrix3f generateSharpenKernel();
    void applySharp(uint8_t *src, uint8_t *sharpenMask, int stride, int width, int height, const float intensity);
    void applyUnsharp(uint8_t *src, uint8_t *sharpenMask, int stride, int width, int height, const float intensity);
}