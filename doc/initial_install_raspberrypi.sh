sudo apt-get install cmake wget unzip cmake wget wput libpcre3 libpcre3-dev build-essential git

mkdir -p ~/xtools
cd ~/xtools

#32 bit tools
#echo "export PATH=~/xtools/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin:\$PATH" >> ~/.bashrc
#64 bit tools
echo "export PATH=~/xtools/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin:\$PATH" >> ~/.bashrc
source ~/.bashrc

git clone https://github.com/raspberrypi/tools.git --depth 1



