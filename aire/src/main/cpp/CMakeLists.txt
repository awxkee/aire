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
        jni/AcquireBitmapPixels.cpp shift/Glitch.cpp halftone/Halftone.cpp
        scale/XScaler.cpp jni/blur/BlurPipes.cpp jni/shift/ShiftPipelines.cpp jni/base/Base.cpp
        base/Grayscale.cpp base/Dilation.cpp base/Channels.cpp base/Threshold.cpp blur/TentBlur.cpp)

add_library(libhwy STATIC IMPORTED)

set_target_properties(${CMAKE_PROJECT_NAME} libhwy PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libhwy.a)

add_definitions(-DHWY_COMPILE_ONLY_STATIC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -funroll-loops")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DHWY_COMPILE_ONLY_STATIC)
endif ()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/algo ${CMAKE_SOURCE_DIR}/conversion)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        libhwy
        log
        -ljnigraphics)