//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#include "Glitch.h"
#include <random>
#include <algorithm>
#include <vector>
#include "halftone/Halftone.h"
#include "conversion/RGBAlpha.h"
#include "color/Blend.h"

using namespace std;

namespace aire {
    template<class V>
    void glitchEffect(V *data, int stride, int width, int height, float channelsShiftX,
                      float channelsShiftY, float corruptionSize, int corruptionCount,
                      float cShiftX, float cShiftY) {
        std::default_random_engine generator;
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(0, width);

        std::vector<V> transient(stride * height);
        std::copy(data, data + (height - 1) * stride, transient.begin());

        int shiftX = channelsShiftX * width;
        int shiftY = channelsShiftY * height;

        for (int y = 0; y < height; ++y) {
            int currentSourceY = y + shiftY;
            if (currentSourceY < 0 || currentSourceY >= height) {
                continue;
            }
            auto dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient.data()) +
                                             currentSourceY * stride);
            auto src = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            for (int x = 0; x < width; ++x) {
                int currentSourceX = x + shiftX;
                if (currentSourceX < 0 || currentSourceX >= width) {
                    continue;
                }
                int pos = currentSourceX * 4;
                dst[pos + 1] = src[x * 4 + 1];
                dst[pos + 2] = src[x * 4 + 2];
            }
        }
        std::vector<V> transient2(stride * height);
        std::copy(transient.begin(), transient.end(), transient2.begin());

        std::uniform_int_distribution<> start(0, width - 1);
        std::uniform_int_distribution<> startY(0, height - 1);
        std::uniform_int_distribution<> length(1, width - 1);

        std::uniform_int_distribution<> randomHeight(1,
                                                     std::max(int(ceil(height * corruptionSize)),
                                                              2));

        int corruptionShiftX = width * cShiftX;
        int corruptionShiftY = height * cShiftY;

        for (size_t i = 0, count = corruptionCount; i < count; i++) {
            int randomY = startY(generator);
            int randomX = start(generator);
            int rh = height * corruptionSize;
            int rl = length(generator);
            for (int z = randomY; z < randomY + rh && z < height; ++z) {
                for (int j = randomX; j < rl + width; ++j) {
                    if (j >= width) {
                        continue;
                    }
                    int sourceY = z + corruptionShiftY;
                    int sourceX = j + corruptionShiftX;
                    if (sourceY < 0 || sourceY >= height) {
                        continue;
                    }
                    if (sourceX < 0 || sourceX >= width) {
                        continue;
                    }

                    auto dst = reinterpret_cast<V *>(
                            reinterpret_cast<uint8_t *>(transient2.data()) + z * stride);
                    auto src = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient.data()) +
                                                     sourceY * stride);

                    dst[j * 4] = src[sourceX * 4];
                    dst[j * 4 + 1] = src[sourceX * 4 + 1];
                    dst[j * 4 + 2] = src[sourceX * 4 + 2];
                }
            }
        }

        std::vector<uint8_t> halftone(stride * height);
        generateHalftoneHorizontalLines(halftone, stride, width, height, 2, 2, uint8_t(0),
                                        uint8_t(255 * 0.15f));


        for (int y = 0; y < height; ++y) {
            auto halfSrc = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(halftone.data()) +
                                                 y * stride);
            auto dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient.data()) +
                                             y * stride);
            auto src = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient2.data()) +
                                             y * stride);
            for (int x = 0; x < width; ++x) {
                int px = x * 4;
                float cb = float((src[px + 3])) / 255.f;
                float ca = float((halfSrc[px + 3])) / 255.f;
                float newR = blendColor(halfSrc[px] / 255.f, src[px] / 255.f, ca, cb);
                float newG = blendColor(halfSrc[px + 1] / 255.f, src[px + 1] / 255.f, ca, cb);
                float newB = blendColor(halfSrc[px + 2] / 255.f, src[px + 2] / 255.f, ca, cb);
                dst[px] = newR * 255;
                dst[px + 1] = newG * 255;
                dst[px + 2] = newB * 255;
                dst[px + 3] = 255;
            }
        }

        std::copy(transient.begin(), transient.end(), data);
    }

    template void
    glitchEffect(uint8_t *data, int stride, int width, int height, float channelsShiftX,
                 float channelsShiftY, float corruptionSize, int corruptionCount,
                 float cShiftX, float cShiftY);
}