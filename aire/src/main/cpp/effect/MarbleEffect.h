//
// Created by Radzivon Bartoshyk on 03/02/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    void marbleEffect(uint8_t *data, int stride, int width, int height, float intensity,
                      float turbulence, float amplitude);
}