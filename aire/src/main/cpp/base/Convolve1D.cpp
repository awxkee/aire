//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#include "Convolve1D.h"
#include "hwy/highway.h"
#include "jni/JNIUtils.h"
#include <thread>
#include "algo/support-inl.h"
#include "concurrency.hpp"
#include "Eigen/Eigen"
#include "FF1DWorkspace.hpp"

namespace aire {

    using namespace hwy;
    using namespace std;
    using namespace hwy::HWY_NAMESPACE;

    void
    convolve1DHorizontalPass(std::vector<uint8_t> &transient,
                             uint8_t *data, int stride,
                             int y, int width,
                             int height,
                             const Eigen::VectorXf &kernel) {

        const int kernelSize = kernel.size();
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

        const int halfOfKernel = kernelSize / 2;
        const bool isEven = kernelSize % 2 == 0;
        const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

        for (int x = 0; x < width; ++x) {
            VF store = zeros;

            int r = -halfOfKernel;

            if (kernelSize == 3) {
                int pos = clamp(x - 1, 0, width - 1) * 4;
                VF dWeight = kernelCache[0];
                VU pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x, 0, width - 1) * 4;
                dWeight = kernelCache[1];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 1, 0, width - 1) * 4;
                dWeight = kernelCache[2];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
            } else if (kernelSize == 5) {
                int pos = clamp(x - 2, 0, width - 1) * 4;
                VF dWeight = kernelCache[0];
                VU pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x - 1, 0, width - 1) * 4;
                dWeight = kernelCache[1];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x, 0, width - 1) * 4;
                dWeight = kernelCache[2];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 1, 0, width - 1) * 4;
                dWeight = kernelCache[3];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 2, 0, width - 1) * 4;
                dWeight = kernelCache[4];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
            } else if (kernelSize == 7) {
                int pos = clamp(x - 3, 0, width - 1) * 4;
                VF dWeight = kernelCache[0];
                VU pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x - 2, 0, width - 1) * 4;
                dWeight = kernelCache[1];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x - 1, 0, width - 1) * 4;
                dWeight = kernelCache[2];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x, 0, width - 1) * 4;
                dWeight = kernelCache[3];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 1, 0, width - 1) * 4;
                dWeight = kernelCache[4];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 2, 0, width - 1) * 4;
                dWeight = kernelCache[5];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 3, 0, width - 1) * 4;
                dWeight = kernelCache[6];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
            } else if (kernelSize == 9) {
                int pos = clamp(x - 4, 0, width - 1) * 4;
                VF dWeight = kernelCache[0];
                VU pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x - 3, 0, width - 1) * 4;
                dWeight = kernelCache[1];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x - 2, 0, width - 1) * 4;
                dWeight = kernelCache[2];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x - 1, 0, width - 1) * 4;
                dWeight = kernelCache[3];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x, 0, width - 1) * 4;
                dWeight = kernelCache[4];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 1, 0, width - 1) * 4;
                dWeight = kernelCache[5];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 2, 0, width - 1) * 4;
                dWeight = kernelCache[6];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 3, 0, width - 1) * 4;
                dWeight = kernelCache[7];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));

                pos = clamp(x + 4, 0, width - 1) * 4;
                dWeight = kernelCache[8];
                pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
            } else {
                for (; r + 4 <= maxKernel && x + r + 4 < width; r += 4) {
                    int pos = clamp((x + r), 0, width - 1) * 4;

                    VF v1, v2, v3, v4;
                    auto pu = LoadU(du8x16, &src[pos]);
                    ConvertToFloatVec16(du8x16, pu, v1, v2, v3, v4);

                    int pf = r + halfOfKernel;

                    VF dWeight = kernelCache[pf];
                    store = Add(store, Mul(v1, dWeight));
                    dWeight = kernelCache[pf + 1];
                    store = Add(store, Mul(v2, dWeight));
                    dWeight = kernelCache[pf + 2];
                    store = Add(store, Mul(v3, dWeight));
                    dWeight = kernelCache[pf + 3];
                    store = Add(store, Mul(v4, dWeight));
                }

                for (; r <= maxKernel; ++r) {
                    int pos = clamp((x + r), 0, width - 1) * 4;
                    VF dWeight = kernelCache[r + halfOfKernel];
                    VU pixels = LoadU(du8, &src[pos]);
                    store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
                }
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
                           const Eigen::VectorXf &kernel) {
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

        const int halfOfKernel = kernel.size() / 2;
        const bool isEven = kernel.size() % 2 == 0;
        const int maxKernel = isEven ? halfOfKernel - 1 : halfOfKernel;

        auto dst = reinterpret_cast<uint8_t *>(data + y * stride);
        for (int x = 0; x < width; ++x) {
            VF store = zeros;

            int r = -halfOfKernel;

            for (; r <= maxKernel; ++r) {
                auto src = reinterpret_cast<uint8_t *>(transient.data() +
                                                       clamp((r + y), 0, height - 1) * stride);
                int pos = clamp(x, 0, width - 1) * 4;
                VF dWeight = kernelCache[r + halfOfKernel];
                VU pixels = LoadU(du8, &src[pos]);
                store = Add(store, Mul(ConvertTo(dfx4, PromoteTo(du32x4, pixels)), dWeight));
            }

            store = Max(Min(Round(store), max255), zeros);
            VU pixelU = DemoteTo(du8, ConvertTo(du32x4, store));

            StoreU(pixelU, du8, dst);

            dst += 4;
        }
    }

    void fftConvolve(uint8_t *data, int stride, int width, int height, const Eigen::VectorXf &horizontal, const Eigen::VectorXf &vertical) {
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rChannel(height, width);
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> gChannel(height, width);
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> bChannel(height, width);
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> aChannel(height, width);

        concurrency::parallel_for(2, height, [&](int y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;
            for (; x < width; ++x) {
                rChannel(y, x) = src[0] / 255.f;
                gChannel(y, x) = src[1] / 255.f;
                bChannel(y, x) = src[2] / 255.f;
                aChannel(y, x) = src[3] / 255.f;
                src += 4;
            }
        });

        std::unique_ptr<FF1DWorkspace> horizontalWorkspace = std::make_unique<FF1DWorkspace>(height, width, horizontal.size());
        horizontalWorkspace->convolve(rChannel.data(), horizontal.data());
        for (int y = 0; y < height; ++y) {
            const auto src = reinterpret_cast<float *>(horizontalWorkspace->getOutput() + y * horizontalWorkspace->getDstWidth());
            std::copy(src, src + width, rChannel.row(y).data());
        }
        horizontalWorkspace->convolve(gChannel.data(), horizontal.data());
        for (int y = 0; y < height; ++y) {
            const auto src = reinterpret_cast<float *>(horizontalWorkspace->getOutput() + y * horizontalWorkspace->getDstWidth());
            std::copy(src, src + width, gChannel.row(y).data());
        }
        horizontalWorkspace->convolve(bChannel.data(), horizontal.data());
        for (int y = 0; y < height; ++y) {
            const auto src = reinterpret_cast<float *>(horizontalWorkspace->getOutput() + y * horizontalWorkspace->getDstWidth());
            std::copy(src, src + width, bChannel.row(y).data());
        }
        horizontalWorkspace->convolve(aChannel.data(), horizontal.data());
        for (int y = 0; y < height; ++y) {
            const auto src = reinterpret_cast<float *>(horizontalWorkspace->getOutput() + y * horizontalWorkspace->getDstWidth());
            std::copy(src, src + width, aChannel.row(y).data());
        }

        horizontalWorkspace.reset();

        rChannel.transposeInPlace();
        gChannel.transposeInPlace();
        bChannel.transposeInPlace();
        aChannel.transposeInPlace();

        std::unique_ptr<FF1DWorkspace> verticalWorkspace = std::make_unique<FF1DWorkspace>(rChannel.rows(),
                                                                                           rChannel.cols(),
                                                                                           vertical.size());
        verticalWorkspace->convolve(rChannel.data(), vertical.data());
        for (int y = 0; y < rChannel.rows(); ++y) {
            const auto src = reinterpret_cast<float *>(verticalWorkspace->getOutput() + y * verticalWorkspace->getDstWidth());
            std::copy(src, src + rChannel.cols(), rChannel.row(y).data());
        }
        verticalWorkspace->convolve(gChannel.data(), vertical.data());
        for (int y = 0; y < gChannel.rows(); ++y) {
            const auto src = reinterpret_cast<float *>(verticalWorkspace->getOutput() + y * verticalWorkspace->getDstWidth());
            std::copy(src, src + gChannel.cols(), gChannel.row(y).data());
        }
        verticalWorkspace->convolve(bChannel.data(), vertical.data());
        for (int y = 0; y < bChannel.rows(); ++y) {
            const auto src = reinterpret_cast<float *>(verticalWorkspace->getOutput() + y * verticalWorkspace->getDstWidth());
            std::copy(src, src + bChannel.cols(), bChannel.row(y).data());
        }
        verticalWorkspace->convolve(aChannel.data(), vertical.data());
        for (int y = 0; y < aChannel.rows(); ++y) {
            const auto src = reinterpret_cast<float *>(verticalWorkspace->getOutput() + y * verticalWorkspace->getDstWidth());
            std::copy(src, src + aChannel.cols(), aChannel.row(y).data());
        }

        rChannel.transposeInPlace();
        gChannel.transposeInPlace();
        bChannel.transposeInPlace();
        aChannel.transposeInPlace();

        concurrency::parallel_for(2, height, [&](int y) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;
            for (; x < width; ++x) {
                dst[0] = std::clamp(rChannel(y, x) * 255.f, 0.f, 255.f);
                dst[1] = std::clamp(gChannel(y, x) * 255.f, 0.f, 255.f);
                dst[2] = std::clamp(bChannel(y, x) * 255.f, 0.f, 255.f);
                dst[3] = std::clamp(aChannel(y, x) * 255.f, 0.f, 255.f);
                dst += 4;
            }
        });
    }

    void convolve1D(uint8_t *data, int stride, int width, int height, const std::vector<float> &horizontal, const std::vector<float> &vertical) {
        std::vector<uint8_t> transient(stride * height);

        Eigen::VectorXf horizontalKernel(horizontal.size());
        for (int i = 0; i < horizontal.size(); ++i) {
            horizontalKernel(i) = horizontal[i];
        }

        Eigen::VectorXf verticalKernel(vertical.size());
        for (int i = 0; i < vertical.size(); ++i) {
            verticalKernel(i) = vertical[i];
        }

        const int fftPathThreshold = 650 * 650;

        if (width * height > fftPathThreshold && (horizontal.size() > 27 || vertical.size() > 27)) {
            fftConvolve(data, stride, width, height, horizontalKernel, verticalKernel);
        } else {
            const int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                              height * width / (256 * 256)), 1, 12);
            concurrency::parallel_for(threadCount, height, [&](int y) {
                convolve1DHorizontalPass(transient, data, stride, y, width, height, horizontalKernel);
            });

            concurrency::parallel_for(threadCount, height, [&](int y) {
                convolve1DVerticalPass(transient, data, stride, y, width, height, verticalKernel);
            });
        }
    }

}