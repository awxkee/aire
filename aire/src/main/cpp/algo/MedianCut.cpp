/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 08/02/24, 6:13 PM
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

#include "MedianCut.h"
#include "median/Wirth.h"
#include <vector>

namespace aire {

    using namespace std;

    static inline uint8_t redFromRgb555(uint16_t color) {
        const uint8_t mask = (1 << 5) - 1;
        return (color >> 10) & mask;
    }

    static inline uint8_t greenFromRgb555(uint16_t color) {
        const uint8_t mask = (1 << 5) - 1;
        return (color >> 5) & mask;
    }

    static inline uint8_t blueFromRgb555(uint16_t color) {
        const uint8_t mask = (1 << 5) - 1;
        return (color >> 0) & mask;
    }

    static inline uint8_t bit5FromBit8(uint8_t value) {
        const uint8_t result = value >> (8 - 5);
        const uint8_t mask = (1 << 5) - 1;
        return (result & mask);
    }

    static inline uint8_t bit8FromBit5(uint8_t value) {
        float t = (float) ((1 << 8) - 1) / ((1 << 5) - 1);
        uint8_t result = value * t;
        return result;
    }

    static inline uint16_t rgb555FromRgba(uint32_t color) {
        const uint8_t *ptr = (uint8_t *) (&color);
        uint8_t r = bit5FromBit8(ptr[0]);
        uint8_t g = bit5FromBit8(ptr[1]);
        uint8_t b = bit5FromBit8(ptr[2]);

        uint16_t v = (r << 10) | (g << 5) | b;
        return v;
    }

    //////////////////////////////////////////
    void Cube::shrink(uint16_t lower, uint16_t upper) {
        _lower = lower;
        _upper = upper;

        _count = 0;
        _rmin = _gmin = _bmin = 255;
        _rmax = _gmax = _bmax = 0;
        for (uint16_t idx = lower; idx < upper; idx++) {
            uint16_t color = _histPtr[idx];
            _count += _hist[color];
            uint8_t r = redFromRgb555(color);
            _rmin = std::min(r, _rmin);
            _rmax = std::max(r, _rmax);

            uint8_t g = greenFromRgb555(color);
            _gmin = std::min(g, _gmin);
            _gmax = std::max(g, _gmax);

            uint8_t b = blueFromRgb555(color);
            _bmin = std::min(b, _bmin);
            _bmax = std::max(b, _bmax);
        }

        _volume = (_rmax - _rmin + 1) * (_gmax - _gmin + 1) * (_bmax - _bmin + 1);
    }

    int Cube::findCubeToSplite(const Cube *cubes, size_t numCubes, size_t maxCubes) {
        size_t maxOrderV = 0;
        int splitpos = -1;

        // test difference order method
        // auto getOrderValue = [](const Cube &cube) { return cube._volume; };
        // auto getOrderValue = [](const Cube &cube) { return cube._count; };
        auto getOrderValue = (numCubes >= maxCubes * 0.75)
                             ? [](const Cube &cube) { return cube._count; }
                             : [](const Cube &cube) { return cube._count * cube._volume; };

        for (size_t i = 0; i < numCubes; i++) {
            const Cube &cube = cubes[i];
            if (cube._upper - cube._lower <= 1) {
                // single color
            } else {
                auto v = getOrderValue(cube);
                if (v > maxOrderV) {
                    maxOrderV = v;
                    splitpos = (int) i;
                }
            }
        }
        return splitpos;
    }

    void Cube::spliteCubes(Cube &cubeA, Cube &cubeB) {
        uint8_t lr = _rmax - _rmin;
        uint8_t lg = _gmax - _gmin;
        uint8_t lb = _bmax - _bmin;

        if (lr >= lg && lr >= lb) {
            std::sort(_histPtr + _lower, _histPtr + _upper, [&](uint16_t color1, uint16_t color2) {
                return redFromRgb555(color1) < redFromRgb555(color2);
            });
        } else if (lg >= lr && lg >= lb) {
            std::sort(_histPtr + _lower, _histPtr + _upper, [&](uint16_t color1, uint16_t color2) {
                return greenFromRgb555(color1) < greenFromRgb555(color2);
            });
        } else if (lb >= lr && lb >= lg) {
            std::sort(_histPtr + _lower, _histPtr + _upper, [&](uint16_t color1, uint16_t color2) {
                return blueFromRgb555(color1) < blueFromRgb555(color2);
            });
        }

        int count = 0;
        int median = 0;
        for (uint16_t i = _lower; i < _upper; i++) {
            uint16_t color = _histPtr[i];
            count += _hist[color];

            if (count * 2 >= _count) {
                median = std::min((int) i + 1, _upper - 1);
                break;
            }
        }

        cubeA.shrink(_lower, median);
        cubeB.shrink(median, _upper);
    }

    RGBA Cube::getAverageRGBA() const {
        int redSum = 0;
        int greenSum = 0;
        int blueSum = 0;

        for (uint16_t i = _lower; i < _upper; i++) {
            uint16_t color = _histPtr[i];
            redSum += redFromRgb555(color) * _hist[color];
            greenSum += greenFromRgb555(color) * _hist[color];
            blueSum += blueFromRgb555(color) * _hist[color];
        }

        const uint8_t mask = (1 << 5) - 1;
        uint8_t r = (redSum / _count) & mask;
        uint8_t g = (greenSum / _count) & mask;
        uint8_t b = (blueSum / _count) & mask;

        RGBA result;
        uint8_t *ptr = (uint8_t *) &result;
        ptr[0] = bit8FromBit5(r);
        ptr[1] = bit8FromBit5(g);
        ptr[2] = bit8FromBit5(b);
        ptr[3] = 255;
        return result;
    }

    //////////////////////////////////////////
    Palette::Palette(const RGBA *colors, size_t colorSize) : _colors(colors), _colorSize(colorSize) {
        const size_t totalBytes = sizeof(int) * kHistSize + sizeof(uint16_t) * kHistSize;
        uint8_t *bytes = (uint8_t *) malloc(totalBytes);
        _hist = (int *) bytes;
        _histPtr = (uint16_t *) (bytes + sizeof(int) * kHistSize);
    }

    Palette::~Palette() {
        uint8_t *bytes = (uint8_t *) _hist;
        if (bytes) {
            free(bytes);
        }
    }

    void Palette::spliteCubes(Cube *cubes, size_t &numCubes, size_t maxCubes) {
        while (numCubes < maxCubes) {
            int splitpos = Cube::findCubeToSplite(cubes, numCubes, maxCubes);
            if (splitpos == -1) {
                break;
            }

            Cube cube = cubes[splitpos];
            Cube cubeA(cube._hist, cube._histPtr);
            Cube cubeB(cube._hist, cube._histPtr);
            cube.spliteCubes(cubeA, cubeB);
            cubes[splitpos] = cubeA;
            cubes[numCubes++] = cubeB;
        }
    }

    void Palette::medianCut(size_t maxcubes, const std::function<void(const Cube &)> &callback) {
        memset(_hist, 0, sizeof(int) * kHistSize);
        std::for_each(_colors, _colors + _colorSize, [&](RGBA color) {
            uint16_t quantizedColor = rgb555FromRgba(color);
            _hist[quantizedColor]++;
        });

        uint16_t lower = 0;
        uint16_t upper = 0;
        for (size_t i = 0; i < kHistSize; i++) {
            if (_hist[i] > 0) {
                _histPtr[upper] = i;
                upper++;
            }
        }

        if (upper - lower <= maxcubes) {
            Cube cube(_hist, _histPtr);
            for (size_t idx = lower; idx < upper; idx++) {
                cube.shrink(idx, idx + 1);
                callback(cube);
            }
        } else {
            Cube cube(_hist, _histPtr);
            cube.shrink(lower, upper);

            Cube *cubes = (Cube *) malloc(sizeof(Cube) * maxcubes);
            size_t numCubes = 0;
            cubes[numCubes++] = cube;
            spliteCubes(cubes, numCubes, maxcubes);

            for (size_t i = 0; i < numCubes; i++) {
                callback(cubes[i]);
            }

            free(cubes);
        }
    }

}