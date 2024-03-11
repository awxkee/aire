cmake_minimum_required(VERSION 3.22.1)

project("aireli")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(aireli SHARED AireJpegli.cpp JPEGEncoder.cpp ../hwy/aligned_allocator.cc
        ../hwy/nanobenchmark.cc ../hwy/per_target.cc ../hwy/print.cc ../hwy/targets.cc
        ../hwy/timer.cc)

add_library(libjpegli STATIC IMPORTED)
set_target_properties(aireli libjpegli PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libjpegli-static.a)

target_include_directories(aireli PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/jpegli)

target_link_libraries(aireli libjpegli)