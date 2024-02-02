//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#pragma once

#include <cstdint>

namespace aire {
    template<class T>
    void split(T *pixels, T *r, T *g, T *b, T *a, int stride, int width, int height);

    template<class T>
    void merge(T *destination, T *r, T *g, T *b, T *a, int stride, int width, int height);
}