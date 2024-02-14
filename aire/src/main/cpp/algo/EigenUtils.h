//
// Created by Radzivon Bartoshyk on 09/02/2024.
//

#pragma once

#include "Eigen/Eigen"

inline
static uint32_t packRGBA(Eigen::Vector4i color) {
    uint32_t rgba = (color.w() << 24) | (color.z() << 16) | (color.y() << 8) | color.x();
    return rgba;
}

inline
static Eigen::Vector4i unpackRGBA(uint32_t color) {
    uint8_t a = (color >> 24) & 0xFF;
    uint8_t b = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t r = color & 0xFF;
    Eigen::Vector4i vec = {r, g, b, a};
    return vec;
}

static Eigen::Vector4i packERGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    Eigen::Vector4i vec = {r, g, b, a};
    return vec;
}

static Eigen::Vector3i rgbToHSV(const Eigen::Vector3i &rgb) {
    double minVal = rgb.minCoeff();
    double maxVal = rgb.maxCoeff();
    double delta = maxVal - minVal;

    double hue = 0.0;
    double saturation = (maxVal > 0.0) ? delta / maxVal : 0.0;
    double value = maxVal;

    if (delta > 0.0) {
        if (maxVal == rgb[0]) { // Red is dominant color
            hue = 60.0 * fmod(((rgb[1] - rgb[2]) / delta), 6.0);
        } else if (maxVal == rgb[1]) { // Green is dominant color
            hue = 60.0 * (((rgb[2] - rgb[0]) / delta) + 2.0);
        } else { // Blue is dominant color
            hue = 60.0 * (((rgb[0] - rgb[1]) / delta) + 4.0);
        }
    }

    if (hue < 0.0) {
        hue += 360.0;
    }

    return Eigen::Vector3i(hue, saturation, value);
}