cmake_minimum_required(VERSION 3.22.1)

project("aire")

add_library(${CMAKE_PROJECT_NAME} SHARED
        aire.cpp
        blur/BilateralBlur.cpp
        blur/BoxBlur.cpp
        blur/GaussBlur.cpp
        blur/MedianBlur.cpp
        blur/ShgStackBlur.cpp
        shift/TiltShift.cpp
        conversion/CopyUnaligned.cpp conversion/F32ToRGB1010102.cpp conversion/Rgb565.cpp conversion/Rgb1010102.cpp
        conversion/Rgb1010102toF16.cpp conversion/Rgba2Rgb.cpp conversion/Rgba8ToF16.cpp conversion/Rgba16bitCopy.cpp
        conversion/Rgba1010102toF32.cpp conversion/RgbaF16bitNBitU8.cpp conversion/RGBAlpha.cpp conversion/HalfFloats.cpp
        jni/AcquireBitmapPixels.cpp shift/Glitch.cpp halftone/Halftone.cpp color/ConvolveToneMapper.cpp
        scale/XScaler.cpp jni/BlurPipes.cpp jni/ShiftPipelines.cpp jni/Base.cpp
        jni/Pipelines.cpp algo/median/QuickSelect.cpp algo/median/Wirth.cpp base/Arithmetics.cpp
        base/Erosion.cpp shift/WindStagger.cpp blur/AnisotropicDiffusion.cpp effect/MarbleEffect.cpp
        jni/EffectsPipelines.cpp effect/OilEffect.cpp effect/CrystallizeEffect.cpp blur/PoissonBlur.cpp
        base/Grayscale.cpp base/Dilation.cpp base/Channels.cpp base/Threshold.cpp blur/TentBlur.cpp
        pipelines/RemoveShadows.cpp blur/FastBilateral.cpp color/Gamut.cpp base/Convolve1D.cpp
        effect/FractalGlassEffect.cpp effect/WaterEffect.cpp jni/ScalePipelines.cpp jni/ToneMappingPipelines.cpp
        effect/PerlinDistortion.cpp base/Vibrance.cpp algo/sleef-hwy.cpp conversion/yuv/YuvConverter.cpp
        jni/YuvPipelines.cpp pipelines/DehazeDarkChannel.cpp color/Adjustments.cpp
        base/Grain.cpp base/Sharpness.cpp base/Convolve2D.cpp base/LUT8.cpp
        hwy/aligned_allocator.cc hwy/nanobenchmark.cc hwy/per_target.cc hwy/print.cc hwy/targets.cc
        hwy/timer.cc
        base/Convolve1Db16.cpp algo/MedianCut.cpp vendor/spng/spng.c base/PNGEncoder.cpp base/RemapPalette.cpp
        algo/WuQuantizer.cpp)

add_library(libfft STATIC IMPORTED)
set_target_properties(${CMAKE_PROJECT_NAME} libfft PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libfftw3.a)

add_library(libzlibng STATIC IMPORTED)
set_target_properties(${CMAKE_PROJECT_NAME} libzlibng PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libz.a)

add_definitions(-DHWY_COMPILE_ONLY_STATIC -DJC_VORONOI_IMPLEMENTATION)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (ANDROID_ABI STREQUAL arm64-v8a)

endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -funroll-loops")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DHWY_COMPILE_ONLY_STATIC)
endif ()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/algo ${CMAKE_SOURCE_DIR}/conversion
        ${CMAKE_SOURCE_DIR}/eigen ${CMAKE_SOURCE_DIR}/eigen/Core ${CMAKE_SOURCE_DIR}/vendor)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        libfft
        libzlibng
        -ljnigraphics)