#!/bin/bash
# how to use: build-all.sh "/path/to/your/ion/rootfolder"
#   example running it from ion root folder:
#       build-all.sh ${PWD}
JOBS="6"
OUTPUTFOLDER="ion-binaries"
# cd to ion root

# HOSTS
PCLINUXHOSTS="x86_64-pc-linux-gnu i686-pc-linux-gnu"
ARMHOSTS="arm-linux-gnueabihf aarch64-linux-gnu"
OSXHOSTS="x86_64-apple-darwin16"
MIPSHOSTS="mipsel-linux-gnu mips-linux-gnu"
POWERPCHOSTS="powerpc64-linux-gnu powerpc64el-linux-gnu"
RISCHOSTS="riscv64-linux-gnu" # riscv32-linux-gnu
S390HOSTS="s390x-linux-gnu"
WINHOSTS="i686-w64-mingw32 x86_64-w64-mingw32"
ANDROIDHOSTS="armv7a-linux-android aarch64-linux-android i686-linux-android x86_64-linux-android"
HOSTS="$PCLINUXHOSTS $ARMHOSTS $OSXHOSTS $WINHOSTS $MIPSHOSTS $POWERPCHOSTS $RISCHOSTS $S390HOSTS"

# for windows hosts only
sudo echo "windows alternatives config" # make sure you type sudo password here, you will not be asked for it afterwards
# windows hosts (make sure you run update-alternatives --config before running this script)
echo "1" | sudo update-alternatives --config x86_64-w64-mingw32-g++
echo "1" | sudo update-alternatives --config x86_64-w64-mingw32-gcc
echo "1" | sudo update-alternatives --config i686-w64-mingw32-g++
echo "1" | sudo update-alternatives --config i686-w64-mingw32-gcc

OUTPUTFOLDER="$PWD/$OUTPUTFOLDER"
mkdir -p $OUTPUTFOLDER

cd "$1"
for a in $HOSTS
do
    cd depends
    make HOST=$a -j$JOBS | tee makedepends-$a.log
    cd ..
    make clean
    ./autogen.sh
    ./configure --prefix=`pwd`/depends/$a  | tee config-$a.log
    make clean
    make -j$JOBS | tee make-$a.log
    mkdir -p $OUTPUTFOLDER/$a/test
    mv src/iond $OUTPUTFOLDER/$a/
    mv src/ion-cli $OUTPUTFOLDER/$a/
    mv src/ion-tx $OUTPUTFOLDER/$a/
    mv src/qt/ion-qt $OUTPUTFOLDER/$a/
    mv test/test_iond $OUTPUTFOLDER/$a/
done
