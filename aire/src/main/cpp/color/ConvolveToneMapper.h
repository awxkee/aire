//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    void logarithmic(uint8_t *data, int stride, int width, int height, float exposure);
    void acesFilm(uint8_t *data, int stride, int width, int height, float exposure);
}