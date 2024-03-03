set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv7)
set(PI02 True)

option(NEON_ENABLED "NEON SIMD support" OFF)

set(CMAKE_C_COMPILER "/home/vassilij/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "/home/vassilij/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_FLAGS "-march=armv7-a -mfpu=neon-vfpv4 -funsafe-math-optimizations -mfp16-format=ieee")
set(CMAKE_CXX_FLAGS "-march=armv7-a -mfpu=neon-vfpv4 -funsafe-math-optimizations -mfp16-format=ieee")

if (NEON_ENABLED)
    set(CMAKE_C_FLAGS "-march=armv7-a -mfpu=neon-vfpv4 -funsafe-math-optimizations -mfp16-format=ieee")
    set(CMAKE_CXX_FLAGS "-march=armv7-a -mfpu=neon-vfpv4 -funsafe-math-optimizations -mfp16-format=ieee")
else()
    set(CMAKE_C_FLAGS "-march=armv7-a -mfpu=vfpv4 -mfloat-abi=hard)")
    set(CMAKE_CXX_FLAGS "-march=armv7-a -mfpu=vfpv4 -mfloat-abi=hard)")
endif()

# cache flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "c++ flags")

# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/pi02.toolchain.cmake -DNEON_ENABLED=ON ..