#!/bin/bash

# 1. SET YOUR PATHS
# Adjust NDK_PATH to match your actual installation version
export NDK_PATH=$HOME/Library/Android/sdk/ndk/27.2.12479018
export TOOLCHAIN=$NDK_PATH/toolchains/llvm/prebuilt/darwin-x86_64

# 2. SET TARGET ARCHITECTURE
# For modern Android, we want arm64-v8a (API 24 is Android 7.0+)
export TARGET=aarch64-linux-android
export API=24
export AR=$TOOLCHAIN/bin/llvm-ar
export AS=$TOOLCHAIN/bin/llvm-as
export CC=$TOOLCHAIN/bin/$TARGET$API-clang
export CXX=$TOOLCHAIN/bin/$TARGET$API-clang++
export LD=$TOOLCHAIN/bin/ld
export RANLIB=$TOOLCHAIN/bin/llvm-ranlib
export STRIP=$TOOLCHAIN/bin/llvm-strip

# 3. CONFIGURE THE BUILD
# We generate the build files specifically for the Android host
./configure \
    --host=$TARGET \
    --with-sysroot=$TOOLCHAIN/sysroot \
    --prefix=$(pwd)/dist/android_arm64 \
    --enable-shared \
    --disable-static

# 4. COMPILE AND INSTALL
make clean
make -j$(sysctl -n hw.ncpu)
make install

echo "----------------------------------------"
echo "Build Complete!"
echo "Binaries are located in: $(pwd)/dist/android_arm64"
echo "----------------------------------------"
