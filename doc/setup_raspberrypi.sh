sudo apt-get install cmake wget unzip cmake wget wput libpcre3 libpcre3-dev build-essential git ia32-libs

cd ~/
mkdir raspberrypi
cd raspberrypi
git clone https://github.com/bitbouncer/csi-http

export BOOST_VERSION=1_55_0
export BOOST_VERSION_DOTTED=1.55.0
export AVRO_VERSION=1.7.6
export CURL_VERSION=7.35.0
export ZLIB_VERSION=1.2.8
export BZLIB2_VERSION=1.0.6


export OPEN_SSL_VERSION=1.0.1g

git clone https://github.com/raspberrypi/tools.git --depth 1

#32 bit tools
#echo "export PATH=~/raspberrypi/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin:\$PATH" >> ~/.bashrc
#64 bit tools
echo "export PATH=~/raspberrypi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin:\$PATH" >> ~/.bashrc
source ~/.bashrc

wget http://curl.haxx.se/download/curl-$CURL_VERSION.tar.bz2 -O ~/curl-$CURL_VERSION.tar.bz2
tar xvf ~/curl-$CURL_VERSION.tar.bz2

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

#BUILD STUFF

cd boost_$BOOST_VERSION
#32 bit
#echo "using gcc : arm : arm-bcm2708hardfp-linux-gnueabi-g++ ;" >> tools/build/v2/user-config.jam
#64 bit
echo "using gcc : arm : arm-linux-gnueabihf-g++ ;" >> tools/build/v2/user-config.jam
./bootstrap.sh
./b2 -j 5 -s ZLIB_SOURCE=$PWD/../zlib-$ZLIB_VERSION  -s BZIP2_SOURCE=$PWD/../bzip2-$BZLIB2_VERSION toolset=gcc-arm
cd ..

#build curl
export CFLAGS='-O2 -march=armv6j -mfpu=vfp -mfloat-abi=hard'
#32 bit
#export CC=arm-bcm2708hardfp-linux-gnueabi-gcc
#64 bit
export CC=arm-linux-gnueabihf-gcc

cd curl-$CURL_VERSION
#32bit
#./configure --host=arm-bcm2708hardfp-linux-gnueabi --disable-shared
#64bit
./configure --host=arm-linux-gnueabihf --disable-shared
make
cd ..

cd avro-cpp-$AVRO_VERSION
export BOOST_ROOT=../boost_$BOOST_VERSION 
export PI_TOOLS_HOME=../tools
rm -rf raspberrypi
mkdir raspberrypi
cd raspberrypi
cmake -DCMAKE_TOOLCHAIN_FILE=../csi-http/toolchains/raspberrypi.toolchain32.cmake ..
cd ..



#we skip openssl form now since csi-http have not enabled support yet
#zlib needs to be there for boost iostreams to compile but since were not using it at the moment - skip this


