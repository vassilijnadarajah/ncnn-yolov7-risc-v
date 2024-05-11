# SPDX-License-Identifier: GPL-3.0-only
# (C) 2024 Vassilij Nadarajah, TU Berlin
# nadarajah@campus.tu-berlin.de

cmake_minimum_required(VERSION 3.16)
project(ncnn_yolo_nvh)

set(CMAKE_CXX_STANDARD 17)

option(RVV "Support for RISC-V vector instructions (RVV)" ON)
option(NEON "Support for ARM NEON SIMD" ON)

if(C906)
    if (RVV)
        set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib_c906_rvv/cmake/ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
    else()
        set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib_c906/cmake/ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
    endif()
elseif(PI02)
    if (NEON)
        set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib_pi02_neon/cmake/ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
    else()
        set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib_pi02/cmake/ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
    endif()
elseif(PI0)
    set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib_pi0/cmake/ncnn")
else()
    set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib_apple_m1/cmake/ncnn")
endif()

find_package(ncnn REQUIRED)

add_executable(ncnn_yolo_nvh
        src/main.cpp
        src/YoloV7.h
        src/YoloV7.cpp
        )

target_link_libraries(ncnn_yolo_nvh ncnn)