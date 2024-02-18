//
// Created by Radzivon Bartoshyk on 05/02/2024.
//

#include "Vibrance.h"
#include <algorithm>
#include "fast_math-inl.h"
#include "MathUtils.hpp"

namespace aire {
    void vibrance(uint8_t *pixels, int stride, int width, int height, float vibrance) {
#pragma omp parallel for num_threads(3) schedule(dynamic)
        for (int y = 0; y < height; ++y) {
            auto data = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(pixels) + y * stride);
            int x = 0;

            for (; x < width; ++x) {
                int red = data[0];
                int green = data[1];
                int blue = data[2];

                int avgIntensity = (red + green + blue) / 3;
                int mx = max3(red, green, blue);
                int vibranceBoost = std::clamp((float(mx) - avgIntensity) * vibrance, -255.f, 255.f);

                data[0] = std::clamp(red + vibranceBoost, 0, 255);
                data[1] = std::clamp(green + vibranceBoost, 0, 255);
                data[2] = std::clamp(blue + vibranceBoost, 0, 255);
                data += 4;
            }
        }
    }
}