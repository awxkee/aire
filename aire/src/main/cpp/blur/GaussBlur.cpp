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

using namespace std;

namespace aire {

    static Eigen::MatrixXf generate2DGaussianKernel(int size, double sigma) {
        Eigen::MatrixXf kernel2d(size, size);
        for (int row = 0; row < kernel2d.rows(); row++) {
            for (int col = 0; col < kernel2d.cols(); col++) {
                double x = exp(-(row * row + col * col) / (2 * sigma * sigma));
                kernel2d(row, col) = x;
            }
        }
        float sum = kernel2d.sum();
        if (sum != 0.f) {
            kernel2d /= sum;
        }
        return kernel2d;
    }

    // https://zingl.github.io/blurring.pdf

    void vertical3Degree(uint8_t *data, const int stride, const int width, const int height, const int radius, const int channels, const int z) {
        int radius3D = radius * radius * radius;

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

        ska::flat_hash_map<int, int> bufferR, bufferG, bufferB;

        const int channels = 4;

        for (int x = start; x < width && x < end; ++x) {
            int difR = 0, derR = 0, difG = 0, derG = 0,
                    difB = 0, derB = 0;
            float sumR = 0, sumG = 0, sumB = 0;
            for (int y = 0 - 3 * radius; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                int px = x * channels;
                if (y >= 0) {
                    difR += 3 * (bufferR[y] - bufferR[y + radius]) - bufferR[y - radius];
                    src[px] = sumR * weight;

                    difG += 3 * (bufferG[y] - bufferG[y + radius]) - bufferG[y - radius];
                    src[px + 1] = sumR * weight;

                    difB += 3 * (bufferB[y] - bufferB[y + radius]) - bufferB[y - radius];
                    src[px + 2] = sumR * weight;
                } else if (y + radius >= 0) {
                    difR += 3 * (bufferR[y] - bufferR[y + radius]);
                    difG += 3 * (bufferG[y] - bufferG[y + radius]);
                    difB += 3 * (bufferB[y] - bufferB[y + radius]);
                } else if (y + 2 * radius >= 0) {
                    difR -= 3 * bufferR[y + radius];
                    difG -= 3 * bufferG[y + radius];
                    difB -= 3 * bufferB[y + radius];
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + std::clamp(y + 3 * radius / 2, 0, height - 1) * stride;
                int pR = srcNext[px];
                sumR += derR += difR += pR;
                bufferR[y + 2 * radius] = pR;

                int pG = srcNext[px + 1];
                sumG += derG += difG += pG;
                bufferG[y + 2 * radius] = pG;

                int pB = srcNext[px + 2];
                sumB += derB += difB += pB;
                bufferB[y + 2 * radius] = pB;
            }
        }
    }

    void horizontal3Degree4Chan(uint8_t *data, const int stride, const int width, const int height, const int radius,
                                const int start, const int end) {
        const float weight = 1.f / (static_cast<float>(radius) * static_cast<float>(radius) * static_cast<float>(radius));

        ska::flat_hash_map<int, int> bufferR, bufferG, bufferB;

        const int channels = 4;

        for (int y = start; y < height && y < end; ++y) {
            int difR = 0, derR = 0, difG = 0, derG = 0,
                    difB = 0, derB = 0;
            float sumR = 0, sumG = 0, sumB = 0;
            for (int x = 0 - 3 * radius; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (x >= 0) {
                    int px = x * channels;
                    difR += 3 * (bufferR[x] - bufferR[x + radius]) - bufferR[x - radius];
                    src[px] = sumR * weight;

                    difG += 3 * (bufferG[x] - bufferG[x + radius]) - bufferG[x - radius];
                    src[px + 1] = sumR * weight;

                    difB += 3 * (bufferB[x] - bufferB[x + radius]) - bufferB[x - radius];
                    src[px + 2] = sumR * weight;
                } else if (x + radius >= 0) {
                    difR += 3 * (bufferR[x] - bufferR[x + radius]);
                    difG += 3 * (bufferG[x] - bufferG[x + radius]);
                    difB += 3 * (bufferB[x] - bufferB[x + radius]);
                } else if (x + 2 * radius >= 0) {
                    difR -= 3 * bufferR[x + radius];
                    difG -= 3 * bufferG[x + radius];
                    difB -= 3 * bufferB[x + radius];
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + y * stride;
                int px = std::clamp(x + radius, 0, width - 1) * channels;
                int pR = srcNext[px];
                sumR += derR += difR += pR;
                bufferR[x + 2 * radius] = pR;

                int pG = srcNext[px + 1];
                sumG += derG += difG += pG;
                bufferG[x + 2 * radius] = pG;

                int pB = srcNext[px + 2];
                sumB += derB += difB += pB;
                bufferB[x + 2 * radius] = pB;
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

        ska::flat_hash_map<int, int> bufferR, bufferG, bufferB;

        const int channels = 4;

        for (int x = start; x < width && x < end; ++x) {
            int difR = 0, sumR = (radius * radius) >> 1, difG = 0, sumG = (radius * radius) >> 1,
                    difB = 0, sumB = (radius * radius) >> 1;
            for (int y = 0 - 2 * radius; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (y >= 0) {
                    src[x * channels] = sumR * weight;
                    difR += bufferR[y - radius] - 2 * bufferR[y];

                    src[x * channels + 1] = sumG * weight;
                    difG += bufferG[y - radius] - 2 * bufferG[y];

                    src[x * channels + 2] = sumB * weight;
                    difB += bufferB[y - radius] - 2 * bufferB[y];
                } else if (y + radius >= 0) {
                    difR -= 2 * bufferR[y];
                    difG -= 2 * bufferG[y];
                    difB -= 2 * bufferB[y];
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + std::clamp(y + radius, 0, height - 1) * stride;
                int pR = srcNext[x * channels];
                sumR += difR += pR;
                bufferR[y + radius] = pR;

                int pG = srcNext[x * channels + 1];
                sumG += difG += pG;
                bufferG[y + radius] = pG;

                int pB = srcNext[x * channels + 2];
                sumB += difB += pB;
                bufferB[y + radius] = pB;
            }
        }
    }

    void horizontal2Degree4Chan(uint8_t *data, const int stride, const int width, const int height, const int radius,
                                const int startY, const int endY) {
        const float weight = 1.f / (static_cast<float>(radius) * static_cast<float>(radius));

        ska::flat_hash_map<int, int> bufferR, bufferG, bufferB;

        const int channels = 4;

        for (int y = startY; y < height && y < endY; ++y) {
            int difR = 0, sumR = (radius * radius) >> 1, difG = 0, sumG = (radius * radius) >> 1,
                    difB = 0, sumB = (radius * radius) >> 1;
            for (int x = 0 - 2 * radius; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (x >= 0) {
                    src[x * channels] = sumR * weight;
                    difR += bufferR[x - radius] - 2 * bufferR[x];

                    src[x * channels + 1] = sumG * weight;
                    difG += bufferG[x - radius] - 2 * bufferG[x];

                    src[x * channels + 2] = sumB * weight;
                    difB += bufferB[x - radius] - 2 * bufferB[x];
                } else if (x + radius >= 0) {
                    difR -= 2 * bufferR[x];
                    difG -= 2 * bufferG[x];
                    difB -= 2 * bufferB[x];
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + y * stride;
                int px = std::clamp(x + radius, 0, width - 1) * channels;
                int pR = srcNext[px];
                sumR += difR += pR;
                bufferR[x + radius] = pR;

                int pG = srcNext[px + 1];
                sumG += difG += pG;
                bufferG[x + radius] = pG;

                int pB = srcNext[px + 2];
                sumB += difB += pB;
                bufferB[x + radius] = pB;
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

        ska::flat_hash_map<int, float> bufferR, bufferG, bufferB;

        const int channels = 4;

        for (int x = start; x < width && x < end; ++x) {
            float difR = 0, derR1 = 0.f, derR2 = 0.f, sumR = 0, difG = 0, sumG = 0, derG1 = 0.f, derG2 = 0.f,
                    difB = 0, sumB = 0, derB1 = 0.f, derB2 = 0.f;
            for (int y = 0 - 4 * radius; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (y >= 0) {
                    difR += -4 * (bufferR[y - radius] + bufferR[y + radius]) + 6 * bufferR[y] + bufferR[y - 2 * radius];
                    src[x * channels] = sumR * weight;

                    difG += -4 * (bufferG[y - radius] + bufferG[y + radius]) + 6 * bufferG[y] + bufferG[y - 2 * radius];
                    src[x * channels + 1] = sumG * weight;

                    difB += -4 * (bufferB[y - radius] + bufferB[y + radius]) + 6 * bufferB[y] + bufferB[y - 2 * radius];
                    src[x * channels + 2] = sumB * weight;
                } else {
                    if (y + 3 * radius >= 0) {
                        difR -= 4 * bufferR[y + radius];
                        difG -= 4 * bufferG[y + radius];
                        difB -= 4 * bufferB[y + radius];
                    }
                    if (y + 2 * radius >= 0) {
                        difR += 6 * bufferR[y];
                        difG += 6 * bufferG[y];
                        difB += 6 * bufferB[y];
                    }
                    if (y + radius >= 0) {
                        difR -= 4 * bufferR[y - radius];
                        difG -= 4 * bufferG[y - radius];
                        difB -= 4 * bufferB[y - radius];
                    }
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + std::clamp(y + 2 * radius - 1, 0, height - 1) * stride;
                int pR = srcNext[x * channels];
                sumR += derR1 += derR2 += difR += pR;
                bufferR[y + 2 * radius] = pR;

                int pG = srcNext[x * channels + 1];
                sumG += derG1 += derG2 += difG += pG;
                bufferG[y + 2 * radius] = pG;

                int pB = srcNext[x * channels + 2];
                sumB += derB1 += derB2 += difB += pB;
                bufferB[y + 2 * radius] = pB;
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

        ska::flat_hash_map<int, float> bufferR, bufferG, bufferB;

        const int channels = 4;

        for (int y = start; y < height && y < end; ++y) {
            float difR = 0, derR1 = 0.f, derR2 = 0.f, sumR = 0, difG = 0, sumG = 0, derG1 = 0.f, derG2 = 0.f,
                    difB = 0, sumB = 0, derB1 = 0.f, derB2 = 0.f;
            for (int x = 0 - 4 * radius; x < width; ++x) {
                auto src = reinterpret_cast<uint8_t *>(data) + y * stride;
                if (x >= 0) {
                    difR += -4 * (bufferR[x - radius] + bufferR[x + radius]) + 6 * bufferR[x] + bufferR[x - 2 * radius];
                    src[x * channels] = sumR * weight;

                    difG += -4 * (bufferG[x - radius] + bufferG[x + radius]) + 6 * bufferG[x] + bufferG[x - 2 * radius];
                    src[x * channels + 1] = sumG * weight;

                    difB += -4 * (bufferB[x - radius] + bufferB[x + radius]) + 6 * bufferB[x] + bufferB[x - 2 * radius];
                    src[x * channels + 2] = sumB * weight;
                } else {
                    if (y + 3 * radius >= 0) {
                        difR -= 4 * bufferR[x + radius];
                        difG -= 4 * bufferG[x + radius];
                        difB -= 4 * bufferB[x + radius];
                    }
                    if (y + 2 * radius >= 0) {
                        difR += 6 * bufferR[x];
                        difG += 6 * bufferG[x];
                        difB += 6 * bufferB[x];
                    }
                    if (y + radius >= 0) {
                        difR -= 4 * bufferR[x - radius];
                        difG -= 4 * bufferG[x - radius];
                        difB -= 4 * bufferB[x - radius];
                    }
                }

                auto srcNext = reinterpret_cast<uint8_t *>(data) + y * stride;
                int px = std::clamp(x + 2 * radius - 1, 0, width - 1)*channels;
                int pR = srcNext[px];
                sumR += derR1 += derR2 += difR += pR;
                bufferR[x + 2 * radius] = pR;

                int pG = srcNext[px + 1];
                sumG += derG1 += derG2 += difG += pG;
                bufferG[x + 2 * radius] = pG;

                int pB = srcNext[px + 2];
                sumB += derB1 += derB2 += difB += pB;
                bufferB[x + 2 * radius] = pB;
            }
        }
    }

    void gaussianApproximation3D(uint8_t *data, int stride, int width, int height, int radius) {
        concurrency::parallel_for_segment(8, width, [&](int start, int end) {
            vertical3Degree4Chan(data, stride, width, height, radius, start, end);
        });
        concurrency::parallel_for_segment(8, width, [&](int start, int end) {
            horizontal3Degree4Chan(data, stride, width, height, radius, start, end);
        });
    }

    void gaussianApproximation2D(uint8_t *data, int stride, int width, int height, int radius) {
        concurrency::parallel_for_segment(8, width, [&](int start, int end) {
            vertical2Degree4Chan(data, stride, width, height, radius, start, end);
        });
        concurrency::parallel_for_segment(8, height, [&](int start, int end) {
            horizontal2Degree4Chan(data, stride, width, height, radius, start, end);
        });
    }

    void gaussianApproximation4D(uint8_t *data, int stride, int width, int height, int radius) {
        concurrency::parallel_for_segment(1, width, [&](int start, int end) {
            vertical4Degree4Chan(data, stride, width, height, radius, start, end);
        });
        concurrency::parallel_for_segment(1, height, [&](int start, int end) {
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
