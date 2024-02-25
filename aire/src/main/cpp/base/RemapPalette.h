/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 09/02/24, 6:13 PM
 *  *
 *  * Permission is hereby granted, free of charge, to any person obtaining a copy
 *  * of this software and associated documentation files (the "Software"), to deal
 *  * in the Software without restriction, including without limitation the rights
 *  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  * copies of the Software, and to permit persons to whom the Software is
 *  * furnished to do so, subject to the following conditions:
 *  *
 *  * The above copyright notice and this permission notice shall be included in all
 *  * copies or substantial portions of the Software.
 *  *
 *  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  * SOFTWARE.
 *  *
 *
 */

#pragma once

#include <cstdint>
#include <vector>
#include "Eigen/Eigen"

namespace aire {

    enum RemapDithering {
        Remap_Dither_Skip = 0,
        Remap_Dither_Floyd_Steinberg = 1,
        Remap_Dither_Jarvis_Judice_Ninke = 2,
    };

    enum RemapMappingStrategy {
        Remap_Search_Linear = 0,
        Remap_Search_KD = 1,
        Remap_Search_Cover = 2
    };

    class RemapPalette {
    public:
        RemapPalette(std::vector<Eigen::Vector4i> palette, uint8_t *data,
                     int stride, int width, int height,
                     const RemapDithering dithering = Remap_Dither_Jarvis_Judice_Ninke,
                     const RemapMappingStrategy strategy = Remap_Search_Cover) : data(data), stride(stride),
                                                                                 width(width), height(height),
                                                                                 palette(palette),
                                                                                 dithering(dithering),
                                                                                 strategy(strategy) {
        }

        std::vector<uint8_t> remap();

        std::vector<uint8_t> indexed();

    private:

        std::vector<Eigen::Vector4i> palette;

        uint8_t *data;
        const int stride;
        const int width;
        const int height;
        const RemapDithering dithering;
        const RemapMappingStrategy strategy;
    };
}