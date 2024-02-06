//
// Created by Radzivon Bartoshyk on 05/02/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    void dehaze(uint8_t *src, int stride, int width, int height, const int radius, const float omega = 0.45);
}