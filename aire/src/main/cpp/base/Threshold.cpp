//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Threshold.h"

namespace aire {
    template<class V>
    void threshold(V *pixels, int width, int height, V thresholdLevel, V max, V min) {
        for (int y = 0; y < height; ++y) {
            auto data = reinterpret_cast<V*>(reinterpret_cast<uint8_t*>(pixels) + y * width);
            for (int x = 0; x < width; ++x) {
                if (data[0] > thresholdLevel) {
                    data[0] = max;
                } else {
                    data[0] = min;
                }
                data += 1;
            }
        }
    }

    template
    void threshold(uint8_t *pixels, int width, int height, uint8_t thresholdLevel, uint8_t max, uint8_t min);
}