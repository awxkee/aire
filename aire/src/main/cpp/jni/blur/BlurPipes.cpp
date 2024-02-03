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
#include "blur/AnisotropicDiffusion.h"

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
                                                                   jint radius, jint selector) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                false,
                                                [radius, selector](std::vector<uint8_t> &input, int stride,
                                                         int width, int height,
                                                         AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::medianBlur(
                                                                reinterpret_cast<uint8_t *>(input.data()),
                                                                stride, width, height, radius,
                                                                static_cast<aire::MedianSelector>(selector));
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
                                                false,
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
extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_anisotropicDiffusionPipeline(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jobject bitmap,
                                                                             jint numOfSteps,
                                                                             jfloat conduction,
                                                                             jfloat diffusion) {
    try {
        if (numOfSteps <= 0) {
            std::string msg("Number of steps must be positive");
            throw AireError(msg);
        }
        if (conduction == 0.f) {
            std::string msg("Conduction must not be 0");
            throw AireError(msg);
        }
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [numOfSteps, conduction, diffusion](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::anisotropicDiffusion(input.data(),
                                                                       stride, width,
                                                                       height, diffusion, conduction, numOfSteps);
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