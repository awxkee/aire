//
// Created by Radzivon Bartoshyk on 06/11/2023.
//

#include "RGBAlpha.h"
#include "concurrency.hpp"

using namespace std;

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "RGBAlpha.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"
#include "Eigen/Eigen"

HWY_BEFORE_NAMESPACE();

namespace aire::HWY_NAMESPACE {

    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    template<typename D>
    HWY_INLINE HWY_FLATTEN VFromD<D>
    RearrangeVec(D d, VFromD<D> vec) {
        const Rebind<uint16_t, decltype(d)> du16x8;
        const Rebind<uint16_t, Half<D>> du16x4;
        const Rebind<uint32_t, Half<D>> du32x4;
        const Rebind<float, Half<D>> df32x4;
        const Rebind<int32_t, decltype(df32x4)> ru32;
        using VU32x4 = Vec<decltype(df32x4)>;
        const VU32x4 mult255 = Set(df32x4, 1.f / 255.f);
        return Combine(du16x8, DemoteTo(du16x4, ConvertTo(ru32, Round(Mul(ConvertTo(df32x4,
                                                                                    PromoteUpperTo(
                                                                                            du32x4,
                                                                                            vec)),
                                                                          mult255)))),
                       DemoteTo(du16x4, ConvertTo(ru32, Round(Mul(ConvertTo(df32x4,
                                                                            PromoteLowerTo(
                                                                                    du32x4,
                                                                                    vec)),
                                                                  mult255)))));
    }

    template<typename D>
    HWY_INLINE HWY_FLATTEN VFromD<D>
    RearrangeVecAlpha(D d, VFromD<D> vec, VFromD<D> alphas) {
        const Rebind<uint16_t, decltype(d)> du16x8;
        const Rebind<uint16_t, Half<decltype(d)>> du16x4;
        const Rebind<uint32_t, Half<decltype(d)>> du32x4;
        const Rebind<float, Half<decltype(d)>> df32x4;
        using VF32 = Vec<decltype(df32x4)>;
        const auto ones = Set(df32x4, 1.0f);
        const auto zeros = Zero(df32x4);
        VF32 lowDiv = ConvertTo(df32x4, PromoteLowerTo(du32x4, alphas));
        VF32 highDiv = ConvertTo(df32x4, PromoteUpperTo(du32x4, alphas));
        lowDiv = IfThenElse(lowDiv == zeros, ones, lowDiv);
        highDiv = IfThenElse(highDiv == zeros, ones, highDiv);
        return Combine(du16x8, DemoteTo(du16x4, ConvertTo(du32x4, Round(Div(ConvertTo(df32x4,
                                                                                      PromoteUpperTo(
                                                                                              du32x4,
                                                                                              vec)),
                                                                            highDiv)))),
                       DemoteTo(du16x4, ConvertTo(du32x4, Round(Div(ConvertTo(df32x4,
                                                                              PromoteLowerTo(
                                                                                      du32x4,
                                                                                      vec)),
                                                                    lowDiv)))));
    }

    void UnpremultiplyRGBAHWYRow(const uint8_t *src, int srcStride,
                                 uint8_t *dst, int dstStride, int width,
                                 int y) {

        const FixedTag<uint8_t, 16> du8x16;
        const FixedTag<uint16_t, 8> du16x8;
        const FixedTag<uint8_t, 8> du8x8;

        using VU8x16 = Vec<decltype(du8x16)>;
        using VU16x8 = Vec<decltype(du16x8)>;

        const int pixels = 16;

        const VU16x8 mult255 = Set(du16x8, 255);

        auto mSrc = reinterpret_cast<const uint8_t *>(src + y * srcStride);
        auto mDst = reinterpret_cast<uint8_t *>(dst + y * dstStride);

        int x = 0;

        for (; x + pixels < width; x += pixels) {
            VU8x16 r8, g8, b8, a8;
            LoadInterleaved4(du8x16, mSrc, r8, g8, b8, a8);

            VU16x8 aLow = PromoteLowerTo(du16x8, a8);
            VU16x8 rLow = PromoteLowerTo(du16x8, r8);
            VU16x8 gLow = PromoteLowerTo(du16x8, g8);
            VU16x8 bLow = PromoteLowerTo(du16x8, b8);
            auto lowADivider = ShiftRight<1>(aLow);
            VU16x8 tmp = Add(Mul(Min(rLow, aLow), mult255), lowADivider);
            rLow = RearrangeVecAlpha(du16x8, tmp, aLow);
            tmp = Add(Mul(Min(gLow, aLow), mult255), lowADivider);
            gLow = RearrangeVecAlpha(du16x8, tmp, aLow);
            tmp = Add(Mul(Min(bLow, aLow), mult255), lowADivider);
            bLow = RearrangeVecAlpha(du16x8, tmp, aLow);

            VU16x8 aHigh = PromoteUpperTo(du16x8, a8);
            VU16x8 rHigh = PromoteUpperTo(du16x8, r8);
            VU16x8 gHigh = PromoteUpperTo(du16x8, g8);
            VU16x8 bHigh = PromoteUpperTo(du16x8, b8);
            auto highADivider = ShiftRight<1>(aHigh);
            tmp = Add(Mul(Min(rHigh, aHigh), mult255), highADivider);
            rHigh = RearrangeVecAlpha(du16x8, tmp, aHigh);
            tmp = Add(Mul(Min(gHigh, aHigh), mult255), highADivider);
            gHigh = RearrangeVecAlpha(du16x8, tmp, aHigh);
            tmp = Add(Mul(Min(bHigh, aHigh), mult255), highADivider);
            bHigh = RearrangeVecAlpha(du16x8, tmp, aHigh);

            r8 = Combine(du8x16, DemoteTo(du8x8, rHigh), DemoteTo(du8x8, rLow));
            g8 = Combine(du8x16, DemoteTo(du8x8, gHigh), DemoteTo(du8x8, gLow));
            b8 = Combine(du8x16, DemoteTo(du8x8, bHigh), DemoteTo(du8x8, bLow));

            StoreInterleaved4(r8, g8, b8, a8, du8x16, mDst);

            mSrc += pixels * 4;
            mDst += pixels * 4;
        }

        for (; x < width; ++x) {
            uint8_t alpha = mSrc[3];
            if (alpha != 0) {
                mDst[0] = (min(mSrc[0], alpha) * 255 + alpha / 2) / alpha;
                mDst[1] = (min(mSrc[1], alpha) * 255 + alpha / 2) / alpha;
                mDst[2] = (min(mSrc[2], alpha) * 255 + alpha / 2) / alpha;
            }
            mDst[3] = alpha;
            mSrc += 4;
            mDst += 4;
        }
    }

    void UnpremultiplyRGBAHWY(const uint8_t *src, int srcStride,
                              uint8_t *dst, int dstStride, int width,
                              int height) {
        concurrency::parallel_for(2, height, [&](int y) {
            UnpremultiplyRGBAHWYRow(src, srcStride, dst, dstStride, width, y);
        });
    }

    void PremultiplyRGBAHWYRow(const uint8_t *src, int srcStride,
                               uint8_t *dst, int dstStride, int width,
                               int y) {
        const FixedTag<uint8_t, 16> du8x16;
        const FixedTag<uint16_t, 8> du16x8;
        const FixedTag<uint8_t, 8> du8x8;

        using VU8x16 = Vec<decltype(du8x16)>;
        using VU16x8 = Vec<decltype(du16x8)>;

        VU16x8 mult255d2 = Set(du16x8, 255 / 2);

        auto mSrc = reinterpret_cast<const uint8_t *>(src + y * srcStride);
        auto mDst = reinterpret_cast<uint8_t *>(dst + y * dstStride);

        int x = 0;
        int pixels = 16;

        for (; x + pixels < width; x += pixels) {
            VU8x16 r8, g8, b8, a8;
            LoadInterleaved4(du8x16, mSrc, r8, g8, b8, a8);

            VU16x8 aLow = PromoteLowerTo(du16x8, a8);
            VU16x8 rLow = PromoteLowerTo(du16x8, r8);
            VU16x8 gLow = PromoteLowerTo(du16x8, g8);
            VU16x8 bLow = PromoteLowerTo(du16x8, b8);
            VU16x8 tmp = Add(Mul(rLow, aLow), mult255d2);
            rLow = RearrangeVec(du16x8, tmp);
            tmp = Add(Mul(gLow, aLow), mult255d2);
            gLow = RearrangeVec(du16x8, tmp);
            tmp = Add(Mul(bLow, aLow), mult255d2);
            bLow = RearrangeVec(du16x8, tmp);

            VU16x8 aHigh = PromoteUpperTo(du16x8, a8);
            VU16x8 rHigh = PromoteUpperTo(du16x8, r8);
            VU16x8 gHigh = PromoteUpperTo(du16x8, g8);
            VU16x8 bHigh = PromoteUpperTo(du16x8, b8);
            tmp = Add(Mul(rHigh, aHigh), mult255d2);
            rHigh = RearrangeVec(du16x8, tmp);
            tmp = Add(Mul(gHigh, aHigh), mult255d2);
            gHigh = RearrangeVec(du16x8, tmp);
            tmp = Add(Mul(bHigh, aHigh), mult255d2);
            bHigh = RearrangeVec(du16x8, tmp);

            r8 = Combine(du8x16, DemoteTo(du8x8, rHigh), DemoteTo(du8x8, rLow));
            g8 = Combine(du8x16, DemoteTo(du8x8, gHigh), DemoteTo(du8x8, gLow));
            b8 = Combine(du8x16, DemoteTo(du8x8, bHigh), DemoteTo(du8x8, bLow));

            StoreInterleaved4(r8, g8, b8, a8, du8x16, mDst);

            mSrc += pixels * 4;
            mDst += pixels * 4;
        }

        for (; x < width; ++x) {
            uint8_t alpha = mSrc[3];
            mDst[0] = (mSrc[0] * alpha + 127) / 255;
            mDst[1] = (mSrc[1] * alpha + 127) / 255;
            mDst[2] = (mSrc[2] * alpha + 127) / 255;
            mDst[3] = alpha;
            mSrc += 4;
            mDst += 4;
        }
    }

    void PremultiplyRGBAHWY(const uint8_t *src, int srcStride,
                            uint8_t *dst, int dstStride, int width,
                            int height) {
        concurrency::parallel_for(2, height, [&](int y) {
            PremultiplyRGBAHWYRow(src, srcStride, dst, dstStride, width, y);
        });
    }
}

HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace aire {
    HWY_EXPORT(UnpremultiplyRGBAHWY);
    HWY_EXPORT(PremultiplyRGBAHWY);

    HWY_DLLEXPORT void UnpremultiplyRGBA(const uint8_t *src, int srcStride,
                                         uint8_t *dst, int dstStride, int width,
                                         int height) {
        HWY_DYNAMIC_DISPATCH(UnpremultiplyRGBAHWY)(src, srcStride, dst, dstStride, width, height);
    }

    HWY_DLLEXPORT void PremultiplyRGBA(const uint8_t *src, int srcStride,
                                       uint8_t *dst, int dstStride, int width,
                                       int height) {
        HWY_DYNAMIC_DISPATCH(PremultiplyRGBAHWY)(src, srcStride, dst, dstStride, width, height);
    }
}
#endif