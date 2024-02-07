//
// Created by Radzivon Bartoshyk on 07/02/2024.
//

#pragma once

#include <cstdint>
#include <vector>

namespace aire {
    class Convolve1Db16 {

    public:
        Convolve1Db16(const std::vector<float> &horizontal, const std::vector<float> &vertical) : horizontal(horizontal), vertical(vertical) {

        }

        void convolve(uint16_t *data, const int stride, const int width, const int height);

    private:
        const std::vector<float> horizontal;
        const std::vector<float> vertical;

        void horizontalPass(std::vector<uint16_t> &transient,
                            uint16_t *data, int stride,
                            int y, int width,
                            int height);

        void verticalPass(std::vector<uint16_t> &transient,
                          uint16_t *data, int stride,
                          int y, int width,
                          int height);
    };
}