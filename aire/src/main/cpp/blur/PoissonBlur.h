//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include <cstdint>
#include <vector>

namespace aire {
    void poissonBlur(uint8_t* data, int stride, int width, int height, int radius);
    std::vector<float> generatePoissonBlur(int radius);
}
