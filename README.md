csi-http
========

async http client and server library written in C++11, builtin support for apache avro
- syncronous  client based on libcurl and boost ASIO
- asyncronous client based on libcurl and boost ASIO
- asyncronous high performance server based on Joyents http_parser and boost ASIO
- generic GET, PUT, POST
- REST calls using avro binary encoded payloads
- REST calls using json encoded payloads (with optional json-spirit library)
- HTTP 1.1, specifically supports connection:keep-alive
- tentative support for HTTPS using OpenSSL
 
Missing:
- no support for avro rpc (REST calls with avro payload supported)

Platforms:
 - Windows, Visual studio 2013
 - Linux, GCC
 - Raspberry Pi, GCC

Ubuntu 14:

Install build tools
```
sudo apt-get install -y automake git wget cmake unzip build-essential g++ python-dev autotools-dev libicu-dev zlib1g-dev openssl libssl-dev libcurl4-openssl-dev libbz2-dev libcurl3 libboost-all-dev

```

Get nessessary dependencies and build
```
mkdir source
cd source
git clone https://github.com/bitbouncer/csi-build-scripts.git
bash csi-build-scripts/ubuntu14_setup.sh
```

Raspberry Pi - cross compiling on ubuntu14/64

Install build tools
```
sudo apt-get install cmake wget unzip cmake wget wput libpcre3 libpcre3-dev build-essential git

mkdir -p ~/xtools
cd ~/xtools
echo "export PATH=~/xtools/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin:\$PATH" >> ~/.bashrc
source ~/.bashrc
git clone https://github.com/raspberrypi/tools.git --depth 1
```
Get nessessary dependencies 
```
export BOOST_VERSION=1_55_0
export BOOST_VERSION_DOTTED=1.55.0
export AVRO_VERSION=1.7.7
export CURL_VERSION=7.35.0
export ZLIB_VERSION=1.2.8
export BZLIB2_VERSION=1.0.6
export OPEN_SSL_VERSION=1.0.1g
export JOYENT_HTTP_VERSION=2.3

mkdir -p ~/source/raspberrypi
cd ~/source/raspberrypi

git clone https://github.com/bitbouncer/csi-http
git clone https://github.com/bitbouncer/json-spirit

wget http://curl.haxx.se/download/curl-$CURL_VERSION.tar.bz2 -Ocurl-$CURL_VERSION.tar.bz2
tar xvf curl-$CURL_VERSION.tar.bz2

wget http://sourceforge.net/projects/boost/files/boost/$BOOST_VERSION_DOTTED/boost_$BOOST_VERSION.tar.gz/download -Oboost_$BOOST_VERSION.tar.gz
tar xvf boost_$BOOST_VERSION.tar.gz

wget http://zlib.net/zlib-$ZLIB_VERSION.tar.gz
tar xvf zlib-$ZLIB_VERSION.tar.gz

wget http://www.bzip.org/$BZLIB2_VERSION/bzip2-$BZLIB2_VERSION.tar.gz
tar xvf bzip2-$BZLIB2_VERSION.tar.gz

wget  http://www.openssl.org/source/openssl-$OPEN_SSL_VERSION.tar.gz -Oopenssl-$OPEN_SSL_VERSION.tar.gz
gzip -d openssl-$OPEN_SSL_VERSION.tar.gz
tar -xvf openssl-$OPEN_SSL_VERSION.tar

wget ftp://ftp.sunet.se/pub/www/servers/apache/dist/avro/avro-$AVRO_VERSION/cpp/avro-cpp-$AVRO_VERSION.tar.gz
tar xvf avro-cpp-$AVRO_VERSION.tar.gz

wget https://github.com/joyent/http-parser/archive/v$JOYENT_HTTP_VERSION.tar.gz -Ohttp_parser-v$JOYENT_HTTP_VERSION.tar.gz
gzip -d http_parser-v$JOYENT_HTTP_VERSION.tar.gz
tar -xvf http_parser-v$JOYENT_HTTP_VERSION.tar
```

Build
```
export BOOST_VERSION=1_55_0
export BOOST_VERSION_DOTTED=1.55.0
export AVRO_VERSION=1.7.7
export CURL_VERSION=7.35.0
export ZLIB_VERSION=1.2.8
export BZLIB2_VERSION=1.0.6
export OPEN_SSL_VERSION=1.0.1g
export JOYENT_HTTP_VERSION=2.3

cd boost_$BOOST_VERSION
echo "using gcc : arm : arm-linux-gnueabihf-g++ ;" >> tools/build/v2/user-config.jam
./bootstrap.sh
./b2 -j 5 -s ZLIB_SOURCE=$PWD/../zlib-$ZLIB_VERSION  -s BZIP2_SOURCE=$PWD/../bzip2-$BZLIB2_VERSION toolset=gcc-arm
cd ..

#build curl
export CFLAGS='-O2 -march=armv6j -mfpu=vfp -mfloat-abi=hard'
export CC=arm-linux-gnueabihf-gcc

cd curl-$CURL_VERSION
./configure --host=arm-linux-gnueabihf --disable-shared
make
cd ..

#build openssl
cd openssl-$OPEN_SSL_VERSION
export CFLAGS='-Os -march=armv6j -mfpu=vfp -mfloat-abi=hard'
export CC=arm-linux-gnueabihf-gcc
./Configure dist threads -D_REENTRANT no-shared
sed -i 's/ -O/ -Os/g' Makefile
make
cd ..

cd avro-cpp-$AVRO_VERSION
export BOOST_ROOT=$PWD/../boost_$BOOST_VERSION 
export Boost_INCLUDE_DIR=$PWD/../boost_$BOOST_VERSION/boost
export PI_TOOLS_HOME=~/xtools/tools
rm -rf avro
rm -rf build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../csi-http/toolchains/raspberrypi.toolchain.x64.cmake ..
make
cd ..
mkdir avro
cp -r api/*.* avro
cd ..

cd json-spirit
export BOOST_ROOT=$PWD/../boost_$BOOST_VERSION 
export Boost_INCLUDE_DIR=$PWD/../boost_$BOOST_VERSION/boost
export PI_TOOLS_HOME=~/xtools/tools
rm -rf avro
rm -rf build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../csi-http/toolchains/raspberrypi.toolchain.x64.cmake ..
make
cd ..
cd ..

#zlib & bzip2 needs to be there for boost iostreams to compile but since were not using it at the moment - skip this

cd csi-http
bash build_raspberrypi.sh
cd ..
```

Windows 64 - Visual Studio 12

Get nessessary dependencies 
```
Install nasm, git and active perl, make sure active perl is before git in PATH

set VISUALSTUDIO_VERSION=12.0
set VISUALSTUDIO_VERSION_MAJOR=12
set BOOST_VERSION=1_57_0
set BOOST_VERSION_DOTTED=1.57.0
set OPEN_SSL_VERSION=openssl-1.0.1j
set CURL_VERSION=7.39.0
set ZLIB_VERSION=1.2.8
set AVRO_VERSION=1.7.7
set LIBEVENT_VERSION=2.0.21
set PTHREAD_VERSION=2-9-1
set JOYENT_HTTP_VERSION=2.3

call "C:\Program Files (x86)\Microsoft Visual Studio %VISUALSTUDIO_VERSION%\VC\vcvarsall.bat" amd64

wget ftp://ftp.sunet.se/pub/www/servers/apache/dist/avro/avro-%AVRO_VERSION%/cpp/avro-cpp-%AVRO_VERSION%.tar.gz
tar xvf avro-cpp-%AVRO_VERSION%.tar.gz

wget http://zlib.net/zlib-%ZLIB_VERSION%.tar.gz
tar xvf zlib-%ZLIB_VERSION%.tar.gz

wget http://sourceforge.net/projects/boost/files/boost/%BOOST_VERSION_DOTTED%/boost_%BOOST_VERSION%.tar.gz/download -Oboost_%BOOST_VERSION%.tar.gz
tar xf boost_%BOOST_VERSION%.tar.gz

#until boost 1.57 when this was supposed to be included
git clone https://github.com/boostorg/endian.git
#back two thing out of trunk to compile under boost < 1.57
sed -i "s:<boost/predef/detail/endian_compat.h>:<boost/detail/endian.hpp>:" endian/include/boost/endian/types.hpp
sed -i "s:<boost/predef/detail/endian_compat.h>:<boost/detail/endian.hpp>:" endian/include/boost/endian/conversion.hpp


git clone https://github.com/bitbouncer/csi-build-scripts.git
git clone https://github.com/bitbouncer/csi-http.git
git clone https://github.com/bitbouncer/json-spirit
wget --no-check-certificate https://github.com/joyent/http-parser/archive/v%JOYENT_HTTP_VERSION%.tar.gz -Ohttp_parser-v%JOYENT_HTTP_VERSION%.tar.gz
tar -xvf http_parser-v%JOYENT_HTTP_VERSION%.tar.gz


wget  http://www.openssl.org/source/%OPEN_SSL_VERSION%.tar.gz 
tar xvf %OPEN_SSL_VERSION%.tar.gz

wget http://curl.haxx.se/download/curl-%CURL_VERSION%.tar.gz
tar xvf curl-%CURL_VERSION%.tar.gz


wget --no-check-certificate https://github.com/libevent/libevent/archive/release-%LIBEVENT_VERSION%-stable.tar.gz -Olibevent-%LIBEVENT_VERSION%-stable.tar.gz
tar xvf libevent-%LIBEVENT_VERSION%-stable.tar.gz

wget ftp://sourceware.org/pub/pthreads-win32/pthreads-w32-%PTHREAD_VERSION%-release.tar.gz
tar xvf pthreads-w32-%PTHREAD_VERSION%-release.tar.gz


@ECHO BUILDING OPEN_SSL
cd %OPEN_SSL_VERSION%
perl Configure VC-WIN64A --prefix=/OpenSSL-Win64
ms\do_win64a
nmake -f ms\nt.mak
cd ..


@ECHO BUILDING LIBCURL
cd curl-%CURL_VERSION%

rmdir /s /q builds
rmdir /s /q libs

mkdir libs
mkdir libs\x64
mkdir libs\x64\Debug
mkdir libs\x64\Release
mkdir libs\win32
mkdir libs\win32\Debug
mkdir libs\win32\Release

cd winbuild
SET INCLUDE=%INCLUDE%;..\..\%OPEN_SSL_VERSION%\include;..\include;..\..\%OPEN_SSL_VERSION%\include\openssl
nmake /f makefile.vc mode=static VC=12 WITH_SSL=no ENABLE_SSPI=no ENABLE_WINSSL=no ENABLE_IDN=no DEBUG=yes MACHINE=x64
nmake /f makefile.vc mode=static VC=12 WITH_SSL=no ENABLE_SSPI=no ENABLE_WINSSL=no ENABLE_IDN=no DEBUG=no MACHINE=x64
cd ..

copy builds\libcurl-vc%VISUALSTUDIO_VERSION_MAJOR%-x64-debug-static-ipv6\lib\libcurl_a_debug.lib libs\x64\Debug\libcurl.lib 
copy builds\libcurl-vc%VISUALSTUDIO_VERSION_MAJOR%-x64-release-static-ipv6\lib\libcurl_a.lib libs\x64\Release\libcurl.lib 
cd ..
@ECHO DONE WITH CURL


@ECHO BUILDING ZLIB
cd zlib-%ZLIB_VERSION%
nmake -f win32/Makefile.msc
cd ..


@ECHO BUILDING LIBEVENT
cd libevent-%LIBEVENT_VERSION%-stable
#./configure --disable-shared
#./configure
#make
#cd ..

@ECHO BUILDING PTHREADS
cd pthreads-w32-%PTHREAD_VERSION%-release
#nmake clean VC-static-debug
#nmake clean VC-static
#test
nmake clean VC
nmake clean VC-debug
cd ..

@ECHO BUILDING BOOST
cd boost_%BOOST_VERSION%
rmdir /s /q bin.v2
call "C:\Program Files (x86)\Microsoft Visual Studio %VISUALSTUDIO_VERSION%\VC\vcvarsall.bat" amd64
call bootstrap.bat
b2 -j 4 -toolset=msvc-%VISUALSTUDIO_VERSION% address-model=64 --build-type=complete --stagedir=lib\x64 stage -s ZLIB_SOURCE=%CD%\..\zlib-%ZLIB_VERSION%
rmdir /s /q bin.v2

call "C:\Program Files (x86)\Microsoft Visual Studio %VISUALSTUDIO_VERSION%\VC\vcvarsall.bat" x86
call bootstrap.bat
b2 -j 4 -toolset=msvc-%VISUALSTUDIO_VERSION% address-model=32 --build-type=complete --stagedir=lib\win32 stage -s ZLIB_SOURCE=%CD%\..\zlib-%ZLIB_VERSION%
rmdir /s /q bin.v2
call "C:\Program Files (x86)\Microsoft Visual Studio %VISUALSTUDIO_VERSION%\VC\vcvarsall.bat" amd64
cd ..


@ECHO BUILDING AVRO
cd avro-cpp-%AVRO_VERSION%
SET BOOST_LIBRARYDIR=%CD%/../boost_%BOOST_VERSION%/lib/x64/lib
SET BOOST_ROOT=%CD%/../boost_%BOOST_VERSION%
SET Boost_INCLUDE_DIR=%CD%/../boost_%BOOST_VERSION%/boost
rmdir /s /q avro
rmdir /s /q build64
mkdir build64
cd build64
cmake -G "Visual Studio 12 Win64" ..
msbuild ALL_BUILD.vcxproj /p:Configuration=Debug /p:Platform=x64
msbuild ALL_BUILD.vcxproj /p:Configuration=Release /p:Platform=x64
cd ..
mkdir avro
cp -r api/*.* avro
cd ..

@ECHO BUILDING JSON-SPIRIT
cd json-spirit
rmdir /S /Q bin
rmdir /S /Q lib
rmdir /S /Q win_build32
rmdir /S /Q win_build64
set BOOST_ROOT=%CD%\..\boost_1_57_0
mkdir win_build64 
cd win_build64
cmake -G "Visual Studio 12 Win64" ..
msbuild ALL_BUILD.vcxproj /p:Configuration=Debug /p:Platform=x64
msbuild ALL_BUILD.vcxproj /p:Configuration=Debug /p:Platform=x64
cd ..
cd ..


cd csi-http
git pull
call build_windows.bat
cd ..
```

License:
- Boost Software License, Version 1.0.











