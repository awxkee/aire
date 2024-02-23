//
// Created by Radzivon Bartoshyk on 03/02/2024.
//

#if defined(AIRE_COMMON_INL_H) == defined(HWY_TARGET_TOGGLE)
#ifdef AIRE_COMMON_INL_H
#undef AIRE_COMMON_INL_H
#else
#define AIRE_COMMON_INL_H
#endif

#include <hwy/highway.h>

namespace hwy::HWY_NAMESPACE {
    template<class D, HWY_IF_F32_D(D), HWY_IF_LANES_D(D, 4)>
    HWY_API VFromD<D> ConvertToFloat(D df, Vec<Rebind<uint8_t, D>> v) {
        const RebindToUnsigned<decltype(df)> du32;
        const RebindToSigned<decltype(df)> d32;

        // Floats have 23 bits of mantissa.
        // We want least significant 8 bits to be shifted to [ 0 .. 255 ], therefore need to add 2^23
        // See this page for details: https://www.h-schmidt.net/FloatConverter/IEEE754.html
        // If you want output floats in [ 0 .. 255.0 / 256.0 ] interval, change into 2^15 = 0x47000000
        constexpr uint32_t offsetValue = 0x4b000000;
        // Check disassembly & verify your compiler has moved this initialization outside the loop
        const auto offsetInt = Set(du32, offsetValue);
        // Bitwise is probably slightly faster than addition, delivers same results for our input
        auto u32 = PromoteTo(du32, v);
        u32 = Or(u32, offsetInt);
        // The only FP operation required is subtraction, hopefully faster than UCVTF
        return Sub(BitCast(df, u32), BitCast(df, offsetInt));
    }

    template<class D, HWY_IF_F32_D(D), HWY_IF_LANES_D(D, 8), class VF>
    HWY_API void ConvertToFloat(D du, Vec<Rebind<uint8_t, D>> v, VF &v1, VF &v2) {
        auto low = LowerHalf(v);
        auto high = UpperHalf(du, v);
        v1 = ConvertToFloat(du, high);
        v2 = ConvertToFloat(du, low);
    }

    template<class D, class VF, HWY_IF_F32_D(DFromV<VF>), HWY_IF_LANES_D(DFromV<VF>, 4), HWY_IF_U8_D(D), HWY_IF_LANES_D(D, 16)>
    HWY_API void ConvertToFloatVec16(D du, Vec<Rebind<uint8_t, D>> v, VF &v1, VF &v2, VF &v3, VF &v4) {
        const FixedTag<uint16_t, 8> vu16x8;
        const FixedTag<uint16_t, 4> vu16x4;
        const FixedTag<float32_t, 4> df;
        const FixedTag<uint32_t, 4> du32x4;

        const auto lowerPart = PromoteLowerTo(vu16x8, v);
        const auto higherPart = PromoteUpperTo(vu16x8, v);

        auto lowlow = PromoteLowerTo(du32x4, lowerPart);
        auto lowhigh = PromoteUpperTo(du32x4, lowerPart);
        auto highlow = PromoteLowerTo(du32x4, higherPart);
        auto highhigh = PromoteUpperTo(du32x4, higherPart);
        v4 = ConvertTo(df, highhigh);
        v3 = ConvertTo(df, highlow);
        v2 = ConvertTo(df, lowhigh);
        v1 = ConvertTo(df, lowlow);
    }

    template<class D, class VF32, HWY_IF_U8_D(D), HWY_IF_LANES_D(D, 4)>
    HWY_API VFromD<D> DemoteToU8(D du, VF32 v) {
        const FixedTag<uint32_t, 4> vu;
        return DemoteTo(du, ConvertTo(vu, v));
    }
}

#endif