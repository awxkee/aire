//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#pragma once

#include "Eigen/Eigen"

inline __attribute__((flatten))
static float blendColor(const float c, const float c1, const float a) {
    float r = c1 * a + c * (1.0f - a);
    return r;
}

inline __attribute__((flatten))
static Eigen::Vector3f blendColor(const Eigen::Vector3f c, const Eigen::Vector3f c1, const Eigen::Vector3f a) {
    const Eigen::Vector3f ones = {1.0f, 1.0f, 1.0f};
    Eigen::Vector3f r = c1.array() * a.array() + c.array() * (ones.array() - a.array());
    return r;
}

inline __attribute__((flatten))
static Eigen::Vector3f mix(const Eigen::Vector3f& x, const Eigen::Vector3f& y, float a) {
//    a = std::max(0.0f, std::min(1.0f, a));
    return x * (1.0f - a) + y * a;
}

inline __attribute__((flatten))
static float mix(const float x, const float y, float a) {
    a = std::max(0.0f, std::min(1.0f, a));
    return x * (1.0f - a) + y * a;
}

static uint8_t blendColor(const uint8_t fg, const uint8_t bg, const uint8_t a) {
    return (fg * a + bg * (255 - a) + 127) / 255;
}

static uint8_t mergeAlpha(const uint8_t fg, const uint8_t bg) {
    return fg + (bg * (255 - fg) + 127) / 255;
}

static float mergeAlpha(const float fg, const float bg) {
    return fg + bg * (1.f - fg);
}

inline __attribute__((flatten))
static float blendColor(const float c1, const float c, const float ca, const float cb) {
    float r = c1 * ca + c * cb * (1.0f - ca);
    return r;
}