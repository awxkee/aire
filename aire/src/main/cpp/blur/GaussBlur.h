//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    void gaussBlurU8(uint8_t *data, int stride, int width, int height, int radius, float sigma);

    void gaussBlurF16(uint16_t *data, int stride, int width, int height, int radius, float sigma);

    void gaussianApproximation2D(uint8_t *data, int stride, int width, int height, int radius);

    void gaussianApproximation3D(uint8_t *data, int stride, int width, int height, int radius);

    void gaussianApproximation4D(uint8_t *data, int stride, int width, int height, int radius);
}
