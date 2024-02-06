//
// Created by Radzivon Bartoshyk on 05/02/2024.
//

#include "DehazeDarkChannel.h"
#include <vector>
#include "Eigen/Eigen"
#include <queue>
#include "hwy/highway.h"
#include "MathUtils.hpp"

namespace aire {

    using namespace std;
    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    void
    getDarkChannel(const uint8_t *pSrc, std::vector<uint8_t> &tmp_vec, const int stride, const int width, const int height, int radius) {
        const ScalableTag<uint8_t> du;
        using VU = Vec<decltype(du)>;

        const int lanes = du.MaxLanes();

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t min_val = 255;

                uint8_t *darkImage = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(tmp_vec.data()) + width * i);

                for (int y = -radius; y <= radius; y++) {
                    const uint8_t *tmp = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(pSrc) + stride * clamp(y + i, 0, height - 1));
                    int x = -radius;

                    for (; x + lanes <= radius && x + j + lanes < width; x += lanes) {
                        int pos = clamp(x + j, 0, width - 1) * 4;
                        VU r, g, b, a;
                        LoadInterleaved4(du, &tmp[pos], r, g,b,a);
                        uint8_t possibleR = ExtractLane(MinOfLanes(du, r), 0);
                        uint8_t possibleG = ExtractLane(MinOfLanes(du, r), 0);
                        uint8_t possibleB = ExtractLane(MinOfLanes(du, r), 0);
                        min_val = std::min(min3(possibleR, possibleG, possibleB), min_val);
                    }

                    for (; x <= radius; x++) {
                        int pos = x + j;
                        if (pos >= width) {
                            break;
                        }
                        pos = clamp(pos, 0, width - 1) * 4;
                        uint8_t b = tmp[pos + 0];
                        uint8_t g = tmp[pos + 1];
                        uint8_t r = tmp[pos + 2];
                        uint8_t minpixel = min3(r, g, b);
                        min_val = std::min(minpixel, min_val);
                    }
                }
                darkImage[j] = min_val;
            }
        }
    }

    void getTransmission(uint8_t *pSrc, std::vector<uint8_t> &tmp_vec, float m_AtmosLight,
                         const int stride, const int width, const int height, float omega) {
        for (int y = 0; y < height; y++) {
            uint8_t *dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(pSrc) + stride * y);
            const uint8_t *darkImage = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(tmp_vec.data()) + width * y);
            for (int x = 0; x < width; x++) {
                int pos = x * 4;
                float r = dst[pos + 0], g = dst[pos + 1], b = dst[pos + 2];
                float minColor = std::min({r, g, b});
                float t0 = 1.0 - omega * (float(minColor) / float(m_AtmosLight));
                float t = std::max(1.0f - omega * (float(darkImage[x]) / float(m_AtmosLight)), t0);

                dst[pos + 0] = std::clamp(static_cast<int>(((r - m_AtmosLight) / t + m_AtmosLight)), int(0), int(255));
                dst[pos + 1] = std::clamp(static_cast<int>(((g - m_AtmosLight) / t + m_AtmosLight)), int(0), int(255));
                dst[pos + 2] = std::clamp(static_cast<int>(((b - m_AtmosLight) / t + m_AtmosLight)), int(0), int(255));
            }
        }
    }

    float getAtmosphericLightEstimate(std::vector<uint8_t> &darkImage, int width, int height) {
        std::priority_queue<uint8_t, vector<uint8_t>, std::greater<uint8_t>> TopValues;

        //find out the 0.1% highest pixels in the dark channel
        int TopAmounts = darkImage.size() * 0.01;
        float total = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                const uint8_t *src = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(darkImage.data()) + width * i);
                if (TopValues.size() < TopAmounts) {
                    TopValues.push(src[j]);
                    total += src[j];
                } else {
                    if (TopValues.top() >= src[j])
                        continue;
                    total -= TopValues.top();
                    total += src[j];
                    TopValues.pop();
                    TopValues.push(src[j]);
                }
            }
        }

        total /= TopAmounts;
        return total;
    }


    void dehaze(uint8_t *src, int stride, int width, int height, const int radius, const float omega) {
        vector<uint8_t> darkImage(width * height);
        getDarkChannel(src, darkImage, stride, width, height, radius);
        float atmosphereLight = getAtmosphericLightEstimate(darkImage, width, height);
        getTransmission(src, darkImage, atmosphereLight, stride, width, height, omega);
    }

}