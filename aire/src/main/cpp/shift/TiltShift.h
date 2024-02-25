//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#pragma once

#include <cstdint>
#include <vector>

namespace aire {
    void tiltShift(uint8_t *data, uint8_t *source, std::vector<uint8_t> &blurred, int stride, int width,
                   int height, float anchorX, float anchorY, float radius);

    void horizontalTiltShift(uint8_t *data, uint8_t *source, std::vector<uint8_t> &blurred, int stride, int width, int height,
                             float anchorX, float anchorY, float radius, float angle);
}
