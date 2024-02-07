//
// Created by Radzivon Bartoshyk on 06/02/2024.
//

#include "Adjustments.h"
#include "Eigen/Eigen"
#include "color/Gamut.h"
#include "color/Blend.h"
#include "color/eotf-inl.h"

namespace aire {

    using namespace aire::HWY_NAMESPACE;

    void colorMatrix(uint8_t *data, int stride, int width, int height, const Eigen::Matrix3f matrix) {
        for (int y = 0; y < height; ++y) {
            auto pixels = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;

            for (; x < width; ++x) {
                Eigen::Vector3f rgb;
                rgb << pixels[0], pixels[1], pixels[2];
                rgb /= 255.f;

                rgb = matrix * rgb;
                rgb = (rgb * 255.f).array().max(0.f).min(255.f);

                pixels[0] = rgb.x();
                pixels[1] = rgb.y();
                pixels[2] = rgb.z();

                pixels += 4;
            }
        }
    }

    void adjustment(uint8_t *data, int stride, int width, int height, float gain, float bias) {
        const Eigen::Vector3f fBias = {bias, bias, bias};
        const Eigen::Vector3f balance = {0.5f, 0.5f, 0.5f};
        for (int y = 0; y < height; ++y) {
            auto pixels = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;

            for (; x < width; ++x) {
                Eigen::Vector3f rgb;
                rgb << pixels[0], pixels[1], pixels[2];
                rgb /= 255.f;

                rgb = gain * (rgb - balance) + balance + fBias;
                rgb = (rgb * 255.f).array().max(0.f).min(255.f);

                pixels[0] = rgb.x();
                pixels[1] = rgb.y();
                pixels[2] = rgb.z();

                pixels += 4;
            }
        }
    }

    void saturation(uint8_t *data, int stride, int width, int height, float saturation) {
        const Eigen::Vector3f lumaPrimaries = {0.2125, 0.7154, 0.0721};
        for (int y = 0; y < height; ++y) {
            auto pixels = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;

            for (; x < width; ++x) {
                Eigen::Vector3f rgb;
                rgb << pixels[0], pixels[1], pixels[2];
                rgb /= 255.f;

                Eigen::Vector3f letf = {SRGBToLinear(rgb[0]), SRGBToLinear(rgb[1]), SRGBToLinear(rgb[2])};

                float luma = LinearSRGBTosRGB(letf.dot(lumaPrimaries));
                Eigen::Vector3f grayscale = {luma, luma, luma};
                rgb = (mix(grayscale, rgb, saturation) * 255.f).array().max(0.f).min(255.f);

                pixels[0] = rgb.x();
                pixels[1] = rgb.y();
                pixels[2] = rgb.z();

                pixels += 4;
            }
        }
    }

}