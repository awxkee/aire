//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#include "WaterEffect.h"
#include <vector>
#include <algorithm>

namespace aire {
    void waterEffect(uint8_t *data, int stride, int width, int height, float fractionSize,
                     float frequencyX, float amplitudeX, float frequencyY, float amplitudeY) {
        std::vector<uint8_t> transient(stride * height);
        int xMove = width * fractionSize;
        int yMove = height * fractionSize;

#pragma omp parallel for num_threads(3) schedule(dynamic)
        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(transient.data()) +
                    y * stride);
            for (int x = 0; x < width; ++x) {
                float dx = xMove * sin(2 * M_PI * y / height * frequencyX);
                float dy = yMove * cos(2 * M_PI * x / width * frequencyY);
                if (amplitudeY != 0) {
                    dy *= amplitudeY;
                } else {
                    dy = 0;
                }
                if (amplitudeX != 0) {
                    dx *= amplitudeX;
                } else {
                    dx = 0;
                }

                int newX = std::clamp(static_cast<int>(x + dx), 0, width - 1);
                int newY = std::clamp(static_cast<int>(y + dy), 0, height - 1);

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