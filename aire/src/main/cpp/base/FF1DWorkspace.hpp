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
        FF1DWorkspace(int hSrc, int wSrc, const int kernelWidth) {
            this->hSrc = hSrc;
            this->wSrc = wSrc;
            this->kernelWidth = kernelWidth;

            wFftw = std::max(static_cast<int>(fft_next_good_size(wSrc + kernelWidth * 2 - 1)), wSrc + kernelWidth * 2 - 1);
            wDst = wFftw;

            inSrc = new float[wFftw];
            outSrc = (float *) fftwf_malloc(sizeof(fftwf_complex) * (wFftw / 2 + 1));
            inKernel = new float[wFftw];
            outKernel = (float *) fftwf_malloc(sizeof(fftwf_complex) * (wFftw / 2 + 1));
            dstFft = new float[wFftw];
            dst = new float[hSrc * wDst];

            pForwSrc = fftwf_plan_dft_r2c_1d(wFftw, inSrc, (fftwf_complex *) outSrc, FFTW_ESTIMATE);
            pForwKernel = fftwf_plan_dft_r2c_1d(wFftw, inKernel, (fftwf_complex *) outKernel, FFTW_ESTIMATE);
            pBack = fftwf_plan_dft_c2r_1d(wFftw, (fftwf_complex *) outKernel, dstFft, FFTW_ESTIMATE);
        }

        void convolve(float *src, const float *kernel) {
            if (wFftw <= 0)
                return;

            fftwCircularConvolution(src, kernel);

//            for (int i = 0; i < hDst; ++i)
//                memcpy(&dst[i * wDst], &dstFft[i * wFftw], wDst * sizeof(float));
        }

        float *getOutput() {
            return dst;
        }

        int getDstWidth() {
            return wDst;
        }

        int getDstHeight() {
            return hSrc;
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
        float *inSrc, *outSrc, *inKernel, *outKernel;
        int hSrc, wSrc, kernelWidth;
        int wFftw;
        float *dstFft;
        float *dst; // The array containing the result
        int wDst; // its size ; This is automatically set by init_workspace
        fftwf_plan pForwSrc;
        fftwf_plan pForwKernel;
        fftwf_plan pBack;

        void fftwCircularConvolution(float *src, const float *kernel) {
            std::fill(dst, dst + hSrc * wDst, 0.f);
            for (int y = 0; y < hSrc; ++y) {
                std::fill(inSrc, inSrc + wFftw, 0.f);
                std::fill(inKernel, inKernel + wFftw, 0.f);

                int reflectedY = std::clamp(y, 0, hSrc - 1);

                if (y >= hSrc + kernelWidth - 1) {
                    reflectedY = 0;
                } else if (y >= hSrc) {
                    reflectedY = hSrc - 1;
                }

                int py = std::clamp(reflectedY, 0, hSrc - 1) * wSrc;

                for (int j = 0; j < wFftw; ++j) {
                    int reflectedX = std::clamp(j, 0, wSrc - 1);
                    if (j >= wSrc + kernelWidth - 1) {
                        reflectedX = 0;
                    } else if (j >= wSrc) {
                        reflectedX = wSrc - 1;
                    }
                    inSrc[j] += src[py + std::clamp(reflectedX, 0, wSrc - 1)];
                }

                for (int i = 0; i < kernelWidth; ++i)
                    inKernel[i] += kernel[i];

                // And we compute their packed FFT
                fftwf_execute(pForwSrc);
                fftwf_execute(pForwKernel);

                const float normalizationFactor = static_cast<float>(wFftw);
                const float fact = 1.0f / normalizationFactor;
                const int complexSize = (wFftw / 2 + 1);

                std::copy(outSrc, outSrc + complexSize, outKernel);

//                for (size_t i = 0; i < complexSize; ++i)
//                    reinterpret_cast<std::complex<float> *>(outKernel)[i] *= fact * reinterpret_cast<std::complex<float> *>(outSrc)[i];

//                for (size_t i = 0; i < complexSize; ++i)
//                    reinterpret_cast<std::complex<float> *>(outKernel)[i] = fact * reinterpret_cast<std::complex<float> *>(outSrc)[i];

                fftwf_execute(pBack);

                memcpy(&dst[y * wDst], &dstFft[0], wDst * sizeof(float));
            }
            const float normalizationFactor = static_cast<float>(wFftw);
            const float fact = 1.0f / normalizationFactor;
            for (int y = 0; y < hSrc; ++y) {
                for (int x = 0; x < wDst; ++x) {
                    dst[y * wDst + x] = dst[y * wDst + x] * fact;
                }
            }
        }

    };
}