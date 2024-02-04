//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include <cstdint>
#include <vector>

namespace aire {
    void convolve1D(uint8_t *data, int stride, int width, int height, const std::vector<float>& horizontal, const std::vector<float> &vertical);
}