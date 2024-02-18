//
// Created by Radzivon Bartoshyk on 07/02/2024.
//

#include "Convolve1Db16.h"
#include "hwy/highway.h"
#include "jni/JNIUtils.h"
#include <thread>
#include "algo/support-inl.h"
#include "concurrency.hpp"

namespace aire {

    using namespace hwy;
    using namespace std;
    using namespace hwy::HWY_NAMESPACE;

    void Convolve1Db16::horizontalPass(std::vector<uint16_t> &transient,
                                       uint16_t *data, int stride,
                                       int y, int width,
                                       int height) {

        const int iRadius = ceil((this->horizontal.size()) / 2);
        auto src = reinterpret_cast<hwy::float16_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
        auto dst = reinterpret_cast<hwy::float16_t *>(reinterpret_cast<uint8_t *>(transient.data()) + y * stride);

        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        const VF zeros = Zero(dfx4);
        const FixedTag<hwy::float16_t, 8> df16x8;
        using VFb16 = Vec<decltype(df16x8)>;

        const FixedTag<hwy::float16_t, 4> df16x4;
        using VFb16x4 = Vec<decltype(df16x4)>;

        // Preheat kernel memory to stack
        VF kernelCache[this->horizontal.size()];
        for (int j = 0; j < this->horizontal.size(); ++j) {
            kernelCache[j] = Set(dfx4, this->horizontal[j]);
        }

        for (int x = 0; x < width; ++x) {
            VF store = zeros;

            int r = -iRadius;

            for (; r + 4 <= iRadius && x + r + 4 < width; r += 4) {
                int pos = clamp((x + r), 0, width - 1) * 4;

                VFb16 lane = LoadU(df16x8, &src[pos]);

                VF dWeight = kernelCache[r + iRadius];
                store = Add(store, Mul(PromoteUpperTo(dfx4, lane), dWeight));
                dWeight = kernelCache[r + iRadius + 1];
                store = Add(store, Mul(PromoteLowerTo(dfx4, lane), dWeight));

                pos = clamp((x + r + 2), 0, width - 1) * 4;

                lane = LoadU(df16x8, &src[pos]);

                dWeight = kernelCache[r + iRadius + 2];
                store = Add(store, Mul(PromoteUpperTo(dfx4, lane), dWeight));
                dWeight = kernelCache[r + iRadius + 3];
                store = Add(store, Mul(PromoteLowerTo(dfx4, lane), dWeight));
            }

            for (; r <= iRadius; ++r) {
                int pos = clamp((x + r), 0, width - 1) * 4;
                VF dWeight = kernelCache[r + iRadius];
                VFb16x4 pixels = LoadU(df16x4, &src[pos]);
                store = Add(store, Mul(PromoteTo(dfx4, pixels), dWeight));
            }

            VFb16x4 pixelU = DemoteTo(df16x4, store);
            StoreU(pixelU, df16x4, dst);

            dst += 4;
        }
    }

    void
    Convolve1Db16::verticalPass(std::vector<uint16_t> &transient,
                                uint16_t *data, int stride,
                                int y, int width,
                                int height) {
        const int iRadius = ceil((vertical.size()) / 2);

        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        const FixedTag<hwy::float16_t, 4> df16x4;
        using VFb16x4 = Vec<decltype(df16x4)>;
        const VF zeros = Zero(dfx4);

        // Preheat kernel memory to stack
        VF kernelCache[vertical.size()];
        for (int j = 0; j < vertical.size(); ++j) {
            kernelCache[j] = Set(dfx4, vertical[j]);
        }

        auto dst = reinterpret_cast<hwy::float16_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
        for (int x = 0; x < width; ++x) {
            VF store = zeros;

            int r = -iRadius;

            for (; r <= iRadius; ++r) {
                auto src = reinterpret_cast<hwy::float16_t * > (reinterpret_cast<uint8_t *>(transient.data()) +
                                                                clamp((r + y), 0, height - 1) * stride);
                int pos = clamp(x, 0, width - 1) * 4;
                VF dWeight = kernelCache[r + iRadius];
                VFb16x4 pixels = LoadU(df16x4, &src[pos]);
                store = Add(store, Mul(PromoteTo(dfx4, pixels), dWeight));
            }

            VFb16x4 pixelU = DemoteTo(df16x4, store);
            StoreU(pixelU, df16x4, dst);

            dst += 4;
        }
    }

    void Convolve1Db16::convolve(uint16_t *data, const int stride, const int width, const int height) {
        std::vector<uint16_t> transient(stride * height);

        int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    height * width / (256 * 256)), 1, 12);

        concurrency::parallel_for(threadCount, height, [&](int y) {
            this->horizontalPass(transient, data, stride, y, width, height);
        });;

        concurrency::parallel_for(threadCount, height, [&](int y) {
            this->verticalPass(transient, data, stride, y, width, height);
        });
    }

}