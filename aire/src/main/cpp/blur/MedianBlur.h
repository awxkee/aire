//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#ifndef JXLCODER_MEDIANBLUR_H
#define JXLCODER_MEDIANBLUR_H

#include <cstdint>

namespace aire {

    enum MedianSelector {
        MEDIAN_QUICK_SELECT = 1,
        MEDIAN_WIRTH = 2,
        MEDIAN_NTH_ELEMENT = 3
    };

    void medianBlurChannel(uint8_t *data, const int width, const int height, const int radius);

    void
    medianBlur(uint8_t *data, const int stride, const int width, const int height, const int radius);
}

#endif //JXLCODER_MEDIANBLUR_H
