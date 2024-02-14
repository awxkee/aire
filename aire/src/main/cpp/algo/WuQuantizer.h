#pragma once

#include <memory>
#include <vector>
#include <cstdint>

using namespace std;

#include "Eigen/Eigen"
#include <unordered_map>

namespace aire {
/**
  Xiaolin Wu color quantization algorithm
*/
    enum Pixel : uint8_t {
        Blue, Green, Red, Alpha
    };

    struct ColorData {
        unique_ptr<long[]> weights;
        unique_ptr<long[]> momentsAlpha;
        unique_ptr<long[]> momentsRed;
        unique_ptr<long[]> momentsGreen;
        unique_ptr<long[]> momentsBlue;
        unique_ptr<float[]> moments;

//        unique_ptr<uint32_t[]> pixels;

        uint32_t pixelsCount = 0;
        uint32_t pixelFillingCounter = 0;

        ColorData(uint32_t sideSize, uint32_t bitmapWidth, uint32_t bitmapHeight) {
            const int TOTAL_SIDESIZE = sideSize * sideSize * sideSize * sideSize;
            weights = make_unique<long[]>(TOTAL_SIDESIZE);
            momentsAlpha = make_unique<long[]>(TOTAL_SIDESIZE);
            momentsRed = make_unique<long[]>(TOTAL_SIDESIZE);
            momentsGreen = make_unique<long[]>(TOTAL_SIDESIZE);
            momentsBlue = make_unique<long[]>(TOTAL_SIDESIZE);
            moments = make_unique<float[]>(TOTAL_SIDESIZE);
            pixelsCount = bitmapWidth * bitmapHeight;
//            pixels = make_unique<uint32_t[]>(pixelsCount);
        }

//        inline uint32_t *GetPixels() {
//            return pixels.get();
//        }

        inline void AddPixel(uint32_t pixel) {
//            pixels[pixelFillingCounter] = pixel;
            ++pixelFillingCounter;
        }
    };

    struct ColorPalette {
        uint32_t *Entries;
        int Count;
    };

    class WuQuantizer {
    public:
        std::vector<Eigen::Vector4i> quantizeImage(uint32_t &nMaxColors, uint8_t alphaThreshold, uint8_t alphaFader);

        WuQuantizer(uint8_t *data, int stride, int width, int height) : data(data), stride(stride),
                                                                        width(width), height(height) {
        }


    private:
        void BuildHistogram(ColorData &colorData, const uint32_t &nMaxColors, uint8_t alphaThreshold, uint8_t alphaFader);

        void Histogram3d(ColorData &colorData, const Eigen::Vector4i &color, const uint8_t alphaThreshold, const uint8_t alphaFader);
        unsigned short nearestColorIndex(const ColorPalette *pPalette, uint32_t value, const uint8_t alphaThreshold);
        void GetQuantizedPalette(const ColorData &data, ColorPalette *pPalette, const uint32_t colorCount, const uint8_t alphaThreshold);
        uint8_t *data;
        const int width;
        const int height;
        const int stride;

        unordered_map<uint32_t, vector<unsigned short> > closestMap;
        unordered_map<uint32_t, unsigned short> nearestMap;
    };
}