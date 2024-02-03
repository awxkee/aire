//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    void removeShadows(uint8_t *src, int stride, int width, int height, int kernelSize);
}