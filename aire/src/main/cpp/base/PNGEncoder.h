//
// Created by Radzivon Bartoshyk on 09/02/2024.
//

#pragma once

#include <cstdint>
#include <vector>
#include "spng/spng.h"
#include "Eigen/Eigen"
#include <string>
#include "jni/JNIUtils.h"

namespace aire {
    class PNGEncoder {
    public:
        PNGEncoder(uint8_t *data, int stride, int width, int height) : data(data), stride(stride),
                                                                       width(width), height(height) {
            ctx = spng_ctx_new(SPNG_CTX_ENCODER);
            spng_set_option(ctx, SPNG_ENCODE_TO_BUFFER, 1);
        }

        ~PNGEncoder() {
            spng_ctx_free(ctx);
        }

        void setCompressionLevel(int level) {
            spng_set_option(ctx, SPNG_IMG_COMPRESSION_LEVEL, level);
            spng_set_option(ctx, SPNG_TEXT_COMPRESSION_LEVEL, level);
        }

        std::vector<uint8_t> encode() {
            ihdr.width = width;
            ihdr.height = height;
            ihdr.color_type = SPNG_COLOR_TYPE_TRUECOLOR_ALPHA;
            ihdr.bit_depth = 8;

            spng_set_ihdr(ctx, &ihdr);

            int ret = spng_encode_image(ctx, data, width * height, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE | SPNG_ENCODE_PROGRESSIVE);
            if (ret) {
                std::string msg("Cannot encode an image");
                throw AireError(msg);
            }
            size_t bufSize = 0;
            int error = 0;
            void *pngBuf = spng_get_png_buffer(ctx, &bufSize, &error);
            if (error != SPNG_OK) {
                std::string msg("Cannot encode an image with error: " + std::to_string(error));
                throw AireError(msg);
            }
            if (bufSize < 0) {
                std::string msg("Invalid buffer size has returned");
                throw AireError(msg);
            }
            std::vector<uint8_t> output(bufSize);
            std::copy(reinterpret_cast<uint8_t *>(pngBuf), reinterpret_cast<uint8_t *>(pngBuf) + bufSize, output.begin());
            return output;
        }

        std::vector<uint8_t> getPNGData() {
            ihdr.width = width;
            ihdr.height = height;
            ihdr.bit_depth = 8;

            bool hasAlpha = false;

            for (int y = 0; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
                for (int x = 0; x < width; ++x) {
                    if (src[3] != 255) {
                        hasAlpha = true;
                        break;
                    }
                    src += 4;
                }
                if (!hasAlpha) {
                    break;
                }
            }

            ihdr.color_type = hasAlpha ? SPNG_COLOR_TYPE_TRUECOLOR_ALPHA : SPNG_COLOR_TYPE_TRUECOLOR;
            int components = hasAlpha ? 4 : 3;

            const int dstStride = sizeof(uint8_t) * width * components;
            std::vector<uint8_t> transient(dstStride * height);
            for (int y = 0; y < height; ++y) {
                auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(data) + y * stride);
                auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(transient.data()) + y * dstStride);
                for (int x = 0; x < width; ++x) {
                    std::copy(src, src + components * sizeof(uint8_t), dst);
                    src += 4;
                    dst += components;
                }
            }

            spng_set_ihdr(ctx, &ihdr);
            spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
            spng_set_gama(ctx, 1 / 2.2f);
            spng_set_option(ctx, SPNG_FILTER_CHOICE, SPNG_FILTER_PAETH);

            int ret = spng_encode_image(ctx, transient.data(), dstStride * height, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE);
            if (ret) {
                std::string msg("Cannot encode an image");
                throw AireError(msg);
            }
            size_t bufSize = 0;
            int error = 0;
            void *pngBuf = spng_get_png_buffer(ctx, &bufSize, &error);
            if (error != SPNG_OK) {
                std::string msg("Cannot encode an image with error: " + std::to_string(error));
                throw AireError(msg);
            }
            if (bufSize < 0) {
                std::string msg("Invalid buffer size has returned");
                throw AireError(msg);
            }
            std::vector<uint8_t> output(bufSize);
            std::copy(reinterpret_cast<uint8_t *>(pngBuf), reinterpret_cast<uint8_t *>(pngBuf) + bufSize, output.begin());
            return output;
        }

        std::vector<uint8_t> encode(std::vector<Eigen::Vector4i> &palette, const float gamma = 1 / 2.4f) {
            ihdr.width = width;
            ihdr.height = height;
            ihdr.color_type = SPNG_COLOR_TYPE_INDEXED;
            ihdr.bit_depth = 8;

            spng_set_ihdr(ctx, &ihdr);
            struct spng_plte plte = {0};
            struct spng_trns trns = {0};
            for (int i = 0; i < palette.size(); i++) {
                auto p = palette[i];

                struct spng_plte_entry *entry =
                        &plte.entries[plte.n_entries];

                entry->red = p.x();
                entry->green = p.y();
                entry->blue = p.z();
                plte.n_entries += 1;

                trns.type3_alpha[trns.n_type3_entries] = p.w();
                trns.n_type3_entries += 1;
            }

            spng_set_plte(ctx, &plte);
            spng_set_gama(ctx, gamma);
            if (trns.n_type3_entries) {
                spng_set_trns(ctx, &trns);
            }
            int ret = spng_encode_image(ctx, data, width * height, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE);
            if (ret) {
                std::string msg("Cannot encode an image");
                throw AireError(msg);
            }
            size_t bufSize = 0;
            int error = 0;
            void *pngBuf = spng_get_png_buffer(ctx, &bufSize, &error);
            if (error != SPNG_OK) {
                std::string msg("Cannot encode an image with error: " + std::to_string(error));
                throw AireError(msg);
            }
            if (bufSize < 0) {
                std::string msg("Invalid buffer size has returned");
                throw AireError(msg);
            }
            std::vector<uint8_t> output(bufSize);
            std::copy(reinterpret_cast<uint8_t *>(pngBuf), reinterpret_cast<uint8_t *>(pngBuf) + bufSize, output.begin());
            return output;
        }

    private:
        spng_ctx *ctx;
        uint8_t *data;
        spng_phys phys = {0};
        const int stride;
        const int width;
        const int height;
        struct spng_ihdr ihdr = {0};
    };
}