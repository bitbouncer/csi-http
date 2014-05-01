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

- Ubuntu:

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

#build the stuff
```
cd json-spirit
export BOOST_ROOT=$PWD/../boost_$BOOST_VERSION 
export Boost_INCLUDE_DIR=$PWD/../boost_$BOOST_VERSION/boost
mkdir build
cd build
cmake ..
make
cd ..
cd ..

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

cd csi-http
bash build_linux.sh
```



License:
- Boost Software License, Version 1.0.











