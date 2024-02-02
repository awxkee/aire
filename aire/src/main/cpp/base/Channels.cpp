//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Channels.h"
#include <iostream>

namespace aire {
    template<class T>
    void split(T *pixels, T *r, T *g, T *b, T *a, int stride, int width, int height) {
        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(pixels) + y * stride);
            T *rDst = nullptr;
            if (r != nullptr) {
                rDst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(r) + y * width);
            }
            T *gDst = nullptr;
            if (r != nullptr) {
                gDst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(g) + y * width);
            }
            T *bDst = nullptr;
            if (r != nullptr) {
                bDst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(b) + y * width);
            }
            T *aDst = nullptr;
            if (r != nullptr) {
                aDst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(a) + y * width);
            }
            for (int x = 0; x < width; ++x) {
                if (rDst) {
                    rDst[0] = src[0];
                    rDst += 1;
                }
                if (gDst) {
                    gDst[0] = src[1];
                    gDst += 1;
                }
                if (bDst) {
                    bDst[0] = src[2];
                    bDst += 1;
                }
                if (aDst) {
                    aDst[0] = src[3];
                    aDst += 1;
                }
                src += 4;
            }
        }
    }

    template<class T>
    void merge(T *destination, T *r, T *g, T *b, T *a, int stride, int width, int height) {
        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(destination) + y * stride);
            auto rSrc = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(r) + y * width);
            auto gSrc = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(g) + y * width);
            auto bSrc = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(b) + y * width);
            auto aSrc = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(a) + y * width);
            for (int x = 0; x < width; ++x) {
                dst[0] = rSrc[0];
                dst[1] = gSrc[0];
                dst[2] = bSrc[0];
                dst[3] = aSrc[0];
                dst += 4;
                rSrc += 1;
                gSrc += 1;
                bSrc += 1;
                aSrc += 1;
            }
        }
    }

    template void
    split(uint8_t *pixels, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a, int stride, int width,
          int height);

    template
    void merge(uint8_t *destination, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a, int stride,
               int width, int height);
}