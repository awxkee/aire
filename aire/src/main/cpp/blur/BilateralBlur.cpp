//
// Created by Radzivon Bartoshyk on 31/01/2024.
//

#include "BilateralBlur.h"
#include <vector>
#include "MathUtils.hpp"
#include <thread>
#include <fast_math-inl.h>
#include "concurrency.hpp"

#include "hwy/highway.h"

namespace aire {

    using namespace std;
    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    template<class V>
    void
    bilateralBlurPass(V *data, V *transient, int y, int stride, int width, int height, const int size,
                      float rangeSigma,
                      float spatialSigma) {
        const float dRangeSigma = 2.f * rangeSigma * rangeSigma;
        const float dSpatialSigma = 2.f * spatialSigma * spatialSigma;

        const FixedTag<uint8_t, 4> du8;
        const FixedTag<uint32_t, 4> du32x4;
        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        using VU = Vec<decltype(du8)>;
        const auto zeros = Zero(dfx4);

        const float lumaPrimaries[4] = {0.299f, 0.587f, 0.114f, 0.f};
        const VF vLumaPrimaries = LoadU(dfx4, lumaPrimaries);

        auto src = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data) + y * stride);
        auto dst = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(transient) +
                                         y * stride);

        std::vector<std::vector<float>> spatialWeights(size,
                                                       std::vector<float>(size, 0));

        const int rad = size / 2;

        for (int j = -rad; j <= rad; ++j) {
            for (int i = -rad; i <= rad; ++i) {
                int py = clamp(y + j, 0, height - 1);
                float dx = (float(i) - float(0));
                float dy = (float(py) - float(y));
                float distance = std::sqrt(dx * dx + dy * dy);
                spatialWeights[j + rad][i + rad] = -distance / dSpatialSigma;
            }
        }

        for (int x = 0; x < width; ++x) {
            VF store = zeros;
            int currentPixelPosition = x * 4;
            VF current = ConvertTo(dfx4, PromoteTo(du32x4, LoadU(du8, &src[currentPixelPosition])));
            float kernelSum = 0.f;
            float intensity = ExtractLane(SumOfLanes(dfx4, Mul(current, vLumaPrimaries)), 0);

            for (int j = -rad; j <= rad; ++j) {
                for (int i = -rad; i <= rad; ++i) {
                    int py = clamp(y + j, 0, height - 1);
                    int px = clamp(x + i, 0, width - 1);
                    auto mSrc = reinterpret_cast<V *>(reinterpret_cast<uint8_t *>(data) +
                                                      py * stride);

                    int srcX = px * 4;

                    VF local = ConvertTo(dfx4, PromoteTo(du32x4, LoadU(du8, &mSrc[srcX])));

                    float localIntensity = ExtractLane(SumOfLanes(dfx4, Mul(local, vLumaPrimaries)),
                                                       0);
                    float drIntensity = localIntensity - intensity;

                    float weight = std::exp(
                            spatialWeights[j + rad][i + rad] -
                            (drIntensity * drIntensity) / dRangeSigma);

                    kernelSum += weight;
                    store = Add(store, Mul(local, Set(dfx4, weight)));
                }
            }

            VU pixel = DemoteTo(du8, ConvertTo(du32x4, Div(store, Set(dfx4, kernelSum))));
            StoreU(pixel, du8, &dst[0]);
            dst[3] = src[3];
            dst += 4;
        }
    }

    template<class V>
    void bilateralBlur(V *data, int stride, int width, int height, const int size, float rangeSigma,
                       float spatialSigma) {
        int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    height * width / (256 * 256)), 1, 12);
        std::vector<V> transient(stride * height);

        concurrency::parallel_for(threadCount, height, [&](int y) {
            bilateralBlurPass(data, transient.data(), y,
                              stride, width, height, size, rangeSigma,
                              spatialSigma);
        });

        std::copy(transient.begin(), transient.end(), data);
    }

    template void
    bilateralBlur(uint8_t *data, int stride, int width, int height, const int size, float sigma,
                  float spatialSigma);
}