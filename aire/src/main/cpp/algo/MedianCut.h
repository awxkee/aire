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

#pragma once

#include <cstdint>
#include <vector>
#include <functional>

namespace aire {
    typedef uint32_t RGBA;

    class Cube final {
    public:
        RGBA getAverageRGBA() const;

    private:
        Cube(const int *hist, uint16_t *histPtr) : _hist(hist), _histPtr(histPtr) {
        }

        void shrink(uint16_t lower, uint16_t upper);

        friend class Palette;

        static int findCubeToSplite(const Cube *cubes, size_t numCubes, size_t maxCubes);

        void spliteCubes(Cube &cubeA, Cube &cubeB);

    private:
        const int *_hist;
        uint16_t *_histPtr;

        uint16_t _lower;
        uint16_t _upper;

        uint8_t _rmin, _rmax;
        uint8_t _gmin, _gmax;
        uint8_t _bmin, _bmax;
        size_t _volume;
        size_t _count;
    };

    class Palette final {
    public:
        Palette(const RGBA *colors, size_t colorSize);

        ~Palette();

        void medianCut(size_t maxcubes, const std::function<void(const Cube &)> &callback);

        Palette(const Palette &) = delete;

        Palette &operator=(const Palette &) = delete;

    private:
        static void spliteCubes(Cube *cubes, size_t &numCubes, size_t maxCubes);

    private:
        const RGBA *_colors;
        const size_t _colorSize;
        enum {
            kHistSize = 32768 // (1 << 15)
        };
        int *_hist;
        uint16_t *_histPtr;
    };
}