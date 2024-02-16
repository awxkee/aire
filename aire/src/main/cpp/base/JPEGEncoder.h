//
// Created by Radzivon Bartoshyk on 16/02/2024.
//

#pragma once

#include <cstdint>
#include <vector>

namespace aire {
    class JPEGEncoder {
    public:
        JPEGEncoder(uint8_t *data, int stride, int width, int height) : data(data), stride(stride),
                                                                        width(width), height(height) {

        }

        std::vector<uint8_t > encode();

        void setQuality(int mQuality) {
            this->quality = mQuality;
        }

    private:
        int quality = 81;
        uint8_t *data;
        const int stride;
        const int width;
        const int height;
    };
}