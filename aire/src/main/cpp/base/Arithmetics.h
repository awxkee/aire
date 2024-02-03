//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#pragma once

#include <cstdint>
#include <vector>

namespace aire {
    void absDiff(uint8_t *destination, uint8_t *s1, uint8_t *s2, int width, int height);

    void diff(uint8_t *destination, uint8_t value, uint8_t *s1, int width, int height);

    template<class V>
    void pickMaxMin(V *source, int width, int height, V *min, V *max);

    template<class V>
    void normalize(V *source, int width, int height, V min, V max);

    void fillSurface(uint8_t *destination, uint32_t value, int stride, int width, int height);
}