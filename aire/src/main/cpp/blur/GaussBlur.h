//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#ifndef JXLCODER_GAUSSBLUR_H
#define JXLCODER_GAUSSBLUR_H

#include <cstdint>

namespace aire {
    void gaussBlurU8(uint8_t *data, int stride, int width, int height, float radius, float sigma);
    template<class V>
    void gaussianBlur(V *data, int stride, int width, int height, float radius, float sigma);
}

#endif //JXLCODER_GAUSSBLUR_H
