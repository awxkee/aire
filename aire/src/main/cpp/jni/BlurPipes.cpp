/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 01/02/24, 6:13 PM
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
#include "blur/ShgStackBlur.h"
#include "blur/MedianBlur.h"
#include "blur/BoxBlur.h"
#include "blur/BilateralBlur.h"
#include "blur/GaussBlur.h"
#include "blur/PoissonBlur.h"
#include <string>
#include "blur/ZoomBlur.hpp"
#include "blur/AnisotropicDiffusion.h"
#include "color/Gamut.h"
#include "EigenUtils.h"

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
                                                         int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::boxBlurU8(input.data(), stride, width,
                                                                        height, radius);
                                                    } else if (fmt == APF_F16) {
                                                        aire::boxBlurF16(reinterpret_cast<uint16_t *>(input.data()),
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
                                                         int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::medianBlur(
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
                                                false,
                                                [radius, sigma, spatialSigma](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
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
        formats.insert(formats.begin(), APF_F16);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius, sigma](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::gaussBlurU8(input.data(),
                                                                          stride, width,
                                                                          height, radius,
                                                                          sigma);
                                                    } else if (fmt == APF_F16) {
                                                        aire::gaussBlurF16(reinterpret_cast<uint16_t *>(input.data()),
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
        formats.insert(formats.begin(), APF_F16);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::tentBlur(input.data(),
                                                                       stride, width,
                                                                       height, radius);
                                                    } else if (fmt == APF_F16) {
                                                        aire::tentBlurF16(reinterpret_cast<uint16_t *>(input.data()),
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
                                                        int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
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

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_poissonBlurPipeline(JNIEnv *env, jobject thiz, jobject bitmap, jint radius) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_F16);
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius](std::vector<uint8_t> &input, int stride,
                                                         int width, int height,
                                                         AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::poissonBlur(input.data(), stride, width,
                                                                          height, radius);
                                                    } else if (fmt == APF_F16) {
                                                        aire::poissonBlurF16(reinterpret_cast<uint16_t *>(input.data()),
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
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_fastGaussian2DImpl(JNIEnv *env, jobject thiz, jobject bitmap, jint radius) {
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
                                                        aire::gaussianApproximation2D(input.data(), stride, width,
                                                                                      height, radius);
                                                        return {
                                                                .data = input,
                                                                .stride = stride,
                                                                .width = width,
                                                                .height = height,
                                                                .pixelFormat = APF_RGBA8888
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
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_fastGaussian3DImpl(JNIEnv *env, jobject thiz, jobject bitmap, jint radius) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [&](std::vector<uint8_t> &input, int stride,
                                                    int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::gaussianApproximation3D(input.data(), stride, width,
                                                                                      height, radius);
                                                        return {
                                                                .data = input,
                                                                .stride = stride,
                                                                .width = width,
                                                                .height = height,
                                                                .pixelFormat = APF_RGBA8888
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
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_fastGaussian4DImpl(JNIEnv *env, jobject thiz, jobject bitmap, jint radius) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [&](std::vector<uint8_t> &input, int stride,
                                                    int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::gaussianApproximation4D(input.data(), stride, width,
                                                                                      height, radius);
                                                        return {
                                                                .data = input,
                                                                .stride = stride,
                                                                .width = width,
                                                                .height = height,
                                                                .pixelFormat = APF_RGBA8888
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
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_zoomBlurImpl(JNIEnv *env, jobject thiz, jobject bitmap,
                                                             jint kernelSize, jfloat sigma,
                                                             jfloat centerX, jfloat centerY,
                                                             jfloat strength, jfloat angle) {
    try {
        if (kernelSize < 1) {
            std::string msg("Kernel size must be > 1, but received " + std::to_string(kernelSize));
            throw AireError(msg);
        }
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [&](std::vector<uint8_t> &input, int stride,
                                                    int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::ZoomBlur zoom(kernelSize, sigma, centerX, centerY, strength, angle);
                                                        zoom.apply(input.data(), stride, width, height);
                                                        return {
                                                                .data = input,
                                                                .stride = stride,
                                                                .width = width,
                                                                .height = height,
                                                                .pixelFormat = APF_RGBA8888
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