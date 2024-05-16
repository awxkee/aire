/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 12/11/23, 6:13 PM
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

#include "YuvConverter.h"
#include <algorithm>
#include <thread>
#include "concurrency.hpp"

using namespace std;

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "YuvConverter.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"
#include "NV21-inl.h"

HWY_BEFORE_NAMESPACE();

namespace aire::HWY_NAMESPACE {

    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    void NV21ToRGBAHWYInterop(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc,
                              int yStride, const uint8_t *uv, int uvStride) {
        sparkyuv::HWY_NAMESPACE::NV21ToRGBAHWY(dst, dstStride, width, height, ySrc, yStride, uv, uvStride);
    }

    void NV21ToRGBHWYInterop(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc,
                             int yStride, const uint8_t *uv, int uvStride) {
        sparkyuv::HWY_NAMESPACE::NV21ToRGBHWY(dst, dstStride, width, height, ySrc, yStride, uv, uvStride);
    }

    void NV21ToBGRHWYInterop(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc,
                             int yStride, const uint8_t *uv, int uvStride) {
        sparkyuv::HWY_NAMESPACE::NV21ToBGRHWY(dst, dstStride, width, height, ySrc, yStride, uv, uvStride);
    }
}

HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace aire {
    HWY_EXPORT(NV21ToRGBAHWYInterop);
    HWY_EXPORT(NV21ToRGBHWYInterop);
    HWY_EXPORT(NV21ToBGRHWYInterop);

    HWY_DLLEXPORT void
    NV21ToRGBA(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc, int yStride,
               const uint8_t *uv, int uvStride) {
        HWY_DYNAMIC_DISPATCH(NV21ToRGBAHWYInterop)(dst, dstStride, width, height, ySrc, yStride, uv, uvStride);
    }

    void
    NV21ToRGB(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc, int yStride,
              const uint8_t *uv, int uvStride) {
        HWY_DYNAMIC_DISPATCH(NV21ToRGBHWYInterop)(dst, dstStride, width, height, ySrc, yStride, uv, uvStride);
    }

    void
    NV21ToBGR(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc, int yStride,
              const uint8_t *uv, int uvStride) {
        HWY_DYNAMIC_DISPATCH(NV21ToBGRHWYInterop)(dst, dstStride, width, height, ySrc, yStride, uv, uvStride);
    }
}
#endif
