//
// Created by Radzivon Bartoshyk on 12/11/2023.
//

#pragma once

#include <cstdint>
#include <vector>

namespace aire {
    void
    NV21ToRGBA(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc, int yStride,
               const uint8_t *uv, int uvStride);

    void
    NV21ToRGB(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc, int yStride,
              const uint8_t *uv, int uvStride);

    void
    NV21ToBGR(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc, int yStride,
              const uint8_t *uv, int uvStride);
}