//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Grayscale.h"
#include "color/Gamut.h"
#include "hwy/highway.h"
#include "color/eotf-inl.h"

namespace aire {

    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;
    using namespace aire::HWY_NAMESPACE;

    template<class D, HWY_IF_U8_D(D), typename T = TFromD<D>>
    void
    grayscaleHWY(D du, T *pixels, T *destination, int stride, int width, int height,
                 const float rPrimary,
                 const float gPrimary, const float bPrimary) {
        const FixedTag<uint32_t, 4> du32x4;
        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        using VU = Vec<decltype(du)>;

        const auto vScale = Set(dfx4, 255.f);
        const auto vRevertScale = ApproximateReciprocal(vScale);

        const float lumaPrimaries[4] = {rPrimary, gPrimary, bPrimary, 0.f};
        const VF vLumaPrimaries = LoadU(dfx4, lumaPrimaries);

        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(destination) + y * stride);
            auto src = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(pixels) + y * stride);
            for (int x = 0; x < width; ++x) {
                VF local = Mul(ConvertTo(dfx4, PromoteTo(du32x4, LoadU(du, &src[0]))), vRevertScale);
                VF pv = Mul(SumOfLanes(dfx4, Mul(
                        aire::HWY_NAMESPACE::SRGBToLinear(dfx4, local),
                        vLumaPrimaries)), vScale);
                T pixel = ExtractLane(DemoteTo(du, ConvertTo(du32x4, pv)), 0);
                dst[0] = pixel;
                dst[1] = pixel;
                dst[2] = pixel;
                dst[3] = src[3];
                src += 4;
                dst += 4;
            }
        }
    }

    void
    grayscale(uint8_t *pixels, uint8_t *destination, int stride, int width, int height,
              const float rPrimary,
              const float gPrimary, const float bPrimary) {
        const FixedTag<uint8_t, 4> du8;
        grayscaleHWY(du8, pixels, destination, stride, width, height, rPrimary, gPrimary,
                     bPrimary);
    }
}