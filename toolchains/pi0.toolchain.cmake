# SPDX-License-Identifier: GPL-3.0-only
# (C) 2024 Vassilij Nadarajah, TU Berlin
# nadarajah@campus.tu-berlin.de

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv6)
set(PI0 True)

# Check for GNU toolchain environment
if(DEFINED ENV{ARM_ROOT_PATH})
    file(TO_CMAKE_PATH $ENV{ARM_ROOT_PATH} ARM_ROOT_PATH)
else()
    message(FATAL_ERROR "ARM_ROOT_PATH env must be defined")
endif()

set(CMAKE_C_COMPILER "${ARM_ROOT_PATH}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${ARM_ROOT_PATH}/bin/arm-linux-gnueabihf-g++")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# ARMv6 compiler flags
set(CMAKE_C_FLAGS "-march=armv6 -mfpu=vfp -mfloat-abi=hard -funsafe-math-optimizations")
set(CMAKE_CXX_FLAGS "-march=armv6 -mfpu=vfp -mfloat-abi=hard -funsafe-math-optimizations")

# cache flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "c++ flags")

# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/pi0.toolchain.cmake ..
