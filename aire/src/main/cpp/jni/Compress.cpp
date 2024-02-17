//
// Created by Radzivon Bartoshyk on 16/02/2024.
//

#include <jni.h>
#include "JNIUtils.h"
#include "AcquireBitmapPixels.h"
#include "algo/WuQuantizer.h"
#include "base/RemapPalette.h"
#include "base/PNGEncoder.h"
#include "algo/MedianCut.h"
#include "conversion/RGBAlpha.h"
#include "MathUtils.hpp"
#include "base/JPEGEncoder.h"
#include "EigenUtils.h"

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_toPNGImpl(JNIEnv *env, jobject thiz,
                                                          jobject bitmap,
                                                          jint maxColors,
                                                          jint aireQuantize,
                                                          jint ditheringStrategy,
                                                          jint mappingStrategy,
                                                          jint compressionLevel) {
    try {
        if (maxColors < 2) {
            std::string msg("Max colors must be at least 2, but was received " + std::to_string(maxColors));
            throw AireError(msg);
        }

        if (compressionLevel < 0 || compressionLevel > 9) {
            std::string msg("Compression level is expected to in 0...9 but received: " + std::to_string(compressionLevel));
            throw AireError(msg);
        }
        aire::RemapDithering dithering = static_cast<aire::RemapDithering>(ditheringStrategy);
        AireQuantize quantize = static_cast<AireQuantize>(aireQuantize);
        aire::RemapMappingStrategy strategy = static_cast<aire::RemapMappingStrategy>(mappingStrategy);
        std::vector<uint8_t> compressedData;
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        AcquireBitmapPixels(env,
                            bitmap,
                            formats,
                            false,
                            [&compressedData, maxColors, quantize, dithering, strategy, compressionLevel](
                                    std::vector<uint8_t> &input, int stride,
                                    int width, int height,
                                    AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                if (fmt == APF_RGBA8888) {
                                    std::vector<Eigen::Vector4i> palette;
                                    uint32_t colors = maxColors;
                                    std::vector<uint8_t> original(input.size());

                                    aire::UnpremultiplyRGBA(input.data(), stride, original.data(), stride, width, height);

                                    switch (quantize) {
                                        case AIRE_QUANTIZE_MEDIAN_CUT: {
                                            aire::Palette cut(reinterpret_cast<uint32_t *>(original.data()), width * height);
                                            cut.medianCut(maxColors, [&palette](const aire::Cube &cube) {
                                                auto clr = cube.getAverageRGBA();
                                                palette.push_back(unpackRGBA(clr));
                                            });
                                        }
                                            break;
                                        case AIRE_QUANTIZE_XIAOLING_WU: {
                                            aire::WuQuantizer wuQuantizer(original.data(), stride, width, height);
                                            palette = wuQuantizer.quantizeImage(colors, 15, 15);
                                        }
                                            break;
                                    }

                                    aire::RemapPalette remapPalette(palette, original.data(), stride, width, height, dithering, strategy);
                                    if (maxColors > 255 || dithering != aire::Remap_Dither_Skip) {
                                        std::vector<uint8_t> remapped = remapPalette.remap();
                                        aire::PNGEncoder encoder(remapped.data(), stride, width, height);
                                        encoder.setCompressionLevel(compressionLevel);
                                        auto output = encoder.getPNGData();
                                        compressedData.resize(output.size());
                                        std::copy(output.begin(), output.end(), compressedData.begin());
                                    } else {
                                        std::vector<uint8_t> remapped = remapPalette.indexed();
                                        aire::PNGEncoder encoder(remapped.data(), stride, width, height);
                                        encoder.setCompressionLevel(compressionLevel);
                                        auto output = encoder.encode(palette);
                                        compressedData.resize(output.size());
                                        std::copy(output.begin(), output.end(), compressedData.begin());
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

        jbyteArray byteArray = env->NewByteArray((jsize) compressedData.size());
        char *memBuf = (char *) ((void *) compressedData.data());
        env->SetByteArrayRegion(byteArray, 0, (jint) compressedData.size(),
                                reinterpret_cast<const jbyte *>(memBuf));
        compressedData.clear();
        return byteArray;
    } catch (AireError &err) {
        std::string msg = err.what();
        throwException(env, msg);
        return nullptr;
    }
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_awxkee_aire_pipeline_BasePipelinesImpl_toJPEGImpl(JNIEnv *env, jobject thiz, jobject bitmap, jint quality) {
    try {
        if (quality < 0 || quality > 100) {
            std::string msg("Quality must be between 0...100 but received: " + std::to_string(quality));
            throw AireError(msg);
        }

        std::vector<uint8_t> compressedData;
        std::vector<AcquirePixelFormat> formats;
        formats.insert(formats.begin(), APF_RGBA8888);
        AcquireBitmapPixels(env,
                            bitmap,
                            formats,
                            false,
                            [&compressedData, quality](
                                    std::vector<uint8_t> &input, int stride,
                                    int width, int height,
                                    AcquirePixelFormat fmt) -> BuiltImagePresentation {
                                if (fmt == APF_RGBA8888) {
                                    std::vector<uint8_t> original(input.size());
                                    aire::UnpremultiplyRGBA(input.data(), stride, original.data(), stride, width, height);
                                    aire::JPEGEncoder encoder(original.data(), stride, width, height);
                                    encoder.setQuality(quality);
                                    auto output = encoder.encode();
                                    compressedData.resize(output.size());
                                    std::copy(output.begin(), output.end(), compressedData.begin());
                                }
                                return {
                                        .data = input,
                                        .stride = stride,
                                        .width = width,
                                        .height = height,
                                        .pixelFormat = fmt
                                };
                            });

        jbyteArray byteArray = env->NewByteArray((jsize) compressedData.size());
        char *memBuf = (char *) ((void *) compressedData.data());
        env->SetByteArrayRegion(byteArray, 0, (jint) compressedData.size(),
                                reinterpret_cast<const jbyte *>(memBuf));
        compressedData.clear();
        return byteArray;
    } catch (AireError &err) {
        std::string msg = err.what();
        throwException(env, msg);
        return nullptr;
    }
}