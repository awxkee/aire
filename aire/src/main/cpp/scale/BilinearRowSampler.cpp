/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 26/02/24, 6:13 PM
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

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "scale/BilinearRowSampler.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"
#include "BilinearRowSampler.hpp"
#include "sampler-inl.h"
#include "algo/math-inl.h"

HWY_BEFORE_NAMESPACE();
namespace aire::HWY_NAMESPACE {

    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    void BilinearRowSampler10Bit::sample(const int y) {
        const auto src8 = reinterpret_cast<const uint8_t *>(mSource);
        auto dst16 = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(mDestination) + y * dstStride);
        for (int x = 0; x < outputWidth; ++x) {
            float srcX = (float) x * xScale;
            float srcY = (float) y * yScale;

            int x1 = static_cast<int>(srcX);
            int y1 = static_cast<int>(srcY);

            int x2 = min(x1 + 1, inputWidth - 1);
            int y2 = min(y1 + 1, inputHeight - 1);

            float dx((float) srcX - (float) x1);
            float dy((float) srcY - (float) y1);

            auto row1 = reinterpret_cast<const uint32_t *>(src8 + y1 * srcStride);
            auto row2 = reinterpret_cast<const uint32_t *>(src8 + y2 * srcStride);

            uint32_t c1 = static_cast<uint32_t>(row1[x1]);
            uint32_t c2 = static_cast<uint32_t>(row1[x2]);
            uint32_t c3 = static_cast<uint32_t>(row2[x1]);
            uint32_t c4 = static_cast<uint32_t>(row2[x2]);

            float r1, g1, b1, a1;
            float r2, g2, b2, a2;
            float r3, g3, b3, a3;
            float r4, g4, b4, a4;

            parseToFloat(c1, r1, g1, b1, a1);
            parseToFloat(c2, r2, g2, b2, a2);
            parseToFloat(c3, r3, g3, b3, a3);
            parseToFloat(c4, r4, g4, b4, a4);

            float rInter = blerp(r1, r2, r3, r4, dx, dy);
            float gInter = blerp(g1, g2, g3, g4, dx, dy);
            float bInter = blerp(b1, b2, b3, b4, dx, dy);
            float aInter = blerp(a1, a2, a3, a4, dx, dy);

            auto R10 = static_cast<uint32_t >(std::clamp(std::round(rInter * maxColors), 0.0f, (float) maxColors));
            auto G10 = static_cast<uint32_t >(std::clamp(std::round(gInter * maxColors), 0.0f, (float) maxColors));
            auto B10 = static_cast<uint32_t >(std::clamp(std::round(bInter * maxColors), 0.0f, (float) maxColors));
            auto A10 = static_cast<uint32_t >(std::clamp(std::round(aInter * 3.f), 0.0f, 3.0f));

            dst16[x] = (A10 << 30) | (R10 << 20) | (G10 << 10) | B10;
        }
    }

    void BilinearRowSampler16Bit::sample(const int y) {
        const FixedTag<float32_t, 4> dfx4;
        const FixedTag<int32_t, 4> dix4;
        const FixedTag<hwy::float16_t, 4> df16x4;
        using VI4 = Vec<decltype(dix4)>;
        using VF4 = Vec<decltype(dfx4)>;
        using VF16x4 = Vec<decltype(df16x4)>;

        const int shift[4] = {0, 1, 2, 3};
        const VI4 shiftV = LoadU(dix4, shift);
        const VF4 xScaleV = Set(dfx4, xScale);
        const VF4 yScaleV = Set(dfx4, yScale);
        const VI4 addOne = Set(dix4, 1);
        const VF4 fOneV = Set(dfx4, 1.0f);
        const VI4 maxWidth = Set(dix4, inputWidth - 1);
        const VI4 maxHeight = Set(dix4, inputHeight - 1);
        const VF4 vfZeros = Zero(dfx4);
        const VI4 srcStrideV = Set(dix4, srcStride);

        const auto src8 = reinterpret_cast<const uint8_t *>(mSource);
        auto dst16 = reinterpret_cast<uint16_t *>(reinterpret_cast<uint8_t *>(mDestination) + y * dstStride);

        for (int x = 0; x < outputWidth; ++x) {
            if (components == 4 && x + 8 < outputWidth) {
                VI4 currentX = Set(dix4, x);
                VI4 currentXV = Add(currentX, shiftV);
                VF4 currentXVF = Mul(ConvertTo(dfx4, currentXV), xScaleV);
                VF4 currentYVF = Mul(ConvertTo(dfx4, Set(dix4, y)), yScaleV);

                VI4 xi1 = ConvertTo(dix4, Floor(currentXVF));
                VI4 yi1 = Min(ConvertTo(dix4, Floor(currentYVF)), maxHeight);

                VI4 xi2 = Min(Add(xi1, addOne), maxWidth);
                VI4 yi2 = Min(Add(yi1, addOne), maxHeight);

                VI4 row1Add = Mul(yi1, srcStrideV);
                VI4 row2Add = Mul(yi2, srcStrideV);

                VF4 dx = Max(Sub(currentXVF, ConvertTo(dfx4, xi1)), vfZeros);
                VF4 dy = Max(Sub(currentYVF, ConvertTo(dfx4, yi1)), vfZeros);

                for (int i = 0; i < 4; i++) {
                    auto row1 = reinterpret_cast<const hwy::float16_t *>(src8 +
                                                                         ExtractLane(row1Add, i));
                    auto row2 = reinterpret_cast<const hwy::float16_t *>(src8 +
                                                                         ExtractLane(row2Add, i));
                    VF16x4 lane = LoadU(df16x4, &row1[ExtractLane(xi1, i) * components]);
                    VF4 c1 = PromoteTo(dfx4, lane);
                    lane = LoadU(df16x4, &row1[ExtractLane(xi2, i) * components]);
                    VF4 c2 = PromoteTo(dfx4, lane);
                    lane = LoadU(df16x4, &row2[ExtractLane(xi1, i) * components]);
                    VF4 c3 = PromoteTo(dfx4, lane);
                    lane = LoadU(df16x4, &row2[ExtractLane(xi2, i) * components]);
                    VF4 c4 = PromoteTo(dfx4, lane);
                    VF4 value = Blerp(dfx4, c1, c2, c3, c4, Set(dfx4, ExtractLane(dx, i)),
                                      Set(dfx4, ExtractLane(dy, i)));
                    VF16x4 pixel = DemoteTo(df16x4, Max(value, vfZeros));
                    auto u8Store = reinterpret_cast<hwy::float16_t *>(&dst16[ExtractLane(currentXV, i) * components]);
                    StoreU(pixel, df16x4, u8Store);
                }

                x += components - 1;
            } else {
                float srcX = (float) x * xScale;
                float srcY = (float) y * yScale;

                int x1 = static_cast<int>(srcX);
                int y1 = static_cast<int>(srcY);

                int x2 = min(x1 + 1, inputWidth - 1);
                int y2 = min(y1 + 1, inputHeight - 1);

                float dx((float) srcX - (float) x1);
                float dy((float) srcY - (float) y1);

                auto row1 = reinterpret_cast<const uint16_t *>(src8 + y1 * srcStride);
                auto row2 = reinterpret_cast<const uint16_t *>(src8 + y2 * srcStride);

                const int px = x * components;

                for (int c = 0; c < components; ++c) {
                    float c1 = hwy::F32FromF16(hwy::float16_t::FromBits(row1[x1 * components + c]));
                    float c2 = hwy::F32FromF16(hwy::float16_t::FromBits(row1[x2 * components + c]));
                    float c3 = hwy::F32FromF16(hwy::float16_t::FromBits(row2[x1 * components + c]));
                    float c4 = hwy::F32FromF16(hwy::float16_t::FromBits(row2[x2 * components + c]));
                    float result = blerp(c1, c2, c3, c4, dx, dy);
                    dst16[px + c] = half(result).data_;
                }
            }
        }
    }

    void BilinearRowSampler8Bit::sample(const int row) {
        auto dst8 = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(mDestination) + row * dstStride);
        auto dst = reinterpret_cast<uint8_t *>(dst8);

        const uint8_t *src8 = mSource;

        for (int x = 0; x < outputWidth; ++x) {
            const float srcX = (float) x * xScale;
            const float srcY = (float) row * yScale;

            const int x1 = static_cast<int>(std::floor(srcX));
            const int y1 = static_cast<int>(std::floor(srcY));

            int x2 = min(x1 + 1, inputWidth - 1);
            int y2 = min(y1 + 1, inputHeight - 1);

            float dx = max((float) srcX - (float) x1, 0.0f);
            float dy = max((float) srcY - (float) y1, 0.0f);

            auto row1 = reinterpret_cast<const uint8_t *>(src8 + y1 * srcStride);
            auto row2 = reinterpret_cast<const uint8_t *>(src8 + y2 * srcStride);

            for (int c = 0; c < components; ++c) {
                float c1 = static_cast<float>(row1[x1 * components + c]);
                float c2 = static_cast<float>(row1[x2 * components + c]);
                float c3 = static_cast<float>(row2[x1 * components + c]);
                float c4 = static_cast<float>(row2[x2 * components + c]);

                float result = blerp(c1, c2, c3, c4, dx, dy);
                float f = result;
                f = clamp(round(f), 0.0f, maxColors);
                dst[x * components + c] = static_cast<uint8_t>(f);
            }
        }
    }

    void BilinearRowSampler4Chan8Bit::sample(const int row) {
        const FixedTag<float32_t, 4> dfx4;
        const FixedTag<int32_t, 4> dix4;
        const FixedTag<uint8_t, 4> du8x4;
        using VU8x4 = Vec<decltype(du8x4)>;

        using VI4 = Vec<decltype(dix4)>;
        using VF4 = Vec<decltype(dfx4)>;
        const int shift[4] = {0, 1, 2, 3};
        const VI4 shiftV = LoadU(dix4, shift);
        const FixedTag<uint32_t, 4> dux4;
        const VF4 xScaleV = Set(dfx4, xScale);
        const VF4 yScaleV = Set(dfx4, yScale);

        const VI4 maxWidth = Set(dix4, inputWidth - 1);
        const VI4 maxHeight = Set(dix4, inputHeight - 1);

        const VI4 addOne = Set(dix4, 1);

        const VF4 vfZeros = Zero(dfx4);
        const VI4 srcStrideV = Set(dix4, srcStride);
        const VF4 maxColorsV = Set(dfx4, maxColors);

        auto dst8 = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(mDestination) + row * dstStride);
        auto dst = reinterpret_cast<uint8_t *>(dst8);

        const uint8_t *src8 = mSource;

        for (int x = 0; x < outputWidth; ++x) {
            if (components == 4 && x + 8 < outputWidth) {
                VI4 currentX = Set(dix4, x);
                VI4 currentXV = Add(currentX, shiftV);
                VF4 currentXVF = Mul(ConvertTo(dfx4, currentXV), xScaleV);
                VF4 currentYVF = Mul(ConvertTo(dfx4, Set(dix4, row)), yScaleV);

                VI4 xi1 = ConvertTo(dix4, Floor(currentXVF));
                VI4 yi1 = Min(ConvertTo(dix4, Floor(currentYVF)), maxHeight);

                VI4 xi2 = Min(Add(xi1, addOne), maxWidth);
                VI4 yi2 = Min(Add(yi1, addOne), maxHeight);

                VF4 dx = Max(Sub(currentXVF, ConvertTo(dfx4, xi1)), vfZeros);
                VF4 dy = Max(Sub(currentYVF, ConvertTo(dfx4, yi1)), vfZeros);

                VI4 row1Add = Mul(yi1, srcStrideV);
                VI4 row2Add = Mul(yi2, srcStrideV);

                for (int i = 0; i < 4; i++) {
                    auto row1 = reinterpret_cast<const uint8_t *>(src8 + ExtractLane(row1Add, i));
                    auto row2 = reinterpret_cast<const uint8_t *>(src8 + ExtractLane(row2Add, i));

                    VU8x4 lane = LoadU(du8x4, reinterpret_cast<const uint8_t *>(&row1[ExtractLane(xi1, i) * components]));
                    VF4 c1 = ConvertTo(dfx4, PromoteTo(dux4, lane));
                    lane = LoadU(du8x4,reinterpret_cast<const uint8_t *>(&row1[ExtractLane(xi2, i) * components]));
                    VF4 c2 = ConvertTo(dfx4, PromoteTo(dux4, lane));
                    lane = LoadU(du8x4,reinterpret_cast<const uint8_t *>(&row2[ExtractLane(xi1, i) * components]));
                    VF4 c3 = ConvertTo(dfx4, PromoteTo(dux4, lane));
                    lane = LoadU(du8x4,reinterpret_cast<const uint8_t *>(&row2[ExtractLane(xi2, i) * components]));
                    VF4 c4 = ConvertTo(dfx4, PromoteTo(dux4, lane));
                    VF4 value = Blerp(dfx4, c1, c2, c3, c4, Set(dfx4, ExtractLane(dx, i)),
                                      Set(dfx4, ExtractLane(dy, i)));
                    VF4 sum = ClampRound(dfx4, value, vfZeros, maxColorsV);
                    VU8x4 pixel = DemoteTo(du8x4, ConvertTo(dux4, sum));
                    auto u8Store = &dst[ExtractLane(currentXV, i) * components];
                    StoreU(pixel, du8x4, u8Store);
                }

                x += components - 1;
            } else {
                const float srcX = (float) x * xScale;
                const float srcY = (float) row * yScale;

                const int x1 = static_cast<int>(std::floor(srcX));
                const int y1 = static_cast<int>(std::floor(srcY));

                int x2 = min(x1 + 1, inputWidth - 1);
                int y2 = min(y1 + 1, inputHeight - 1);

                float dx = max((float) srcX - (float) x1, 0.0f);
                float dy = max((float) srcY - (float) y1, 0.0f);

                auto row1 = reinterpret_cast<const uint8_t *>(src8 + y1 * srcStride);
                auto row2 = reinterpret_cast<const uint8_t *>(src8 + y2 * srcStride);

                for (int c = 0; c < components; ++c) {
                    float c1 = static_cast<float>(row1[x1 * components + c]);
                    float c2 = static_cast<float>(row1[x2 * components + c]);
                    float c3 = static_cast<float>(row2[x1 * components + c]);
                    float c4 = static_cast<float>(row2[x2 * components + c]);

                    float result = blerp(c1, c2, c3, c4, dx, dy);
                    float f = result;
                    f = clamp(round(f), 0.0f, maxColors);
                    dst[x * components + c] = static_cast<uint8_t>(f);
                }
            }
        }
    }

} // aire
HWY_AFTER_NAMESPACE();