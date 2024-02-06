//
// Created by Radzivon Bartoshyk on 06/02/2024.
//

#include "Grain.h"
#include <random>

namespace aire {

    using namespace std;

    void grain(uint8_t *data, int stride, int width, int height, float intensity) {
        default_random_engine generator;
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
        normal_distribution<double> distribution(0, 127 * intensity);

        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(data) + y * stride);
            for (int x = 0; x < width; ++x) {

                int grain = distribution(generator);

                int px = x * 4;
                dst[px] = clamp(dst[px] + float(grain), 0.f, 255.f);
                dst[px + 1] = clamp(dst[px + 1] + float(grain), 0.f, 255.f);
                dst[px + 2] = clamp(dst[px + 2] + float(grain), 0.f, 255.f);
            }
        }
    }
}