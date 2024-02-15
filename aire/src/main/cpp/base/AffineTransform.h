//
// Created by Radzivon Bartoshyk on 15/02/2024.
//

#pragma once

#include "Eigen/Eigen"

namespace aire {
    class AffineTransform {
    public:
        AffineTransform(uint8_t *data, int stride, int width, int height) : data(data), stride(stride),
                                                                            width(width), height(height) {

        }

        void setTransform(Eigen::Affine3f mTransform) {
            this->transform = mTransform;
        }

        void apply(uint8_t* destination, int dstStride, int newWidth, int newHeight);

    private:
        Eigen::Affine3f transform = Eigen::Affine3f::Identity();
        uint8_t *data;
        const int stride;
        const int width;
        const int height;
    };
}