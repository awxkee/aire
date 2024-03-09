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

#include "AcquireBitmapPixels.h"
#include <android/bitmap.h>
#include "JNIUtils.h"
#include "Rgb1010102toF16.h"
#include "Rgb1010102.h"
#include "RgbaF16bitNBitU8.h"
#include "Rgb565.h"
#include "Rgba8ToF16.h"
#include "CopyUnaligned.h"

using namespace std;

int androidOSVersion() {
    return android_get_device_api_level();
}

jobject AcquireBitmapPixels(JNIEnv *env, jobject bitmap,
                            std::vector<AcquirePixelFormat> allowedFormats,
                            bool allowsMemoryAlignment,
                            std::function<BuiltImagePresentation(std::vector<uint8_t> &, int, int,
                                                                 int,
                                                                 AcquirePixelFormat)> worker) {
    try {
        int osVersion = androidOSVersion();
        if (osVersion < 26) {
            auto it = std::find_if(allowedFormats.begin(), allowedFormats.end(),
                                   [](const AcquirePixelFormat &format) {
                                       return format == APF_F16;
                                   });
            if (it != allowedFormats.end()) {
                allowedFormats.erase(it);
            }
        }
        if (osVersion < 33) {
            auto it = std::find_if(allowedFormats.begin(), allowedFormats.end(), [](const AcquirePixelFormat &format) {
                return format == APF_RGBA1010102;
            });
            if (it != allowedFormats.end()) {
                allowedFormats.erase(it);
            }
        }

        if (allowedFormats.empty()) {
            std::string err("Allowed formats must not be empty");
            throw AireError(err);
        }

        AndroidBitmapInfo info;
        if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
            std::string err("Cannot acquire bitmap info");
            throw AireError(err);
        }

        if (info.flags & ANDROID_BITMAP_FLAGS_IS_HARDWARE) {
            std::string exc = "Hardware bitmap is not supported by JXL Coder";
            throw AireError(exc);
        }

        if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 &&
            info.format != ANDROID_BITMAP_FORMAT_RGBA_F16 &&
            info.format != ANDROID_BITMAP_FORMAT_RGBA_1010102 &&
            info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
            string msg(
                    "Currently support encoding only RGBA_8888, RGBA_F16, RGBA_1010102, RBR_565 images pixel format");
            throw AireError(msg);
        }

        void *addr = nullptr;
        if (AndroidBitmap_lockPixels(env, bitmap, &addr) != 0) {
            std::string exc = "Cannot acquire bitmap pixels";
            throw AireError(exc);
        }

        vector<uint8_t> rgbaPixels(info.stride * info.height);
        std::copy(reinterpret_cast<uint8_t *>(addr), reinterpret_cast<uint8_t *>(addr) + info.stride * info.height, rgbaPixels.begin());

        if (AndroidBitmap_unlockPixels(env, bitmap) != 0) {
            string exc = "Unlocking pixels has failed";
            throw AireError(exc);
        }

        int imageStride = (int) info.stride;

        auto isAllowed1010102Lambda = [](const AcquirePixelFormat &format) {
            return format == APF_RGBA1010102;
        };
        auto sAllowed1010102Iterator = std::find_if(allowedFormats.begin(), allowedFormats.end(),
                                                    isAllowed1010102Lambda);
        auto is1010102Allowed = sAllowed1010102Iterator != allowedFormats.end();

        auto isAllowedF16Lambda = [](const AcquirePixelFormat &format) {
            return format == APF_F16;
        };
        auto sAllowedF16Iterator = std::find_if(allowedFormats.begin(), allowedFormats.end(),
                                                isAllowedF16Lambda);
        auto isF16Allowed = sAllowedF16Iterator != allowedFormats.end();

        auto isAllowed565Lambda = [](const AcquirePixelFormat &format) {
            return format == APF_565;
        };
        auto sAllowed565Iterator = std::find_if(allowedFormats.begin(), allowedFormats.end(),
                                                isAllowed565Lambda);
        auto is565Allowed = sAllowed565Iterator != allowedFormats.end();

        auto isAllowed888Lambda = [](const AcquirePixelFormat &format) {
            return format == APF_RGBA8888;
        };
        auto sAllowed888Iterator = std::find_if(allowedFormats.begin(), allowedFormats.end(),
                                                isAllowed888Lambda);
        auto is888Allowed = sAllowed888Iterator != allowedFormats.end();

        AcquirePixelFormat usingFormat = APF_RGBA8888;

        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_F16) {
            if (isF16Allowed) {
                usingFormat = APF_F16;
            } else {
                if (is1010102Allowed) {
                    imageStride = (int) info.width * 4 * (int) sizeof(uint8_t);
                    vector<uint8_t> halfFloatPixels(imageStride * info.height);
                    aire::F16ToRGBA1010102(reinterpret_cast<const uint16_t *>(rgbaPixels.data()),
                                           (int) info.stride,
                                           reinterpret_cast<uint8_t *>(halfFloatPixels.data()),
                                           (int) imageStride,
                                           (int) info.width,
                                           (int) info.height);
                    usingFormat = APF_RGBA1010102;
                    rgbaPixels = halfFloatPixels;
                } else if (is888Allowed) {
                    imageStride = (int) info.width * 4 * (int) sizeof(uint8_t);
                    vector<uint8_t> halfFloatPixels(imageStride * info.height);
                    aire::RGBAF16BitToNBitU8(reinterpret_cast<const uint16_t *>(rgbaPixels.data()),
                                             (int) info.stride,
                                             reinterpret_cast<uint8_t *>(halfFloatPixels.data()),
                                             (int) imageStride,
                                             (int) info.width,
                                             (int) info.height, 8, true);
                    usingFormat = APF_RGBA8888;
                    rgbaPixels = halfFloatPixels;
                } else if (is565Allowed) {
                    imageStride = (int) info.width * (int) sizeof(uint16_t);
                    vector<uint8_t> r888Pixels(imageStride * info.height);
                    aire::RGBAF16To565(reinterpret_cast<const uint16_t *>(rgbaPixels.data()),
                                       (int) info.stride,
                                       reinterpret_cast<uint16_t *>(r888Pixels.data()),
                                       (int) imageStride,
                                       (int) info.width,
                                       (int) info.height);
                    usingFormat = APF_RGBA8888;
                    rgbaPixels = r888Pixels;
                } else {
                    string ss = getPixelFormatName(APF_F16);
                    string exc = "Unknown " + ss + " conversion path";
                    throw AireError(exc);
                }
            }
        } else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_1010102) {
            if (!is1010102Allowed) {
                if (isF16Allowed) {
                    imageStride = (int) info.width * 4 * (int) sizeof(uint16_t);
                    vector<uint8_t> halfFloatPixels(imageStride * info.height);
                    aire::ConvertRGBA1010102toF16(
                            reinterpret_cast<const uint8_t *>(rgbaPixels.data()),
                            (int) info.stride,
                            reinterpret_cast<uint16_t *>(halfFloatPixels.data()),
                            (int) imageStride,
                            (int) info.width,
                            (int) info.height);
                    usingFormat = APF_F16;
                    rgbaPixels = halfFloatPixels;
                } else if (is888Allowed) {
                    imageStride = (int) info.width * 4 * (int) sizeof(uint8_t);
                    vector<uint8_t> r888Pixels(imageStride * info.height);
                    aire::RGBA1010102ToUnsigned(reinterpret_cast<const uint8_t *>(rgbaPixels.data()),
                                                (int) info.stride,
                                                reinterpret_cast<uint8_t *>(r888Pixels.data()),
                                                (int) imageStride,
                                                (int) info.width,
                                                (int) info.height, 8);
                    usingFormat = APF_RGBA8888;
                    rgbaPixels = r888Pixels;
                } else if (is565Allowed) {
                    imageStride = (int) info.width * (int) sizeof(uint16_t);
                    vector<uint8_t> r888Pixels(imageStride * info.height);
                    aire::RGBA1010102To565(reinterpret_cast<const uint8_t *>(rgbaPixels.data()),
                                           (int) info.stride,
                                           reinterpret_cast<uint16_t *>(r888Pixels.data()),
                                           (int) imageStride,
                                           (int) info.width,
                                           (int) info.height);
                    usingFormat = APF_565;
                    rgbaPixels = r888Pixels;
                } else {
                    string ss = getPixelFormatName(APF_RGBA1010102);
                    string exc = "Unknown " + ss + " conversion path";
                    throw AireError(exc);
                }
            } else {
                usingFormat = APF_RGBA1010102;
            }
        } else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            if (is888Allowed) {
                usingFormat = APF_RGBA8888;
            } else {
                if (isF16Allowed) {
                    imageStride = (int) info.width * 4 * (int) sizeof(uint16_t);
                    vector<uint8_t> halfFloatPixels(imageStride * info.height);
                    aire::Rgba8ToF16(reinterpret_cast<const uint8_t *>(rgbaPixels.data()),
                                     (int) info.stride,
                                     reinterpret_cast<uint16_t *>(halfFloatPixels.data()),
                                     (int) imageStride,
                                     (int) info.width,
                                     (int) info.height, 8, true);
                    usingFormat = APF_F16;
                    rgbaPixels = halfFloatPixels;
                } else if (is1010102Allowed) {
                    imageStride = (int) info.width * 4 * (int) sizeof(uint8_t);
                    vector<uint8_t> halfFloatPixels(imageStride * info.height);
                    aire::Rgba8ToRGBA1010102(reinterpret_cast<const uint8_t *>(rgbaPixels.data()),
                                             (int) info.stride,
                                             reinterpret_cast<uint8_t *>(halfFloatPixels.data()),
                                             (int) imageStride,
                                             (int) info.width,
                                             (int) info.height, true);
                    usingFormat = APF_RGBA1010102;
                    rgbaPixels = halfFloatPixels;
                } else if (is565Allowed) {
                    imageStride = (int) info.width * (int) sizeof(uint16_t);
                    vector<uint8_t> halfFloatPixels(imageStride * info.height);
                    aire::Rgba8To565(reinterpret_cast<const uint8_t *>(rgbaPixels.data()),
                                     (int) info.stride,
                                     reinterpret_cast<uint16_t *>(halfFloatPixels.data()),
                                     (int) imageStride,
                                     (int) info.width,
                                     (int) info.height, 8, true);
                    usingFormat = APF_RGBA1010102;
                    rgbaPixels = halfFloatPixels;
                } else {
                    string ss = getPixelFormatName(APF_RGBA8888);
                    string exc = "Unknown " + ss + " conversion path";
                    throw AireError(exc);
                }
            }
        } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
            if (is565Allowed) {
                usingFormat = APF_565;
            } else {
                if (isF16Allowed) {
                    int newStride = (int) info.width * 4 * (int) sizeof(uint16_t);
                    std::vector<uint8_t> rgba8888Pixels(newStride * info.height);
                    aire::Rgb565ToF16(reinterpret_cast<const uint16_t *>(rgbaPixels.data()),
                                      (int) info.stride,
                                      reinterpret_cast<uint16_t *>(rgba8888Pixels.data()),
                                      newStride,
                                      (int) info.width, (int) info.height);

                    imageStride = newStride;
                    rgbaPixels = rgba8888Pixels;
                    usingFormat = APF_F16;
                } else if (is1010102Allowed) {
                    int newStride = (int) info.width * 4 * (int) sizeof(uint8_t);
                    std::vector<uint8_t> rgba8888Pixels(newStride * info.height);
                    aire::Rgb565ToRGBA1010102(reinterpret_cast<const uint16_t *>(rgbaPixels.data()),
                                              (int) info.stride,
                                              rgba8888Pixels.data(), newStride,
                                              (int) info.width, (int) info.height);
                    usingFormat = APF_RGBA1010102;
                    imageStride = newStride;
                    rgbaPixels = rgba8888Pixels;
                } else if (is888Allowed) {
                    int newStride = (int) info.width * 4 * (int) sizeof(uint8_t);
                    std::vector<uint8_t> rgba8888Pixels(newStride * info.height);
                    aire::Rgb565ToUnsigned8(reinterpret_cast<const uint16_t *>(rgbaPixels.data()),
                                            (int) info.stride,
                                            rgba8888Pixels.data(), newStride,
                                            (int) info.width, (int) info.height, 8, 255);
                    usingFormat = APF_RGBA8888;
                    imageStride = newStride;
                    rgbaPixels = rgba8888Pixels;
                } else {
                    string ss = getPixelFormatName(APF_565);
                    string exc = "Unknown " + ss + " conversion path";
                    throw AireError(exc);
                }
            }
        }

        int pixelSize = getPixelSize(usingFormat);
        int components = getComponents(usingFormat);

        if (!allowsMemoryAlignment && imageStride != info.width * components * pixelSize) {
            std::vector<uint8_t> newPixels(
                    info.width * components * pixelSize * (int) info.height);
            rgbaPixels = newPixels;
            imageStride = info.width * components * pixelSize;
        }

        auto result = worker(rgbaPixels, imageStride, info.width, info.height, usingFormat);

        std::string bitmapPixelConfig = getAndroidFormat(result.pixelFormat);
        jclass bitmapConfig = env->FindClass("android/graphics/Bitmap$Config");
        jfieldID rgba8888FieldID = env->GetStaticFieldID(bitmapConfig,
                                                         bitmapPixelConfig.c_str(),
                                                         "Landroid/graphics/Bitmap$Config;");
        jobject rgba8888Obj = env->GetStaticObjectField(bitmapConfig, rgba8888FieldID);

        jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
        jmethodID createBitmapMethodID = env->GetStaticMethodID(bitmapClass, "createBitmap",
                                                                "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        jobject bitmapObj = env->CallStaticObjectMethod(bitmapClass, createBitmapMethodID,
                                                        static_cast<jint>(result.width),
                                                        static_cast<jint>(result.height),
                                                        rgba8888Obj);


        if (AndroidBitmap_getInfo(env, bitmapObj, &info) < 0) {
            std::string exc = "Cannot get destination bitmap info";
            throw AireError(exc);
        }

        addr = nullptr;

        if (AndroidBitmap_lockPixels(env, bitmapObj, &addr) != 0) {
            std::string exc = "Cannot acquire destination bitmap pixels";
            throw AireError(exc);
        }

        aire::CopyUnaligned(reinterpret_cast<const uint8_t *>(result.data.data()),
                            result.stride,
                            reinterpret_cast<uint8_t *>(addr), (int) info.stride,
                            (int) info.width * getComponents(result.pixelFormat),
                            (int) info.height, getPixelSize(result.pixelFormat));

        if (AndroidBitmap_unlockPixels(env, bitmapObj) != 0) {
            std::string exc = "Cannot unlock destination bitmap pixels";
            throw AireError(exc);
        }

        return bitmapObj;
    } catch (std::bad_alloc &err) {
        throw AireError(err.what());
    }
}