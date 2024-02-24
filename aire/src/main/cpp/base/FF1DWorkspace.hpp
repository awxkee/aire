//
// Created by Radzivon Bartoshyk on 23/02/2024.
//

#pragma once

#include <fftw3.h>

#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include "factorize.h"
#include "fftw3.h"

namespace aire {
    class FF1DWorkspace {
    public:
        FF1DWorkspace(int hSrc, int wSrc, int wKernel) {
            this->hSrc = hSrc;
            this->wSrc = wSrc;
            this->wKernel = wKernel;

            hFftw = std::max(static_cast<int>(fft_next_good_size(hSrc + wKernel * 2 - 1)), hSrc + wKernel * 2 - 1);
            wFftw = std::max(static_cast<int>(fft_next_good_size(wSrc + wKernel * 2 - 1)), wSrc + wKernel * 2 - 1);
            hDst = hFftw;
            wDst = wFftw;

            inSrc = new float[hFftw * wFftw];
            outSrc = (float *) fftwf_malloc(sizeof(fftwf_complex) * hFftw * (wFftw / 2 + 1));
            inKernel = new float[hFftw * wFftw];
            outKernel = (float *) fftwf_malloc(sizeof(fftwf_complex) * hFftw * (wFftw / 2 + 1));
            dstFft = new float[hFftw * wFftw];
            dst = new float[hDst * wDst];

            pForwSrc = fftwf_plan_dft_r2c_2d(hFftw, wFftw, inSrc, reinterpret_cast<fftwf_complex *>(outSrc), FFTW_ESTIMATE);
            pForwKernel = fftwf_plan_dft_r2c_2d(hFftw, wFftw, inKernel, reinterpret_cast<fftwf_complex *>(outKernel), FFTW_ESTIMATE);
            pBack = fftwf_plan_dft_c2r_2d(hFftw, wFftw, reinterpret_cast<fftwf_complex *>(outKernel), dstFft, FFTW_ESTIMATE);
        }

        float *getOutput() {
            return dst;
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
            delete[] inSrc;
            fftwf_free((fftw_complex *) outSrc);
            delete[] inKernel;
            fftwf_free((fftw_complex *) outKernel);

            delete[] dstFft;
            delete[] dst;

            fftwf_destroy_plan(pForwSrc);
            fftwf_destroy_plan(pForwKernel);
            fftwf_destroy_plan(pBack);
        }

    private:

        void fftwCircularConvolution(float *src, const float *kernel) {
            std::fill(inSrc, inSrc + hFftw * wFftw, 0.f);
            std::fill(inKernel, inKernel + hFftw * wFftw, 0.f);

            for (int i = 0; i < hFftw; ++i) {
                int reflectedY = std::clamp(i, 0, hSrc - 1);

                if (i >= hSrc + wKernel - 1) {
                    reflectedY = 0;
                } else if (i >= hSrc) {
                    reflectedY = hSrc - 1;
                }

                int py = std::clamp(reflectedY, 0, hSrc - 1) * wSrc;

                for (int j = 0; j < wFftw; ++j) {
                    int reflectedX = std::clamp(j, 0, wSrc - 1);
                    if (j >= wSrc + wKernel - 1) {
                        reflectedX = 0;
                    } else if (j >= wSrc) {
                        reflectedX = wSrc - 1;
                    }

                    inSrc[i * wFftw + j] += src[py + std::clamp(reflectedX, 0, wSrc - 1)];
                }
            }

            for (int j = 0; j < wKernel; ++j)
                inKernel[(j % wFftw)] += kernel[j];

            // And we compute their packed FFT
            fftwf_execute(pForwSrc);
            fftwf_execute(pForwKernel);

            const float normalizationFactor = static_cast<float>(hFftw * wFftw);

            const float fact = 1.0f / normalizationFactor;
            const int complexSize = hFftw * (wFftw / 2 + 1);
            for (size_t i = 0; i < complexSize; ++i)
                reinterpret_cast<std::complex<float> *>(outKernel)[i] *= fact * reinterpret_cast<std::complex<float> *>(outSrc)[i];

            fftwf_execute(pBack);
        }

        float *inSrc, *outSrc, *inKernel, *outKernel;
        int hSrc, wSrc, wKernel;
        int wFftw, hFftw;
        float *dstFft;
        float *dst; // The array containing the result
        int hDst, wDst;
        fftwf_plan pForwSrc;
        fftwf_plan pForwKernel;
        fftwf_plan pBack;
    };
}