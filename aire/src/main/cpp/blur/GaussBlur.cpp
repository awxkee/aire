//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#include "GaussBlur.h"
#include <vector>
#include <thread>

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "blur/GaussBlur.cpp"

#include "hwy/foreach_target.h"
#include "hwy/highway.h"
#include <algorithm>
#include "MathUtils.hpp"

using namespace std;

HWY_BEFORE_NAMESPACE();

namespace aire::HWY_NAMESPACE {

    using hwy::HWY_NAMESPACE::FixedTag;
    using hwy::HWY_NAMESPACE::Vec;
    using hwy::float32_t;
    using hwy::HWY_NAMESPACE::LoadU;
    using hwy::HWY_NAMESPACE::Set;
    using hwy::HWY_NAMESPACE::Zero;
    using hwy::HWY_NAMESPACE::PromoteTo;
    using hwy::HWY_NAMESPACE::ConvertTo;
    using hwy::HWY_NAMESPACE::Mul;
    using hwy::HWY_NAMESPACE::Add;
    using hwy::HWY_NAMESPACE::Min;
    using hwy::HWY_NAMESPACE::Max;
    using hwy::HWY_NAMESPACE::Round;
    using hwy::HWY_NAMESPACE::DemoteTo;
    using hwy::HWY_NAMESPACE::StoreU;
    using hwy::HWY_NAMESPACE::PromoteLowerTo;
    using hwy::HWY_NAMESPACE::PromoteUpperTo;
    using hwy::HWY_NAMESPACE::LowerHalf;
    using hwy::HWY_NAMESPACE::ExtractLane;
    using hwy::HWY_NAMESPACE::UpperHalf;

    void
    gaussBlurHorizontal(std::vector<uint8_t> &transient,
                        uint8_t *data, int stride,
                        int y, int width,
                        int height, float radius,
                        vector<float> &kernel) {

        const int iRadius = ceil(radius);
        auto src = reinterpret_cast<uint8_t *>(data + y * stride);
        auto dst = reinterpret_cast<uint8_t *>(transient.data() + y * stride);

        const FixedTag<uint8_t, 4> du8;
        const FixedTag<uint8_t, 16> du8x16;
        const FixedTag<uint32_t, 4> du32x4;
        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        using VU = Vec<decltype(du8)>;
        const auto max255 = Set(dfx4, 255.0f);
        const VF zeros = Zero(dfx4);

        for (int x = 0; x < width; ++x) {
            VF store = zeros;

            int r = -iRadius;

            for (; r <= iRadius; ++r) {
                int pos = clamp((x + r), 0, width - 1) * 4;
                float weight = kernel[r + iRadius];
                VF dWeight = Set(dfx4, weight);
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
    gaussBlurVertical(std::vector<uint8_t> &transient,
                      uint8_t *data, int stride,
                      int y, int width,
                      int height, float radius,
                      vector<float> &kernel) {
        const int iRadius = ceil(radius);

        const FixedTag<uint8_t, 4> du8;
        const FixedTag<uint32_t, 4> du32x4;
        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        using VU = Vec<decltype(du8)>;
        const auto max255 = Set(dfx4, 255.0f);
        const VF zeros = Zero(dfx4);

        auto dst = reinterpret_cast<uint8_t *>(data + y * stride);
        for (int x = 0; x < width; ++x) {
            VF store = zeros;

            int r = -iRadius;

            for (; r <= iRadius; ++r) {
                auto src = reinterpret_cast<uint8_t *>(transient.data() +
                                                       clamp((r + y), 0, height - 1) * stride);
                int pos = clamp(x, 0, width - 1) * 4;
                float weight = kernel[r + iRadius];
                VF dWeight = Set(dfx4, weight);
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
    gaussBlurTear(uint8_t *data, int stride, int width, int height, float radius, float sigma) {
        vector<float> kernel = compute1DGaussianKernel(radius * 2 + 1, sigma);

        std::vector<uint8_t> transient(stride * height);
        int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    height * width / (256 * 256)), 1, 12);
        vector<thread> workers;

        int segmentHeight = height / threadCount;

        for (int i = 0; i < threadCount; i++) {
            int start = i * segmentHeight;
            int end = (i + 1) * segmentHeight;
            if (i == threadCount - 1) {
                end = height;
            }
            workers.emplace_back(
                    [start, end, width, height, stride, data, radius, &transient, &kernel]() {
                        for (int y = start; y < end; ++y) {
                            gaussBlurHorizontal(transient, data, stride, y, width, height, radius,
                                                kernel);
                        }
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }

        workers.clear();

        for (int i = 0; i < threadCount; i++) {
            int start = i * segmentHeight;
            int end = (i + 1) * segmentHeight;
            if (i == threadCount - 1) {
                end = height;
            }
            workers.emplace_back(
                    [start, end, width, height, stride, data, radius, &transient, &kernel]() {
                        for (int y = start; y < end; ++y) {
                            gaussBlurVertical(transient, data, stride, y, width, height, radius,
                                              kernel);
                        }
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }

        transient.clear();
    }
}

HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace aire {
    HWY_EXPORT(gaussBlurTear);

    void gaussBlurU8(uint8_t *data, int stride, int width, int height, float radius, float sigma) {
        HWY_DYNAMIC_DISPATCH(gaussBlurTear)(data, stride, width, height, radius, sigma);
    }

    template<class V>
    void gaussianBlur(V *data, int stride, int width, int height, float radius, float sigma) {
        vector<float> kernel = compute1DGaussianKernel(radius * 2 + 1, sigma);

        float rStore = 0.f;
        float gStore = 0.f;
        float bStore = 0.f;
        float aStore = 0.f;
        std::vector<V> transient(stride * height);
        const int iRadius = ceil(radius);
        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            auto dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient.data()) +
                                             y * stride);
            for (int x = 0; x < width; ++x) {

                rStore = 0.f;
                gStore = 0.f;
                bStore = 0.f;
                aStore = 0.f;

                for (int r = -iRadius; r <= iRadius; ++r) {
                    int px = clamp(x + r, 0, width - 1);
                    int pos = px * 4;
                    float weight = kernel[r + iRadius];

                    rStore += src[pos + 0] * weight;
                    gStore += src[pos + 1] * weight;
                    bStore += src[pos + 2] * weight;
                    aStore += src[pos + 3] * weight;
                }

                dst[0] = clamp(ceil(rStore), 0, 255);
                dst[1] = clamp(ceil(gStore), 0, 255);
                dst[2] = clamp(ceil(bStore), 0, 255);
                dst[3] = clamp(ceil(aStore), 0, 255);

                dst += 4;
            }
        }

        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            for (int x = 0; x < width; ++x) {
                rStore = 0.f;
                gStore = 0.f;
                bStore = 0.f;
                aStore = 0.f;

                for (int r = -iRadius; r <= iRadius; ++r) {
                    int py = clamp((r + y), 0, height - 1);
                    V *src = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient.data()) +
                                                   py * stride);
                    int px = clamp(x, 0, width - 1);
                    int pos = px * 4;

                    float weight = kernel[r + iRadius];
                    rStore += src[pos + 0] * weight;
                    gStore += src[pos + 1] * weight;
                    bStore += src[pos + 2] * weight;
                    aStore += src[pos + 3] * weight;
                }

                dst[0] = clamp(ceil(rStore), 0, 255);
                dst[1] = clamp(ceil(gStore), 0, 255);
                dst[2] = clamp(ceil(bStore), 0, 255);
                dst[3] = clamp(ceil(aStore), 0, 255);

                dst += 4;
            }
        }
    }

    template void
    gaussianBlur(uint8_t *data, int stride, int width, int height, float radius, float sigma);

    template void
    gaussianBlur(uint16_t *data, int stride, int width, int height, float radius, float sigma);

}

#endif