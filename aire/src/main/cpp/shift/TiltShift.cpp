//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#include "TiltShift.h"
#include <algorithm>
#include <vector>
#include "color/Blend.h"
#include "jni/JNIUtils.h"
#include <thread>
#include "concurrency.hpp"

namespace aire {

    void verticalLinearTiltShift(uint8_t *data, uint8_t *source, std::vector<uint8_t> &blurred, int stride, int width, int height,
                                 float anchorX, float radius) {
        const float availableDistance = std::sqrt(width * width);
        const float minDistance = availableDistance * radius;
        const int centerX = width * anchorX;

        // Y always considered to be 0

        concurrency::parallel_for(4, height, [&](int y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(source) + y * stride);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto blurredSource = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(blurred.data()) + y * stride);
            for (int x = 0; x < width; ++x) {
                const float dx = x - centerX;
                float currentDistance = std::sqrt(dx * dx);

                auto fr = currentDistance / minDistance;
                auto fraction = std::clamp(fr * fr * fr, 0.0f, 1.0f);
                auto blurredA = blurredSource[3];
                auto sourceA = src[3];
                auto newA = blurredA * fraction;

                Eigen::Vector3f mSource = {src[0], src[1], src[2]};
                mSource /= 255.f;
                Eigen::Vector3f blurSource = {blurredSource[0], blurredSource[1], blurredSource[2]};
                blurSource /= 255.f;
                auto blended = mix(mSource, blurSource, newA / 255.f);
                auto cmp = (blended * 255.f).array().min(255.f).max(0.f);

                dst[0] = cmp.x();
                dst[1] = cmp.y();
                dst[2] = cmp.z();
                dst[3] = sourceA;
                src += 4;
                dst += 4;
                blurredSource += 4;
            }
        });
    }

    void horizontalLinearTiltShift(uint8_t *data, uint8_t *source, std::vector<uint8_t> &blurred, int stride, int width, int height,
                                   float anchorY, float radius) {
        const float availableDistance = std::sqrt(height * height);
        const float minDistance = availableDistance * radius;
        const int centerY = height * anchorY;

        // X always considered to be 0

        concurrency::parallel_for(4, height, [&](int y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(source) + y * stride);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto blurredSource = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(blurred.data()) + y * stride);
            for (int x = 0; x < width; ++x) {
                const float dy = y - centerY;
                float currentDistance = std::sqrt(dy * dy);

                auto fr = currentDistance / minDistance;
                auto fraction = std::clamp(fr * fr * fr, 0.0f, 1.0f);
                auto blurredA = blurredSource[3];
                auto sourceA = src[3];
                auto newA = blurredA * fraction;

                Eigen::Vector3f mSource = {src[0], src[1], src[2]};
                mSource /= 255.f;
                Eigen::Vector3f blurSource = {blurredSource[0], blurredSource[1], blurredSource[2]};
                blurSource /= 255.f;
                auto blended = mix(mSource, blurSource, newA / 255.f);
                auto cmp = (blended * 255.f).array().min(255.f).max(0.f);

                dst[0] = cmp.x();
                dst[1] = cmp.y();
                dst[2] = cmp.z();
                dst[3] = sourceA;
                src += 4;
                dst += 4;
                blurredSource += 4;
            }
        });
    }

    void tiltShift(uint8_t *data, uint8_t *source, std::vector<uint8_t> &blurred, int stride, int width, int height,
                   float anchorX, float anchorY, float radius) {
        const float availableDistance = std::sqrt(height * height + width * width);
        const float minDistance = availableDistance * radius;
        const int centerX = width * anchorX;
        const int centerY = height * anchorY;

        concurrency::parallel_for(4, height, [&](int y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(source) + y * stride);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto blurredSource = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(blurred.data()) + y * stride);
            for (int x = 0; x < width; ++x) {
                const float dx = x - centerX;
                const float dy = y - centerY;
                float currentDistance = std::sqrt(dx * dx + dy * dy);

                auto fr = currentDistance / minDistance;
                auto fraction = std::clamp(fr * fr * fr, 0.0f, 1.0f);
                auto blurredA = blurredSource[3];
                auto sourceA = src[3];
                auto newA = blurredA * fraction;

                Eigen::Vector3f mSource = {src[0], src[1], src[2]};
                mSource /= 255.f;
                Eigen::Vector3f blurSource = {blurredSource[0], blurredSource[1], blurredSource[2]};
                blurSource /= 255.f;
                auto blended = mix(mSource, blurSource, newA / 255.f);
                auto cmp = (blended * 255.f).array().min(255.f).max(0.f);

                dst[0] = cmp.x();
                dst[1] = cmp.y();
                dst[2] = cmp.z();
                dst[3] = sourceA;
                src += 4;
                dst += 4;
                blurredSource += 4;
            }
        });
    }
}