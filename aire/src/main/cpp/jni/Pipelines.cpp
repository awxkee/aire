//
// Created by Radzivon Bartoshyk on 02/02/2024.
//

#include <jni.h>
#include "JNIUtils.h"
#include "AcquireBitmapPixels.h"
#include "pipelines/RemoveShadows.h"
#include "pipelines/DehazeDarkChannel.h"
#include "MathUtils.hpp"
#include "Eigen/Eigen"
#include "base/Convolve2D.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_ProcessingPipelinesImpl_removeShadowsPipelines(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jobject bitmap,
                                                                             jint kernelSize) {
    try {
        if (kernelSize < 3 || kernelSize > 9) {
            std::string message =
                    "Kernel size must be in 3..9 but received " + std::to_string(kernelSize);
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

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_ProcessingPipelinesImpl_convolve2DImpl(JNIEnv *env, jobject thiz,
                                                                     jobject bitmap,
                                                                     jfloatArray kernel) {
    try {
        jsize length = env->GetArrayLength(kernel);
        if (!isSquareRootInteger(length)) {
            std::string msg = "Matrix must be square";
            throwException(env, msg);
            return nullptr;
        }

        int size = std::sqrt(length);

        Eigen::MatrixXf matrix(size, size);
        jfloat *inputElements = env->GetFloatArrayElements(kernel, 0);
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size; ++i) {
                matrix(j, i) = inputElements[j * size + i];
            }
        }
        env->ReleaseFloatArrayElements(kernel, inputElements, 0);

        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [matrix](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::Convolve2D convolve2D(matrix);
                                                        convolve2D.convolve(input.data(), stride, width, height);
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