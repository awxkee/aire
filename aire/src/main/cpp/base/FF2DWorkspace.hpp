#pragma once

#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include "factorize.h"
#include "fftw3.h"
#include <complex>

namespace aire {

    class FF2DWorkspace {
    public:
        FF2DWorkspace(int hSrc, int wSrc, int hKernel, int wKernel) {
            this->hSrc = hSrc;
            this->wSrc = wSrc;
            this->hKernel = hKernel;
            this->wKernel = wKernel;

            hFftw = std::max(static_cast<int>(fft_next_good_size(hSrc + hKernel * 2 - 1)), hSrc + hKernel * 2 - 1);
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

        ~FF2DWorkspace() {
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
            // Reset the content of ws.inSrc
            std::fill(inSrc, inSrc + hFftw * wFftw, 0.f);
            std::fill(inKernel, inKernel + hFftw * wFftw, 0.f);

            for (int i = 0; i < hFftw; ++i) {
                int reflectedY = std::clamp(i, 0, hSrc - 1);

                if (i >= hSrc + hKernel - 1) {
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

            for (int i = 0; i < hKernel; ++i)
                for (int j = 0; j < wKernel; ++j)
                    inKernel[(i % hFftw) * wFftw + (j % wFftw)] += kernel[i * wKernel + j];

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
        int hSrc, wSrc, hKernel, wKernel;
        int wFftw, hFftw;
        float *dstFft;
        float *dst; // The array containing the result
        int hDst, wDst;
        fftwf_plan pForwSrc;
        fftwf_plan pForwKernel;
        fftwf_plan pBack;
    };

}