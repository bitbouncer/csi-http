sudo apt-get install cmake wget unzip ant subversion cmake wget wput openjdk-6-jdk patch emacs libpcre3 libpcre3-dev build-essential git

export BOOST_VERSION=1_55_0
export BOOST_VERSION_DOTTED=1.55.0
export AVRO_VERSION=1.7.6
export CURL_VERSION=7.35.0
export ZLIB_VERSION=1.2.8

cd ~/
mkdir raspberrypi
cd raspberrypi

git clone https://github.com/raspberrypi/tools.git --depth 1

echo "export PATH=~/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin:\$PATH" >> ~/.bashrc
source ~/.bashrc

wget http://curl.haxx.se/download/curl-$CURL_VERSION.tar.bz2 -O ~/curl-$CURL_VERSION.tar.bz2
tar xvf ~/curl-$CURL_VERSION.tar.bz2

wget http://sourceforge.net/projects/boost/files/boost/1.52.0/boost_$BOOST_VERSION.tar.gz/download -Oboost_$BOOST_VERSION.tar.gz
tar xvf boost_$BOOST_VERSION.tar.gz

wget http://zlib.net/zlib-$ZLIB_VERSION.tar.gz
tar xvf zlib-$ZLIB_VERSION.tar.gz

wget  http://www.openssl.org/source/$OPEN_SSL_VERSION.tar.gz -Oopenssl-$OPEN_SSL_VERSION.tar.gz
gzip -d openssl-$OPEN_SSL_VERSION.tar.gz
tar -xvf openssl-$OPEN_SSL_VERSION.tar

wget ftp://ftp.sunet.se/pub/www/servers/apache/dist/avro/avro-$AVRO_VERSION/cpp/avro-cpp-$AVRO_VERSION.tar.gz
tar xvf avro-cpp-$AVRO_VERSION.tar.gz

#BUILD STUFF

cd boost_$BOOST_VERSION
echo "using gcc : arm : arm-bcm2708hardfp-linux-gnueabi-g++ ;" >> tools/build/v2/user-config.jam
./bootstrap.sh
./b2 toolset=gcc-arm
cd ..

#build curl
export CFLAGS='-O2 -march=armv6j -mfpu=vfp -mfloat-abi=hard'
export CC=arm-bcm2708hardfp-linux-gnueabi-gcc

cd curl-$CURL_VERSION
./configure --host=arm-bcm2708hardfp-linux-gnueabi --disable-shared
make
cd ..

cd avro-cpp-$AVRO_VERSION



#we skip openssl form now since csi-http have not enabled support yet
#zlib needs to be there for boost iostreams to compile but since were not using it at the moment - skip this


