# ncnn-yolov7-risc-v
A simple CMake project for performing object detection with yolov7 on the RISC-V Allwinner D1 with vector instructions (RVV). 

<p align="center">
    <img src="example_detection.png"  alt="Example detection with yolov7" title="Example detection with yolov7"/>
</p>

## How to install

Clone the repository:
```shell
git clone https://github.com/vassilijnadarajah/ncnn-yolov7-risc-v.git
cd ncnn-yolov7-risc-v
```

### Install for Allwinner D1 (RISC-V) with and without RVV support

Download the C906 toolchain from [XuanTie](https://xuantie.t-head.cn/community/download?id=4224193099938729984) and unpack it in the toolchain directory.
```shell
mkdir -p ${HOME}/toolchains
tar xvf Xuantie-900-gcc-linux-5.10.4-glibc-x86_64-V2.6.1-20220906.tar.gz -C ${HOME}/toolchains
```

Set the `RISCV_ROOT_PATH` environment for CMake to be able to find the compiler toolchain.
```shell
export RISCV_ROOT_PATH=${HOME}/toolchains/Xuantie-900-gcc-linux-5.10.4-glibc-x86_64-V2.6.1
```

Create a build directory and build the project with RVV enabled.
To disable RVV set `DRVV=OFF`.
```shell
mkdir -p build-c906
cd build-c906
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/c906-v226.toolchain.cmake DRVV_ENABLED=ON ..
cmake --build . -j 2
```

### Install for Raspberry Pi Zero 2 (Armv8) with and without NEON support
If host machine is arm64, cmake will automatically detect the compiler.
Otherwise download the GNU toolchain from [Arm](https://developer.arm.com/downloads/-/gnu-a) and unpack it in the toolchain directory.
```shell
mkdir -p ${HOME}/toolchains
tar xvf arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-linux-gnueabihf.tar.xz -C ${HOME}/toolchains
```
<!-- 
tar xvf gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf.tar.gz -C ${HOME}/toolchains
export ARM_ROOT_PATH=${HOME}/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf
-->

and set the `ARM_ROOT_PATH` environment for CMake to be able to find the compiler toolchain.
```shell
export ARM_ROOT_PATH=${HOME}/toolchains/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-linux-gnueabihf
```

Create a build directory and build the project with NEON enabled.
To disable NEON set `DNEON=OFF`.
```shell
mkdir -p build-pi02
cd build-pi02
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/pi02.toolchain.cmake DNEON=ON ..
cmake --build . -j 3
```


### Install for Raspberry Pi Zero (Armv6) 

Download the GNU toolchain for the Pi Zero from [SourceForge](https://sourceforge.net/projects/raspberry-pi-cross-compilers/) and unpack it in the toolchain directory.
```shell
mkdir -p ${HOME}/toolchains
tar xvf cross-gcc-10.3.0-pi_0-1.tar.gz -C ${HOME}/toolchains
```

and set the `ARM_ROOT_PATH` environment for CMake to be able to find the compiler toolchain.
```shell
export ARM_ROOT_PATH=${HOME}/toolchains/cross-pi-gcc-10.3.0-0
```

Create a build directory and build the project.
```shell
mkdir -p build-pi0
cd build-pi0
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/pi0.toolchain.cmake ..
cmake --build . -j 2
```