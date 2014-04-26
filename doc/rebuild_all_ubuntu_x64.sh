#!/usr/bin/bash
. third_part_versions

mkdir -p ~/source/linux
cd ~/source/linux

wget ftp://ftp.sunet.se/pub/www/servers/apache/dist/avro/avro-$AVRO_VERSION/cpp/avro-cpp-$AVRO_VERSION.tar.gz
tar xvf avro-cpp-$AVRO_VERSION.tar.gz

wget http://sourceforge.net/projects/boost/files/boost/$BOOST_VERSION_DOTTED/boost_$BOOST_VERSION.zip/download -Oboost_$BOOST_VERSION.zip
unzip boost_$BOOST_VERSION.zip

#wget  http://www.openssl.org/source/$OPEN_SSL_VERSION.tar.gz 
#gzip -d $OPEN_SSL_VERSION.tar.gz
#tar -xvf $OPEN_SSL_VERSION.tar

#wget http://curl.haxx.se/download/curl-$CURL_VERSION.zip
#unzip curl-$CURL_VERSION.zip

git clone https://github.com/bitbouncer/csi-http


#build the stuff
cd boost_$BOOST_VERSION
./bootstrap.sh
./b2 -j 4
cd ..

cd avro-cpp-$AVRO_VERSION
export BOOST_ROOT=../boost_$BOOST_VERSION 
bash build.sh test
bash build.sh dist
mkdir avro
cp -r api/*.* avro
cd ..

cd csi-http
bash build_linux.sh
