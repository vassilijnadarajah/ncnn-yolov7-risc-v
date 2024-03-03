# SPDX-License-Identifier: GPL-3.0-only
# (C) 2024 Vassilij Nadarajah, TU Berlin
# nadarajah@campus.tu-berlin.de

cmake_minimum_required(VERSION 3.16)
project(ncnn_yolov7_risc_v)

set(CMAKE_CXX_STANDARD 17)

option(RVV_ENABLED "Support for RISC-V vector instructions (RVV)" OFF)

if(C906)
    if (RVV_ENABLED)
        set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib_c906_rvv/cmake/ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
    else()
    set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib_c906/cmake/ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
    endif()
elseif(PI02)
    set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib_pi02/cmake/ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
else()
    set(ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib_apple_m1/cmake/ncnn" CACHE PATH "Directory that contains ncnnConfig.cmake")
endif()

find_package(ncnn REQUIRED)

add_executable(ncnn_yolov7_risc_v
        main.cpp
        src/yolov7.h
        src/yolov7.cpp
)

target_link_libraries(ncnn_yolov7_risc_v ncnn)
