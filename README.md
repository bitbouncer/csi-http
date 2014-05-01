csi-http
========


avro aware http client and server library written in C++ (11) 

- syncronous  client based on libcurl and boost ASIO
- asyncronous client based on libcurl and boost ASIO
- asyncronous high performance server based on Joyents http_parser and boost ASIO
- generic GET, PUT, POST
- REST calls using avro binary encoded payloads
- HTTP 1.1, specifically supports connection:keep-alive 
 
Missing:
- no support for avro rpc

Platforms:
 - Windows, Visual studio 2013
 - Linux, GCC
 - Raspberry Pi, GCC

Setup instructions can be found in the doc directory.

Ubuntu:

Install build tools
```
sudo apt-get install git wget cmake unzip emacs build-essential g++ python-dev autotools-dev libicu-dev zlib1g-dev openssl libssl-dev libcurl4-openssl-dev libbz2-dev libcur13
```

Get nessessary dependencies 
```
export BOOST_VERSION=1_55_0
export BOOST_VERSION_DOTTED=1.55.0
export AVRO_VERSION=1.7.6
export CURL_VERSION=7.35.0
export ZLIB_VERSION=1.2.8
export BZLIB2_VERSION=1.0.6
export OPEN_SSL_VERSION=1.0.1g
export JOYENT_HTTP_VERSION=2.3


rm -rf ~/source/linux
mkdir -p ~/source/linux
cd ~/source/linux

git clone https://github.com/bitbouncer/csi-http
git clone https://github.com/bitbouncer/json-spirit

wget ftp://ftp.sunet.se/pub/www/servers/apache/dist/avro/avro-$AVRO_VERSION/cpp/avro-cpp-$AVRO_VERSION.tar.gz
tar xvf avro-cpp-$AVRO_VERSION.tar.gz

wget http://sourceforge.net/projects/boost/files/boost/$BOOST_VERSION_DOTTED/boost_$BOOST_VERSION.zip/download -Oboost_$BOOST_VERSION.zip
unzip boost_$BOOST_VERSION.zip

#uncomment the lines below if you do not want the built in versions
#wget  http://www.openssl.org/source/$OPEN_SSL_VERSION.tar.gz 
#gzip -d $OPEN_SSL_VERSION.tar.gz
#tar -xvf $OPEN_SSL_VERSION.tar

#wget http://curl.haxx.se/download/curl-$CURL_VERSION.zip
#unzip curl-$CURL_VERSION.zip

wget https://github.com/joyent/http-parser/archive/v$JOYENT_HTTP_VERSION.tar.gz -Ohttp_parser-v$JOYENT_HTTP_VERSION.tar.gz
gzip -d http_parser-v$JOYENT_HTTP_VERSION.tar.gz
tar -xvf http_parser-v$JOYENT_HTTP_VERSION.tar
```

Build
```
export BOOST_VERSION=1_55_0
export BOOST_VERSION_DOTTED=1.55.0
export AVRO_VERSION=1.7.6
export CURL_VERSION=7.35.0
export ZLIB_VERSION=1.2.8
export BZLIB2_VERSION=1.0.6
export OPEN_SSL_VERSION=1.0.1g
export JOYENT_HTTP_VERSION=2.3

cd boost_$BOOST_VERSION
./bootstrap.sh
./b2 -j 8
cd ..

cd avro-cpp-$AVRO_VERSION
export BOOST_ROOT=../boost_$BOOST_VERSION 
bash build.sh test
bash build.sh dist
mkdir avro
cp -r api/*.* avro
cd ..

cd json-spirit
export BOOST_ROOT=$PWD/../boost_$BOOST_VERSION 
export Boost_INCLUDE_DIR=$PWD/../boost_$BOOST_VERSION/boost
mkdir build
cd build
cmake ..
make
cd ..
cd ..

cd csi-http
bash build_linux.sh
```

Raspberry Pi - cross compiling on ubuntu 64

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
export AVRO_VERSION=1.7.6
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
export AVRO_VERSION=1.7.6
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
set VISUALSTUDIO_VERSION=12.0
set BOOST_VERSION=1_55_0
set BOOST_VERSION_DOTTED=1.55.0
set OPEN_SSL_VERSION=openssl-1.0.1g
set CURL_VERSION=7.35.0
set AVRO_VERSION=1.7.6
set ZLIB_VERSION=1.2.8
set JOYENT_HTTP_VERSION=2.3

start /wait git clone https://github.com/bitbouncer/csi-http
start /wait git clone https://github.com/bitbouncer/json-spirit

start /wait call wget ftp://ftp.sunet.se/pub/www/servers/apache/dist/avro/avro-1.7.6/cpp/avro-cpp-%AVRO_VERSION%.tar.gz
start /wait call tar xvf avro-cpp-%AVRO_VERSION%.tar.gz

start /wait wget http://zlib.net/zlib-%ZLIB_VERSION%.tar.gz
start /wait tar xvf zlib-%ZLIB_VERSION%.tar.gz

start /wait wget http://sourceforge.net/projects/boost/files/boost/%BOOST_VERSION_DOTTED%/boost_%BOOST_VERSION%.zip/download -Oboost_%BOOST_VERSION%.zip
start /wait unzip boost_%BOOST_VERSION%.zip

start /wait wget  http://www.openssl.org/source/%OPEN_SSL_VERSION%.tar.gz 
start /wait gzip -d %OPEN_SSL_VERSION%.tar.gz
start /wait tar -xvf %OPEN_SSL_VERSION%.tar

start /wait wget http://curl.haxx.se/download/curl-%CURL_VERSION%.zip
start /wait unzip curl-%CURL_VERSION%.zip

start /wait wget https://github.com/joyent/http-parser/archive/v%JOYENT_HTTP_VERSION%.tar.gz -Ohttp_parser-v%JOYENT_HTTP_VERSION%.tar.gz
start /wait gzip -d http_parser-v%JOYENT_HTTP_VERSION%.tar.gz
start /wait tar -xvf http_parser-v%JOYENT_HTTP_VERSION%.tar
```

Build
```
set VISUALSTUDIO_VERSION=12.0
set BOOST_VERSION=1_55_0
set BOOST_VERSION_DOTTED=1.55.0
set OPEN_SSL_VERSION=openssl-1.0.1g
set CURL_VERSION=7.35.0
set AVRO_VERSION=1.7.6
set ZLIB_VERSION=1.2.8
set JOYENT_HTTP_VERSION=2.3

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64

@ECHO BUILDING OPENSSL
cd %OPEN_SSL_VERSION%
perl Configure VC-WIN64A --prefix=/OpenSSL-Win64"
ms\do_win64a
nmake -f ms\nt.mak
nmake -f ms\ntdll.mak
cd ..
@ECHO DONE WITH OPENSSL


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
nmake /f makefile.vc mode=dll VC=12 WITH_SSL=yes ENABLE_SSPI=no ENABLE_WINSSL=no ENABLE_IDN=no DEBUG=yes MACHINE=x64
nmake /f makefile.vc mode=dll VC=12 WITH_SSL=yes ENABLE_SSPI=no ENABLE_WINSSL=no ENABLE_IDN=no DEBUG=no MACHINE=x64
cd ..
copy builds\libcurl-vc12-x64-debug-dll-ipv6-spnego\lib\libcurl_debug.lib libs\x64\Debug\libcurl.lib 
copy builds\libcurl-vc12-x64-release-dll-ipv6-spnego\lib\libcurl.lib libs\x64\Release\libcurl.lib 
copy builds\libcurl-vc12-x64-debug-dll-ipv6-spnego\bin\libcurl_debug.dll libs\x64\Debug\libcurl_debug.dll 
copy builds\libcurl-vc12-x64-release-dll-ipv6-spnego\bin\libcurl.dll libs\x64\Release\libcurl.dll 
cd ..
@ECHO DONE WITH CURL


@ECHO BUILDING BOOST
cd boost_%BOOST_VERSION%
rmdir /s /q bin.v2
rmdir /s /q bin.v2
call bootstrap.bat
b2 -j 4 -toolset=msvc-12.0 address-model=64 --build-type=complete --stagedir=lib\x64 stage -s ZLIB_SOURCE=%CD%\..\zlib-%ZLIB_VERSION%
rmdir /s /q bin.v2
cd ..
@ECHO DONE WITH BOOST



@ECHO BUILDING AVRO
REM this is a ugly patch because I havn't figured out how to the original Cmakefile find boost...
copy csi-http\patches\avro\CMakeLists.txt avro-cpp-%AVRO_VERSION%

cd avro-cpp-%AVRO_VERSION%
rmdir /S /Q avro
rmdir /S /Q bin
rmdir /S /Q lib
rmdir /S /Q win_build32
rmdir /S /Q win_build64
mkdir win_build64 
cd win_build64
del CMakeCache.txt
cmake -G "Visual Studio 12 Win64" ..
devenv ALL_BUILD.vcxproj /Build "Debug|x64"
devenv ALL_BUILD.vcxproj /Build "Release|x64"
cd ..
mkdir avro
xcopy /e /s api\*.* avro
cd ..

cd json-spirit
call build_windows.bat
cd ..


cd csi-http
git pull
call build_windows.bat
cd ..
```


License:
- Boost Software License, Version 1.0.











