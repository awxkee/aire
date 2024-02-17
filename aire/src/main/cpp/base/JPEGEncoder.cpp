//
// Created by Radzivon Bartoshyk on 16/02/2024.
//

#include "JPEGEncoder.h"
#include "turbojpeg/jpeglib.h"
#include "Rgba2Rgb.h"
#include <setjmp.h>
#include "jni/JNIUtils.h"
#include <string>

namespace aire {

    struct aire_jpeg_error_mng {
        struct jpeg_error_mgr pub;
        jmp_buf setjmp_buffer;
    };

    METHODDEF(void)
    handleJpegError(j_common_ptr cinfo) {
        aire_jpeg_error_mng *myerr = (aire_jpeg_error_mng *) cinfo->err;
//        (*cinfo->err->output_message) (cinfo);
        longjmp(myerr->setjmp_buffer, 1);
    }

    struct stdvector_destination_mgr {
        struct jpeg_destination_mgr pub;
        std::vector<uint8_t> *vec = nullptr;
    };

    void init_stdvector_destination(j_compress_ptr /*cinfo*/) {
        // Nothing to do
    }

    boolean
    empty_stdvector_output_buffer(j_compress_ptr cinfo) {
        auto *dest = reinterpret_cast< stdvector_destination_mgr * >( cinfo->dest );

        // Double vector capacity
        const auto currentSize = dest->vec->size();
        dest->vec->resize(currentSize * 2);

        // Point to newly allocated data
        dest->pub.next_output_byte = dest->vec->data() + currentSize;
        dest->pub.free_in_buffer = currentSize;

        return TRUE;
    }

    void
    term_stdvector_destination(j_compress_ptr cinfo) {
        auto *dest = reinterpret_cast< stdvector_destination_mgr * >( cinfo->dest );

        // Resize vector to number of bytes actually used
        const auto used_bytes = dest->vec->capacity() - dest->pub.free_in_buffer;
        dest->vec->resize(used_bytes);
    }

    void
    jpeg_stdvector_dest(j_compress_ptr cinfo, std::vector<uint8_t> &vec) {
        if (cinfo->dest == NULL) {
            cinfo->dest = (struct jpeg_destination_mgr *) (*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(stdvector_destination_mgr));
        }

        auto *dest = reinterpret_cast< stdvector_destination_mgr * >( cinfo->dest );
        dest->pub.init_destination = init_stdvector_destination;
        dest->pub.empty_output_buffer = empty_stdvector_output_buffer;
        dest->pub.term_destination = term_stdvector_destination;

        // Set output buffer and initial size
        dest->vec = &vec;
        dest->vec->resize(4096);

        // Initialize public buffer ptr and size
        dest->pub.next_output_byte = dest->vec->data();
        dest->pub.free_in_buffer = dest->vec->size();
    }

    std::vector<uint8_t> JPEGEncoder::encode() {
        int newStride = sizeof(uint8_t) * width * 3;
        std::vector<uint8_t> rgbData(newStride * height);
        rgb8bit2BGR(data, stride, rgbData.data(), newStride, width, height);

        struct jpeg_compress_struct cinfo = {0};
        struct aire_jpeg_error_mng jerr = {0};

        cinfo.err = jpeg_std_error(reinterpret_cast<jpeg_error_mgr *>(&jerr));
        jerr.pub.error_exit = handleJpegError;

        jpeg_create_compress(&cinfo);

        if (setjmp(jerr.setjmp_buffer)) {
            jpeg_destroy_compress(&cinfo);
            std::string msg("JPEG compression has failed");
            throw AireError(msg);
        }

        std::vector<uint8_t> output;
        jpeg_stdvector_dest(&cinfo, output);

        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_EXT_BGR;

        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, quality, TRUE);

        jpeg_start_compress(&cinfo, TRUE);

        JSAMPROW row_pointer;

        while (cinfo.next_scanline < cinfo.image_height) {
            row_pointer = reinterpret_cast<uint8_t *>(rgbData.data()) + cinfo.next_scanline * newStride;
            jpeg_write_scanlines(&cinfo, &row_pointer, 1);
        }

        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);

        return output;
    }
}