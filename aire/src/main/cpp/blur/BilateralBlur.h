//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#ifndef JXLCODER_BILATERALBLUR_H
#define JXLCODER_BILATERALBLUR_H

#include <cstdint>
#include "FastBilateral.h"

namespace aire {
    template<class V>
    void bilateralBlur(V *data, int stride, int width, int height, const int size, float sigma,
                       float spatialSigma);
}

#endif //JXLCODER_BILATERALBLUR_H
