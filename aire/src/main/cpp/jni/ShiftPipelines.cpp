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

#include "JNIUtils.h"
#include "AcquireBitmapPixels.h"
#include "shift/TiltShift.h"
#include "blur/GaussBlur.h"
#include "shift/Glitch.h"
#include "shift/WindStagger.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_ShiftPipelineImpl_tiltShiftImpl(JNIEnv *env, jobject thiz,
                                                              jobject bitmap, jint radius,
                                                              jfloat sigma, jfloat anchorX,
                                                              jfloat anchorY, jfloat tiltRadius) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [radius, sigma, anchorX, anchorY, tiltRadius](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    std::vector<uint8_t> blurred(input.size());
                                                    std::copy(input.begin(), input.end(), blurred.begin());
                                                    std::vector<uint8_t> output(input.size());
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::gaussBlurU8(blurred.data(),
                                                                          stride, width, height,
                                                                          radius, sigma);
                                                        aire::tiltShift(output.data(),
                                                                        input.data(),
                                                                        blurred,
                                                                        stride, width,
                                                                        height,
                                                                        anchorX, anchorY,
                                                                        tiltRadius);
                                                    }
                                                    blurred.clear();
                                                    return {
                                                            .data = output,
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
Java_com_awxkee_aire_pipeline_ShiftPipelineImpl_glitchImpl(JNIEnv *env, jobject thiz,
                                                           jobject bitmap, jfloat shiftX,
                                                           jfloat shiftY, jfloat corruptionSize,
                                                           jint corruptions, jfloat cShiftX,
                                                           jfloat cShiftY) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [shiftX, shiftY, corruptionSize, corruptions, cShiftX, cShiftY](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::glitchEffect(input.data(),
                                                                           stride, width,
                                                                           height, shiftX, shiftY,
                                                                           corruptionSize, corruptions, cShiftX, cShiftY);
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
Java_com_awxkee_aire_pipeline_ShiftPipelineImpl_horizontalWindStaggerImpl(JNIEnv *env, jobject thiz,
                                                                          jobject bitmap,
                                                                          jfloat windStrength, jint streamsCount,
                                                                          jint clearColor) {
    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                false,
                                                [windStrength, streamsCount, clearColor](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    if (fmt == APF_RGBA8888) {
                                                        std::vector<uint8_t> output(stride * height);
                                                        aire::horizontalWindStagger(output.data(),
                                                                                    input.data(),
                                                                                    stride, width, height,
                                                                                    windStrength, streamsCount,
                                                                                    static_cast<uint32_t>(clearColor));
                                                        input = std::move(output);
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
Java_com_awxkee_aire_pipeline_ShiftPipelineImpl_horizontalTiltShiftImpl(JNIEnv *env, jobject thiz, jobject bitmap, jint radius, jfloat sigma,
                                                                        jfloat anchorX, jfloat anchorY,
                                                                        jfloat tiltRadius, jfloat angle) {

    try {
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        jobject newBitmap = AcquireBitmapPixels(env,
                                                bitmap,
                                                formats,
                                                true,
                                                [&](
                                                        std::vector<uint8_t> &input, int stride,
                                                        int width, int height,
                                                        AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                                    std::vector<uint8_t> blurred(input.size());
                                                    std::copy(input.begin(), input.end(),
                                                              blurred.begin());
                                                    std::vector<uint8_t> output(input.size());
                                                    if (fmt == APF_RGBA8888) {
                                                        aire::gaussBlurU8(blurred.data(),
                                                                          stride, width, height,
                                                                          radius, sigma);
                                                        aire::horizontalTiltShift(output.data(),
                                                                                  input.data(),
                                                                                  blurred,
                                                                                  stride, width,
                                                                                  height,
                                                                                  anchorX,
                                                                                  anchorY,
                                                                                  tiltRadius,
                                                                                  angle);
                                                    }
                                                    blurred.clear();
                                                    return {
                                                            .data = output,
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