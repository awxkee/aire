//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#pragma once

inline __attribute__((flatten))
float blendColor(const float c, const float c1, const float a) {
    float r = c1 * a + c * (1.0f - a);
    return r;
}

inline __attribute__((flatten))
float blendColor(const float c1, const float c, const float ca, const float cb) {
    float r = c1 * ca + c * cb * (1.0f - ca);
    return r;
}