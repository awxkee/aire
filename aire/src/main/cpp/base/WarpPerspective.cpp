//
// Created by Radzivon Bartoshyk on 15/02/2024.
//

#include "WarpPerspective.h"
#include "EigenUtils.h"
#include "MathUtils.hpp"

namespace aire {

    WarpResult WarpPerspective::apply() {
        auto boundingBox = computeBoundingBox(perspective, width, height);

        double minX = boundingBox.col(0).minCoeff();
        double minY = boundingBox.col(1).minCoeff();
        double maxX = boundingBox.col(0).maxCoeff();
        double maxY = boundingBox.col(1).maxCoeff();

        int newWidth = static_cast<int>(std::ceil(maxX - minX));
        int newHeight = static_cast<int>(std::ceil(maxY - minY));
        int dstStride = computeStride(newWidth, sizeof(uint8_t), 4);

        std::vector<uint8_t > dst(dstStride * newHeight);

        return {
            .result = dst,
            .stride = dstStride,
            .width = newWidth,
            .height = newHeight
        };
    }

} // aire