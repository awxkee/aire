//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#include "Halftone.h"

namespace aire {
    template<class V>
    void
    generateHalftoneHorizontalLines(std::vector<V> &data, int stride, int width, int height,
                                    int lineSize, int lineSpacing, V lineColor, V alpha) {
        for (int y = 0; y < height; y += lineSize + lineSpacing) {
            for (int j = y; j < lineSize + y && j < height; ++j) {
                auto dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data.data()) +
                                                 j * stride);
                for (int x = 0; x < width; ++x) {
                    int ps = x * 4;
                    dst[ps] = lineColor;
                    dst[ps + 1] = lineColor;
                    dst[ps + 2] = lineColor;
                    dst[ps + 3] = alpha;
                }
            }
        }
    }

    template void
    generateHalftoneHorizontalLines(std::vector<uint8_t> &data, int stride, int width, int height,
                                    int lineSize, int lineSpacing, uint8_t lineColor,
                                    uint8_t alpha);

}