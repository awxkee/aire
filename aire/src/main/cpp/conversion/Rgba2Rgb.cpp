/*
 * MIT License
 *
 * Copyright (c) 2023 Radzivon Bartoshyk
 * jxl-coder [https://github.com/awxkee/jxl-coder]
 *
 * Created by Radzivon Bartoshyk on 04/09/2023
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "Rgba2Rgb.h"
#include <cstdint>
#include <vector>
#include <thread>
#include "concurrency.hpp"

using namespace std;

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "Rgba2Rgb.cpp"

#include <hwy/foreach_target.h>  // IWYU pragma: keep

#include <hwy/highway.h>
#include "hwy/base.h"

#include <android/log.h>

HWY_BEFORE_NAMESPACE();
namespace aire::HWY_NAMESPACE {

    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    template<class D>
    void
    RgbaToRGB(D du, const TFromD<D> *HWY_RESTRICT src, TFromD<D> *HWY_RESTRICT dst, const int width, const int *HWY_RESTRICT permuteMap) {
        using V = Vec<decltype(du)>;
        int x = 0;
        auto srcPixels = reinterpret_cast<const TFromD<D> *>(src);
        auto dstPixels = reinterpret_cast<TFromD<D> *>(dst);
        int pixels = du.MaxLanes();

        int idx1 = permuteMap[0];
        int idx2 = permuteMap[1];
        int idx3 = permuteMap[2];

        for (; x + pixels < width; x += pixels) {
            V pixels1;
            V pixels2;
            V pixels3;
            V pixels4;
            LoadInterleaved4(du, srcPixels, pixels1, pixels2, pixels3, pixels4);

            V map[3] = {pixels1, pixels2, pixels3};

            StoreInterleaved3(map[idx1], map[idx2], map[idx3], du, dstPixels);

            srcPixels += 4 * pixels;
            dstPixels += 3 * pixels;
        }

        for (; x < width; ++x) {
            TFromD<D> vec[3] = {srcPixels[0], srcPixels[1], srcPixels[2]};
            dstPixels[0] = vec[idx1];
            dstPixels[1] = vec[idx2];
            dstPixels[2] = vec[idx3];

            srcPixels += 4;
            dstPixels += 3;
        }
    }

    void HRgba16bit2RGB(const uint16_t *HWY_RESTRICT src, int srcStride,
                        uint16_t *HWY_RESTRICT dst, int dstStride, int width,
                        int height, const int *HWY_RESTRICT permuteMap) {
        auto rgbaData = reinterpret_cast<const uint8_t *>(src);
        auto rgbData = reinterpret_cast<uint8_t *>(dst);

        const ScalableTag<uint16_t> du16;

        concurrency::parallel_for(2, height, [&](int y) {
            RgbaToRGB(du16, reinterpret_cast<const uint16_t *>(rgbaData + srcStride * y),
                      reinterpret_cast<uint16_t *>(rgbData + dstStride * y), width, permuteMap);
        });
    }

    void rgb8bit2RGBH(const uint8_t *src, int srcStride, uint8_t *dst, int dstStride, int width,
                      int height, const int *HWY_RESTRICT permuteMap) {
        auto rgbaData = reinterpret_cast<const uint8_t *>(src);
        auto rgbData = reinterpret_cast<uint8_t *>(dst);

        const ScalableTag<uint8_t> du8;

        concurrency::parallel_for(2, height, [&](int y) {
            RgbaToRGB(du8, reinterpret_cast<const uint8_t *>(rgbaData + srcStride * y),
                      reinterpret_cast<uint8_t *>(rgbData + dstStride * y), width, permuteMap);
        });
    }

}
HWY_AFTER_NAMESPACE();

#if HWY_ONCE

namespace aire {
    HWY_EXPORT(HRgba16bit2RGB);
    HWY_DLLEXPORT void Rgba16bit2RGB(const uint16_t *HWY_RESTRICT src, int srcStride,
                                     uint16_t *HWY_RESTRICT dst, int dstStride, int width,
                                     int height) {
        const int permuteMap[3] = {0, 1, 2};
        HWY_DYNAMIC_DISPATCH(HRgba16bit2RGB)(src, srcStride, dst, dstStride, width, height, permuteMap);
    }

    HWY_EXPORT(rgb8bit2RGBH);

    void rgb8bit2RGB(const uint8_t *src, int srcStride, uint8_t *dst, int dstStride, int width,
                     int height) {
        const int permuteMap[3] = {0, 1, 2};
        HWY_DYNAMIC_DISPATCH(rgb8bit2RGBH)(src, srcStride, dst, dstStride, width, height, permuteMap);
    }

    void rgb8bit2BGR(const uint8_t *src, int srcStride, uint8_t *dst, int dstStride, int width,
                     int height) {
        const int permuteMap[3] = {2, 1, 0};
        HWY_DYNAMIC_DISPATCH(rgb8bit2RGBH)(src, srcStride, dst, dstStride, width, height, permuteMap);
    }
}

#endif