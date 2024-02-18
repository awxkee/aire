//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#include "Gamut.h"
#include "eotf-inl.h"

namespace aire {
    using namespace aire::HWY_NAMESPACE;
    void bitmapToXYZ(uint8_t *data, int stride, float *xyzBitmap, int xyzStride, int width, int height, TransferFunction function,
                     Eigen::Matrix3f conversionMatrix) {
#pragma omp parallel for num_threads(2) schedule(dynamic)
        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto dst = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(xyzBitmap) + xyzStride * y);
            for (int x = 0; x < width; ++x) {
                float r, g, b;
                r = src[0];
                g = src[1];
                b = src[2];
                if (function == TRANSFER_SRGB) {
                    r = SRGBToLinear(r / 255.f);
                    g = SRGBToLinear(g / 255.f);
                    b = SRGBToLinear(b / 255.f);
                }
                Eigen::Vector3f vec3 = {r, g, b};
                auto result = conversionMatrix * vec3;
                dst[0] = result.x();
                dst[1] = result.y();
                dst[2] = result.z();
                dst += 3;
                src += 4;
            }
        }
    }

    void xyzToBitmap(uint8_t *data, int stride, float *xyzBitmap, int xyzStride, int width, int height, TransferFunction function,
                     Eigen::Matrix3f conversionMatrix) {
#pragma omp parallel for num_threads(2) schedule(dynamic)
        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(xyzBitmap) + xyzStride * y);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + stride * y);
            for (int x = 0; x < width; ++x) {
                float r, g, b;
                r = src[0];
                g = src[1];
                b = src[2];
                if (function == TRANSFER_SRGB) {
                    r = LinearSRGBTosRGB(r);
                    g = LinearSRGBTosRGB(g);
                    b = LinearSRGBTosRGB(b);
                }
                Eigen::Vector3f vec3 = {r, g, b};
                auto result = (conversionMatrix * vec3 * 255.f).array().max(0.f).min(255.f);
                dst[0] = result.x();
                dst[1] = result.y();
                dst[2] = result.z();
                src += 3;
                dst += 4;
            }
        }
    }
}