//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#include <jni.h>
#include "scale/XScaler.h"
#include "JNIUtils.h"
#include "AcquireBitmapPixels.h"
#include "conversion/RGBAlpha.h"
#include "blur/GaussBlur.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_ScalePipelinesImpl_scaleImpl(JNIEnv *env, jobject thiz, jobject bitmap, jint dstWidth,
                                                           jint dstHeight, jint scaleMode, jboolean antialias) {
    try {
        if (dstWidth < 0 || dstHeight < 0) {
            std::string msg("Width and height must be > but received (" + std::to_string(dstWidth) + ", " + std::to_string(dstHeight) + ")");
            throw AireError(msg);
        }
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        formats.insert(formats.begin(), APF_F16);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [dstHeight, dstWidth, scaleMode, antialias](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        int lineWidth = dstWidth * sizeof(uint8_t) * 4;
                                                        int alignment = 64;
                                                        int padding = (alignment - (lineWidth % alignment)) % alignment;
                                                        int dstStride = lineWidth + padding;

                                                        std::vector<uint8_t> output(dstStride * dstHeight);

                                                        aire::UnpremultiplyRGBA(reinterpret_cast<uint8_t *>(input.data()),
                                                                                stride,
                                                                                reinterpret_cast<uint8_t *>(input.data()),
                                                                                stride,
                                                                                width, height);

                                                        float ratio = dstWidth / width;
                                                        if (antialias) {
                                                            if (ratio < 0.5f) {
                                                                if (scaleMode == bilinear || scaleMode == nearest) {
                                                                    aire::gaussBlurU8(reinterpret_cast<uint8_t *>(input.data()),
                                                                                      stride,
                                                                                      width, height, 1, 1.5f);
                                                                } else {
                                                                    aire::gaussBlurU8(reinterpret_cast<uint8_t *>(input.data()),
                                                                                      stride,
                                                                                      width, height, 2, 1.55f);
                                                                }
                                                            }
                                                        }

                                                        aire::scaleImageU8(
                                                                reinterpret_cast<const uint8_t *>(input.data()),
                                                                stride,
                                                                width, height,
                                                                output.data(),
                                                                dstStride, dstWidth,
                                                                dstHeight,
                                                                4, 8,
                                                                static_cast<XSampler>(scaleMode)
                                                        );

                                                        aire::PremultiplyRGBA(reinterpret_cast<uint8_t *>(output.data()),
                                                                              dstStride,
                                                                              reinterpret_cast<uint8_t *>(output.data()),
                                                                              dstStride,
                                                                              dstWidth, dstHeight);

                                                        return {
                                                                .data = output,
                                                                .stride = dstStride,
                                                                .width = dstWidth,
                                                                .height = dstHeight,
                                                                .pixelFormat = fmt
                                                        };
                                                    } else if (fmt == APF_F16) {
                                                        int lineWidth = dstWidth * sizeof(uint16_t) * 4;
                                                        int alignment = 64;
                                                        int padding = (alignment - (lineWidth % alignment)) % alignment;
                                                        int dstStride = lineWidth + padding;

                                                        std::vector<uint8_t> output(dstStride * dstHeight);

                                                        float ratio = dstWidth / width;
                                                        if (antialias) {
                                                            if (ratio < 0.5f) {
                                                                if (scaleMode == bilinear || scaleMode == nearest) {
                                                                    aire::gaussBlurF16(reinterpret_cast<uint16_t *>(input.data()),
                                                                                       stride,
                                                                                       width, height, 1, 1.5f);
                                                                } else {
                                                                    aire::gaussBlurF16(reinterpret_cast<uint16_t *>(input.data()),
                                                                                       stride,
                                                                                       width, height, 2, 1.55f);
                                                                }
                                                            }
                                                        }

                                                        aire::scaleImageFloat16(
                                                                reinterpret_cast<const uint16_t *>(input.data()),
                                                                stride,
                                                                width, height,
                                                                reinterpret_cast<uint16_t *>(output.data()),
                                                                dstStride, dstWidth,
                                                                dstHeight, 4,
                                                                static_cast<XSampler>(scaleMode)
                                                        );

                                                        return {
                                                                .data = output,
                                                                .stride = dstStride,
                                                                .width = dstWidth,
                                                                .height = dstHeight,
                                                                .pixelFormat = fmt
                                                        };
                                                    }
                                                    return {
                                                            .data = input,
                                                            .stride = stride,
                                                            .width = width,
                                                            .height = height,
                                                            .pixelFormat = fmt
                                                    };
                                                });
        return newBitmap;
    } catch (AireError &err) {
        std::string msg = err.what();
        throwException(env, msg);
        return nullptr;
    }
}