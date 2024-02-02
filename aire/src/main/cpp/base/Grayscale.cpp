//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Grayscale.h"

namespace aire {
    template<class T>
    void
    grayscale(T *pixels, T *destination, int stride, int width, int height, const float rPrimary,
              const float gPrimary, const float bPrimary) {
        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(destination) + y * stride);
            auto src = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(pixels) + y * stride);
            for (int x = 0; x < width; ++x) {
                T pixel = rPrimary * src[0] + gPrimary * src[1] + bPrimary * src[2];
                dst[0] = pixel;
                dst[1] = pixel;
                dst[2] = pixel;
                dst[3] = src[3];
                src += 4;
                dst += 4;
            }
        }
    }

    template void
    grayscale(uint8_t *pixels, uint8_t *destination, int stride, int width, int height,
              const float rPrimary,
              const float gPrimary, const float bPrimary);
}