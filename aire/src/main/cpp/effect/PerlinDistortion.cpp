//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#include "PerlinDistortion.h"
#include "algo/PerlinNoise.hpp"
#include <algorithm>
#include <chrono>

namespace aire {

    using namespace std;

    void perlinDistortion(uint8_t *data, int stride, int width, int height, float intensity, float turbulence, float amplitude) {
        const siv::PerlinNoise::seed_type seed = std::chrono::system_clock::now().time_since_epoch().count();

        const siv::PerlinNoise perlin{seed};

        std::vector<uint8_t> output(stride * height);

        float sinTable[256];
        float cosTable[256];
        for (int i = 0; i < 256; i++) {
            float angle = 2 * M_PI * i / 256.f * turbulence;
            sinTable[i] = (float) (-sin(angle)) * -(intensity * width);
            cosTable[i] = (float) (cos(angle)) * (intensity * width);
        }

        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(output.data()) +
                                                   y * stride);
            for (int x = 0; x < width; ++x) {
                int px = x * 4;

                float noise = perlin.octave2D_01(float(x) * 0.01f, float(y) * 0.01f, 4) * amplitude;

                int displacement = clamp((int) (127 * (1 + noise)), 0, 255);

                int sourceX = clamp(static_cast<int>(floor(x + sinTable[displacement])), 0,
                                    width - 1);
                int sourceY = clamp(static_cast<int>(floor(y + cosTable[displacement])), 0,
                                    height - 1);

                auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) +
                                                       sourceY * stride);

                int sc = sourceX * 4;

                dst[px] = clamp((src[sc] + 127 * noise) / 1.25f, 0.f, 255.f);
                dst[px + 1] = clamp((src[sc + 1] + 127 * noise) / 1.25f, 0.f, 255.f);
                dst[px + 2] = clamp((src[sc + 2] + 127 * noise) / 1.25f, 0.f, 255.f);
                dst[px + 3] = src[px + 3];
            }
        }

        std::copy(output.begin(), output.end(), data);
    }
}