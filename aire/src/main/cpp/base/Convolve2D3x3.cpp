//
// Created by Radzivon Bartoshyk on 06/02/2024.
//

#include "Convolve2D3x3.h"
#include <vector>
#include <thread>
#include <algorithm>

namespace aire {

    using namespace std;

    void Convolve2D3x3::convolve(uint8_t *data, int stride, int width, int height) {
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
                        const Eigen::Matrix3f mt = this->matrix;
                        for (int y = start; y < end; ++y) {
                            auto dst = reinterpret_cast<uint8_t *>(
                                    reinterpret_cast<uint8_t *>(destination.data()) + y * stride);
                            for (int x = 0; x < width; ++x) {

                                Eigen::Matrix3f rLocal;
                                Eigen::Matrix3f gLocal;
                                Eigen::Matrix3f bLocal;

                                for (int j = -1; j <= 1; ++j) {
                                    auto src = reinterpret_cast<uint8_t *>(
                                            reinterpret_cast<uint8_t *>(data) +
                                            clamp(y + j, 0, height - 1) * stride);
                                    for (int i = -1; i <= 1; ++i) {
                                        int px = clamp(x + i, 0, width - 1) * 4;
                                        rLocal(j + 1, i + 1) = src[px];
                                        gLocal(j + 1, i + 1) = src[px + 1];
                                        bLocal(j + 1, i + 1) = src[px + 2];
                                    }
                                }

                                rLocal /= 255.f;
                                gLocal /= 255.f;
                                bLocal /= 255.f;

                                float r = (rLocal.cwiseProduct(mt).sum()) * 255.f;
                                float g = (gLocal.cwiseProduct(mt).sum()) * 255.f;
                                float b = (bLocal.cwiseProduct(mt).sum()) * 255.f;

                                auto src = reinterpret_cast<uint8_t *>(
                                        reinterpret_cast<uint8_t *>(data) +
                                        clamp(y, 0, height - 1) * stride);

                                int px = x*4;
                                dst[px] = clamp(r, 0.f, 255.f);
                                dst[px + 1] = clamp(g, 0.f, 255.f);
                                dst[px + 2] = clamp(b, 0.f, 255.f);
                                dst[px + 3] = src[px];
                            }
                        }
                    });
        }

        for (std::thread &thread: workers) {
            thread.join();
        }

        std::copy(destination.begin(), destination.end(), data);
    }
}