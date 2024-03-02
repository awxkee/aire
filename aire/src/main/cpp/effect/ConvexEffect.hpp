//
// Created by Radzivon Bartoshyk on 01/03/2024.
//

#pragma once

#include <vector>
#include <cstdint>
#include "blur/ShgStackBlur.h"
#include "color/Blend.h"
#include "Eigen/Eigen"
#include "algo/BezierInterpolator.hpp"
#include "MathUtils.hpp"

namespace aire {
    class ConvexEffect {
    public:
        ConvexEffect(const float strength) : strength(strength) {

        }

        void apply(uint8_t *data, int stride, int width, int height) {
            std::vector<uint8_t> transient(stride * height);
            std::vector<uint8_t> blurred(stride * height);
            std::copy(data, data + height * stride, blurred.begin());
            shgStackBlur(blurred.data(), width, height, 27);
            const float centerX = width / 2.0;
            const float centerY = height / 2.0;

            const float maxDistance = std::min(width / 2.0f, height / 2.0f);

            auto interpolator = getEaseOutCubicInterpolator();

            const float preparedStrength = strength / 5000.f;

            for (int y = 0; y < height; ++y) {
                auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(transient.data()) + y * stride);
                for (int x = 0; x < width; ++x) {
                    float dx = static_cast<float>(x) - centerX;
                    float dy = static_cast<float>(y) - centerY;
                    float distance = std::sqrt(dx * dx + dy * dy);

                    float factor = 1.0 - preparedStrength * distance;
                    int newX = static_cast<int>(centerX + dx * factor);
                    int newY = static_cast<int>(centerY + dy * factor);

                    newX = std::clamp(static_cast<int>(newX), 0, width - 1);
                    newY = std::clamp(static_cast<int>(newY), 0, height - 1);
                    auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + newY * stride);
                    auto blurSrc = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(blurred.data()) + newY * stride);

                    Eigen::Vector4f bg = {src[newX * 4], src[newX * 4 + 1], src[newX * 4 + 2], src[newX * 4 + 3]};
                    Eigen::Vector4f fg = {blurSrc[newX * 4], blurSrc[newX * 4 + 1], blurSrc[newX * 4 + 2], blurSrc[newX * 4 + 3]};
                    bg /= 255.f;
                    fg /= 255.f;

                    float minState = interpolator.getInterpolation(std::clamp(1.f - distance / maxDistance, 0.f, 1.f));

                    Eigen::Vector4f color = mix(fg, bg, minState);
                    Eigen::Vector<uint8_t, 4> iColor = (color * 255.f).array().min(255.f).max(0.f).cast<uint8_t>();

                    dst[0] = iColor.x();
                    dst[1] = iColor.y();
                    dst[2] = iColor.z();
                    dst[3] = iColor.w();
                    dst += 4;
                }
            }
            std::copy(transient.begin(), transient.end(), data);
        }

    private:
        const float strength;
    };
}