#pragma once

#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include "factorize.h"
#include "fftw3.h"
#include <complex>
#include "jni/JNIUtils.h"

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

            try {
                inSrc.resize(hFftw * wFftw);
            } catch (std::bad_alloc &err) {
                std::string msg("Cannot allocate in src memory with size (" + std::to_string(hFftw) + "," + std::to_string(wFftw) + ")");
                throw AireError(msg);
            }

            outSrc = (float *) fftwf_malloc(sizeof(fftwf_complex) * hFftw * (wFftw / 2 + 1));
            if (!outSrc) {
                std::string msg("Cannot allocate output src data with size (" + std::to_string(hFftw) + "," + std::to_string(wFftw) + ")");
                throw AireError(msg);
            }

            try {
                inKernel.resize(hFftw * wFftw);
            } catch (std::bad_alloc &err) {
                std::string msg("Cannot allocate in kernel memory with size (" + std::to_string(hFftw) + "," + std::to_string(wFftw) + ")");
                throw AireError(msg);
            }

            outKernel = (float *) fftwf_malloc(sizeof(fftwf_complex) * hFftw * (wFftw / 2 + 1));
            if (!outKernel) {
                std::string msg("Cannot allocate output kernel data with size (" + std::to_string(hFftw) + "," + std::to_string(wFftw) + ")");
                throw AireError(msg);
            }

            try {
                dstFft.resize(hFftw * wFftw);
            } catch (std::bad_alloc &err) {
                std::string msg("Cannot allocate dst fft memory with size (" + std::to_string(hFftw) + "," + std::to_string(wFftw) + ")");
                throw AireError(msg);
            }
            try {
                dst.resize(hDst * wDst);
            } catch (std::bad_alloc &err) {
                std::string msg("Cannot allocate dst memory with size (" + std::to_string(hFftw) + "," + std::to_string(wFftw) + ")");
                throw AireError(msg);
            }

            pForwSrc = fftwf_plan_dft_r2c_2d(hFftw, wFftw, inSrc.data(), reinterpret_cast<fftwf_complex *>(outSrc), FFTW_ESTIMATE);
            if (!pForwSrc) {
                std::string msg("Cannot create FFT in data plan");
                throw AireError(msg);
            }
            pForwKernel = fftwf_plan_dft_r2c_2d(hFftw, wFftw, inKernel.data(), reinterpret_cast<fftwf_complex *>(outKernel), FFTW_ESTIMATE);
            if (!pForwKernel) {
                std::string msg("Cannot create FFT kernel plan");
                throw AireError(msg);
            }
            pBack = fftwf_plan_dft_c2r_2d(hFftw, wFftw, reinterpret_cast<fftwf_complex *>(outKernel), dstFft.data(), FFTW_ESTIMATE);
            if (!pBack) {
                std::string msg("Cannot create FFT backward plan");
                throw AireError(msg);
            }
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

        ~FF2DWorkspace() {
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
            // Reset the content of ws.inSrc
            std::fill(inSrc.begin(), inSrc.end(), 0.f);
            std::fill(inKernel.begin(), inKernel.end(), 0.f);

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

        std::vector<float> inSrc;
        std::vector<float> inKernel;

        float *outSrc = nullptr, *outKernel = nullptr;
        int hSrc, wSrc, hKernel, wKernel;
        int wFftw, hFftw;
        std::vector<float> dstFft;
        std::vector<float> dst; // The array containing the result
        int hDst, wDst;
        fftwf_plan pForwSrc = nullptr;
        fftwf_plan pForwKernel = nullptr;
        fftwf_plan pBack = nullptr;
    };

}