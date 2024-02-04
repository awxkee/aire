//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    void fractalGlassEffect(uint8_t *data, int stride, int width, int height, float glassSize, float amplitude);
}