#include <jni.h>
#include "JNIUtils.h"
#include "AcquireBitmapPixels.h"
#include "base/Grayscale.h"
#include "base/Channels.h"
#include "base/Dilation.h"
#include "base/Threshold.h"
#include "base/Erosion.h"
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