#!/usr/bin/bash
rm -rf linux
mkdir linux
cd linux
cmake -D__LINUX__=1 ..
make
cd ..

