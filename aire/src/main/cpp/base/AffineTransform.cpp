//
// Created by Radzivon Bartoshyk on 15/02/2024.
//

#include "AffineTransform.h"
#include "scale/sampler.h"
#include <algorithm>

namespace aire {

    using namespace std;

    void AffineTransform::apply(uint8_t *destination, int dstStride, int newWidth, int newHeight) {
#pragma omp parallel for num_threads(4) schedule(dynamic)
        for (int newY = 0; newY < newHeight; ++newY) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination) + newY * dstStride);
            for (int newX = 0; newX < newWidth; ++newX) {

                int srcY = newY;
                int srcX = newX;

                Eigen::Vector3f point = {srcX, srcY, 0.f};
                point = transform * point;

                srcX = std::floor(point.x());
                srcY = std::floor(point.y());

                if (srcX < 0 || srcX >= width || srcY < 0 || srcY >= height) {
                    continue;
                }

                int x2 = std::clamp(srcX + 1, 0, width - 1);
                int y2 = std::clamp(srcY + 1, 0, height - 1);

                float dx = (float) point.x() - static_cast<float>(srcX);
                float dy = (float) point.y() - static_cast<float>(srcY);

                auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + srcY * stride);
                auto src2 = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y2 * stride);

                const int components = 4;
                const int dstX = newX * components;

                for (int c = 0; c < 4; ++c) {
                    float c1 = static_cast<float>(src[srcX * components + c]);
                    float c2 = static_cast<float>(src[x2 * components + c]);
                    float c3 = static_cast<float>(src2[srcX * components + c]);
                    float c4 = static_cast<float>(src2[x2 * components + c]);

                    float result = blerp(c1, c2, c3, c4, dx, dy);
                    float f = std::clamp(std::ceil(result), 0.0f, 255.f);
                    dst[dstX + c] = static_cast<uint8_t>(f);
                }
            }
        }
    }
}