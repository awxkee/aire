//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include "Arithmetics.h"
#include "hwy/highway.h"
#include "algo/support-inl.h"

namespace aire {

    using namespace std;
    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    void absDiff(uint8_t *destination, uint8_t *s1, uint8_t *s2, int width, int height) {
        const ScalableTag<uint8_t> du;
        using VU = Vec<decltype(du)>;
        const RebindToSigned<decltype(du)> di;
        using VI = Vec<decltype(du)>;
        const int lanes = du.MaxLanes();
        for (int y = 0; y < height; ++y) {
            auto ms = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(s1) + y * width);
            auto ds = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(s2) +
                                                  y * width);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination) +
                                                   y * width);
            int x = 0;
            for (; x + lanes < width; x += lanes) {
                auto m = LoadU(di, reinterpret_cast<int8_t *>(ms));
                auto s = LoadU(di, reinterpret_cast<int8_t *>(ds));
                StoreU(Abs(Sub(m, s)), di, reinterpret_cast<int8_t *>(dst));
                dst += lanes;
                ms += lanes;
                ds += lanes;
            }
            for (; x < width; ++x) {
                dst[0] = std::abs(ms[0] - ds[0]);
                ds += 1;
                ms += 1;
                dst += 1;
            }
        }
    }

    template<class V>
    void pickMaxMin(V *source, int width, int height, V *min, V *max) {
        V mMin = 0;
        V mMax = 0;

        const ScalableTag<V> du;
        using VU = Vec<decltype(du)>;
        const int lanes = du.MaxLanes();

        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<V *>(reinterpret_cast<V *>(source) + y * width);
            int x = 0;
            for (; x + lanes < width; x += lanes) {
                auto lane = LoadU(du, src);
                auto possibleMin = ExtractLane(MinOfLanes(du, lane), 0);
                auto possibleMax = ExtractLane(MaxOfLanes(du, lane), 0);
                if (possibleMin < mMin) {
                    mMin = possibleMin;
                }
                if (possibleMax > mMax) {
                    mMax = possibleMax;
                }
                src += lanes;
            }
            for (; x < width; ++x) {
                if (src[0] < mMin) {
                    mMin = src[0];
                }
                if (src[0] > mMax) {
                    mMax = src[0];
                }
                src += 1;
            }
        }
        *min = mMin;
        *max = mMax;
    }

    void fillSurface(uint8_t *destination, uint32_t value, int stride, int width, int height) {
        const FixedTag<uint32_t, 1> du32x1;
        const FixedTag<uint32_t, 4> du32x4;
        const auto low = Set(du32x1, value);
        const auto high = Set(du32x4, value);
        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(destination) +
                                                    y * stride);
            int x = 0;
            for (; x + 4 < width; x += 4) {
                StoreU(high, du32x4, src);
                src += 4;
            }
            for (; x < width; ++x) {
                StoreU(low, du32x1, src);
                src[0] = value;
                src += 1;
            }
        }
    }

    template<class V>
    void normalize(V *source, int width, int height, V min, V max) {
        V globalMax = 0;
        V globalMin = 0;

        const FixedTag<uint8_t, 4> du8;
        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        using VU = Vec<decltype(du8)>;

        pickMaxMin<V>(reinterpret_cast<V *>(source), width, height,
                      reinterpret_cast<V *>(&globalMin), reinterpret_cast<V *>(&globalMax));
        const V oldMin = 0;
        const int lanes = 4;
        const auto vGlobalMin = Set(dfx4, globalMin);
        const auto vGlobalRange = ApproximateReciprocal(Set(dfx4, globalMax - globalMin));
        const auto vNewRange = Set(dfx4, static_cast<float>(max - min));
        const auto vNewMin = Set(dfx4, static_cast<float>(min));
        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(source) + y * width);
            int x = 0;
            for (; x + lanes < width; x += lanes) {
                VF px = ConvertToFloat(dfx4, LoadU(du8, src));
                VF normalized = Mul(Sub(px, vGlobalMin), vGlobalRange);
                VF rebalanced = Ceil(MulAdd(normalized, vNewRange, vNewMin));
                StoreU(DemoteToU8(du8, rebalanced), du8, src);
                src += lanes;
            }
            for (; x < width; ++x) {
                float normalized = ((float(src[0]) - float(globalMin)) /
                                    (float(globalMax) - float(globalMin)));
                float vl = normalized * (max - min) + min;
                V value = ceil(vl);
                src[0] = value;
                src += 1;
            }
        }
    }

    void diff(uint8_t *destination, uint8_t value, uint8_t *s1, int width, int height) {

        const ScalableTag<uint8_t> du16;
        using VU = Vec<decltype(du16)>;

        const auto leading = Set(du16, value);
        const int lanes = du16.MaxLanes();

        for (int y = 0; y < height; ++y) {
            auto ms = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(s1) + y * width);
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination) +
                                                   y * width);
            int x = 0;

            for (; x + lanes < width; x += lanes) {
                VU pixels = LoadU(du16, ms);
                VU rs = Sub(leading, pixels);
                StoreU(rs, du16, dst);
                ms += lanes;
                dst += lanes;
            }

            for (; x < width; ++x) {
                dst[0] = value - ms[0];
                ms += 1;
                dst += 1;
            }
        }
    }

    template
    void pickMaxMin(uint8_t *source, int width, int height, uint8_t *min, uint8_t *max);

    template void normalize(uint8_t *source, int width, int height, uint8_t min, uint8_t max);

}