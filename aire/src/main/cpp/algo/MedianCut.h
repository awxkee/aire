//
// Created by Radzivon Bartoshyk on 08/02/2024.
//

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