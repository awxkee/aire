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

    template<class D, class VF32, HWY_IF_U8_D(D), HWY_IF_LANES_D(D, 4)>
    HWY_API VFromD<D> DemoteToU8(D du, VF32 v) {
        const FixedTag<uint32_t , 4> vu;
        return DemoteTo(du, ConvertTo(vu, v));
    }
}

#endif