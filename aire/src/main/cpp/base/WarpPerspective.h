//
// Created by Radzivon Bartoshyk on 15/02/2024.
//

#pragma once

#include <cstdint>
#include "Eigen/Eigen"

namespace aire {

    struct WarpResult {
        std::vector<uint8_t> result;
        int stride;
        int width;
        int height;
    };

    class WarpPerspective {
    public:
        WarpPerspective(uint8_t *data, int stride, int width, int height) : data(data), stride(stride),
                                                                            width(width), height(height) {

        }

        WarpResult apply();

        void setTransform(Eigen::Matrix3f mTransform) {
            this->perspective = mTransform;
        }

    private:
        uint8_t *data;
        const int stride;
        const int width;
        const int height;
        Eigen::Matrix3f perspective;
    };

} // aire
