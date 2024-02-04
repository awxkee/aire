//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#pragma once

inline __attribute__((flatten))
static float blendColor(const float c, const float c1, const float a) {
    float r = c1 * a + c * (1.0f - a);
    return r;
}

static uint8_t blendColor(const uint8_t fg, const uint8_t bg, const uint8_t a) {
    return (fg * a + bg * (255 - a) + 127) / 255;
}

inline __attribute__((flatten))
static float blendColor(const float c1, const float c, const float ca, const float cb) {
    float r = c1 * ca + c * cb * (1.0f - ca);
    return r;
}