//
// Created by Radzivon Bartoshyk on 07/02/2024.
//

#include "LUT8.h"
#include <algorithm>
#include "concurrency.hpp"

namespace aire {

    using namespace std;

    void LUT8::apply(uint8_t *data, int stride, int width, int height) const {
        concurrency::parallel_for(2, height, [&](int y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(data) + y * stride);
            for (int x = 0; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(data) + y * stride);
                int px = x * 4;
                dst[px] = this->table[clamp(src[px], uint8_t(0), uint8_t(255))];
                dst[px + 1] = this->table[clamp(src[px + 1], uint8_t(0), uint8_t(255))];
                dst[px + 2] = this->table[clamp(src[px + 2], uint8_t(0), uint8_t(255))];
            }
        });
    }
}