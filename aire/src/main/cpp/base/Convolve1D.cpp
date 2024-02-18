//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#include "Convolve1D.h"
#include "hwy/highway.h"
#include "jni/JNIUtils.h"
#include <thread>
#include "algo/support-inl.h"
#include <omp.h>

namespace aire {

    using namespace hwy;
    using namespace std;
    using namespace hwy::HWY_NAMESPACE;

    void
    convolve1DHorizontalPass(std::vector<uint8_t> &transient,
                             uint8_t *data, int stride,
                             int y, int width,
                             int height,
                             const vector<float> &kernel) {

        const int iRadius = ceil((kernel.size()) / 2);
        auto src = reinterpret_cast<uint8_t *>(data + y * stride);
        auto dst = reinterpret_cast<uint8_t *>(transient.data() + y * stride);

        const FixedTag<uint8_t, 4> du8;
        const FixedTag<uint32_t, 4> du32x4;
        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        using VU = Vec<decltype(du8)>;
        const auto max255 = Set(dfx4, 255.0f);
        const VF zeros = Zero(dfx4);
        const FixedTag<uint8_t, 16> du8x16;

        // Preheat kernel memory to stack
        VF kernelCache[kernel.size()];
        for (int j = 0; j < kernel.size(); ++j) {
            kernelCache[j] = Set(dfx4, kernel[j]);
        }

        for (int x = 0; x < width; ++x) {
            VF store = zeros;

            int r = -iRadius;

            for (; r + 4 <= iRadius && x + r + 4 < width; r += 4) {
                int pos = clamp((x + r), 0, width - 1) * 4;

                VF v1, v2, v3, v4;
                auto pu = LoadU(du8x16, &src[pos]);
                ConvertToFloatVec16(du8x16, pu, v1, v2, v3, v4);

                VF dWeight = kernelCache[r + iRadius];
                store = Add(store, Mul(v1, dWeight));
                dWeight = kernelCache[r + iRadius + 1];
                store = Add(store, Mul(v2, dWeight));
                dWeight = kernelCache[r + iRadius + 2];
                store = Add(store, Mul(v3, dWeight));
                dWeight = kernelCache[r + iRadius + 3];
                store = Add(store, Mul(v4, dWeight));
            }

            for (; r <= iRadius; ++r) {
                int pos = clamp((x + r), 0, width - 1) * 4;
                VF dWeight = kernelCache[r + iRadius];
                VU pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
            }

            store = Max(Min(Round(store), max255), zeros);
            VU pixelU = DemoteTo(du8, ConvertTo(du32x4, store));
            StoreU(pixelU, du8, dst);

            dst += 4;
        }
    }

    void
    convolve1DVerticalPass(std::vector<uint8_t> &transient, uint8_t *data, int stride,
                           int y, int width, int height,
                           const vector<float> &kernel) {
        const int iRadius = ceil((kernel.size()) / 2);

        const FixedTag<uint8_t, 4> du8;
        const FixedTag<uint32_t, 4> du32x4;
        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        using VU = Vec<decltype(du8)>;
        const auto max255 = Set(dfx4, 255.0f);
        const VF zeros = Zero(dfx4);

        // Preheat kernel memory to stack
        VF kernelCache[kernel.size()];
        for (int j = 0; j < kernel.size(); ++j) {
            kernelCache[j] = Set(dfx4, kernel[j]);
        }

        auto dst = reinterpret_cast<uint8_t *>(data + y * stride);
        for (int x = 0; x < width; ++x) {
            VF store = zeros;

            int r = -iRadius;

            for (; r <= iRadius; ++r) {
                auto src = reinterpret_cast<uint8_t *>(transient.data() +
                                                       clamp((r + y), 0, height - 1) * stride);
                int pos = clamp(x, 0, width - 1) * 4;
                VF dWeight = kernelCache[r + iRadius];
                VU pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
            }

            store = Max(Min(Round(store), max255), zeros);
            VU pixelU = DemoteTo(du8, ConvertTo(du32x4, store));

            StoreU(pixelU, du8, dst);

            dst += 4;
        }
    }

    void convolve1D(uint8_t *data, int stride, int width, int height, const std::vector<float> &horizontal, const std::vector<float> &vertical) {
        std::vector<uint8_t> transient(stride * height);

#pragma omp parallel for num_threads(6)
        for (int y = 0; y < height; ++y) {
            convolve1DHorizontalPass(transient, data, stride, y, width, height, horizontal);
        }

#pragma omp parallel for num_threads(6)
        for (int y = 0; y < height; ++y) {
            convolve1DVerticalPass(transient, data, stride, y, width, height, vertical);
        }

//        vector<thread> workers;
//
//        int segmentHeight = height / threadCount;
//
//        for (int i = 0; i < threadCount; i++) {
//            int start = i * segmentHeight;
//            int end = (i + 1) * segmentHeight;
//            if (i == threadCount - 1) {
//                end = height;
//            }
//            workers.emplace_back(
//                    [start, end, width, height, stride, data, &transient, &horizontal]() {
//                        for (int y = start; y < end; ++y) {
//                            convolve1DHorizontalPass(transient, data, stride, y, width, height,
//                                                     horizontal);
//                        }
//                    });
//        }
//
//        for (std::thread &thread: workers) {
//            thread.join();
//        }

//        workers.clear();
//
//        for (int i = 0; i < threadCount; i++) {
//            int start = i * segmentHeight;
//            int end = (i + 1) * segmentHeight;
//            if (i == threadCount - 1) {
//                end = height;
//            }
//            workers.emplace_back(
//                    [start, end, width, height, stride, data, &transient, &vertical]() {
//                        for (int y = start; y < end; ++y) {
//                            convolve1DVerticalPass(transient, data, stride, y, width, height,
//                                                   vertical);
//                        }
//                    });
//        }
//
//        for (std::thread &thread: workers) {
//            thread.join();
//        }
//
//        transient.clear();
    }

    }