//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#pragma once

#include "cstdint"

namespace aire {
    void horizontalWindStagger(uint8_t *data, uint8_t *source, int stride, int width, int height,
                               float windStrength, int streamsCount, uint32_t clearColor);
}