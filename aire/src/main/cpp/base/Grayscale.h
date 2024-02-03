//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#pragma once

#include <valarray>

namespace aire {
    void
    grayscale(uint8_t *pixels, uint8_t *destination, int stride, int width, int height,
              const float rPrimary = 0.299f,
              const float gPrimary = 0.587f, const float bPrimary = 0.114f);
}