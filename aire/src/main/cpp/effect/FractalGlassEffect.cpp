//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#include "FractalGlassEffect.h"
#include <vector>
#include "MathUtils.hpp"

namespace aire {
    void fractalGlassEffect(uint8_t *data, int stride, int width, int height, float glassSize, float amplitude) {
        std::vector<uint8_t> transient(stride * height);
        std::vector<uint8_t> displacement(width * height);
        int displacementWidth = width * glassSize;
        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(displacement.data()) +
                    y * width);
            int currentDisplacementWidth = 0;
            for (int x = 0; x < width; ++x, ++currentDisplacementWidth) {
                float px = float(currentDisplacementWidth) / float(displacementWidth);
                float intensity = px * 255.f;
                dst[x] = static_cast<uint8_t >(intensity);
                if (currentDisplacementWidth >= displacementWidth) {
                    currentDisplacementWidth = 0;
                }
            }
        }
        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(transient.data()) +
                    y * stride);
            auto displacementSource = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(displacement.data()) +
                    y * width);
            for (int x = 0; x < width; ++x) {
//                float dx = 20.f * sin(2 * M_PI * y / height * 8.f);
//                float dy = intensity * cos(2 * M_PI * x / width);

                float displ = displacementSource[x] / 255.f * (width * amplitude);

                int newX = clamp(static_cast<int>((displ + x)) % width, 0, width - 1);
                int newY = y;

                auto src = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(data) +
                        newY * stride);

                dst[0] = src[newX * 4];
                dst[1] = src[newX * 4 + 1];
                dst[2] = src[newX * 4 + 2];
                dst[3] = src[newX * 4 + 3];
                dst += 4;
            }
        }

        std::copy(transient.begin(), transient.end(), data);
    }
}