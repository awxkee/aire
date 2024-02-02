#include <jni.h>

//
// Created by Radzivon Bartoshyk on 01/02/2024.
//

#include "../JNIUtils.h"
#include "../AcquireBitmapPixels.h"
#include "blur/ShgStackBlur.h"
#include "blur/MedianBlur.h"
#include "blur/BoxBlur.h"
#include "blur/BilateralBlur.h"
#include "blur/GaussBlur.h"
#include <string>
#include "blur/TentBlur.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_stackNativeBlurPipeline(JNIEnv *env, jobject thiz,
                                                                        jobject bitmap,
                                                                        jint radius) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                false,
                                                [radius](std::vector<uint8_t> &input, int stride,
                                                         int width, int height,
                                                         AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    shgStackBlur(input.data(), width, height,
                                                                 radius);
                                                    return {
                                                            .data = input,
                                                            .stride = stride,
                                                            .width = width,
                                                            .height = height,
                                                            .pixelFormat = APF_RGBA8888
                                                    };
                                                });
        return newBitmap;
    } catch (AireError &err) {
        std::string msg = err.what();
        throwException(env, msg);
        return nullptr;
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_boxBlurPipeline(JNIEnv *env, jobject thiz,
                                                                jobject bitmap,
                                                                jint radius) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        formats.insert(formats.begin(), APF_F16);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius](std::vector<uint8_t> &input, int stride,
                                                         int width, int height,
                                                         AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::boxBlurU8(input.data(), stride, width,
                                                                        height, radius);
                                                    } else if (fmt == APF_F16) {
                                                        aire::boxBlurF16(
                                                                reinterpret_cast<uint16_t *>(input.data()),
                                                                stride,
                                                                width, height, radius);
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
extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_medianBlurPipeline(JNIEnv *env, jobject thiz,
                                                                   jobject bitmap,
                                                                   jint radius) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                false,
                                                [radius](std::vector<uint8_t> &input, int stride,
                                                         int width, int height,
                                                         AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        medianBlur(
                                                                reinterpret_cast<uint8_t *>(input.data()),
                                                                stride, width, height, radius);
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
extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_bilateralBlurPipeline(JNIEnv *env, jobject thiz,
                                                                      jobject bitmap,
                                                                      jint radius, jfloat sigma,
                                                                      jfloat spatialSigma) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius, sigma, spatialSigma](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::bilateralBlur<uint8_t>(input.data(),
                                                                                     stride, width,
                                                                                     height, radius,
                                                                                     sigma,
                                                                                     spatialSigma);
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
extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_gaussianBlurPipeline(JNIEnv *env, jobject thiz,
                                                                     jobject bitmap,
                                                                     jint radius, jfloat sigma) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius, sigma](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::gaussBlurU8(input.data(),
                                                                          stride, width,
                                                                          height, radius,
                                                                          sigma);
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
extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_tentBlurPipeline(JNIEnv *env, jobject thiz,
                                                                 jobject bitmap, jint radius) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::tentBlur(input.data(),
                                                                       stride, width,
                                                                       height, radius);
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