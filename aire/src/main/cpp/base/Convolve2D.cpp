/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 06/02/24, 6:13 PM
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

#include "Convolve2D.h"
#include <vector>
#include <thread>
#include <algorithm>
#include "FF2DWorkspace.hpp"
#include "hwy/highway.h"
#include "algo/support-inl.h"
#include "concurrency.hpp"

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

        const int dxR = 0;
        const int dyR = 0;

        const int dstWidth = workspace->getDstWidth();
        const VF zeros = Zero(dfx4);
        const VF max255 = Set(dfx4, 255);

        const auto src = workspace->getOutput();

        for (int y = 0; y < height; ++y) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;
            for (; x + 4 < width && x + dxR + 4 < dstWidth; x += 4) {
                auto vec = LoadU(dfx4, &src[(y + dyR) * dstWidth + (x + dxR)]);
                vec = Clamp(Mul(vec, max255), zeros, max255);
                auto target = DemoteTo(du8x4, vec);
                dst[chanIndex] = ExtractLane(vec, 0);
                dst[chanIndex + 4] = ExtractLane(vec, 1);
                dst[chanIndex + 8] = ExtractLane(vec, 2);
                dst[chanIndex + 12] = ExtractLane(vec, 3);
                dst += 16;
            }

            for (; x < width; ++x) {
                auto r = src[(y + dyR) * workspace->getDstWidth() + (x + dxR)];
                dst[chanIndex] = std::clamp(r * 255.0, 0.0, 255.0);
                dst += 4;
            }
        }
    }

    void Convolve2D::bruteForceConvolve(uint8_t *data, int stride, int width, int height) {
        std::vector<uint8_t> destination(stride * height);

        const Eigen::MatrixXf mt = this->matrix;

        const FixedTag<uint8_t, 4> du8;
        const FixedTag<float32_t, 4> dfx4;
        using VF = Vec<decltype(dfx4)>;
        const VF zeros = Zero(dfx4);
        const auto max255 = Set(dfx4, 255.0f);
        const auto revertScale = ApproximateReciprocal(max255);

        const int cols = this->matrix.cols();

        concurrency::parallel_for(8, height, [&](int y) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(destination.data()) + y * stride);

            const int ySize = this->matrix.rows() / 2;
            const int xSize = this->matrix.cols() / 2;

            const bool yEven = this->matrix.rows() % 2 == 0;
            const bool xEven = this->matrix.cols() % 2 == 0;
            const int jMax = yEven ? ySize - 1 : ySize;
            const int iMax = xEven ? xSize - 1 : xSize;

            for (int x = 0; x < width; ++x) {

                VF store = zeros;

                for (int j = -ySize; j <= jMax; ++j) {
                    auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + clamp(y + j, 0, height - 1) * stride);
                    if (cols == 3) {
                        int px = clamp(x - 1, 0, width - 1) * 4;
                        auto pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        auto weight = Set(dfx4, matrix(j + ySize, 0));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 1));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 1, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 2));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));
                    } else if (cols == 5) {
                        int px = clamp(x - 2, 0, width - 1) * 4;
                        auto pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        auto weight = Set(dfx4, matrix(j + ySize, 0));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x - 1, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 1));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 2));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 1, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 3));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 2, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 4));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));
                    } else if (cols == 7) {
                        int px = clamp(x - 3, 0, width - 1) * 4;
                        auto pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        auto weight = Set(dfx4, matrix(j + ySize, 0));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x - 2, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 1));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x - 1, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 2));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 3));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 1, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 4));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 2, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 5));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 3, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 6));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));
                    } else if (cols == 9) {
                        int px = clamp(x - 4, 0, width - 1) * 4;
                        auto pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        auto weight = Set(dfx4, matrix(j + ySize, 0));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x - 3, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 1));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x - 2, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 2));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x - 1, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 3));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 4));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 1, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 5));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 2, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 6));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 3, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 7));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));

                        px = clamp(x + 4, 0, width - 1) * 4;
                        pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                        weight = Set(dfx4, matrix(j + ySize, 8));
                        store = Add(store, Mul(Mul(pixels, revertScale), weight));
                    } else {
                        int i = -xSize;

                        for (; i + 4 < iMax && x + 4 < width; i += 4) {
                            int px = clamp(x + i, 0, width - 1) * 4;
                            auto pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                            auto weight = Set(dfx4, matrix(j + ySize, i + xSize));
                            store = Add(store, Mul(Mul(pixels, revertScale), weight));

                            px = clamp(x + i + 1, 0, width - 1) * 4;
                            pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                            weight = Set(dfx4, matrix(j + ySize, i + 1 + xSize));
                            store = Add(store, Mul(Mul(pixels, revertScale), weight));

                            px = clamp(x + i + 2, 0, width - 1) * 4;
                            pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                            weight = Set(dfx4, matrix(j + ySize, i + 2 + xSize));
                            store = Add(store, Mul(Mul(pixels, revertScale), weight));

                            px = clamp(x + i + 3, 0, width - 1) * 4;
                            pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                            weight = Set(dfx4, matrix(j + ySize, i + 3 + xSize));
                            store = Add(store, Mul(Mul(pixels, revertScale), weight));
                        }

                        for (; i <= iMax; ++i) {
                            int px = clamp(x + i, 0, width - 1) * 4;
                            auto pixels = PromoteTo(dfx4, LoadU(du8, &src[px]));
                            auto weight = Set(dfx4, matrix(j + ySize, i + xSize));
                            store = Add(store, Mul(Mul(pixels, revertScale), weight));
                        }
                    }
                }

                int px = x * 4;

                StoreU(DemoteTo(du8, Clamp(Round(Mul(store, max255)), zeros, max255)), du8, &dst[px]);
            }
        });
        std::copy(destination.begin(), destination.end(), data);
    }

    void Convolve2D::fftConvolve(uint8_t *data, int stride, int width, int height) {
        std::vector<float> rV(width * height, 0.f);
        std::vector<float> gV(width * height, 0.f);
        std::vector<float> bV(width * height, 0.f);
        std::vector<float> aV(width * height, 0.f);

        const FixedTag<float32_t, 4> dfx4;
        const FixedTag<uint8_t, 16> du8x16;
        using VF = Vec<decltype(dfx4)>;
        using VU8x16 = Vec<decltype(du8x16)>;
        const auto scale = ApproximateReciprocal(Set(dfx4, 255.0f));

        for (int y = 0; y < height; y++) {
            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
            int x = 0;

            for (; x + 16 < width; x += 16) {
                VU8x16 r, g, b, a;
                LoadInterleaved4(du8x16, dst, r, g, b, a);
                VF toptop, toplow, lowtop, lowlow;
                ConvertToFloatVec16(du8x16, r, toptop, toplow, lowtop, lowlow);

                toptop = Mul(toptop, scale);
                toplow = Mul(toplow, scale);
                lowtop = Mul(lowtop, scale);
                lowlow = Mul(lowlow, scale);

                auto rvStart = rV.data() + y * width + x;

                StoreU(toptop, dfx4, rvStart);
                StoreU(toplow, dfx4, rvStart + 4);
                StoreU(lowtop, dfx4, rvStart + 8);
                StoreU(lowlow, dfx4, rvStart + 12);

                ConvertToFloatVec16(du8x16, g, toptop, toplow, lowtop, lowlow);

                toptop = Mul(toptop, scale);
                toplow = Mul(toplow, scale);
                lowtop = Mul(lowtop, scale);
                lowlow = Mul(lowlow, scale);

                auto gvStart = gV.data() + y * width + x;

                StoreU(toptop, dfx4, gvStart);
                StoreU(toplow, dfx4, gvStart + 4);
                StoreU(lowtop, dfx4, gvStart + 8);
                StoreU(lowlow, dfx4, gvStart + 12);

                ConvertToFloatVec16(du8x16, b, toptop, toplow, lowtop, lowlow);

                toptop = Mul(toptop, scale);
                toplow = Mul(toplow, scale);
                lowtop = Mul(lowtop, scale);
                lowlow = Mul(lowlow, scale);

                auto bvStart = bV.data() + y * width + x;

                StoreU(toptop, dfx4, bvStart);
                StoreU(toplow, dfx4, bvStart + 4);
                StoreU(lowtop, dfx4, bvStart + 8);
                StoreU(lowlow, dfx4, bvStart + 12);

                ConvertToFloatVec16(du8x16, a, toptop, toplow, lowtop, lowlow);

                toptop = Mul(toptop, scale);
                toplow = Mul(toplow, scale);
                lowtop = Mul(lowtop, scale);
                lowlow = Mul(lowlow, scale);

                auto avStart = aV.data() + y * width + x;

                StoreU(toptop, dfx4, avStart);
                StoreU(toplow, dfx4, avStart + 4);
                StoreU(lowtop, dfx4, avStart + 8);
                StoreU(lowlow, dfx4, avStart + 12);

                dst += 4 * 16;
            }

            for (; x < width; ++x) {
                Eigen::Vector4f color = {dst[0], dst[1], dst[2], dst[3]};
                color /= 255.f;
                int py = y * width;
                rV[py + x] = color.x();
                gV[py + x] = color.y();
                bV[py + x] = color.z();
                aV[py + x] = color.w();
                dst += 4;
            }
        }

        std::unique_ptr<FF2DWorkspace> workspace = std::make_unique<FF2DWorkspace>(height, width,
                                                                                   matrix.rows(), matrix.cols());
        workspace->convolve(rV.data(), matrix.data());
        rV.clear();
        applyChannel(workspace.get(), data, stride, 0, width, height);

        workspace->convolve(gV.data(), matrix.data());
        gV.clear();
        applyChannel(workspace.get(), data, stride, 1, width, height);

        workspace->convolve(bV.data(), matrix.data());
        bV.clear();
        applyChannel(workspace.get(), data, stride, 2, width, height);

        workspace->convolve(aV.data(), matrix.data());
        aV.clear();
        applyChannel(workspace.get(), data, stride, 3, width, height);
        workspace.reset();
    }

    void Convolve2D::convolve(uint8_t *data, int stride, int width, int height) {
        const int activationSize = 500 * 500;
        if (width * height > activationSize && matrix.rows() > 9) {
            fftConvolve(data, stride, width, height);
        } else {
            this->bruteForceConvolve(data, stride, width, height);
        }
    }
}