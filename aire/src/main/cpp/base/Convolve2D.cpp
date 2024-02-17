//
// Created by Radzivon Bartoshyk on 06/02/2024.
//

#include "Convolve2D.h"
#include <vector>
#include <thread>
#include <algorithm>
#include "FF2DWorkspace.h"
#include "hwy/highway.h"
#include "algo/support-inl.h"

namespace aire {

    using namespace hwy;
    using namespace std;
    using namespace hwy::HWY_NAMESPACE;

    void Convolve2D::applyChannel(FF2DWorkspace *workspace,
                                  uint8_t *data, const int stride, const int chanIndex, const int width,
                                  const int height) {

        const FixedTag<float32_t, 4> dfx4;
        const FixedTag<uint8_t, 4> du8x4;
        using VF = Vec<decltype(dfx4)>;

        const int dxR = matrix.cols();
        const int dyR = matrix.rows();

        const int dstWidth = workspace->getDstWidth();
        const VF zeros = Zero(dfx4);
        const VF max255 = Set(dfx4, 255);

        const auto src = workspace->getOutput();
        for (int y = 0; y < height; y++) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;
            for (; x + 4 < width && x + dxR + 4 < dstWidth; x += 4) {
                auto vec = LoadU(dfx4, &src[(y + dyR) * dstWidth + (x + dxR)]);
                vec = Clamp(Mul(vec, max255), zeros, max255);
                auto target = DemoteToU8(du8x4, vec);
                dst[chanIndex] = ExtractLane(vec, 0);
                dst[chanIndex + 4] = ExtractLane(vec, 1);
                dst[chanIndex + 8] = ExtractLane(vec, 2);
                dst[chanIndex + 12] = ExtractLane(vec, 3);
                dst += 4*4;
            }

            for (; x < width; ++x) {
                auto r = src[(y + dyR) * workspace->getDstWidth() + (x + dxR)];
                dst[chanIndex] = std::clamp(r * 255.0, 0.0, 255.0);
                dst += 4;
            }
        }
    }

    void Convolve2D::bruteForceConvolve(uint8_t *data, int stride, int width, int height) {
        int threadCount = clamp(min(static_cast<int>(std::thread::hardware_concurrency()),
                                    height * width / (256 * 256)), 1, 12);
        vector<thread> workers;

        int segmentHeight = height / threadCount;

        std::vector<uint8_t> destination(stride * height);

        for (int i = 0; i < threadCount; i++) {
            int start = i * segmentHeight;
            int end = (i + 1) * segmentHeight;
            if (i == threadCount - 1) {
                end = height;
            }
            workers.emplace_back(
                    [start, end, width, height, this, &destination, data, stride]() {
                        const Eigen::MatrixXf mt = this->matrix;

                        const FixedTag<uint8_t, 4> du8;
                        const FixedTag<float32_t, 4> dfx4;
                        using VF = Vec<decltype(dfx4)>;
                        const VF zeros = Zero(dfx4);
                        const auto max255 = Set(dfx4, 255.0f);
                        const auto revertScale = ApproximateReciprocal(max255);

                        for (int y = start; y < end; ++y) {
                            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination.data()) + y * stride);
                            for (int x = 0; x < width; ++x) {

                                VF store = zeros;

                                const int ySize = this->matrix.rows() / 2;
                                const int xSize = this->matrix.cols() / 2;

                                for (int j = -ySize; j <= ySize; ++j) {
                                    auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + clamp(y + j, 0, height - 1) * stride);
                                    int i = -xSize;
                                    for (; i <= xSize; ++i) {
                                        int px = clamp(x + i, 0, width - 1) * 4;
                                        auto pixels = ConvertToFloat(dfx4, LoadU(du8, &src[px]));
                                        auto weight = Set(dfx4, matrix(j + ySize, i + xSize));
                                        store = Add(store, Mul(Mul(pixels, revertScale), weight));
                                    }
                                }

                                int px = x * 4;

                                StoreU(DemoteToU8(du8, Clamp(Round(Mul(store, max255)), zeros, max255)), du8, &dst[px]);
                            }
                        }
                    });
        }
        for (std::thread &thread: workers) {
            thread.join();
        }

        std::copy(destination.begin(), destination.end(), data);
    }

    void Convolve2D::fftConvolve(uint8_t *data, int stride, int width, int height) {
        std::vector<float> rV(width * height, 0.0);
        std::vector<float> gV(width * height, 0.0);
        std::vector<float> bV(width * height, 0.0);
        std::vector<float> aV(width * height, 0.0);

        for (int y = 0; y < height; y++) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;
            for (; x < width; ++x) {
                rV[y * width + x] = dst[0] / 255.0;
                gV[y * width + x] = dst[1] / 255.0;
                bV[y * width + x] = dst[2] / 255.0;
                aV[y * width + x] = dst[3] / 255.0;
                dst += 4;
            }
        }

        std::vector<float> kernel(matrix.rows() * matrix.cols());
        for (int y = 0; y < matrix.rows(); y++) {
            for (int x = 0; x < matrix.cols(); ++x) {
                kernel[y * matrix.rows() + x] = matrix(y, x);
            }
        }

        std::unique_ptr<FF2DWorkspace> workspace = std::make_unique<FF2DWorkspace>(height, width,
                                                                                   matrix.rows(), matrix.cols());
        workspace->convolveWorkspace(rV.data(), kernel.data());
        rV.clear();
        applyChannel(workspace.get(), data, stride, 0, width, height);

        workspace->convolveWorkspace(gV.data(), kernel.data());
        gV.clear();
        applyChannel(workspace.get(), data, stride, 1, width, height);

        workspace->convolveWorkspace(bV.data(), kernel.data());
        bV.clear();
        applyChannel(workspace.get(), data, stride, 2, width, height);

        workspace->convolveWorkspace(aV.data(), kernel.data());
        aV.clear();
        applyChannel(workspace.get(), data, stride, 3, width, height);

        workspace.reset();
    }

    void Convolve2D::convolve(uint8_t *data, int stride, int width, int height) {
        if (this->matrix.rows() < 7 && this->matrix.cols() < 7) {
            this->bruteForceConvolve(data, stride, width, height);
            return;
        }
        fftConvolve(data, stride, width, height);
    }
}