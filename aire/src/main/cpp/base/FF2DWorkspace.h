#pragma once

#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include "factorize.h"
#include "fftw3.h"

namespace aire {

    class FF2DWorkspace {
    public:
        FF2DWorkspace(int hSrc, int wSrc, int hKernel, int wKernel) {
            h_src = hSrc;
            w_src = wSrc;
            h_kernel = hKernel;
            w_kernel = wKernel;

            h_fftw = find_closest_factor(h_src + h_kernel * 2 - 1, const_cast<int *>(FFTW_FACTORS));
            w_fftw = find_closest_factor(w_src + w_kernel * 2 - 1, const_cast<int *>(FFTW_FACTORS));
            h_dst = h_src + h_kernel * 2 - 1;
            w_dst = w_src + w_kernel * 2 - 1;

            in_src = new float[h_fftw * w_fftw];
            out_src = (float *) fftwf_malloc(sizeof(fftwf_complex) * h_fftw * (w_fftw / 2 + 1));
            in_kernel = new float[h_fftw * w_fftw];
            out_kernel = (float *) fftwf_malloc(sizeof(fftwf_complex) * h_fftw * (w_fftw / 2 + 1));
            dst_fft = new float[h_fftw * w_fftw];
            dst = new float[h_dst * w_dst];

            p_forw_src = fftwf_plan_dft_r2c_2d(h_fftw, w_fftw, in_src, (fftwf_complex *) out_src, FFTW_ESTIMATE);
            p_forw_kernel = fftwf_plan_dft_r2c_2d(h_fftw, w_fftw, in_kernel, (fftwf_complex *) out_kernel, FFTW_ESTIMATE);
            p_back = fftwf_plan_dft_c2r_2d(h_fftw, w_fftw, (fftwf_complex *) out_kernel, dst_fft, FFTW_ESTIMATE);
        }

        float *getOutput() {
            return dst;
        }

        int getDstWidth() {
            return w_dst;
        }

        int getDstHeight() {
            return h_dst;
        }

        void convolveWorkspace(float *src, float *kernel) {
            if (h_fftw <= 0 || w_fftw <= 0)
                return;

            fftw_circular_convolution(src, kernel);

            for (int i = 0; i < h_dst; ++i)
                memcpy(&dst[i * w_dst], &dst_fft[i * w_fftw], w_dst * sizeof(float));
        }

        ~FF2DWorkspace() {
            delete[] in_src;
            fftwf_free((fftw_complex *) out_src);
            delete[] in_kernel;
            fftwf_free((fftw_complex *) out_kernel);

            delete[] dst_fft;
            delete[] dst;

            fftwf_destroy_plan(p_forw_src);
            fftwf_destroy_plan(p_forw_kernel);
            fftwf_destroy_plan(p_back);
        }

    private:

        void fftw_circular_convolution(float *src, float *kernel) {
            float *ptr, *ptr_end, *ptr2;

            // Reset the content of ws.in_src
            for (ptr = in_src, ptr_end = in_src + h_fftw * w_fftw; ptr < ptr_end; ++ptr)
                *ptr = 0.f;
            for (ptr = in_kernel, ptr_end = in_kernel + h_fftw * w_fftw; ptr < ptr_end; ++ptr)
                *ptr = 0.f;

            for (int i = 0; i < h_fftw; ++i)
                for (int j = 0; j < w_fftw; ++j, ++ptr) {
                    int reflectedX = std::clamp(j, 0, w_src - 1);
                    if (j < 0)
                        reflectedX = -j;
                    else if (j >= w_src)
                        reflectedX = 2 * w_src - j - 2;

                    int reflectedY = std::clamp(i, 0, h_src - 1);
                    if (i < 0)
                        reflectedY = -i;
                    else if (i >= h_src)
                        reflectedY = 2 * h_src - i - 2;

                    in_src[(i) * w_fftw + j] += src[std::clamp(reflectedY, 0, h_src - 1) * w_src + std::clamp(reflectedX, 0, w_src - 1)];
                }

            for (int i = 0; i < h_kernel; ++i)
                for (int j = 0; j < w_kernel; ++j, ++ptr)
                    in_kernel[(i % h_fftw) * w_fftw + (j % w_fftw)] += kernel[i * w_kernel + j];

            // And we compute their packed FFT
            fftwf_execute(p_forw_src);
            fftwf_execute(p_forw_kernel);

            // Compute the element-wise product on the packed terms
            // Let's put the element wise products in ws.in_kernel
            float re_s, im_s, re_k, im_k;
            for (ptr = out_src, ptr2 = out_kernel, ptr_end = out_src + 2 * h_fftw * (w_fftw / 2 + 1); ptr != ptr_end; ++ptr, ++ptr2) {
                re_s = *ptr;
                im_s = *(++ptr);
                re_k = *ptr2;
                im_k = *(++ptr2);
                *(ptr2 - 1) = re_s * re_k - im_s * im_k;
                *ptr2 = re_s * im_k + im_s * re_k;
            }

            // Compute the backward FFT
            // Carefull, The backward FFT does not preserve the output
            fftwf_execute(p_back);
            // Scale the transform

//            for (ptr = dst_fft, ptr_end = dst_fft + w_fftw * h_fftw; ptr < ptr_end; ++ptr)
//                if (max < *ptr) {
//                    max = *ptr;
//                }
//
//            if (max != 0.f) {
//                for (ptr = dst_fft, ptr_end = dst_fft + w_fftw * h_fftw; ptr < ptr_end; ++ptr)
//                    *ptr /= max;
//            }

            const float normalizationFactor = float(h_fftw * w_fftw);

            for (ptr = dst_fft, ptr_end = dst_fft + w_fftw * h_fftw; ptr < ptr_end; ++ptr)
                *ptr /= normalizationFactor;
        }


        float *in_src, *out_src, *in_kernel, *out_kernel;
        int h_src, w_src, h_kernel, w_kernel;
        int w_fftw, h_fftw;
        float *dst_fft;
        float *dst; // The array containing the result
        int h_dst, w_dst; // its size ; This is automatically set by init_workspace
        fftwf_plan p_forw_src;
        fftwf_plan p_forw_kernel;
        fftwf_plan p_back;

        const int FFTW_FACTORS[7] = {13, 11, 7, 5, 3, 2, 0};;
    };

}