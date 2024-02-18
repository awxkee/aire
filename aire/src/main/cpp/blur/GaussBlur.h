//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    void gaussBlurU8(uint8_t *data, int stride, int width, int height, int radius, float sigma);

    void gaussBlurF16(uint16_t *data, int stride, int width, int height, int radius, float sigma);
}
