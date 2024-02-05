//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    void logarithmic(uint8_t *data, int stride, int width, int height, float exposure);
    void acesFilm(uint8_t *data, int stride, int width, int height, float exposure);
    void hejlBurgess(uint8_t *data, int stride, int width, int height, float exposure);
    void hableFilmic(uint8_t *data, int stride, int width, int height, float exposure);
    void acesHill(uint8_t *data, int stride, int width, int height, float exposure);
    void monochrome(uint8_t *data, int stride, int width, int height, float colors[4], float exposure);
    void exposure(uint8_t *data, int stride, int width, int height, float exposure);
    void whiteBalance(uint8_t *data, int stride, int width, int height, const float temperature = 5000, const float tint = 0.f);
}