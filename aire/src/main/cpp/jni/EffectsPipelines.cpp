#include <jni.h>
#include "effect/MarbleEffect.h"
#include "JNIUtils.h"
#include "AcquireBitmapPixels.h"
#include "effect/FractalGlassEffect.h"
#include "effect/OilEffect.h"
#include "effect/CrystallizeEffect.h"
#include "effect/WaterEffect.h"
#include "effect/PerlinDistortion.h"
#include "base/Dilation.h"
#include "color/ConvolveToneMapper.h"
#include "MathUtils.hpp"

//
// Created by Radzivon Bartoshyk on 03/02/2024.
//

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_marbleImpl(JNIEnv *env, jobject thiz,
                                                             jobject bitmap, jfloat intensity,
                                                             jfloat turbulence, jfloat amplitude) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [intensity, turbulence, amplitude](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::marbleEffect(input.data(),
                                                                           stride, width,
                                                                           height, intensity,
                                                                           turbulence,
                                                                           amplitude);
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
Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_oilImpl(JNIEnv *env, jobject thiz, jobject bitmap,
                                                          jint radius, jfloat levels) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [levels, radius](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::oilEffect(input.data(),
                                                                        stride, width,
                                                                        height, radius,
                                                                        levels);
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
Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_crystallizeImpl(JNIEnv *env, jobject thiz,
                                                                  jobject bitmap, jint clustersCount,
                                                                  jint strokeColor) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [clustersCount, strokeColor](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::crystallize(input.data(),
                                                                          stride, width,
                                                                          height,
                                                                          clustersCount,
                                                                          strokeColor);
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
Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_fractalGlassImpl(JNIEnv *env, jobject thiz, jobject bitmap, jfloat glassSize, jfloat amplitude) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [glassSize, amplitude](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::fractalGlassEffect(input.data(),
                                                                                 stride, width,
                                                                                 height, glassSize, amplitude);
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
Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_waterEffectImpl(JNIEnv *env, jobject thiz, jobject bitmap,
                                                                  jfloat fractionSize,
                                                                  jfloat frequencyX, jfloat amplitudeX,
                                                                  jfloat frequencyY, jfloat amplitudeY) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [frequencyX, amplitudeX, frequencyY, amplitudeY, fractionSize](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::waterEffect(input.data(),
                                                                          stride, width,
                                                                          height,
                                                                          fractionSize,
                                                                          frequencyX,
                                                                          amplitudeX,
                                                                          frequencyY,
                                                                          amplitudeY);
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
Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_perlinDistortionImpl(JNIEnv *env, jobject thiz, jobject bitmap, jfloat intensity, jfloat turbulence,
                                                                       jfloat amplitude) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [intensity, turbulence, amplitude](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::perlinDistortion(input.data(),
                                                                          stride, width,
                                                                          height,
                                                                          intensity,
                                                                          turbulence,
                                                                          amplitude);
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
Java_com_awxkee_aire_pipeline_EffectsPipelineImpl_bokehImpl(JNIEnv *env, jobject thiz, jobject bitmap,
                                                            jint radius, jfloat angle, jint sides) {
    if (radius < 1) {
        std::string msg("Radius must be > 0 but received: " + std::to_string(radius));
        throw AireError(msg);
    }
    if (sides < 1) {
        std::string msg("Sides must be > 0 but received: " + std::to_string(sides));
        throw AireError(msg);
    }
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius, angle, sides](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        auto kernel = getBokehEffect(radius, angle, sides);
                                                        std::vector<uint8_t> output(stride * height);
                                                        aire::dilateRGBA(input.data(),
                                                                         output.data(),
                                                                         stride,
                                                                         width,
                                                                         height,
                                                                         kernel);
                                                        input = output;
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