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

#pragma once

#include <jni.h>
#include <vector>
#include <iostream>
#include <functional>

enum AcquirePixelFormat {
    APF_RGBA8888,
    APF_565,
    APF_F16,
    APF_RGBA1010102
};

static std::string getPixelFormatName(AcquirePixelFormat px) {
    switch (px) {
        case APF_RGBA8888:
            return "APF_RGBA8888";
        case APF_565:
            return "APF_565";
        case APF_F16:
            return "APF_F16";
        case APF_RGBA1010102:
            return "APF_RGBA1010102";
    }
    return "APF_UNKNOWN";
}

static int getPixelSize(AcquirePixelFormat px) {
    switch (px) {
        case APF_RGBA8888:
            return sizeof(uint8_t);
        case APF_565:
            return sizeof(uint16_t);
        case APF_F16:
            return sizeof(uint16_t);
        case APF_RGBA1010102:
            return sizeof(uint8_t);
    }
    return 0;
}

static int getComponents(AcquirePixelFormat px) {
    switch (px) {
        case APF_RGBA8888:
            return 4;
        case APF_565:
            return 1;
        case APF_F16:
            return 4;
        case APF_RGBA1010102:
            return 4;
    }
    return 0;
}

static std::string getAndroidFormat(AcquirePixelFormat px) {
    switch (px) {
        case APF_RGBA8888:
            return "ARGB_8888";
        case APF_565:
            return "RGB_565";
        case APF_F16:
            return "RGBA_F16";
        case APF_RGBA1010102:
            return "RGBA_1010102";
    }
    return "";
}

struct BuiltImagePresentation {
    std::vector<uint8_t> data;
    int stride;
    int width;
    int height;
    AcquirePixelFormat pixelFormat;
};

jobject AcquireBitmapPixels(JNIEnv *env, jobject bitmap,
                         std::vector<AcquirePixelFormat> allowedFormats,
                         bool allowsMemoryAlignment,
                         std::function<BuiltImagePresentation(std::vector<uint8_t> &, int, int, int,
                                                              AcquirePixelFormat)> worker);