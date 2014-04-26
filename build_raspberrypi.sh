#!/usr/bin/bash
rm -rf raspberrypi_debug
rm -rf raspberrypi_release
rm -f CMakeCache.txt
rm -f ./lib/armeabi-v7a/*

cd raspberrypi_release
cmake -D__LINUX__=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=./toochains/raspberrypi.toolchain.cmake .. 
make
cd ..


