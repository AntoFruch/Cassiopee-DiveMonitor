#!/bin/bash

# 1. DÉTECTION DE L'OS ET DU TOOLCHAIN
OS_TYPE=$(uname -s | tr '[:upper:]' '[:lower:]')
if [ "$OS_TYPE" == "darwin" ]; then
    HOST_TAG="darwin-x86_64"
    CPU_COUNT=$(sysctl -n hw.ncpu)
else
    HOST_TAG="linux-x86_64"
    CPU_COUNT=$(nproc)
fi

# 2. SET YOUR PATHS
export NDK_PATH=$HOME/Library/Android/sdk/ndk/27.2.12479018
# Sur Linux, le chemin du SDK est souvent different, ex: $HOME/Android/Sdk
export TOOLCHAIN=$NDK_PATH/toolchains/llvm/prebuilt/$HOST_TAG

# 3. SET TARGET ARCHITECTURE
export TARGET=aarch64-linux-android
export API=24
export AR=$TOOLCHAIN/bin/llvm-ar
export AS=$TOOLCHAIN/bin/llvm-as
export CC=$TOOLCHAIN/bin/$TARGET$API-clang
export CXX=$TOOLCHAIN/bin/$TARGET$API-clang++
export LD=$TOOLCHAIN/bin/ld
export RANLIB=$TOOLCHAIN/bin/llvm-ranlib
export STRIP=$TOOLCHAIN/bin/llvm-strip

# 4. CONFIGURE THE BUILD
./configure \
    --host=$TARGET \
    --with-sysroot=$TOOLCHAIN/sysroot \
    --prefix=$(pwd)/dist/android_arm64 \
    --enable-shared \
    --disable-static

# 5. COMPILE AND INSTALL
make clean
make -j$CPU_COUNT
make install

echo "Build Complete pour $OS_TYPE !"