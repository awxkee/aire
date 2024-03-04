/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 31/01/24, 6:13 PM
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

#include "GaussBlur.h"
#include <vector>
#include <thread>
#include <algorithm>
#include "MathUtils.hpp"
#include "base/Convolve1D.h"
#include "base/Convolve1Db16.h"
#include "base/Convolve2D.h"
#include "Eigen/Eigen"
#include "flat_hash_map.hpp"
#include "concurrency.hpp"

#if defined(__clang__)
#pragma clang fp contract(fast) exceptions(ignore) reassociate(on)
#endif

using namespace std;

namespace aire {

    static Eigen::MatrixXf generate2DGaussianKernel(int size, double sigma) {
        const float scale = 1.f / (2 * M_PI * sigma * sigma);
        Eigen::MatrixXf kernel2d(size, size);
        for (int row = 0; row < kernel2d.rows(); row++) {
            for (int col = 0; col < kernel2d.cols(); col++) {
                double x = std::expf(-((row * row + col * col)) / (2 * sigma * sigma)) * scale;
                kernel2d(row, col) = x;
            }
        }
        float sum = kernel2d.sum();
        if (sum != 0.f) {
            kernel2d /= sum;
        }
        return kernel2d;
    }

    void vertical3Degree(uint8_t *data, const int stride, const int width, const int height, const int radius, const int channels, const int z) {
        const int radius3D = radius * radius * radius;

        ska::flat_hash_map<int, int> buffer;

        for (int x = 0; x < width; ++x) {
            int dif = 0, der = 0;
            float sum = 0;
            for (int y = 0 - 3 * radius; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (y >= 0) {
                    dif += 3 * (buffer[y] - buffer[y + radius]) - buffer[y - radius];
                    src[x * channels + z] = sum / radius3D;
                } else if (y + radius >= 0) {
                    dif += 3 * (buffer[y] - buffer[y + radius]);
                } else if (y + 2 * radius >= 0) {
                    dif -= 3 * buffer[y + radius];
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + std::clamp(y + 3 * radius / 2, 0, height - 1) * stride;
                int p = srcNext[x * channels + z];
                sum += der += dif += p;
                buffer[y + 2 * radius] = p;
            }
        }
    }

    void horizontal3Degree(uint8_t *data, const int stride, const int width, const int height, const int radius,
                           const int channels, const int z) {
        int radius3D = radius * radius * radius;

        ska::flat_hash_map<int, int> buffer;

        for (int y = 0; y < height; ++y) {
            int dif = 0, der = 0;
            float sum = 0;
            for (int x = 0 - 3 * radius; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (x >= 0) {
                    dif += 3 * (buffer[x] - buffer[x + radius]) - buffer[x - radius];
                    src[x * channels + z] = sum / radius3D;
                } else if (x + radius >= 0) {
                    dif += 3 * (buffer[x] - buffer[x + radius]);
                } else if (x + 2 * radius >= 0) {
                    dif -= 3 * buffer[x + radius];
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + y * stride;
                int p = srcNext[std::clamp(x + 3 * radius / 2, 0, width - 1) * channels + z];
                sum += der += dif += p;
                buffer[x + 2 * radius] = p;
            }
        }
    }

    void vertical3Degree4Chan(uint8_t *data, const int stride, const int width, const int height, const int radius, const int start, const int end) {
        const float weight = 1.f / (static_cast<float>(radius) * static_cast<float>(radius) * static_cast<float>(radius));

        constexpr int bufLength = 1023;
        int bufferR[bufLength + 1], bufferG[bufLength + 1], bufferB[bufLength + 1];

        const int channels = 4;

        for (int x = start; x < width && x < end; ++x) {
            int difR = 0, derR = 0, difG = 0, derG = 0,
                    difB = 0, derB = 0;
            float sumR = 0, sumG = 0, sumB = 0;
            for (int y = 0 - 3 * radius; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                int px = x * channels;
                if (y >= 0) {
                    int mpy = y & bufLength;
                    int mpyPRadius = (y + radius) & bufLength;
                    int mpyMRadius = (y - radius) & bufLength;
                    difR += 3 * (bufferR[mpy] - bufferR[mpyPRadius]) - bufferR[mpyMRadius];
                    src[px] = sumR * weight;

                    difG += 3 * (bufferG[mpy] - bufferG[mpyPRadius]) - bufferG[mpyMRadius];
                    src[px + 1] = sumG * weight;

                    difB += 3 * (bufferB[mpy] - bufferB[mpyPRadius]) - bufferB[mpyMRadius];
                    src[px + 2] = sumB * weight;
                } else if (y + radius >= 0) {
                    int mpy = y & bufLength;
                    int mpyPRadius = (y + radius) & bufLength;
                    difR += 3 * (bufferR[mpy] - bufferR[mpyPRadius]);
                    difG += 3 * (bufferG[mpy] - bufferG[mpyPRadius]);
                    difB += 3 * (bufferB[mpy] - bufferB[mpyPRadius]);
                } else if (y + 2 * radius >= 0) {
                    int mpyPRadius = (y + radius) & bufLength;
                    difR -= 3 * bufferR[mpyPRadius];
                    difG -= 3 * bufferG[mpyPRadius];
                    difB -= 3 * bufferB[mpyPRadius];
                }

                int mPNextRad = (y + 2 * radius) & bufLength;

                auto srcNext = reinterpret_cast<uint8_t *>(data) + std::clamp(y + 3 * radius / 2, 0, height - 1) * stride;
                int pR = srcNext[px];
                sumR += derR += difR += pR;
                bufferR[mPNextRad] = pR;

                int pG = srcNext[px + 1];
                sumG += derG += difG += pG;
                bufferG[mPNextRad] = pG;

                int pB = srcNext[px + 2];
                sumB += derB += difB += pB;
                bufferB[mPNextRad] = pB;
            }
        }
    }

    void horizontal3Degree4Chan(uint8_t *data, const int stride, const int width, const int height, const int radius,
                                const int start, const int end) {
        const float weight = 1.f / (static_cast<float>(radius) * static_cast<float>(radius) * static_cast<float>(radius));

        constexpr int bufLength = 1023;
        int bufferR[bufLength + 1], bufferG[bufLength + 1], bufferB[bufLength + 1];

        const int channels = 4;

        for (int y = start; y < height && y < end; ++y) {
            int difR = 0, derR = 0, difG = 0, derG = 0, difB = 0, derB = 0;
            float sumR = 0, sumG = 0, sumB = 0;
            for (int x = 0 - 3 * radius; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (x >= 0) {
                    int mpy = x & bufLength;
                    int mpyPRadius = (x + radius) & bufLength;
                    int mpyMRadius = (x - radius) & bufLength;
                    int px = x * channels;
                    difR += 3 * (bufferR[mpy] - bufferR[mpyPRadius]) - bufferR[mpyMRadius];
                    src[px] = sumR * weight;

                    difG += 3 * (bufferG[mpy] - bufferG[mpyPRadius]) - bufferG[mpyMRadius];
                    src[px + 1] = sumG * weight;

                    difB += 3 * (bufferB[mpy] - bufferB[mpyPRadius]) - bufferB[mpyMRadius];
                    src[px + 2] = sumB * weight;
                } else if (x + radius >= 0) {
                    int mpy = x & bufLength;
                    int mpyPRadius = (x + radius) & bufLength;
                    difR += 3 * (bufferR[mpy] - bufferR[mpyPRadius]);
                    difG += 3 * (bufferG[mpy] - bufferG[mpyPRadius]);
                    difB += 3 * (bufferB[mpy] - bufferB[mpyPRadius]);
                } else if (x + 2 * radius >= 0) {
                    int mpyPRadius = (x + radius) & bufLength;
                    difR -= 3 * bufferR[mpyPRadius];
                    difG -= 3 * bufferG[mpyPRadius];
                    difB -= 3 * bufferB[mpyPRadius];
                }

                int mPNextRad = (x + 2 * radius) & bufLength;

                auto srcNext = reinterpret_cast<uint8_t *>(data) + y * stride;
                int px = std::clamp(x + radius, 0, width - 1) * channels;
                int pR = srcNext[px];
                sumR += derR += difR += pR;
                bufferR[mPNextRad] = pR;

                int pG = srcNext[px + 1];
                sumG += derG += difG += pG;
                bufferG[mPNextRad] = pG;

                int pB = srcNext[px + 2];
                sumB += derB += difB += pB;
                bufferB[mPNextRad] = pB;
            }
        }
    }

    void vertical2Degree4Chan(uint8_t *data,
                              const int stride,
                              const int width,
                              const int height,
                              const int radius,
                              const int start,
                              const int end) {
        const float weight = 1.f / (static_cast<float>(radius) * static_cast<float>(radius));

        constexpr int bufLength = 1023;
        int bufferR[bufLength + 1], bufferG[bufLength + 1], bufferB[bufLength + 1];

        const int channels = 4;

        for (int x = start; x < width && x < end; ++x) {
            int difR = 0, sumR = (radius * radius) >> 1, difG = 0, sumG = (radius * radius) >> 1,
                    difB = 0, sumB = (radius * radius) >> 1;
            for (int y = 0 - 2 * radius; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (y >= 0) {
                    src[x * channels] = sumR * weight;
                    int arrIndex = (y - radius) & bufLength;
                    int dArrIndex = y & bufLength;
                    difR += bufferR[arrIndex] - 2 * bufferR[dArrIndex];

                    src[x * channels + 1] = sumG * weight;
                    difG += bufferG[arrIndex] - 2 * bufferG[dArrIndex];

                    src[x * channels + 2] = sumB * weight;
                    difB += bufferB[arrIndex] - 2 * bufferB[dArrIndex];
                } else if (y + radius >= 0) {
                    int arrIndex = (y) & bufLength;
                    difR -= 2 * bufferR[arrIndex];
                    difG -= 2 * bufferG[arrIndex];
                    difB -= 2 * bufferB[arrIndex];
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + std::clamp(y + radius, 0, height - 1) * stride;

                int arrIndex = (y + radius) & bufLength;

                int pR = srcNext[x * channels];
                sumR += difR += pR;
                bufferR[arrIndex] = pR;

                int pG = srcNext[x * channels + 1];
                sumG += difG += pG;
                bufferG[arrIndex] = pG;

                int pB = srcNext[x * channels + 2];
                sumB += difB += pB;
                bufferB[arrIndex] = pB;
            }
        }
    }

    void horizontal2Degree4Chan(uint8_t *data, const int stride, const int width, const int height, const int radius,
                                const int startY, const int endY) {
        const float weight = 1.f / (static_cast<float>(radius) * static_cast<float>(radius));

        constexpr int bufLength = 1023;
        int bufferR[bufLength + 1], bufferG[bufLength + 1], bufferB[bufLength + 1];

        const int channels = 4;

        for (int y = startY; y < height && y < endY; ++y) {
            int difR = 0, sumR = (radius * radius) >> 1, difG = 0, sumG = (radius * radius) >> 1,
                    difB = 0, sumB = (radius * radius) >> 1;
            for (int x = 0 - 2 * radius; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (x >= 0) {
                    int arrIndex = (x - radius) & bufLength;
                    int dArrIndex = x & bufLength;

                    src[x * channels] = sumR * weight;
                    difR += bufferR[arrIndex] - 2 * bufferR[dArrIndex];

                    src[x * channels + 1] = sumG * weight;
                    difG += bufferG[arrIndex] - 2 * bufferG[dArrIndex];

                    src[x * channels + 2] = sumB * weight;
                    difB += bufferB[arrIndex] - 2 * bufferB[dArrIndex];
                } else if (x + radius >= 0) {
                    int dx = x & bufLength;
                    difR -= 2 * bufferR[dx];
                    difG -= 2 * bufferG[dx];
                    difB -= 2 * bufferB[dx];
                }

                int arrIndex = (x + radius) & bufLength;

                auto srcNext = reinterpret_cast<uint8_t *>(data) + y * stride;
                int px = std::clamp(x + radius, 0, width - 1) * channels;
                int pR = srcNext[px];
                sumR += difR += pR;
                bufferR[arrIndex] = pR;

                int pG = srcNext[px + 1];
                sumG += difG += pG;
                bufferG[arrIndex] = pG;

                int pB = srcNext[px + 2];
                sumB += difB += pB;
                bufferB[arrIndex] = pB;
            }
        }
    }

    void vertical2Degree(uint8_t *data, const int stride, const int width, const int height, const int radius, const int channels, const int z) {
        const float weight = 1.f / (static_cast<float>(radius) * static_cast<float>(radius));

        ska::flat_hash_map<int, int> buffer;

        for (int x = 0; x < width; ++x) {
            int dif = 0, sum = (radius * radius) >> 1;
            for (int y = 0 - 2 * radius; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (y >= 0) {
                    src[x * channels + z] = sum * weight;
                    dif += buffer[y - radius] - 2 * buffer[y];
                } else if (y + radius >= 0) {
                    dif -= 2 * buffer[y];
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + std::clamp(y + radius, 0, height - 1) * stride;
                int p = srcNext[x * channels + z];
                sum += dif += p;
                buffer[y + radius] = p;
            }
        }
    }

    void horizontal2Degree(uint8_t *data, const int stride, const int width, const int height, const int radius, const int channels, const int z) {
        int radius2D = radius * radius;

        ska::flat_hash_map<int, int> buffer;

        for (int y = 0; y < height; ++y) {
            int dif = 0, sum = (radius * radius) >> 1;
            for (int x = 0 - 2 * radius; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (x >= 0) {
                    src[x * channels + z] = sum / radius2D;
                    dif += buffer[x - radius] - 2 * buffer[x];
                } else if (x + radius >= 0) {
                    dif -= 2 * buffer[x];
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + y * stride;
                int p = srcNext[std::clamp(x + radius, 0, width - 1) * channels + z];
                sum += dif += p;
                buffer[x + radius] = p;
            }
        }
    }

    void vertical4Degree4Chan(uint8_t *data,
                              const int stride,
                              const int width,
                              const int height,
                              const int radius,
                              const int start,
                              const int end) {
        int radius4D = radius * radius;
        radius4D *= radius4D;
        const float weight = 1.f / radius4D;

        constexpr int bufLength = 1023;
        int bufferR[bufLength + 1], bufferG[bufLength + 1], bufferB[bufLength + 1];

        const int channels = 4;

        for (int x = start; x < width && x < end; ++x) {
            float difR = 0, derR1 = 0.f, derR2 = 0.f, sumR = 0, difG = 0, sumG = 0, derG1 = 0.f, derG2 = 0.f,
                    difB = 0, sumB = 0, derB1 = 0.f, derB2 = 0.f;
            for (int y = 0 - 4 * radius; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (y >= 0) {

                    int mpy = y & bufLength;
                    int mpyPRadius = (y + radius) & bufLength;
                    int mpyMRadius = (y - radius) & bufLength;
                    int mpyM2Radius = (y - 2 * radius) & bufLength;

                    difR += -4 * (bufferR[mpyMRadius] + bufferR[mpyPRadius]) + 6 * bufferR[mpy] + bufferR[mpyM2Radius];
                    src[x * channels] = sumR * weight;

                    difG += -4 * (bufferG[mpyMRadius] + bufferG[mpyPRadius]) + 6 * bufferG[mpy] + bufferG[mpyM2Radius];
                    src[x * channels + 1] = sumG * weight;

                    difB += -4 * (bufferB[mpyMRadius] + bufferB[mpyPRadius]) + 6 * bufferB[mpy] + bufferB[mpyM2Radius];
                    src[x * channels + 2] = sumB * weight;
                } else {
                    if (y + 3 * radius >= 0) {
                        int mpyPRadius = (y + radius) & bufLength;
                        difR -= 4 * bufferR[mpyPRadius];
                        difG -= 4 * bufferG[mpyPRadius];
                        difB -= 4 * bufferB[mpyPRadius];
                    }
                    if (y + 2 * radius >= 0) {
                        int mpy = y & bufLength;
                        difR += 6 * bufferR[mpy];
                        difG += 6 * bufferG[mpy];
                        difB += 6 * bufferB[mpy];
                    }
                    if (y + radius >= 0) {
                        int mpyMRadius = (y - radius) & bufLength;
                        difR -= 4 * bufferR[mpyMRadius];
                        difG -= 4 * bufferG[mpyMRadius];
                        difB -= 4 * bufferB[mpyMRadius];
                    }
                }

                int mpyP2Radius = (y + 2 * radius) & bufLength;

                auto srcNext = reinterpret_cast<uint8_t *>(data) + std::clamp(y + 2 * radius - 1, 0, height - 1) * stride;
                int pR = srcNext[x * channels];
                sumR += derR1 += derR2 += difR += pR;
                bufferR[mpyP2Radius] = pR;

                int pG = srcNext[x * channels + 1];
                sumG += derG1 += derG2 += difG += pG;
                bufferG[mpyP2Radius] = pG;

                int pB = srcNext[x * channels + 2];
                sumB += derB1 += derB2 += difB += pB;
                bufferB[mpyP2Radius] = pB;
            }
        }
    }

    void horizontal4Degree4Chan(uint8_t *data,
                                const int stride,
                                const int width,
                                const int height,
                                const int radius,
                                const int start,
                                const int end) {
        int radius4D = radius * radius;
        radius4D *= radius4D;
        const float weight = 1.f / radius4D;

        constexpr int bufLength = 1023;
        int bufferR[bufLength + 1], bufferG[bufLength + 1], bufferB[bufLength + 1];

        const int channels = 4;

        for (int y = start; y < height && y < end; ++y) {
            float difR = 0, derR1 = 0.f, derR2 = 0.f, sumR = 0, difG = 0, sumG = 0, derG1 = 0.f, derG2 = 0.f,
                    difB = 0, sumB = 0, derB1 = 0.f, derB2 = 0.f;
            for (int x = 0 - 4 * radius; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (x >= 0) {
                    int mpy = x & bufLength;
                    int mpyPRadius = (x + radius) & bufLength;
                    int mpyMRadius = (x - radius) & bufLength;
                    int mpyM2Radius = (x - 2 * radius) & bufLength;

                    difR += -4 * (bufferR[mpyMRadius] + bufferR[mpyPRadius]) + 6 * bufferR[mpy] + bufferR[mpyM2Radius];
                    src[x * channels] = sumR * weight;

                    difG += -4 * (bufferG[mpyMRadius] + bufferG[mpyPRadius]) + 6 * bufferG[mpy] + bufferG[mpyM2Radius];
                    src[x * channels + 1] = sumG * weight;

                    difB += -4 * (bufferB[mpyMRadius] + bufferB[mpyPRadius]) + 6 * bufferB[mpy] + bufferB[mpyM2Radius];
                    src[x * channels + 2] = sumB * weight;
                } else {
                    if (x + 3 * radius >= 0) {
                        int mpyPRadius = (x + radius) & bufLength;
                        difR -= 4 * bufferR[mpyPRadius];
                        difG -= 4 * bufferG[mpyPRadius];
                        difB -= 4 * bufferB[mpyPRadius];
                    }
                    if (x + 2 * radius >= 0) {
                        int mpy = x & bufLength;
                        difR += 6 * bufferR[mpy];
                        difG += 6 * bufferG[mpy];
                        difB += 6 * bufferB[mpy];
                    }
                    if (x + radius >= 0) {
                        int mpyMRadius = (x - radius) & bufLength;
                        difR -= 4 * bufferR[mpyMRadius];
                        difG -= 4 * bufferG[mpyMRadius];
                        difB -= 4 * bufferB[mpyMRadius];
                    }
                }

                int mpyP2Radius = (x + 2 * radius) & bufLength;

                auto srcNext = reinterpret_cast<uint8_t *>(data) + y * stride;
                int px = std::clamp(x + 2 * radius - 1, 0, width - 1) * channels;
                int pR = srcNext[px];
                sumR += derR1 += derR2 += difR += pR;
                bufferR[mpyP2Radius] = pR;

                int pG = srcNext[px + 1];
                sumG += derG1 += derG2 += difG += pG;
                bufferG[mpyP2Radius] = pG;

                int pB = srcNext[px + 2];
                sumB += derB1 += derB2 += difB += pB;
                bufferB[mpyP2Radius] = pB;
            }
        }
    }

    void gaussianApproximation3D(uint8_t *data, int stride, int width, int height, int radius) {
        int threadCount = clamp(min(static_cast<int>(thread::hardware_concurrency()),
                                    width * height / (256 * 256)), 1, 12);
        concurrency::parallel_for_segment(threadCount, width, [&](int start, int end) {
            vertical3Degree4Chan(data, stride, width, height, radius, start, end);
        });
        concurrency::parallel_for_segment(threadCount, width, [&](int start, int end) {
            horizontal3Degree4Chan(data, stride, width, height, radius, start, end);
        });
    }

    void gaussianApproximation2D(uint8_t *data, int stride, int width, int height, int radius) {
        int threadCount = clamp(min(static_cast<int>(thread::hardware_concurrency()),
                                    width * height / (256 * 256)), 1, 12);
        concurrency::parallel_for_segment(threadCount, width, [&](int start, int end) {
            vertical2Degree4Chan(data, stride, width, height, radius, start, end);
        });
        concurrency::parallel_for_segment(threadCount, height, [&](int start, int end) {
            horizontal2Degree4Chan(data, stride, width, height, radius, start, end);
        });
    }

    void gaussianApproximation4D(uint8_t *data, int stride, int width, int height, int radius) {
        int threadCount = clamp(min(static_cast<int>(thread::hardware_concurrency()),
                                    width * height / (256 * 256)), 1, 12);
        concurrency::parallel_for_segment(threadCount, width, [&](int start, int end) {
            vertical4Degree4Chan(data, stride, width, height, radius, start, end);
        });
        concurrency::parallel_for_segment(threadCount, height, [&](int start, int end) {
            horizontal4Degree4Chan(data, stride, width, height, radius, start, end);
        });
    }

    void gaussBlurU8(uint8_t *data, int stride, int width, int height, const int size, float sigma) {
        vector<float> kernel = compute1DGaussianKernel(size, sigma);
        convolve1D(data, stride, width, height, kernel, kernel);
    }

    void gaussBlurF16(uint16_t *data, int stride, int width, int height, const int size, float sigma) {
        vector<float> kernel = compute1DGaussianKernel(size, sigma);
        Convolve1Db16 convolution(kernel, kernel);
        convolution.convolve(data, stride, width, height);
    }

}
