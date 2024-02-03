//
// Created by Radzivon Bartoshyk on 01/02/2024.
//


#pragma once

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_common.hpp"

static const glm::vec2 IlluminantD65 = {0.31272, 0.32903};

static const glm::mat3x2 SRGBPrimaries = {{0.640f, 0.330f},
                                          {0.300f, 0.600f},
                                          {0.150f, 0.060f}};

static glm::vec3 XyToXYZ(const float x, const float y) {
    glm::vec3 ret(0.f, 0.f, 0.f);
    ret[0] = x / y;
    ret[1] = 1.0f;
    ret[2] = (1.0 - x - y) / y;
    return ret;
}

static const glm::mat3x3 getPrimariesXYZ(const glm::mat3x2 primariesXy) {
    glm::mat3x3 ret;
    ret[0] = XyToXYZ(primariesXy[0][0], primariesXy[0][1]);
    ret[1] = XyToXYZ(primariesXy[1][0], primariesXy[1][1]);
    ret[2]  = XyToXYZ(primariesXy[2][0], primariesXy[2][1]);
    return ret;
}

static const glm::vec3 getWhitePoint(const glm::vec2 whitePoint) {
    return XyToXYZ(whitePoint[0], whitePoint[1]);
}

static glm::mat3x3
GamutRgbToXYZ(const glm::mat3x2 primariesXy, const glm::vec2 whitePoint) {
    glm::mat3x3 xyZMatrix = getPrimariesXYZ(primariesXy);
    const glm::vec3 whiteXyz = getWhitePoint(whitePoint);
    const glm::mat3x3 inverted = glm::inverse(xyZMatrix);
    const glm::vec3 s = inverted * whiteXyz;
    xyZMatrix = glm::transpose(xyZMatrix);
    const glm::mat3x3 ret = {
            xyZMatrix[0] * s,
            xyZMatrix[1] * s,
            xyZMatrix[2] * s,
    };
    return ret;
}

inline __attribute__((flatten))
float SRGBToLinear(float v) {
    if (v <= 0.045f) {
        return v / 12.92f;
    } else {
        return pow((v + 0.055f) / 1.055f, 2.4f);
    }
}
