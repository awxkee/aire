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

            hFftw = fft_next_good_size(hSrc + kernelWidth * 2 - 1);
            wFftw = fft_next_good_size(wSrc + kernelWidth * 2 - 1);
            hDst = hSrc + kernelWidth * 2 - 1;
            wDst = wSrc + kernelWidth * 2 - 1;

            inSrc = new float[hFftw * wFftw];
            outSrc = (float *) fftwf_malloc(sizeof(fftwf_complex) * hFftw * (wFftw / 2 + 1));
            uint32_t kernelSize = hFftw * wFftw;
            inKernel = new float[kernelSize];
            outKernel = (float *) fftwf_malloc(sizeof(fftwf_complex) * hFftw * (wFftw / 2 + 1));
            dstFft = new float[hFftw * wFftw];
            dst = new float[hDst * wDst];

            pForwSrc = fftwf_plan_dft_r2c_2d(hFftw, wFftw, inSrc, (fftwf_complex *) outSrc, FFTW_ESTIMATE);
            pForwKernel = fftwf_plan_dft_r2c_1d(kernelSize, inKernel, (fftwf_complex *) outKernel, FFTW_ESTIMATE);
            pBack = fftwf_plan_dft_c2r_2d(hFftw, wFftw, (fftwf_complex *) outKernel, dstFft, FFTW_ESTIMATE);
        }

        void convolve(float *src, float *kernel) {
            if (hFftw <= 0 || wFftw <= 0)
                return;

            fftwCircularConvolution(src, kernel);

            for (int i = 0; i < hDst; ++i)
                memcpy(&dst[i * wDst], &dstFft[i * wFftw], wDst * sizeof(float));
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
        int wFftw, hFftw;
        float *dstFft;
        float *dst; // The array containing the result
        int hDst, wDst; // its size ; This is automatically set by init_workspace
        fftwf_plan pForwSrc;
        fftwf_plan pForwKernel;
        fftwf_plan pBack;

        void fftwCircularConvolution(float *src, float *kernel) {
            float *ptr, *ptr_end, *ptr2;

            for (ptr = inSrc, ptr_end = inSrc + hFftw * wFftw; ptr < ptr_end; ++ptr)
                *ptr = 0.f;
            for (ptr = inKernel, ptr_end = inKernel + hFftw * wFftw; ptr < ptr_end; ++ptr)
                *ptr = 0.f;

            for (int i = 0; i < hFftw; ++i)
                for (int j = 0; j < wFftw; ++j, ++ptr) {
                    int reflectedX = std::clamp(j, 0, wSrc - 1);
                    if (j >= wSrc + kernelWidth - 1) {
                        reflectedX = 0;
                    } else if (j >= wSrc) {
                        reflectedX = wSrc - 1;
                    }

                    int reflectedY = std::clamp(i, 0, hSrc - 1);

                    if (i >= hSrc + kernelWidth - 1) {
                        reflectedY = 0;
                    } else if (i >= hSrc) {
                        reflectedY = hSrc - 1;
                    }
                    inSrc[(i) * wFftw + j] += src[std::clamp(reflectedY, 0, hSrc - 1) * wSrc + std::clamp(reflectedX, 0, wSrc - 1)];
                }

            for (int i = 0; i < kernelWidth; ++i)
                inKernel[i] += kernel[i];

            // And we compute their packed FFT
            fftwf_execute(pForwSrc);
            fftwf_execute(pForwKernel);

            // Compute the element-wise product on the packed terms
            // Let's put the element wise products in ws.inKernel
            float re_s, im_s, re_k, im_k;
            for (ptr = outSrc, ptr2 = outKernel, ptr_end = outSrc + 2 * hFftw * (wFftw / 2 + 1); ptr != ptr_end; ++ptr, ++ptr2) {
                re_s = *ptr;
                im_s = *(++ptr);
                re_k = *ptr2;
                im_k = *(++ptr2);
                *(ptr2 - 1) = re_s * re_k - im_s * im_k;
                *ptr2 = re_s * im_k + im_s * re_k;
            }

            fftwf_execute(pBack);

            const float normalizationFactor = static_cast<float>(hFftw * wFftw);

            for (ptr = dstFft, ptr_end = dstFft + wFftw * hFftw; ptr < ptr_end; ++ptr)
                *ptr /= normalizationFactor;
        }

    };
}