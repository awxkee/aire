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
        Eigen::MatrixXf rChannel(height, width);
        Eigen::MatrixXf gChannel(height, width);
        Eigen::MatrixXf bChannel(height, width);
        Eigen::MatrixXf aChannel(height, width);

        std::vector<float> rChan(width * height);

        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;
            for (; x < width; ++x) {
                rChan[y * width + x] = src[0] / 255.f;
                rChannel(y, x) = src[0] / 255.f;
                gChannel(y, x) = src[1] / 255.f;
                bChannel(y, x) = src[2] / 255.f;
                aChannel(y, x) = src[3] / 255.f;
                src += 4;
            }
        }

        std::unique_ptr<FF1DWorkspace> workspace = std::make_unique<FF1DWorkspace>(width, height, horizontal.size());
        workspace->convolve(rChan.data(), horizontal.data());
        for (int y = 0; y < height; ++y) {
            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(workspace->getOutput()) + y * workspace->getDstWidth());
            for (int x = 0; x < width; ++x) {
                rChannel(y, x) = src[0];
                rChan[y * width + x] = src[0];
                src += 1;
            }
        }
//        workspace->convolve(gChannel.data(), horizontal.data());
//        for (int y = 0; y < height; ++y) {
//            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(workspace->getOutput()) + y * workspace->getDstWidth());
////            memcpy(gChannel.row(y).data(), src, width * sizeof(float));
//            for (int x = 0; x < width; ++ x) {
//                gChannel(y, x) = src[0];
//                src += 1;
//            }
//        }
//        workspace->convolve(bChannel.data(), horizontal.data());
//        for (int y = 0; y < height; ++y) {
//            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(workspace->getOutput()) + y * workspace->getDstWidth());
////            memcpy(bChannel.row(y).data(), src, width * sizeof(float));
//            for (int x = 0; x < width; ++ x) {
//                bChannel(y, x) = src[0];
//                src += 1;
//            }
//        }
//        workspace->convolve(aChannel.data(), horizontal.data());
//        for (int y = 0; y < height; ++y) {
//            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(workspace->getOutput()) + y * width);
//            memcpy(aChannel.row(y).data(), src, width * sizeof(float));
//        }

//        rChannel.transposeInPlace();
//        gChannel.transposeInPlace();
//        bChannel.transposeInPlace();
//        aChannel.transposeInPlace();
//
//        workspace->convolve(rChannel.data(), vertical.data());
//        for (int y = 0; y < width; ++y) {
//            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(workspace->getOutput()) + y * height);
//            memcpy(rChannel.row(y).data(), src, height * sizeof(float));
//        }
//        workspace->convolve(gChannel.data(), vertical.data());
//        for (int y = 0; y < width; ++y) {
//            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(workspace->getOutput()) + y * height);
//            memcpy(gChannel.row(y).data(), src, height * sizeof(float));
//        }
//        workspace->convolve(bChannel.data(), vertical.data());
//        for (int y = 0; y < width; ++y) {
//            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(workspace->getOutput()) + y * height);
//            memcpy(bChannel.row(y).data(), src, height * sizeof(float));
//        }
//        workspace->convolve(aChannel.data(), vertical.data());
//        for (int y = 0; y < width; ++y) {
//            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(workspace->getOutput()) + y * height);
//            memcpy(aChannel.row(y).data(), src, height * sizeof(float));
//        }
//
//        rChannel.transposeInPlace();
//        gChannel.transposeInPlace();
//        bChannel.transposeInPlace();
//        aChannel.transposeInPlace();

        workspace.reset();

        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;
            for (; x < width; ++x) {
//                dst[0] = rChan[y*width + x] * 255.f;
//                dst[1] = rChan[y*width + x] * 255.f;
//                dst[2] = rChan[y*width + x] * 255.f;
                dst[0] = rChannel(y, x) * 255.f;
                dst[1] = rChannel(y, x) * 255.f;
                dst[2] = rChannel(y, x) * 255.f;
//                dst[1] = gChannel(y, x) * 255.f;
//                dst[2] = bChannel(y, x) * 255.f;
                dst[3] = aChannel(y, x) * 255.f;
                dst += 4;
            }
        }

    }

    void convolve1D(uint8_t *data, int stride, int width, int height, const std::vector<float> &horizontal, const std::vector<float> &vertical) {
        std::vector<uint8_t> transient(stride * height);

        int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    height * width / (256 * 256)), 1, 12);

        Eigen::VectorXf horizontalKernel(horizontal.size());
        for (int i = 0; i < horizontal.size(); ++i) {
            horizontalKernel(i) = horizontal[i];
        }

        Eigen::VectorXf verticalKernel(vertical.size());
        for (int i = 0; i < vertical.size(); ++i) {
            verticalKernel(i) = vertical[i];
        }
//        fftConvolve(data, stride, width, height, horizontalKernel, verticalKernel);

        concurrency::parallel_for(threadCount, height, [&](int y) {
            convolve1DHorizontalPass(transient, data, stride, y, width, height, horizontalKernel);
        });

        concurrency::parallel_for(threadCount, height, [&](int y) {
            convolve1DVerticalPass(transient, data, stride, y, width, height, verticalKernel);
        });
    }

}