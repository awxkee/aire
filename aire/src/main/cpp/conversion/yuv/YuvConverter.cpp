//
// Created by Radzivon Bartoshyk on 12/11/2023.
//

#include "YuvConverter.h"
#include <algorithm>
#include <thread>

using namespace std;

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "YuvConverter.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();

namespace aire::HWY_NAMESPACE {

    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    template<typename D>
    inline void
    YUVToRGBValuesI(D di, Vec<D> Y, Vec<D> U, Vec<D> V, Vec<D> &R, Vec<D> &G, Vec<D> &B) {
        using VI = Vec<D>;
        const VI corr298 = Set(di, 298);
        const VI corr409 = Set(di, 409);
        const VI corr128 = Set(di, 128);
        const VI corr100 = Set(di, 100);
        const VI corr208 = Set(di, 208);
        const VI corr516 = Set(di, 516);
        const VI and0xff = Set(di, 0xff);
        VI YCorrected = Add(corr128, Mul(corr298, Y));
        VI R1 = ShiftRight<8>(MulAdd(corr409, V, YCorrected));
        VI G1 = ShiftRight<8>(NegMulAdd(corr100, U, NegMulAdd(corr208, V, YCorrected)));
        VI B1 = ShiftRight<8>(MulAdd(corr516, U, YCorrected));
        const VI zeros = Zero(di);
        const VI max = Set(di, 255);
        R = Max(Min(R1, max), zeros);
        G = Max(Min(G1, max), zeros);
        B = Max(Min(B1, max), zeros);
    }

    template<typename D, typename V>
    inline Vec<V>
    ExpandUVRowX2(D dv, V v, Vec<D> rv) {
        using VU = Vec<decltype(v)>;
        auto low = LowerHalf(rv);
        auto widenLow = InterleaveLower(rv, rv);
        auto widenHigh = InterleaveUpper(dv, rv, rv);
        VU result = Combine(v, widenHigh, widenLow);
        return result;
    }

    template<typename D>
    inline void
    YUVToRGBValues(D di, Vec<D> Y, Vec<D> U, Vec<D> V, Vec<D> &R, Vec<D> &G, Vec<D> &B) {
        FixedTag<int32_t, 4> di32x4;
        using VI32 = Vec<decltype(di32x4)>;
        VI32 rLow, gLow, bLow;
        YUVToRGBValuesI(di32x4, PromoteLowerTo(di32x4, Y), PromoteLowerTo(di32x4, U),
                        PromoteLowerTo(di32x4, V), rLow, gLow, bLow);
        VI32 rHigh, gHigh, bHigh;
        YUVToRGBValuesI(di32x4, PromoteUpperTo(di32x4, Y), PromoteUpperTo(di32x4, U),
                        PromoteUpperTo(di32x4, V), rHigh, gHigh, bHigh);
        FixedTag<int16_t, 4> di16x4;
        R = Combine(di, DemoteTo(di16x4, rHigh), DemoteTo(di16x4, rLow));
        G = Combine(di, DemoteTo(di16x4, gHigh), DemoteTo(di16x4, gLow));
        B = Combine(di, DemoteTo(di16x4, bHigh), DemoteTo(di16x4, bLow));
    }

    void
    NV21ToRGBARow(uint8_t *__restrict__ dst, int width, const uint8_t *__restrict__ ySrc,
                  const uint8_t *uv, const int permuteMap[4]) {
        int x = 0;
        auto store = reinterpret_cast<uint8_t *>(dst);
        int pixels = 16;

        FixedTag<uint8_t, 16> du;
        FixedTag<uint8_t, 8> du8x8;
        FixedTag<int16_t, 8> di16x8;
        using VU = Vec<decltype(du)>;
        using VU8x8 = Vec<decltype(du8x8)>;
        using VU16 = Vec<decltype(di16x8)>;

        const VU16 yCorrection = Set(di16x8, 16);
        const VU16 uvCorrection = Set(di16x8, 128);

        const VU newA = Set(du, 255);

        for (; x + pixels < width; x += pixels) {
            VU y;
            VU8x8 u8, v8;
            y = LoadU(du, ySrc + x);
            LoadInterleaved2(du8x8, uv + min(x / 2, width / 2 - 1), u8, v8);
            VU u, v;
            u = ExpandUVRowX2(du8x8, du, u8);
            v = ExpandUVRowX2(du8x8, du, v8);
            auto uLow = Sub(PromoteLowerTo(di16x8, u), uvCorrection);
            auto uHigh = Sub(PromoteUpperTo(di16x8, u), uvCorrection);
            auto vLow = Sub(PromoteLowerTo(di16x8, v), uvCorrection);
            auto vHigh = Sub(PromoteUpperTo(di16x8, v), uvCorrection);
            auto yLow = Sub(PromoteLowerTo(di16x8, y), yCorrection);
            auto yHigh = Sub(PromoteUpperTo(di16x8, y), yCorrection);
            VU16 rLow, gLow, bLow;
            YUVToRGBValues(di16x8, yLow, uLow,
                           vLow, rLow, gLow, bLow);
            VU16 rHigh, gHigh, bHigh;
            YUVToRGBValues(di16x8, yHigh, uHigh, vHigh, rHigh, gHigh, bHigh);
            VU rNew = Combine(du, DemoteTo(du8x8, rHigh), DemoteTo(du8x8, rLow));
            VU gNew = Combine(du, DemoteTo(du8x8, gHigh), DemoteTo(du8x8, gLow));
            VU bNew = Combine(du, DemoteTo(du8x8, bHigh), DemoteTo(du8x8, bLow));
            VU toStore[4] = {rNew, gNew, bNew, newA};
            StoreInterleaved4(toStore[permuteMap[0]], toStore[permuteMap[1]],
                              toStore[permuteMap[2]], toStore[permuteMap[3]], du, store);
            store += pixels * 4;
        }

        for (; x < width; ++x) {
            int yValue = ySrc[x];
            int pos = min(x / 2, width / 2 - 1);
            int uValue = uv[pos];
            int vValue = uv[pos + 1];

            int c = yValue - 16;
            int d = uValue - 128;
            int e = vValue - 128;

            int r = clamp((298 * c + 409 * e + 128) >> 8, 0, 255);
            int g = clamp((298 * c - 100 * d - 208 * e + 128) >> 8, 0, 255);
            int b = clamp((298 * c + 516 * d + 128) >> 8, 0, 255);
            store[permuteMap[0]] = r;
            store[permuteMap[1]] = g;
            store[permuteMap[2]] = b;
            store[permuteMap[3]] = 255;
            store += 4;
        }
    }

    void
    NV21ToRGBRow(uint8_t *__restrict__ dst, int width, const uint8_t *__restrict__ ySrc,
                 const uint8_t *uv, const int permuteMap[4]) {
        int x = 0;
        auto store = reinterpret_cast<uint8_t *>(dst);
        int pixels = 16;

        FixedTag<uint8_t, 16> du;
        FixedTag<uint8_t, 8> du8x8;
        FixedTag<int16_t, 8> di16x8;
        using VU = Vec<decltype(du)>;
        using VU8x8 = Vec<decltype(du8x8)>;
        using VU16 = Vec<decltype(di16x8)>;

        const VU16 yCorrection = Set(di16x8, 16);
        const VU16 uvCorrection = Set(di16x8, 128);

        for (; x + pixels < width; x += pixels) {
            VU y;
            VU8x8 u8, v8;
            y = LoadU(du, ySrc + x);
            LoadInterleaved2(du8x8, uv + min(x / 2, width / 2 - 1), u8, v8);
            VU u, v;
            u = ExpandUVRowX2(du8x8, du, u8);
            v = ExpandUVRowX2(du8x8, du, v8);
            auto uLow = Sub(PromoteLowerTo(di16x8, u), uvCorrection);
            auto uHigh = Sub(PromoteUpperTo(di16x8, u), uvCorrection);
            auto vLow = Sub(PromoteLowerTo(di16x8, v), uvCorrection);
            auto vHigh = Sub(PromoteUpperTo(di16x8, v), uvCorrection);
            auto yLow = Sub(PromoteLowerTo(di16x8, y), yCorrection);
            auto yHigh = Sub(PromoteUpperTo(di16x8, y), yCorrection);
            VU16 rLow, gLow, bLow;
            YUVToRGBValues(di16x8, yLow, uLow,
                           vLow, rLow, gLow, bLow);
            VU16 rHigh, gHigh, bHigh;
            YUVToRGBValues(di16x8, yHigh, uHigh, vHigh, rHigh, gHigh, bHigh);
            VU rNew = Combine(du, DemoteTo(du8x8, rHigh), DemoteTo(du8x8, rLow));
            VU gNew = Combine(du, DemoteTo(du8x8, gHigh), DemoteTo(du8x8, gLow));
            VU bNew = Combine(du, DemoteTo(du8x8, bHigh), DemoteTo(du8x8, bLow));
            VU toStore[3] = {rNew, gNew, bNew};
            StoreInterleaved3(toStore[permuteMap[0]], toStore[permuteMap[1]],
                              toStore[permuteMap[2]], du, store);
            store += pixels * 3;
        }

        for (; x < width; ++x) {
            int yValue = ySrc[x];
            int pos = min(x / 2, width / 2 - 1);
            int uValue = uv[pos];
            int vValue = uv[pos + 1];

            int c = yValue - 16;
            int d = uValue - 128;
            int e = vValue - 128;

            int r = clamp((298 * c + 409 * e + 128) >> 8, 0, 255);
            int g = clamp((298 * c - 100 * d - 208 * e + 128) >> 8, 0, 255);
            int b = clamp((298 * c + 516 * d + 128) >> 8, 0, 255);
            store[permuteMap[0]] = r;
            store[permuteMap[1]] = g;
            store[permuteMap[2]] = b;
            store += 3;
        }
    }

    void NV21ToRGBAHWY(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc,
                       int yStride,
                       const uint8_t *uv, int uvStride, const int permuteMap[4]) {
#pragma omp parallel for num_threads(4) schedule(dynamic)
        for (int y = 0; y < height; ++y) {
            NV21ToRGBARow(dst + y * dstStride, width, ySrc + y * yStride,
                          uv + min(y / 2, height / 2 - 1) * uvStride, permuteMap);
        }
    }

    void NV21ToRGBHWY(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc,
                      int yStride,
                      const uint8_t *uv, int uvStride, const int permuteMap[3]) {
#pragma omp parallel for num_threads(4) schedule(dynamic)
        for (int y = 0; y < height; ++y) {
            NV21ToRGBRow(dst + y * dstStride, width, ySrc + y * yStride,
                         uv + min(y / 2, height / 2 - 1) * uvStride, permuteMap);
        }
    }
}

HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace aire {
    HWY_EXPORT(NV21ToRGBAHWY);
    HWY_EXPORT(NV21ToRGBHWY);

    HWY_DLLEXPORT void
    NV21ToRGBA(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc, int yStride,
               const uint8_t *uv, int uvStride) {
        const int permuteMap[4] = {0, 1, 2, 3};
        HWY_DYNAMIC_DISPATCH(NV21ToRGBAHWY)(dst, dstStride, width, height, ySrc, yStride, uv,
                                            uvStride, permuteMap);
    }

    void
    NV21ToRGB(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc, int yStride,
              const uint8_t *uv, int uvStride) {
        const int permuteMap[3] = {0, 1, 2};
        HWY_DYNAMIC_DISPATCH(NV21ToRGBHWY)(dst, dstStride, width, height, ySrc, yStride, uv,
                                           uvStride, permuteMap);
    }

    void
    NV21ToBGR(uint8_t *dst, int dstStride, int width, int height, const uint8_t *ySrc, int yStride,
              const uint8_t *uv, int uvStride) {
        const int permuteMap[3] = {2, 1, 0};
        HWY_DYNAMIC_DISPATCH(NV21ToRGBHWY)(dst, dstStride, width, height, ySrc, yStride, uv,
                                           uvStride, permuteMap);
    }
}
#endif
