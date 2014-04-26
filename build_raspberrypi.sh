#!/usr/bin/bash
rm -rf raspberrypi_debug
rm -rf raspberrypi_release
rm -f CMakeCache.txt
rm -f ./lib/armeabi-v7a/*

export PI_TOOLS_HOME=$PWD/../tools
mkdir raspberrypi_release
cd raspberrypi_release
cmake -D__LINUX__=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../toolchains/raspberrypi.toolchain.x64.cmake .. 
make
cd ..


