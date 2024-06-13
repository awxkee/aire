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
#include "base/Convolve2D.h"
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
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_fastBilateralPipeline(JNIEnv *env, jobject thiz, jobject bitmap, jfloat rangeSigma, jfloat spatialSigma) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                false,
                                                [rangeSigma, spatialSigma](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height, AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        int xyzStride = width * 3 * sizeof(float);
                                                        std::vector<float> xyzBitmap(xyzStride * height);
//
                                                        for (int y = 0; y < height; ++y) {
                                                            auto src = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(input.data()) + y * stride);
                                                            auto dst = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(xyzBitmap.data()) + xyzStride * y);
                                                            for (int x = 0; x < width; ++x) {
                                                                float r, g, b;
                                                                r = src[0];
                                                                g = src[1];
                                                                b = src[2];
                                                                dst[0] = r / 255.f;
                                                                dst[1] = g / 255.f;
                                                                dst[2] = b / 255.f;
                                                                dst += 3;
                                                                src += 4;
                                                            }
                                                        }
                                                        aire::FastBilateralFilter<float> bf;
                                                        bf.setSigmaR(rangeSigma);
                                                        bf.setSigmaS(spatialSigma);
                                                        bf.applyFilter(reinterpret_cast<float *>(xyzBitmap.data()),
                                                                       xyzStride, width,
                                                                       height, 0, 3);
                                                        bf.applyFilter(reinterpret_cast<float *>(xyzBitmap.data()),
                                                                       xyzStride, width,
                                                                       height, 1, 3);
                                                        bf.applyFilter(reinterpret_cast<float *>(xyzBitmap.data()),
                                                                       xyzStride, width,
                                                                       height, 2, 3);

                                                        for (int y = 0; y < height; ++y) {
                                                            auto dst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(input.data()) + y * stride);
                                                            auto src = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(xyzBitmap.data()) + xyzStride * y);
                                                            for (int x = 0; x < width; ++x) {
                                                                float r, g, b;
                                                                r = src[0];
                                                                g = src[1];
                                                                b = src[2];
                                                                dst[0] = std::clamp(r * 255.f, 0.f, 255.f);
                                                                dst[1] = std::clamp(g * 255.f, 0.f, 255.f);
                                                                dst[2] = std::clamp(b * 255.f, 0.f, 255.f);
                                                                dst += 4;
                                                                src += 3;
                                                            }
                                                        }
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

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_BlurPipelinesImpl_bokehBlurImpl(JNIEnv *env,
                                                              jobject thiz, jobject bitmap,
                                                              jint kernelSize, jint sides) {
    try {
        if (kernelSize < 3) {
            std::string msg("Kernel size must be >= 3, but received " + std::to_string(kernelSize));
            throw AireError(msg);
        }
        if (sides < 3) {
            std::string msg("Sides must be >= 3, but received " + std::to_string(sides));
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
                                                        auto krn = getBokehEffect(kernelSize, sides);
                                                        auto bokehKernel = krn.cast<float>().eval();
                                                        auto sigma = std::max(static_cast<float>(bokehKernel.cols()), static_cast<float>(bokehKernel.rows()));
                                                        const auto center = std::max(static_cast<float>(bokehKernel.cols()), static_cast<float>(bokehKernel.rows())) / 2;
                                                        for (int i = 0; i < bokehKernel.rows(); ++i) {
                                                            for (int j = 0; j < bokehKernel.cols(); ++j) {
                                                                if (bokehKernel(i, j) == 1.f) {
                                                                    const float scale = 1.f / (std::sqrtf(2 * M_PI) * sigma);
                                                                    float distance = (i - center) * (i - center) + (j - center) * (j - center);
                                                                    float value = std::expf(-(distance * distance) / (2.f * sigma * sigma)) * scale;
                                                                    bokehKernel(i, j) = value;
                                                                }
                                                            }
                                                        }
                                                        float sum = bokehKernel.sum();
                                                        if (sum != 0.f) {
                                                            bokehKernel /= sum;
                                                        }
                                                        aire::Convolve2D convolve2D(bokehKernel);
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