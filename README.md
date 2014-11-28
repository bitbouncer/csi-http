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

Ubuntu 14 x64:

Install build tools
```
sudo apt-get install -y automake autogen shtool libtool git wget cmake unzip build-essential g++ python-dev autotools-dev libicu-dev zlib1g-dev openssl libssl-dev libcurl4-openssl-dev libbz2-dev libcurl3 libboost-all-dev

```

Get and build nessessary dependencies
```
sudo ls
mkdir source
cd source
git clone https://github.com/bitbouncer/csi-build-scripts.git
bash csi-build-scripts/ubuntu14_setup.sh
```

Raspberry Pi - cross compiling on ubuntu14 x32

Install build tools
```
sudo apt-get -y install cmake wget unzip cmake wget wput libpcre3 libpcre3-dev build-essential git subversion 
mkdir -p ~/xtools
cd ~/xtools
git clone https://github.com/raspberrypi/tools.git --depth 1
cd ..
echo "export PATH=~/xtools/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin:\$PATH" >> ~/.bashrc
source ~/.bashrc


```
Get and build nessessary dependencies 
```
sudo ls
mkdir raspbian_bitbouncer
cd raspbian_bitbouncer
git clone https://github.com/bitbouncer/csi-build-scripts.git
bash csi-build-scripts/raspbian_ubuntu32_setup.sh
```

Windows x64 - Visual Studio 12

Get and build nessessary dependencies
```
Install Visual Studio, nasm, git and active perl, make sure active perl is before git in PATH

mkdir source
cd source
git clone https://github.com/bitbouncer/csi-build-scripts.git
csi-build-scripts\windows_x64_vc12_setup.bat
```

License:
- Boost Software License, Version 1.0.











