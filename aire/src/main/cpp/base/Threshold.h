//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    template<class V>
    void threshold(V *pixels, int width, int height, V thresholdLevel, V max, V min);
}