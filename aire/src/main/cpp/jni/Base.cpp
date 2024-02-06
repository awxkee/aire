#include <jni.h>
#include "JNIUtils.h"
#include "AcquireBitmapPixels.h"
#include "base/Grayscale.h"
#include "base/Channels.h"
#include "base/Dilation.h"
#include "base/Threshold.h"
#include "base/Erosion.h"
#include "base/Vibrance.h"
#include "base/Convolve2D3x3.h"
#include "base/Grain.h"
#include "base/Sharpness.h"
#include "base/Convolve2D.h"
#include "blur/GaussBlur.h"
#include "color/Adjustments.h"
#include "MathUtils.hpp"

//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_grayscalePipeline(JNIEnv *env, jobject thiz,
                                                                  jobject bitmap, jfloat rPrimary,
                                                                  jfloat gPrimary,
                                                                  jfloat bPrimary) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [rPrimary, gPrimary, bPrimary](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::grayscale(input.data(), input.data(),
                                                                        stride, width,
                                                                        height, rPrimary, gPrimary,
                                                                        bPrimary);
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
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_dilatePipeline(JNIEnv *env, jobject thiz,
                                                               jobject bitmap,
                                                               jint kernelSize) {
    try {
        if (kernelSize <= 0) {
            std::string msg("Kernel size must be >= 1");
            throw AireError(msg);
        }
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [kernelSize](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        auto kernel = getStructuringKernel(
                                                                kernelSize);

                                                        std::vector<uint8_t> output(
                                                                stride * height);

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
extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_thresholdPipeline(JNIEnv *env, jobject thiz,
                                                                  jobject bitmap, jint level) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [level](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {

                                                        std::vector<uint8_t> output(
                                                                stride * height);

                                                        std::vector<uint8_t> rChannel(
                                                                width * height);
                                                        std::vector<uint8_t> gChannel(
                                                                width * height);
                                                        std::vector<uint8_t> bChannel(
                                                                width * height);
                                                        std::vector<uint8_t> aChannel(
                                                                width * height);
                                                        aire::split(input.data(),
                                                                    rChannel.data(),
                                                                    gChannel.data(),
                                                                    bChannel.data(),
                                                                    aChannel.data(),
                                                                    stride, width, height);

                                                        aire::threshold(rChannel.data(), width,
                                                                        height, uint8_t(level),
                                                                        uint8_t(255), uint8_t(0));

                                                        aire::merge(input.data(), rChannel.data(),
                                                                    rChannel.data(),
                                                                    rChannel.data(),
                                                                    aChannel.data(), stride, width,
                                                                    height);
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
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_erodePipeline(JNIEnv *env, jobject thiz,
                                                              jobject bitmap, jint kernelSize) {
    try {
        if (kernelSize <= 0) {
            std::string msg("Kernel size must be >= 1");
            throw AireError(msg);
        }
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [kernelSize](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        auto kernel = getStructuringKernel(
                                                                kernelSize);

                                                        std::vector<uint8_t> output(
                                                                stride * height);

                                                        aire::erodeRGBA(input.data(),
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
extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_vibrancePipeline(JNIEnv *env, jobject thiz, jobject bitmap, jfloat vibrance) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [vibrance](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::vibrance(input.data(),
                                                                       stride,
                                                                       width,
                                                                       height,
                                                                       vibrance);
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
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_contrastImpl(JNIEnv *env, jobject thiz, jobject bitmap, jfloat gain) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [gain](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::adjustment(input.data(),
                                                                         stride,
                                                                         width,
                                                                         height,
                                                                         gain,
                                                                         0.0f);
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
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_brightnessImpl(JNIEnv *env, jobject thiz, jobject bitmap, jfloat bias) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [bias](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::adjustment(input.data(),
                                                                         stride,
                                                                         width,
                                                                         height,
                                                                         1.0f,
                                                                         bias);
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
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_colorMatrixImpl(JNIEnv *env, jobject thiz, jobject bitmap, jfloatArray jColorMatrix) {
    try {
        jsize length = env->GetArrayLength(jColorMatrix);
        if (length != 9) {
            std::string msg = "Colors array must be exactly four elements";
            throwException(env, msg);
            return nullptr;
        }

        Eigen::Matrix3f colorMatrix;

        jfloat *inputElements = env->GetFloatArrayElements(jColorMatrix, 0);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                colorMatrix(i, j) = inputElements[i * 3 + j];
            }
        }
        env->ReleaseFloatArrayElements(jColorMatrix, inputElements, 0);

        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [colorMatrix](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::colorMatrix(input.data(),
                                                                          stride,
                                                                          width,
                                                                          height,
                                                                          colorMatrix);
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
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_embossImpl(JNIEnv *env, jobject thiz, jobject bitmap, jfloat intensity) {
    Eigen::Matrix3f colorMatrix;
    colorMatrix << intensity * -2, -intensity, 0, -intensity, 1, intensity, 0, intensity, intensity * 2;
    std::vector<AcquirePixelFormat> formats;
    try {
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [colorMatrix, intensity](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::Convolve2D3x3 convolve2D(colorMatrix);
                                                        convolve2D.convolve(input.data(),
                                                                          stride,
                                                                          width,
                                                                          height);
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
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_grainImpl(JNIEnv *env, jobject thiz, jobject bitmap, jfloat intensity) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [intensity](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::grain(input.data(),
                                                                         stride,
                                                                         width,
                                                                         height,
                                                                         intensity);
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
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_sharpnessImpl(JNIEnv *env, jobject thiz, jobject bitmap, jfloat intensity) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [intensity](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        std::vector<uint8_t > sharpen(stride * height);
                                                        std::copy(input.begin(), input.end(), sharpen.begin());
                                                        auto kernel = aire::generateSharpenKernel();
                                                        aire::Convolve2D3x3 convolve2D(kernel);
                                                        convolve2D.convolve(sharpen.data(),
                                                                            stride,
                                                                            width,
                                                                            height);
                                                        aire::applySharp(input.data(), sharpen.data(), stride, width, height, intensity);
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
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_unsharpImpl(JNIEnv *env, jobject thiz, jobject bitmap, jfloat intensity) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [intensity](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        std::vector<uint8_t > sharpen(stride * height);
                                                        std::copy(input.begin(), input.end(), sharpen.begin());
                                                        aire::gaussBlurU8(sharpen.data(), stride, width, height, 5, 4.f);
                                                        aire::applyUnsharp(input.data(), sharpen.data(), stride, width, height, intensity);
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