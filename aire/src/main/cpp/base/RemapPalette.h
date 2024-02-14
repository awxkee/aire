//
// Created by Radzivon Bartoshyk on 09/02/2024.
//

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