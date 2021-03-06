#!/usr/bin/bash
rm -rf build
rm -f CMakeCache.txt
rm -f ./lib/armeabi-v7a/*

export PI_TOOLS_HOME=~/xtools/tools
mkdir build
cd build
cmake -D__CSI_HAS_OPENSSL__=1  -D__LINUX__=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../toolchains/raspberrypi.toolchain.x64.cmake .. 
make
cd ..


