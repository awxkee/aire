//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    void waterEffect(uint8_t *data, int stride, int width, int height,
                     float fractionSize,
                     float frequencyX, float amplitudeX, float frequencyY, float amplitudeY);
}