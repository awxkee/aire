/*
 *
 *  * MIT License
 *  *
 *  * Copyright (c) 2024 Radzivon Bartoshyk
 *  * aire [https://github.com/awxkee/aire]
 *  *
 *  * Created by Radzivon Bartoshyk on 05/02/24, 6:13 PM
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
#include "conversion/yuv/YuvConverter.h"
#include <vector>
#include <string>
#include "JNIUtils.h"

using namespace std;

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_YuvPipelinesImpl_Yuv420nV21ToRGBA(JNIEnv *env, jobject thiz, jobject dstBuffer,
                                                                jobject yBuffer,
                                                                jint yStride, jobject uvBuffer,
                                                                jint uvStride, jint width, jint height) {
    try {
        auto yBufferAddress = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(yBuffer));
        int yLength = (int) env->GetDirectBufferCapacity(yBuffer);
        if (!yBufferAddress || yLength <= 0) {
            std::string errorString = "Only direct byte buffers are supported";
            throwException(env, errorString);
            return nullptr;
        }
        auto uvBufferAddress = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(uvBuffer));
        int uvLength = (int) env->GetDirectBufferCapacity(uvBuffer);
        if (!uvBufferAddress || uvLength <= 0) {
            std::string errorString = "Only direct byte buffers are supported";
            throwException(env, errorString);
            return nullptr;
        }
        int rgbaStride = (int) sizeof(uint8_t) * 4 * width;
        vector<uint8_t> rgbaBuffer(rgbaStride * height);
        aire::NV21ToRGBA(rgbaBuffer.data(), rgbaStride, width, height, yBufferAddress, yStride,
                         uvBufferAddress, uvStride);
        auto dstBufferAddress = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(dstBuffer));
        int dstLength = (int) env->GetDirectBufferCapacity(dstBuffer);
        if (dstBufferAddress == nullptr || (dstLength == -1 || dstLength != rgbaStride * height)) {
            std::string exception = "Not enough memory to decode this image";
            throwException(env, exception);
            return static_cast<jobject>(nullptr);
        }
        copy(rgbaBuffer.begin(), rgbaBuffer.end(), dstBufferAddress);
        return dstBuffer;
    } catch (std::bad_alloc &err) {
        std::string exception = "Not enough memory to decode this image";
        throwException(env, exception);
        return static_cast<jobject>(nullptr);
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_awxkee_aire_pipeline_YuvPipelinesImpl_Yuv420nV21ToBGRImpl(JNIEnv *env, jobject thiz, jobject dstBuffer,
                                                                   jobject yBuffer, jint yStride, jobject uvBuffer,
                                                                   jint uvStride, jint width, jint height) {
    try {
        auto yBufferAddress = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(yBuffer));
        int yLength = (int) env->GetDirectBufferCapacity(yBuffer);
        if (!yBufferAddress || yLength <= 0) {
            std::string errorString = "Only direct byte buffers are supported";
            throwException(env, errorString);
            return nullptr;
        }
        auto uvBufferAddress = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(uvBuffer));
        int uvLength = (int) env->GetDirectBufferCapacity(uvBuffer);
        if (!uvBufferAddress || uvLength <= 0) {
            std::string errorString = "Only direct byte buffers are supported";
            throwException(env, errorString);
            return nullptr;
        }
        int rgbaStride = (int) sizeof(uint8_t) * 3 * width;
        vector<uint8_t> rgbaBuffer(rgbaStride * height);
        aire::NV21ToBGR(rgbaBuffer.data(), rgbaStride, width, height, yBufferAddress, yStride,
                        uvBufferAddress, uvStride);
        auto dstBufferAddress = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(dstBuffer));
        int dstLength = (int) env->GetDirectBufferCapacity(dstBuffer);
        if (dstBufferAddress == nullptr || (dstLength == -1 || dstLength != rgbaStride * height)) {
            std::string exception = "Not enough memory to decode this image";
            throwException(env, exception);
            return static_cast<jobject>(nullptr);
        }
        copy(rgbaBuffer.begin(), rgbaBuffer.end(), dstBufferAddress);
        return dstBuffer;
    } catch (std::bad_alloc &err) {
        std::string exception = "Not enough memory to decode this image";
        throwException(env, exception);
        return static_cast<jobject>(nullptr);
    }
}