/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 25/02/24, 6:13 PM
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

#pragma once

#include <fftw3.h>

#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include "FftUtils.h"
#include "fftw3.h"

namespace aire {
    class FF1DWorkspace {
    public:
        FF1DWorkspace(int hSrc, int wSrc, int wKernel) {
            std::lock_guard guard(fftSharedRcLock);
            this->hSrc = hSrc;
            this->wSrc = wSrc;
            this->wKernel = wKernel;

            // Adding padding to make a continuous signal for a convolution -> signal width = Signal Size + Kernel Width + Kernel Width / 2 - 1
            hFftw = std::max(static_cast<int>(fft_next_good_size(hSrc + wKernel + wKernel / 2 - 1)), hSrc + wKernel + wKernel / 2 - 1);
            wFftw = std::max(static_cast<int>(fft_next_good_size(wSrc + wKernel + wKernel / 2 - 1)), wSrc + wKernel + wKernel / 2 - 1);
            hDst = hFftw;
            wDst = wFftw;

            inSrc.resize(hFftw * wFftw);
            inKernel.resize(hFftw * wFftw);
            dstFft.resize(hFftw * wFftw);
            dst.resize(hDst * wDst);
            outSrc = (float *) fftwf_malloc(sizeof(fftwf_complex) * hFftw * (wFftw / 2 + 1));
            if (!outSrc) {
                std::string msg("Cannot allocate memory for FFTW");
                throw AireError(msg);
            }
            outKernel = (float *) fftwf_malloc(sizeof(fftwf_complex) * hFftw * (wFftw / 2 + 1));
            if (!outKernel) {
                fftwf_free(outSrc);
                outSrc = nullptr;
                std::string msg("Cannot allocate memory for FFTW");
                throw AireError(msg);
            }

            pForwSrc = fftwf_plan_dft_r2c_2d(hFftw, wFftw, inSrc.data(), reinterpret_cast<fftwf_complex *>(outSrc), FFTW_ESTIMATE);
            pForwKernel = fftwf_plan_dft_r2c_2d(hFftw, wFftw, inKernel.data(), reinterpret_cast<fftwf_complex *>(outKernel), FFTW_ESTIMATE);
            pBack = fftwf_plan_dft_c2r_2d(hFftw, wFftw, reinterpret_cast<fftwf_complex *>(outKernel), dstFft.data(), FFTW_ESTIMATE);
        }

        float *getOutput() {
            return dst.data();
        }

        int getDstWidth() {
            return wDst;
        }

        int getDstHeight() {
            return hDst;
        }

        void convolve(float *src, const float *kernel) {
            if (hFftw <= 0 || wFftw <= 0)
                return;

            fftwCircularConvolution(src, kernel);

            for (int i = 0; i < hDst; ++i)
                memcpy(&dst[i * wDst], &dstFft[i * wFftw], wDst * sizeof(float));
        }

        ~FF1DWorkspace() {
            std::lock_guard guard(fftSharedRcLock);
            if (outSrc) {
                fftwf_free((fftw_complex *) outSrc);
            }
            if (outKernel) {
                fftwf_free((fftw_complex *) outKernel);
            }

            inSrc.clear();
            inKernel.clear();
            dstFft.clear();
            dst.clear();

            if (pForwSrc) {
                fftwf_destroy_plan(pForwSrc);
            }
            if (pForwKernel) {
                fftwf_destroy_plan(pForwKernel);
            }
            if (pBack) {
                fftwf_destroy_plan(pBack);
            }
        }

    private:

        void fftwCircularConvolution(float *src, const float *kernel) {
            std::fill(inSrc.begin(), inSrc.end(), 0.f);
            std::fill(inKernel.begin(), inKernel.end(), 0.f);

            const int yReflectionBreak = hSrc + wKernel / 2 - 1;
            const int xReflectionBreak = wSrc + wKernel / 2 - 1;

            for (int i = 0; i < hFftw; ++i) {
                int reflectedY = std::clamp(i, 0, hSrc - 1);

                if (i >= yReflectionBreak) {
                    reflectedY = 0;
                } else if (i >= hSrc) {
                    reflectedY = hSrc - 1;
                }

                int py = std::clamp(reflectedY, 0, hSrc - 1) * wSrc;

                for (int j = 0; j < wFftw; ++j) {
                    int reflectedX = std::clamp(j, 0, wSrc - 1);
                    if (j >= xReflectionBreak) {
                        reflectedX = 0;
                    } else if (j >= wSrc) {
                        reflectedX = wSrc - 1;
                    }

                    inSrc[i * wFftw + j] += src[py + std::clamp(reflectedX, 0, wSrc - 1)];
                }
            }

            for (int j = 0; j < wKernel; ++j)
                inKernel[(j % wFftw)] += kernel[j];

            fftwf_execute(pForwSrc);
            fftwf_execute(pForwKernel);

            const float normalizationFactor = static_cast<float>(hFftw * wFftw);

            const float fact = 1.0f / normalizationFactor;
            const int complexSize = hFftw * (wFftw / 2 + 1);
            for (size_t i = 0; i < complexSize; ++i)
                reinterpret_cast<std::complex<float> *>(outKernel)[i] *= fact * reinterpret_cast<std::complex<float> *>(outSrc)[i];

            fftwf_execute(pBack);
        }

        float *outSrc = nullptr, *outKernel = nullptr;
        std::vector<float> inSrc;
        std::vector<float> inKernel;
        int hSrc, wSrc, wKernel;
        int wFftw, hFftw;
        std::vector<float> dstFft;
        std::vector<float> dst;
        int hDst, wDst;
        fftwf_plan pForwSrc = nullptr;
        fftwf_plan pForwKernel = nullptr;
        fftwf_plan pBack = nullptr;
    };
}