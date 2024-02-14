//
// Created by Radzivon Bartoshyk on 09/02/2024.
//

#include "RemapPalette.h"
#include "MathUtils.hpp"
#include "EigenUtils.h"
#include "KDColorTree.hpp"
#include <exception>
#include "OctTree.h"
#include "NearestColorSearch.hpp"

namespace aire {

    std::vector<uint8_t> RemapPalette::indexed() {
        std::vector<uint8_t> destination(width * height);

        LinearNearestSearch linear(this->palette);

        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination.data()) + y * width);
            for (int x = 0; x < width; ++x) {
                uint32_t clr = reinterpret_cast<uint32_t *>(src)[0];
                Eigen::Vector4i original = unpackRGBA(clr);
                auto color = linear.findClosestIndex(original);

                dst[0] = color;

                src += 4;
                dst += 1;
            }
        }

        return std::move(destination);
    }

    std::vector<uint8_t> RemapPalette::remap() {
        std::vector<uint8_t> destination(stride * height);

        std::unique_ptr<NearestColorSearch> search;
        switch (strategy) {
            case Remap_Search_KD:
                search = std::make_unique<KDNearestSearch>(palette);
                break;
            case Remap_Search_Cover:
                search = std::make_unique<CoverNearestSearch>(palette);
                break;
            default:
                search = std::make_unique<LinearNearestSearch>(palette);
                break;
        }

        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination.data()) + y * stride);
            for (int x = 0; x < width; ++x) {
                uint32_t clr = reinterpret_cast<uint32_t *>(src)[0];
                Eigen::Vector4i original = unpackRGBA(clr);
                auto color = search->getNearest(original);

                dst[x * 4 + 0] = color.x();
                dst[x * 4 + 1] = color.y();
                dst[x * 4 + 2] = color.z();
                dst[x * 4 + 3] = color.w();

                if (dithering == Remap_Dither_Floyd_Steinberg) {
                    int errorR = original.x() - color.x();
                    int errorG = original.y() - color.y();
                    int errorB = original.z() - color.z();

                    if (x + 1 < width) {
                        dst[(x + 1) * 4] += errorR * 7 / 16;
                        dst[(x + 1) * 4 + 1] += errorG * 7 / 16;
                        dst[(x + 1) * 4 + 2] += errorB * 7 / 16;
                    }
                    if (y + 1 < height) {
                        auto dstNew = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination.data()) + (y + 1) * stride);
                        if (x - 1 >= 0 && x - 1 < width) {
                            dstNew[clamp(x - 1, 0, width - 1) * 4] += errorR * 3 / 16;
                            dstNew[clamp(x - 1, 0, width - 1) * 4 + 1] += errorG * 3 / 16;
                            dstNew[clamp(x - 1, 0, width - 1) * 4 + 2] += errorB * 3 / 16;
                        }
                        dstNew[x * 4] += errorR * 5 / 16;
                        dstNew[x * 4 + 1] += errorG * 5 / 16;
                        dstNew[x * 4 + 2] += errorB * 5 / 16;
                        if (x + 1 >= 0 && x + 1 < width) {
                            dstNew[(x + 1) * 4] += errorR * 1 / 16;
                            dstNew[(x + 1) * 4 + 1] += errorG * 1 / 16;
                            dstNew[(x + 1) * 4 + 2] += errorB * 1 / 16;
                        }
                    }
                } else if (dithering == Remap_Dither_Jarvis_Judice_Ninke) {
                    int errorR = original.x() - color.x();
                    int errorG = original.y() - color.y();
                    int errorB = original.z() - color.z();
                    if (x + 1 < width) {
                        dst[(x + 1) * 4] += errorR * 7 / 48;
                        dst[(x + 1) * 4 + 1] += errorG * 7 / 48;
                        dst[(x + 1) * 4 + 2] += errorB * 7 / 48;
                    }
                    if (x + 2 < width) {
                        dst[(x + 2) * 4] += errorR * 5 / 48;
                        dst[(x + 2) * 4 + 1] += errorG * 5 / 48;
                        dst[(x + 2) * 4 + 2] += errorB * 5 / 48;
                    }
                    if (x - 2 >= 0 && x - 2 < width && y + 1 < height) {
                        auto dstNew = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination.data()) + (y + 1) * stride);
                        dstNew[(x - 2) * 4] += errorR * 3 / 48;
                        dstNew[(x - 2) * 4 + 1] += errorG * 3 / 48;
                        dstNew[(x - 2) * 4 + 2] += errorB * 3 / 48;
                    }
                    if (y + 1 < height) {
                        auto dstNew = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination.data()) + (y + 1) * stride);
                        if (x - 1 >= 0 && x - 1 < width) {
                            dstNew[(x - 1) * 4] += errorR * 5 / 48;
                            dstNew[(x - 1) * 4 + 1] += errorR * 5 / 48;
                            dstNew[(x - 1) * 4 + 2] += errorR * 5 / 48;
                        }
                        dstNew[x * 4] = errorR * 7 / 48;
                        dstNew[x * 4 + 1] = errorR * 7 / 48;
                        dstNew[x * 4 + 2] = errorR * 7 / 48;
                        if (x + 1 >= 0 && x + 1 < width) {
                            dstNew[(x + 1) * 4] = errorR * 5 / 48;
                            dstNew[(x + 1) * 4 + 1] = errorR * 5 / 48;
                            dstNew[(x + 1) * 4 + 2] = errorR * 5 / 48;
                        }
                        if (x + 2 >= 0 && x + 2 < width) {
                            dstNew[(x + 2) * 4] = errorR * 3 / 48;
                            dstNew[(x + 2) * 4 + 1] = errorR * 3 / 48;
                            dstNew[(x + 2) * 4 + 2] = errorR * 3 / 48;
                        }
                    }
                    if (y + 2 < height) {
                        auto dstNew = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination.data()) + (y + 2) * stride);
                        if (x - 2 >= 0 && x - 2 < width) {
                            dstNew[(x - 2) * 4] = errorR * 1 / 48;
                            dstNew[(x - 2) * 4 + 1] = errorR * 1 / 48;
                            dstNew[(x - 2) * 4 + 2] = errorR * 1 / 48;
                        }
                        if (x - 1 >= 0 && x - 1 < width) {
                            dstNew[(x - 1) * 4] = errorR * 3 / 48;
                            dstNew[(x - 1) * 4 + 1] = errorR * 3 / 48;
                            dstNew[(x - 1) * 4 + 2] = errorR * 3 / 48;
                        }

                        dstNew[(x) * 4] = errorR * 5 / 48;
                        dstNew[(x) * 4 + 1] = errorR * 5 / 48;
                        dstNew[(x) * 4 + 2] = errorR * 5 / 48;

                        if (x + 1 >= 0 && x + 1 < width) {
                            dstNew[(x + 1) * 4] = errorR * 3 / 48;
                            dstNew[(x + 1) * 4 + 1] = errorR * 3 / 48;
                            dstNew[(x + 1) * 4 + 2] = errorR * 3 / 48;
                        }

                        if (x + 2 >= 0 && x + 2 < width) {
                            dstNew[(x + 2) * 4] = errorR * 1 / 48;
                            dstNew[(x + 2) * 4 + 1] = errorR * 1 / 48;
                            dstNew[(x + 2) * 4 + 2] = errorR * 1 / 48;
                        }
                    }
                }
                src += 4;
            }
        }

        return std::move(destination);
    }
}