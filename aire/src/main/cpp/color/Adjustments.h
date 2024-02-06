//
// Created by Radzivon Bartoshyk on 06/02/2024.
//

#pragma once

#include <cstdint>
#include "Eigen/Eigen"

namespace aire {
    void colorMatrix(uint8_t *data, int stride, int width, int height, const Eigen::Matrix3f matrix);
    void saturation(uint8_t *data, int stride, int width, int height, float saturation);
    void adjustment(uint8_t *data, int stride, int width, int height, float gain, float bias);
}