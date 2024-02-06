#include <jni.h>
#include "JNIUtils.h"
#include "AcquireBitmapPixels.h"
#include "pipelines/RemoveShadows.h"
#include "pipelines/DehazeDarkChannel.h"

//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_ProcessingPipelinesImpl_removeShadowsPipelines(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jobject bitmap,
                                                                             jint kernelSize) {
    try {
        if (kernelSize < 3 || kernelSize > 7) {
            std::string message =
                    "Kernel size must be in 3..7 but received " + std::to_string(kernelSize);
            throw AireError(message);
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
                                                        aire::removeShadows(input.data(), stride,
                                                                            width, height, kernelSize);
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
Java_com_awxkee_aire_pipeline_ProcessingPipelinesImpl_dehazeImpl(JNIEnv *env, jobject thiz,
                                                                 jobject bitmap, jint radius, jfloat omega) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius, omega](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::dehaze(input.data(), stride,
                                                                     width, height, radius, omega);
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