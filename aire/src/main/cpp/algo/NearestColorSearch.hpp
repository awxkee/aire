/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 12/02/24, 6:13 PM
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

#include <vector>
#include "Eigen/Eigen"
#include "CoverTree.h"
#include "CoverTreePoint.h"
#include "KDColorTree.hpp"
#include "unordered_map.hpp"

namespace aire {

    class NearestColorSearch {
    public:
        NearestColorSearch() {

        }

        virtual ~NearestColorSearch() {  };

        virtual Eigen::Vector4i getNearest(Eigen::Vector4i &color) = 0;
    };

    class KDNearestSearch : public NearestColorSearch {
    public:
        KDNearestSearch(std::vector<Eigen::Vector4i> initialPalette) : NearestColorSearch() {
            palette = initialPalette;

            std::sort(palette.begin(), palette.end(), [](const Eigen::Vector4i &a,
                                                         const Eigen::Vector4i &b) {
                return a[0] < b[0] && a[1] < b[1] && a[2] < b[2];
            });
            kdTree.createTree(palette);
        }

        ~KDNearestSearch() override = default;

        Eigen::Vector4i getNearest(Eigen::Vector4i &color) override {
            return kdTree.nearestColor(color);
        }

    private:
        std::vector<Eigen::Vector4i> palette;
        KDColorTree kdTree;
    };

    class CoverNearestSearch : public NearestColorSearch {
    public:
        CoverNearestSearch(std::vector<Eigen::Vector4i> initialPalette) : NearestColorSearch() {
            palette = initialPalette;

            std::sort(palette.begin(), palette.end(), [](const Eigen::Vector4i &a,
                                                         const Eigen::Vector4i &b) {
                return a[0] < b[0] && a[1] < b[1] && a[2] < b[2];
            });

            for (int x = 0; x < palette.size(); ++x) {
                treePoint.insert(CoverTreePoint({palette[x].x(), palette[x].y(), palette[x].z(), palette[x].w()}));
            }
        }

        ~CoverNearestSearch() override = default;

        Eigen::Vector4i getNearest(Eigen::Vector4i &color) override {
            auto packed = packRGBA(color);
            auto it = lut.find(packed);
            if (it != lut.end()) {
                return it->second;
            }
            CoverTreePoint pt(color);
            auto vcolors = treePoint.kNearestNeighbors(pt, 0);
            if (vcolors.empty()) {
                vcolors = {CoverTreePoint({0, 0, 0, 0})};
            }
            auto bestMatch = vcolors[0].getColor();
            lut[packed] = bestMatch;
            return bestMatch;
        }

    private:
        std::vector<Eigen::Vector4i> palette;
        CoverTree <CoverTreePoint> treePoint;
        ska::unordered_map<uint32_t, Eigen::Vector4i> lut;
    };

    class LinearNearestSearch : public NearestColorSearch {
    public:
        LinearNearestSearch(std::vector<Eigen::Vector4i> initialPalette) : NearestColorSearch() {
            palette = initialPalette;

            std::sort(palette.begin(), palette.end(), [](const Eigen::Vector4i &a,
                                                         const Eigen::Vector4i &b) {
                return a[0] < b[0] && a[1] < b[1] && a[2] < b[2];
            });
        }

        ~LinearNearestSearch() override = default;

        Eigen::Vector4i getNearest(Eigen::Vector4i &color) override {
            Eigen::Vector4i closestColor = palette[0];
            float minDistance = std::numeric_limits<float>::max();

            const uint32_t packed = packRGBA(color);

            for (const Eigen::Vector4i &paletteColor: palette) {
                auto it = lut.find(packed);
                if (it != lut.end()) {
                    return it->second;
                }
                Eigen::Vector4i diff = color - paletteColor;
                int mask = (color[0] < 128) ? 3 : 2;
                Eigen::Array4i weights;
                weights << 2, 4, mask, 1;
                Eigen::Array4i squaredDiff = diff.array().square() * weights;
                float distance = squaredDiff.sum();

                if (distance < minDistance) {
                    minDistance = distance;
                    closestColor = paletteColor;
                }
            }

            lut.insert(std::make_pair(packed, closestColor));

            return closestColor;
        }

        int findClosestIndex(const Eigen::Vector4i &inputColor) {
            Eigen::Vector4i closestColor = palette[0];
            float minDistance = std::numeric_limits<float>::max();

            const uint32_t packed = packRGBA(inputColor);

            int index = 0;

            for (int i = 0; i < palette.size(); ++i) {
                auto it = indexesLut.find(packed);
                if (it != indexesLut.end()) {
                    return it->second;
                }
                Eigen::Vector4i &paletteColor = palette[i];

                float diff = colorDistance(inputColor, paletteColor);

                if (diff < minDistance) {
                    minDistance = diff;
                    closestColor = paletteColor;
                    index = i;
                }
            }

            indexesLut.insert(std::make_pair(packed, index));

            return index;
        }

    private:
        ska::unordered_map<uint32_t, Eigen::Vector4i> lut;
        std::vector<Eigen::Vector4i> palette;
        ska::unordered_map<uint32_t, int> indexesLut;

        float colorDistance(const Eigen::Vector4i &a, const Eigen::Vector4i &b) {
            Eigen::Vector4i diff = a - b;
            int mask = (a[0] < 128) ? 3 : 2;
            Eigen::Array4i weights;
            weights << 2, 4, mask, 1;
            Eigen::Array4i squaredDiff = diff.array().square() * weights;
            return squaredDiff.sum();
        }
    };
}