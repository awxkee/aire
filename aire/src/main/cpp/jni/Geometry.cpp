/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 15/02/24, 6:13 PM
 *  *
 *  * Permission is hereby granted, free of charge, to any person obtaining a copy
 *  * of this software and associated documentation files (the "Software"), to deal
 *  * in the Software without restriction, including without limitation the rights
 *  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  * copies of the Software, and to permit persons to whom the Software is
 *  * furnished to do so, subject to the following conditions:
 *  *
 *  * The above copyright notice and this permission notice shall be included in all
 *  * copies or substantial portions of the Software.
 *  *
 *  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  * SOFTWARE.
 *  *
 *
 */

#include <jni.h>
#include "JNIUtils.h"
#include "AcquireBitmapPixels.h"
#include "MathUtils.hpp"
#include "base/AffineTransform.h"
#include "EigenUtils.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_cropImpl(JNIEnv *env, jobject thiz, jobject bitmap,
                                                         jint baseX, jint baseY, jint newWidth, jint newHeight) {
    try {
        if (newWidth < 0 || newHeight < 0) {
            std::string msg = "Width and height must be > 0 but received (" + std::to_string(newWidth) + "," + std::to_string(newHeight) + ")";
            throw AireError(msg);
        }
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [baseX, baseY, newWidth, newHeight](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        int newStride = computeStride(newWidth, sizeof(uint8_t), 4);
                                                        std::vector<uint8_t> output(newStride * newHeight);
                                                        Eigen::Affine3f matrix = Eigen::Affine3f::Identity();
                                                        matrix.translation() << baseX, baseY, 0;
                                                        aire::AffineTransform transform(input.data(), stride, width, height);
                                                        transform.setTransform(matrix);
                                                        transform.apply(output.data(), newStride, newWidth, newHeight);
                                                        return {
                                                                .data = output,
                                                                .stride = newStride,
                                                                .width = newWidth,
                                                                .height = newHeight,
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

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_rotateImpl(JNIEnv *env, jobject thiz, jobject bitmap,
                                                           jfloat angle, jint anchorPointX, jint anchorPointY,
                                                           jint newWidth, jint newHeight) {
    try {
        if (newWidth < 0 || newHeight < 0) {
            std::string msg = "Width and height must be > 0 but received (" + std::to_string(newWidth) + "," + std::to_string(newHeight) + ")";
            throw AireError(msg);
        }
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [angle, newWidth, newHeight, anchorPointX, anchorPointY](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        int newStride = computeStride(newWidth, sizeof(uint8_t), 4);
                                                        std::vector<uint8_t> output(newStride * newHeight);
                                                        Eigen::Affine3f matrix = Eigen::Affine3f::Identity();

                                                        auto t = Eigen::Translation3f(Eigen::Vector3f{anchorPointX, anchorPointY, 0.f});
                                                        Eigen::Affine3f tr(t);

                                                        auto comp = Eigen::Translation3f(Eigen::Vector3f{-anchorPointX, -anchorPointY, 0.f});
                                                        Eigen::Affine3f trc(comp);

                                                        Eigen::Vector3f axis = {0.f, 0.f, 1.f};
                                                        matrix = matrix * tr;
                                                        matrix.rotate(Eigen::AngleAxis<float>(angle, axis));
                                                        matrix = matrix * trc;

                                                        matrix.translate(Eigen::Vector3f{-(newWidth - width) / 2.f, -(newHeight - height) / 2.f, 0.f});

                                                        aire::AffineTransform transform(input.data(), stride, width, height);
                                                        transform.setTransform(matrix);
                                                        transform.apply(output.data(), newStride, newWidth, newHeight);
                                                        return {
                                                                .data = output,
                                                                .stride = newStride,
                                                                .width = newWidth,
                                                                .height = newHeight,
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

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_warpAffineImpl(JNIEnv *env, jobject thiz, jobject bitmap,
                                                               jfloatArray transform, jint newWidth,
                                                               jint newHeight) {
    try {
        if (newWidth < 0 || newHeight < 0) {
            std::string msg = "Width and height must be > 0 but received (" + std::to_string(newWidth) + "," + std::to_string(newHeight) + ")";
            throw AireError(msg);
        }

        jsize length = env->GetArrayLength(transform);
        if (length != 9) {
            std::string msg = "Affine transform must be exactly 3x3";
            throwException(env, msg);
            return nullptr;
        }

        Eigen::Matrix3f colorMatrix;

        Eigen::Affine3f affine = Eigen::Affine3f::Identity();
        jfloat *inputElements = env->GetFloatArrayElements(transform, 0);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                affine(i, j) = inputElements[i * 3 + j];
            }
        }
        env->ReleaseFloatArrayElements(transform, inputElements, 0);

        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [newWidth, newHeight, affine](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        int newStride = computeStride(newWidth, sizeof(uint8_t), 4);
                                                        std::vector<uint8_t> output(newStride * newHeight);

                                                        aire::AffineTransform transform(input.data(), stride, width, height);
                                                        transform.setTransform(affine);
                                                        transform.apply(output.data(), newStride, newWidth, newHeight);
                                                        return {
                                                                .data = output,
                                                                .stride = newStride,
                                                                .width = newWidth,
                                                                .height = newHeight,
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