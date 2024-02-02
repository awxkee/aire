//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#pragma once

#include <vector>

namespace aire {
    std::vector<std::vector<float>> generateTentFilter(int N);
    std::vector<std::vector<float>> generateTentFilterNormalized(int N);
    std::vector<float> generate1DTentFilterKernelNormalized(int size);
    void tentBlur(uint8_t *data, int stride, int width, int height, int radius);
}
