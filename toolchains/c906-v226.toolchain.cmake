set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
set(C906 True)

if(DEFINED ENV{RISCV_ROOT_PATH})
    file(TO_CMAKE_PATH $ENV{RISCV_ROOT_PATH} RISCV_ROOT_PATH)
else()
    message(FATAL_ERROR "RISCV_ROOT_PATH env must be defined")
endif()

set(CMAKE_C_COMPILER "${RISCV_ROOT_PATH}/bin/riscv64-unknown-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "${RISCV_ROOT_PATH}/bin/riscv64-unknown-linux-gnu-g++")

set(CMAKE_FIND_ROOT_PATH "${RISCV_ROOT_PATH}/riscv64-unknown-linux-gnu")

set(CMAKE_SYSROOT "${RISCV_ROOT_PATH}/sysroot")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

if (RVV_ENABLED)
    set(CMAKE_C_FLAGS "-march=rv64gcv0p7_zfh_xtheadc -mabi=lp64d -mtune=c906 -DC906=1 -static")
    set(CMAKE_CXX_FLAGS "-march=rv64gcv0p7_zfh_xtheadc -mabi=lp64d -mtune=c906 -DC906=1 -static")
else ()
    set(CMAKE_C_FLAGS "-march=rv64gc -mabi=lp64d -static")
    set(CMAKE_CXX_FLAGS "-march=rv64gc -mabi=lp64d -static")
endif()

# cache flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "c++ flags")

# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/c906-v226.toolchain.cmake DRVV_ENABLED=ON..
