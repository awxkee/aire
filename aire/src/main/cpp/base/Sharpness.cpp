//
// Created by Radzivon Bartoshyk on 06/02/2024.
//

#include "Sharpness.h"
#include "Eigen/Eigen"

namespace aire {
    using namespace std;
    using namespace Eigen;

    void applySharp(uint8_t *src, uint8_t *sharpenMask, int stride, int width, int height, const float intensity) {
        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(src) + y * stride);
            for (int x = 0; x < width; ++x) {
                auto mask = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(sharpenMask) + y * stride);
                int px = x * 4;
                float srcR = dst[px] / 255.f;
                float srcG = dst[px + 1] / 255.f;
                float srcB = dst[px + 2] / 255.f;
                dst[px] = clamp((srcR + intensity * (srcR - mask[px] / 255.f)) * 255.f, 0.f, 255.f);
                dst[px + 1] = clamp((srcG + intensity * (srcG - mask[px + 1] / 255.f)) * 255.f, 0.f, 255.f);
                dst[px + 2] = clamp((srcB + intensity * (srcB - mask[px + 2] / 255.f)) * 255.f, 0.f, 255.f);
            }
        }
    }

    void applyUnsharp(uint8_t *src, uint8_t *sharpenMask, int stride, int width, int height, const float intensity) {
        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(
                    reinterpret_cast<uint8_t *>(src) + y * stride);
            for (int x = 0; x < width; ++x) {
                auto mask = reinterpret_cast<uint8_t *>(
                        reinterpret_cast<uint8_t *>(sharpenMask) + y * stride);
                int px = x * 4;
                float srcR = dst[px] / 255.f;
                float srcG = dst[px + 1] / 255.f;
                float srcB = dst[px + 2] / 255.f;
                dst[px] = clamp((srcR - intensity * (srcR - mask[px] / 255.f)) * 255.f, 0.f, 255.f);
                dst[px + 1] = clamp((srcG - intensity * (srcG - mask[px + 1] / 255.f)) * 255.f, 0.f, 255.f);
                dst[px + 2] = clamp((srcB - intensity * (srcB - mask[px + 2] / 255.f)) * 255.f, 0.f, 255.f);
            }
        }
    }

    Eigen::Matrix3f generateSharpenKernel() {
        Eigen::Matrix3f kernel;
        kernel << 0, -1, 0, -1, 5, -1, 0, -1, 0;
        kernel /= kernel.sum();
        return kernel;
    }

    Eigen::MatrixXf generateUnsharpKernel() {
        Eigen::MatrixXf kernel(5, 5);
        float sigma = 7.f;
        int center = 5 / 2;

        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                int x = i - center;
                int y = j - center;
                kernel(i, j) = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
            }
        }
        kernel /= kernel.sum();
        return kernel;
    }

}