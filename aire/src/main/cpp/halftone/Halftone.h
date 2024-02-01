//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#pragma once

#include <vector>

namespace aire {
    template<class V>
    void
    generateHalftoneHorizontalLines(std::vector<V>& data, int stride, int width, int height,
                                    int lineSize, int lineSpacing, V lineColor, V alpha);
}