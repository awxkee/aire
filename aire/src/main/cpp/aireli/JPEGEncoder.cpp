/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 16/02/24, 6:13 PM
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

#include "JPEGEncoder.h"
#include "jpegli/jpeglib.h"
#include <setjmp.h>
#include "../jni/JNIUtils.h"
#include "jpegli/encode.h"
#include <string>

#include <hwy/foreach_target.h>  // IWYU pragma: keep
#include <hwy/highway.h>
#include "hwy/base.h"

namespace aire {

    struct aire_jpeg_error_mng {
        struct jpeg_error_mgr pub;
        jmp_buf setjmp_buffer;
    };

    using namespace hwy;
    using namespace hwy::HWY_NAMESPACE;

    METHODDEF(void)
    handleJpegError(j_common_ptr cinfo) {
        aire_jpeg_error_mng *myerr = (aire_jpeg_error_mng *) cinfo->err;
        longjmp(myerr->setjmp_buffer, 1);
    }

    template<class T>
    void
    RgbaToRGB(const T *__restrict__ src, const uint32_t srcStride,
              T *__restrict__ dst, const uint32_t newStride,
              const uint32_t width, const uint32_t height,
              const int *__restrict__ permuteMap) {

        const int idx1 = permuteMap[0];
        const int idx2 = permuteMap[1];
        const int idx3 = permuteMap[2];

        ScalableTag<T> du;
        using V = Vec<decltype(du)>;
        const int pixels = du.MaxLanes();

        for (int y = 0; y < height; ++y) {

            auto srcPixels = reinterpret_cast<const T *>(reinterpret_cast<const uint8_t *>(src) + y * srcStride);
            auto dstPixels = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(dst) + y * newStride);

            int x = 0;

            for (; x + pixels < width; x += pixels) {
                V pixels1;
                V pixels2;
                V pixels3;
                V pixels4;
                LoadInterleaved4(du, srcPixels, pixels1, pixels2, pixels3, pixels4);

                V map[3] = {pixels1, pixels2, pixels3};

                StoreInterleaved3(map[idx1], map[idx2], map[idx3], du, dstPixels);

                srcPixels += 4 * pixels;
                dstPixels += 3 * pixels;
            }

            for (; x < width; ++x) {
                T vec[3] = {srcPixels[0], srcPixels[1], srcPixels[2]};
                dstPixels[0] = vec[idx1];
                dstPixels[1] = vec[idx2];
                dstPixels[2] = vec[idx3];

                srcPixels += 4;
                dstPixels += 3;
            }
        }
    }

    std::vector<uint8_t> JPEGLIEncoder::encode() {
        int newStride = sizeof(uint8_t) * width * 3;
        std::vector<uint8_t> rgbData(newStride * height);
        int permuteMap[3] = {0, 1, 2};
        RgbaToRGB(data, stride, rgbData.data(), newStride, width, height, permuteMap);

        struct jpeg_compress_struct cinfo = {0};
        struct aire_jpeg_error_mng jerr = {0};

        cinfo.err = jpegli_std_error(reinterpret_cast<jpeg_error_mgr *>(&jerr));
        jerr.pub.error_exit = handleJpegError;

        jpegli_create_compress(&cinfo);

        unsigned char *outputBuffer = nullptr;
        unsigned long outputSize = 0;

        jpegli_mem_dest(&cinfo, &outputBuffer, &outputSize);

        if (setjmp(jerr.setjmp_buffer)) {
            jpegli_destroy_compress(&cinfo);
            if (outputBuffer) {
                free(outputBuffer);
            }
            std::string msg("JPEG compression has failed");
            throw AireError(msg);
        }

        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;

        jpegli_set_defaults(&cinfo);
        jpegli_set_quality(&cinfo, quality, TRUE);
        jpegli_simple_progression(&cinfo);

        jpegli_start_compress(&cinfo, TRUE);

        JSAMPROW row_pointer;

        while (cinfo.next_scanline < cinfo.image_height) {
            row_pointer = reinterpret_cast<uint8_t *>(rgbData.data()) + cinfo.next_scanline * newStride;
            jpegli_write_scanlines(&cinfo, &row_pointer, 1);
        }

        jpegli_finish_compress(&cinfo);
        jpegli_destroy_compress(&cinfo);

        std::vector<uint8_t> output(outputSize);
        std::copy(outputBuffer, outputBuffer + outputSize, output.begin());

        if (outputBuffer) {
            free(outputBuffer);
        }

        return output;
    }
}