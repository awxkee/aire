///////////////////////////////////////////////////////////////////////
//	    C Implementation of Wu's Color Quantizer (v. 2)
//	    (see Graphics Gems vol. II, pp. 126-133)
//
// Author:	Xiaolin Wu
// Dept. of Computer Science
// Univ. of Western Ontario
// London, Ontario N6A 5B7
// wu@csd.uwo.ca
//
// Copyright(c) 2018 - 2021 Miller Cy Chan
//
// Algorithm: Greedy orthogonal bipartition of RGB space for variance
// 	   minimization aided by inclusion-exclusion tricks.
// 	   For speed no nearest neighbor search is done. Slightly
// 	   better performance can be expected by more sophisticated
// 	   but more expensive versions.
//
// Free to distribute, comments and suggestions are appreciated.
///////////////////////////////////////////////////////////////////////

#include "WuQuantizer.h"
#include <unordered_map>
#include "MathUtils.hpp"
#include "EigenUtils.h"
#include <cstdint>

namespace aire {
    /// <summary><para>Shift color values right this many bits.</para><para>This reduces the granularity of the color maps produced, making it much faster.</para></summary>
    /// 3 = value error of 8 (0 and 7 will look the same to it, 0 and 8 different); Takes ~4MB for color tables; ~.25 -> .50 seconds
    /// 2 = value error of 4; Takes ~64MB for color tables; ~3 seconds
    /// RAM usage roughly estimated with: ( ( 256 >> SidePixShift ) ^ 4 ) * 60
    /// Default SidePixShift = 3
    const uint8_t SIDEPIXSHIFT = 3;
    const uint8_t MAXSIDEINDEX = 256 / (1 << SIDEPIXSHIFT);
    const uint8_t SIDESIZE = MAXSIDEINDEX + 1;
    const uint32_t TOTAL_SIDESIZE = SIDESIZE * SIDESIZE * SIDESIZE * SIDESIZE;

    bool hasSemiTransparency = false;
    int m_transparentPixelIndex = -1;
//    uint32_t m_transparentColor = 0;
    double PR = .299, PG = .587, PB = .114;

    struct Box {
        uint8_t AlphaMinimum = 0;
        uint8_t AlphaMaximum = 0;
        uint8_t RedMinimum = 0;
        uint8_t RedMaximum = 0;
        uint8_t GreenMinimum = 0;
        uint8_t GreenMaximum = 0;
        uint8_t BlueMinimum = 0;
        uint8_t BlueMaximum = 0;
        uint32_t Size = 0;
    };

    struct CubeCut {
        bool valid;
        uint8_t position;
        float value;

        CubeCut(bool isValid, uint8_t cutPoint, float result) {
            valid = isValid;
            position = cutPoint;
            value = result;
        }
    };

    inline uint32_t Index(uint8_t red, uint8_t green, uint8_t blue) {
        return red + green * SIDESIZE + blue * SIDESIZE * SIDESIZE;
    }

    inline uint32_t Index(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) {
        return alpha + red * SIDESIZE + green * SIDESIZE * SIDESIZE + blue * SIDESIZE * SIDESIZE * SIDESIZE;
    }

    inline float Volume(const Box &cube, long *moment) {
        return (moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMaximum)] -
                moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMaximum)] -
                moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMaximum)] +
                moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)] -
                moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMaximum)] +
                moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMaximum)] +
                moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMaximum)] -
                moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)]) -
               (moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMinimum)] -
                moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMinimum)] -
                moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] +
                moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] -
                moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] +
                moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] +
                moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)] -
                moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)]);
    }

    inline float Volume(const Box &cube, float *moment) {
        return (moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMaximum)] -
                moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMaximum)] -
                moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMaximum)] +
                moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)] -
                moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMaximum)] +
                moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMaximum)] +
                moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMaximum)] -
                moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)]) -
               (moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMinimum)] -
                moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMinimum)] -
                moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] +
                moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] -
                moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] +
                moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] +
                moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)] -
                moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)]);
    }

    inline float Top(const Box &cube, Pixel direction, uint8_t position, long *moment) {
        switch (direction) {
            case Alpha:
                return (moment[Index(position, cube.RedMaximum, cube.GreenMaximum, cube.BlueMaximum)] -
                        moment[Index(position, cube.RedMaximum, cube.GreenMinimum, cube.BlueMaximum)] -
                        moment[Index(position, cube.RedMinimum, cube.GreenMaximum, cube.BlueMaximum)] +
                        moment[Index(position, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)]) -
                       (moment[Index(position, cube.RedMaximum, cube.GreenMaximum, cube.BlueMinimum)] -
                        moment[Index(position, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] -
                        moment[Index(position, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] +
                        moment[Index(position, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)]);

            case Red:
                return (moment[Index(cube.AlphaMaximum, position, cube.GreenMaximum, cube.BlueMaximum)] -
                        moment[Index(cube.AlphaMaximum, position, cube.GreenMinimum, cube.BlueMaximum)] -
                        moment[Index(cube.AlphaMinimum, position, cube.GreenMaximum, cube.BlueMaximum)] +
                        moment[Index(cube.AlphaMinimum, position, cube.GreenMinimum, cube.BlueMaximum)]) -
                       (moment[Index(cube.AlphaMaximum, position, cube.GreenMaximum, cube.BlueMinimum)] -
                        moment[Index(cube.AlphaMaximum, position, cube.GreenMinimum, cube.BlueMinimum)] -
                        moment[Index(cube.AlphaMinimum, position, cube.GreenMaximum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMinimum, position, cube.GreenMinimum, cube.BlueMinimum)]);

            case Green:
                return (moment[Index(cube.AlphaMaximum, cube.RedMaximum, position, cube.BlueMaximum)] -
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, position, cube.BlueMaximum)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMaximum, position, cube.BlueMaximum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, position, cube.BlueMaximum)]) -
                       (moment[Index(cube.AlphaMaximum, cube.RedMaximum, position, cube.BlueMinimum)] -
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, position, cube.BlueMinimum)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMaximum, position, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, position, cube.BlueMinimum)]);

            case Blue:
                return (moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMaximum, position)] -
                        moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMinimum, position)] -
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMaximum, position)] +
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, position)]) -
                       (moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMaximum, position)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, position)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, position)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, position)]);

            default:
                return 0;
        }
    }

    inline int sqr(int x) {
        return x * x;
    }

    inline float Bottom(const Box &cube, Pixel direction, long *moment) {
        switch (direction) {
            case Alpha:
                return (-moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMaximum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMaximum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMaximum)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)]) -
                       (-moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)]);

            case Red:
                return (-moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMaximum)] +
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMaximum)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)]) -
                       (-moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)]);

            case Green:
                return (-moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMaximum)] +
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMaximum)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMaximum)]) -
                       (-moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)]);

            case Blue:
                return (-moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMaximum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] -
                        moment[Index(cube.AlphaMaximum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)]) -
                       (-moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMaximum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMaximum, cube.GreenMinimum, cube.BlueMinimum)] +
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMaximum, cube.BlueMinimum)] -
                        moment[Index(cube.AlphaMinimum, cube.RedMinimum, cube.GreenMinimum, cube.BlueMinimum)]);

            default:
                return 0;
        }
    }

    void WuQuantizer::Histogram3d(ColorData &colorData, const Eigen::Vector4i &color, const uint8_t alphaThreshold, const uint8_t alphaFader) {
        uint8_t pixelBlue = color.z();
        uint8_t pixelGreen = color.y();
        uint8_t pixelRed = color.x();
        uint8_t pixelAlpha = color.w();

        uint8_t indexAlpha = static_cast<uint8_t>((pixelAlpha >> SIDEPIXSHIFT) + 1);
        uint8_t indexRed = static_cast<uint8_t>((pixelRed >> SIDEPIXSHIFT) + 1);
        uint8_t indexGreen = static_cast<uint8_t>((pixelGreen >> SIDEPIXSHIFT) + 1);
        uint8_t indexBlue = static_cast<uint8_t>((pixelBlue >> SIDEPIXSHIFT) + 1);

        if (pixelAlpha > alphaThreshold) {
            if (pixelAlpha < UINT8_MAX) {
                short alpha = pixelAlpha + (pixelAlpha % alphaFader);
                pixelAlpha = static_cast<uint8_t>(alpha > UINT8_MAX ? UINT8_MAX : alpha);
                indexAlpha = static_cast<uint8_t>((pixelAlpha >> 3) + 1);
            }

            const int index = Index(indexAlpha, indexRed, indexGreen, indexBlue);
            if (index < TOTAL_SIDESIZE) {
                colorData.weights[index]++;
                colorData.momentsRed[index] += pixelRed;
                colorData.momentsGreen[index] += pixelGreen;
                colorData.momentsBlue[index] += pixelBlue;
                colorData.momentsAlpha[index] += pixelAlpha;
                colorData.moments[index] += sqr(pixelAlpha) + sqr(pixelRed) + sqr(pixelGreen) + sqr(pixelBlue);
            }
        }

        colorData.AddPixel(packRGBA(pixelRed, pixelGreen, pixelBlue, pixelAlpha));
    }

    void AdjustMoments(const ColorData &colorData, const uint32_t &nMaxColors) {
        vector<int> indices;
        for (int i = 0; i < TOTAL_SIDESIZE; ++i) {
            double d = colorData.weights[i];
            if (d > 0)
                indices.emplace_back(i);
        }

        if (sqr(nMaxColors) / indices.size() < .04)
            return;

        for (const auto &i: indices) {
            double d = colorData.weights[i];
            d = (colorData.weights[i] = std::sqrt(d)) / d;
            colorData.momentsRed[i] *= d;
            colorData.momentsGreen[i] *= d;
            colorData.momentsBlue[i] *= d;
            colorData.momentsAlpha[i] *= d;
            colorData.moments[i] *= d;
        }
    }

    void WuQuantizer::BuildHistogram(ColorData &colorData, const uint32_t &nMaxColors, uint8_t alphaThreshold, uint8_t alphaFader) {
        const uint32_t bitDepth = 8;
        const uint32_t bitmapWidth = width;
        const uint32_t bitmapHeight = height;

        int pixelIndex = 0;

        for (uint32_t y = 0; y < bitmapHeight; ++y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            for (uint32_t x = 0; x < bitmapWidth; ++x, ++pixelIndex) {
                Eigen::Vector4i color = packERGBA(src[x * 4], src[x * 4 + 1], src[x * 4 + 2], src[x * 4 + 3]);
                if (color.w() < 0xE0) {
                    if (color.w() == 0) {
                        m_transparentPixelIndex = pixelIndex;
//                        m_transparentColor = packRGBA(color);
                    } else if (color.w() > alphaThreshold)
                        hasSemiTransparency = true;
                }
                Histogram3d(colorData, color, alphaThreshold, alphaFader);
            }
        }

        AdjustMoments(colorData, nMaxColors);
    }

    void CalculateMoments(ColorData &data) {
        const uint32_t SIDESIZE_3 = SIDESIZE * SIDESIZE * SIDESIZE;
        for (uint8_t alphaIndex = 1; alphaIndex <= MAXSIDEINDEX; ++alphaIndex) {
            auto xarea = make_unique<uint32_t[]>(SIDESIZE_3);
            auto xareaAlpha = make_unique<uint32_t[]>(SIDESIZE_3);
            auto xareaRed = make_unique<uint32_t[]>(SIDESIZE_3);
            auto xareaGreen = make_unique<uint32_t[]>(SIDESIZE_3);
            auto xareaBlue = make_unique<uint32_t[]>(SIDESIZE_3);
            auto xarea2 = make_unique<float[]>(SIDESIZE_3);

            for (uint8_t redIndex = 1; redIndex <= MAXSIDEINDEX; ++redIndex) {
                uint32_t area[SIDESIZE] = {0};
                uint32_t areaAlpha[SIDESIZE] = {0};
                uint32_t areaRed[SIDESIZE] = {0};
                uint32_t areaGreen[SIDESIZE] = {0};
                uint32_t areaBlue[SIDESIZE] = {0};
                float area2[SIDESIZE] = {0};

                for (uint8_t greenIndex = 1; greenIndex <= MAXSIDEINDEX; ++greenIndex) {
                    uint32_t line = 0;
                    uint32_t lineAlpha = 0;
                    uint32_t lineRed = 0;
                    uint32_t lineGreen = 0;
                    uint32_t lineBlue = 0;
                    float line2 = 0.0f;

                    for (uint8_t blueIndex = 1; blueIndex <= MAXSIDEINDEX; ++blueIndex) {
                        const uint32_t index = Index(alphaIndex, redIndex, greenIndex, blueIndex);
                        line += data.weights[index];
                        lineAlpha += data.momentsAlpha[index];
                        lineRed += data.momentsRed[index];
                        lineGreen += data.momentsGreen[index];
                        lineBlue += data.momentsBlue[index];
                        line2 += data.moments[index];

                        area[blueIndex] += line;
                        areaAlpha[blueIndex] += lineAlpha;
                        areaRed[blueIndex] += lineRed;
                        areaGreen[blueIndex] += lineGreen;
                        areaBlue[blueIndex] += lineBlue;
                        area2[blueIndex] += line2;

                        const uint32_t rgbIndex = Index(redIndex, greenIndex, blueIndex);
                        const uint32_t prevRgbIndex = Index(redIndex - 1, greenIndex, blueIndex);
                        xarea[rgbIndex] = xarea[prevRgbIndex] + area[blueIndex];
                        xareaAlpha[rgbIndex] = xareaAlpha[prevRgbIndex] + areaAlpha[blueIndex];
                        xareaRed[rgbIndex] = xareaRed[prevRgbIndex] + areaRed[blueIndex];
                        xareaGreen[rgbIndex] = xareaGreen[prevRgbIndex] + areaGreen[blueIndex];
                        xareaBlue[rgbIndex] = xareaBlue[prevRgbIndex] + areaBlue[blueIndex];
                        xarea2[rgbIndex] = xarea2[prevRgbIndex] + area2[blueIndex];

                        const uint32_t prevIndex = Index(alphaIndex - 1, redIndex, greenIndex, blueIndex);
                        data.weights[index] = data.weights[prevIndex] + xarea[rgbIndex];
                        data.momentsAlpha[index] = data.momentsAlpha[prevIndex] + xareaAlpha[rgbIndex];
                        data.momentsRed[index] = data.momentsRed[prevIndex] + xareaRed[rgbIndex];
                        data.momentsGreen[index] = data.momentsGreen[prevIndex] + xareaGreen[rgbIndex];
                        data.momentsBlue[index] = data.momentsBlue[prevIndex] + xareaBlue[rgbIndex];
                        data.moments[index] = data.moments[prevIndex] + xarea2[rgbIndex];
                    }
                }
            }
        }
    }

    CubeCut
    Maximize(const ColorData &data, const Box &cube, Pixel direction, uint8_t first, uint8_t last, uint32_t wholeAlpha, uint32_t wholeRed, uint32_t wholeGreen,
             uint32_t wholeBlue, uint32_t wholeWeight) {
        auto bottomAlpha = Bottom(cube, direction, data.momentsAlpha.get());
        auto bottomRed = Bottom(cube, direction, data.momentsRed.get());
        auto bottomGreen = Bottom(cube, direction, data.momentsGreen.get());
        auto bottomBlue = Bottom(cube, direction, data.momentsBlue.get());
        auto bottomWeight = Bottom(cube, direction, data.weights.get());

        bool valid = false;
        auto result = 0.0f;
        uint8_t cutPoint = 0;

        for (int position = first; position < last; ++position) {
            auto halfAlpha = bottomAlpha + Top(cube, direction, position, data.momentsAlpha.get());
            auto halfRed = bottomRed + Top(cube, direction, position, data.momentsRed.get());
            auto halfGreen = bottomGreen + Top(cube, direction, position, data.momentsGreen.get());
            auto halfBlue = bottomBlue + Top(cube, direction, position, data.momentsBlue.get());
            auto halfWeight = bottomWeight + Top(cube, direction, position, data.weights.get());

            if (halfWeight == 0)
                continue;

            auto halfDistance = sqr(halfAlpha) + sqr(halfRed) + sqr(halfGreen) + sqr(halfBlue);
            auto temp = halfDistance / halfWeight;

            halfAlpha = wholeAlpha - halfAlpha;
            halfRed = wholeRed - halfRed;
            halfGreen = wholeGreen - halfGreen;
            halfBlue = wholeBlue - halfBlue;
            halfWeight = wholeWeight - halfWeight;

            if (halfWeight != 0) {
                halfDistance = sqr(halfAlpha) + sqr(halfRed) + sqr(halfGreen) + sqr(halfBlue);
                temp += halfDistance / halfWeight;

                if (temp > result) {
                    valid = true;
                    result = temp;
                    cutPoint = position;
                }
            }
        }

        return CubeCut(valid, cutPoint, result);
    }

    bool Cut(const ColorData &data, Box &first, Box &second) {
        auto wholeAlpha = Volume(first, data.momentsAlpha.get());
        auto wholeRed = Volume(first, data.momentsRed.get());
        auto wholeGreen = Volume(first, data.momentsGreen.get());
        auto wholeBlue = Volume(first, data.momentsBlue.get());
        auto wholeWeight = Volume(first, data.weights.get());

        auto maxAlpha = Maximize(data, first, Alpha, static_cast<uint8_t>(first.AlphaMinimum + 1), first.AlphaMaximum, wholeAlpha, wholeRed, wholeGreen,
                                 wholeBlue, wholeWeight);
        auto maxRed = Maximize(data, first, Red, static_cast<uint8_t>(first.RedMinimum + 1), first.RedMaximum, wholeAlpha, wholeRed, wholeGreen, wholeBlue,
                               wholeWeight);
        auto maxGreen = Maximize(data, first, Green, static_cast<uint8_t>(first.GreenMinimum + 1), first.GreenMaximum, wholeAlpha, wholeRed, wholeGreen,
                                 wholeBlue, wholeWeight);
        auto maxBlue = Maximize(data, first, Blue, static_cast<uint8_t>(first.BlueMinimum + 1), first.BlueMaximum, wholeAlpha, wholeRed, wholeGreen, wholeBlue,
                                wholeWeight);

        Pixel direction = Blue;
        if ((maxAlpha.value >= maxRed.value) && (maxAlpha.value >= maxGreen.value) && (maxAlpha.value >= maxBlue.value)) {
            if (!maxAlpha.valid)
                return false;
            direction = Alpha;
        } else if ((maxRed.value >= maxAlpha.value) && (maxRed.value >= maxGreen.value) && (maxRed.value >= maxBlue.value))
            direction = Red;
        else if ((maxGreen.value >= maxAlpha.value) && (maxGreen.value >= maxRed.value) && (maxGreen.value >= maxBlue.value))
            direction = Green;

        second.AlphaMaximum = first.AlphaMaximum;
        second.RedMaximum = first.RedMaximum;
        second.GreenMaximum = first.GreenMaximum;
        second.BlueMaximum = first.BlueMaximum;

        switch (direction) {
            case Alpha:
                second.AlphaMinimum = first.AlphaMaximum = maxAlpha.position;
                second.RedMinimum = first.RedMinimum;
                second.GreenMinimum = first.GreenMinimum;
                second.BlueMinimum = first.BlueMinimum;
                break;

            case Red:
                second.RedMinimum = first.RedMaximum = maxRed.position;
                second.AlphaMinimum = first.AlphaMinimum;
                second.GreenMinimum = first.GreenMinimum;
                second.BlueMinimum = first.BlueMinimum;
                break;

            case Green:
                second.GreenMinimum = first.GreenMaximum = maxGreen.position;
                second.AlphaMinimum = first.AlphaMinimum;
                second.RedMinimum = first.RedMinimum;
                second.BlueMinimum = first.BlueMinimum;
                break;

            case Blue:
                second.BlueMinimum = first.BlueMaximum = maxBlue.position;
                second.AlphaMinimum = first.AlphaMinimum;
                second.RedMinimum = first.RedMinimum;
                second.GreenMinimum = first.GreenMinimum;
                break;
        }

        first.Size = (first.AlphaMaximum - first.AlphaMinimum) * (first.RedMaximum - first.RedMinimum) * (first.GreenMaximum - first.GreenMinimum) *
                     (first.BlueMaximum - first.BlueMinimum);
        second.Size = (second.AlphaMaximum - second.AlphaMinimum) * (second.RedMaximum - second.RedMinimum) * (second.GreenMaximum - second.GreenMinimum) *
                      (second.BlueMaximum - second.BlueMinimum);

        return true;
    }

    float CalculateVariance(const ColorData &data, const Box &cube) {
        auto volumeAlpha = Volume(cube, data.momentsAlpha.get());
        auto volumeRed = Volume(cube, data.momentsRed.get());
        auto volumeGreen = Volume(cube, data.momentsGreen.get());
        auto volumeBlue = Volume(cube, data.momentsBlue.get());
        auto volumeMoment = Volume(cube, data.moments.get());
        auto volumeWeight = Volume(cube, data.weights.get());

        float distance = sqr(volumeAlpha) + sqr(volumeRed) + sqr(volumeGreen) + sqr(volumeBlue);

        return volumeWeight != 0.0f ? (volumeMoment - distance / volumeWeight) : 0.0f;
    }

    void SplitData(vector<Box> &boxList, uint32_t &colorCount, ColorData &data) {
        int next = 0;
        auto volumeVariance = make_unique<float[]>(colorCount);
        boxList.resize(colorCount);
        boxList[0].AlphaMaximum = MAXSIDEINDEX;
        boxList[0].RedMaximum = MAXSIDEINDEX;
        boxList[0].GreenMaximum = MAXSIDEINDEX;
        boxList[0].BlueMaximum = MAXSIDEINDEX;

        for (int cubeIndex = 1; cubeIndex < colorCount; ++cubeIndex) {
            if (Cut(data, boxList[next], boxList[cubeIndex])) {
                volumeVariance[next] = boxList[next].Size > 1 ? CalculateVariance(data, boxList[next]) : 0.0f;
                volumeVariance[cubeIndex] = boxList[cubeIndex].Size > 1 ? CalculateVariance(data, boxList[cubeIndex]) : 0.0f;
            } else {
                volumeVariance[next] = 0.0f;
                cubeIndex--;
            }

            next = 0;
            auto temp = volumeVariance[0];

            for (int index = 1; index <= cubeIndex; ++index) {
                if (volumeVariance[index] <= temp)
                    continue;
                temp = volumeVariance[index];
                next = index;
            }

            if (temp > 0.0f)
                continue;

            colorCount = cubeIndex + 1;
            break;
        }
        boxList.resize(colorCount);
    }

    void BuildLookups(ColorPalette *pPalette, vector<Box> &cubes, const ColorData &data) {
        uint32_t lookupsCount = 0;
        if (m_transparentPixelIndex >= 0)
            pPalette->Entries[lookupsCount++] = 0;

        for (auto const &cube: cubes) {
            auto weight = Volume(cube, data.weights.get());

            if (weight <= 0)
                continue;

            uint8_t alpha = static_cast<uint8_t>(Volume(cube, data.momentsAlpha.get()) / weight);
            uint8_t red = static_cast<uint8_t>(Volume(cube, data.momentsRed.get()) / weight);
            uint8_t green = static_cast<uint8_t>(Volume(cube, data.momentsGreen.get()) / weight);
            uint8_t blue = static_cast<uint8_t>(Volume(cube, data.momentsBlue.get()) / weight);
            pPalette->Entries[lookupsCount++] = packRGBA(red, green, blue, alpha);
        }

        if (lookupsCount < pPalette->Count)
            pPalette->Count = lookupsCount;
    }

    unsigned short WuQuantizer::nearestColorIndex(const ColorPalette *pPalette, uint32_t value, const uint8_t alphaThreshold) {
        Eigen::Vector4i c = unpackRGBA(value);
        unsigned short k = 0;
        if (c.w() <= alphaThreshold)
            c = {0, 0, 0, 0};

        auto got = nearestMap.find(value);
        if (got == nearestMap.end()) {
            double mindist = SHRT_MAX;
            for (uint32_t i = 0; i < pPalette->Count; i++) {
                Eigen::Vector4i c2 = unpackRGBA(pPalette->Entries[k]);
                double curdist = sqr(c2.w() - c.w());
                if (curdist > mindist)
                    continue;

                curdist += PR * sqr(c2.x() - c.x());
                if (curdist > mindist)
                    continue;

                curdist += PG * sqr(c2.y() - c.y());
                if (curdist > mindist)
                    continue;

                curdist += PB * sqr(c2.z() - c.z());
                if (curdist > mindist)
                    continue;

                mindist = curdist;
                k = i;
            }

            nearestMap[value] = k;
        } else
            k = got->second;

        return k;
    }

//    void WuQuantizer::GetQuantizedPalette(const ColorData &data, ColorPalette *pPalette, const uint32_t colorCount, const uint8_t alphaThreshold) {
//        auto alphas = make_unique<uint32_t[]>(colorCount);
//        auto reds = make_unique<uint32_t[]>(colorCount);
//        auto greens = make_unique<uint32_t[]>(colorCount);
//        auto blues = make_unique<uint32_t[]>(colorCount);
//        auto sums = make_unique<uint32_t[]>(colorCount);
//
//        int pixelsCount = data.pixelsCount;
//
//        for (uint32_t pixelIndex = 0; pixelIndex < pixelsCount; ++pixelIndex) {
//            auto value = data.pixels[pixelIndex];
//            Eigen::Vector4i pixel = unpackRGBA(value);
//            if (pixel.w() <= alphaThreshold)
//                pixel = {0, 0, 0, 0};
//
//            uint32_t bestMatch = nearestColorIndex(pPalette, value, alphaThreshold);
//
//            alphas[bestMatch] += static_cast<uint8_t >(pixel.w());
//            reds[bestMatch] += static_cast<uint8_t >(pixel.x());
//            greens[bestMatch] += static_cast<uint8_t >(pixel.y());
//            blues[bestMatch] += static_cast<uint8_t >(pixel.z());
//            sums[bestMatch]++;
//        }
//
//        nearestMap.clear();
//
//        uint32_t paletteIndex = (m_transparentPixelIndex < 0) ? 0 : 1;
//        for (; paletteIndex < colorCount; ++paletteIndex) {
//            if (sums[paletteIndex] > 0) {
//                alphas[paletteIndex] /= sums[paletteIndex];
//                reds[paletteIndex] /= sums[paletteIndex];
//                greens[paletteIndex] /= sums[paletteIndex];
//                blues[paletteIndex] /= sums[paletteIndex];
//            }
//
//            pPalette->Entries[paletteIndex] = packRGBA(reds[paletteIndex], greens[paletteIndex], blues[paletteIndex], alphas[paletteIndex]);
//        }
//    }

    std::vector<Eigen::Vector4i> WuQuantizer::quantizeImage(uint32_t &nMaxColors, uint8_t alphaThreshold, uint8_t alphaFader) {
        const uint32_t bitmapWidth = width;
        const uint32_t bitmapHeight = height;
        const auto area = (size_t) (bitmapWidth * bitmapHeight);

        ColorPalette palette = {};
        palette.Count = nMaxColors;
        palette.Entries = reinterpret_cast<uint32_t *>(malloc(sizeof(uint32_t) * (nMaxColors + 1)));

        auto pPalette = &palette;

        if (nMaxColors <= 32)
            PR = PG = PB = 1;

        auto qPixels = make_unique<unsigned short[]>(area);

        ColorData colorData(SIDESIZE, bitmapWidth, bitmapHeight);
        BuildHistogram(colorData, nMaxColors, alphaThreshold, alphaFader);
        CalculateMoments(colorData);
        vector<Box> cubes;
        SplitData(cubes, nMaxColors, colorData);

        BuildLookups(pPalette, cubes, colorData);
        cubes.clear();

        nMaxColors = pPalette->Count;

        std::vector<Eigen::Vector4i> plte(pPalette->Count);

//        GetQuantizedPalette(colorData, pPalette, nMaxColors, alphaThreshold);

//        if (m_transparentPixelIndex >= 0) {
//            uint32_t k = qPixels[m_transparentPixelIndex];
//            if (nMaxColors > 2)
//                pPalette->Entries[k] = m_transparentColor;
//            else if (pPalette->Entries[k] != m_transparentColor)
//                swap(pPalette->Entries[0], pPalette->Entries[1]);
//        }
        closestMap.clear();
        nearestMap.clear();

        for (int i = 0; i < plte.size(); ++i) {
            auto pack = unpackRGBA(pPalette->Entries[i]);
            plte[i] = pack;
        }

        free(pPalette->Entries);

        return plte;
    }

}
