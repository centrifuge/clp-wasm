#!/bin/bash

BUILD_CONFIG=$1
if [ -z "$1" ] ; then
    BUILD_CONFIG=release
fi

BUILD_PLATFORM=$2
if [ -z "$2" ] ; then
    BUILD_PLATFORM=wasm
fi

BUILD_DIR=build_$BUILD_PLATFORM_$BUILD_CONFIG

if [ $BUILD_PLATFORM == "wasm" ]; then
    echo 'Running wasm build ... '
    source /emsdk/emsdk_env.sh
    cmake -G Ninja -B$BUILD_DIR \
    -DCMAKE_BUILD_TYPE=${BUILD_CONFIG} \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
    -DCMAKE_MODULE_PATH=$EMSDK/upstream/emscripten/cmake \
    -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake .
fi

if [ $BUILD_PLATFORM == "x64" ]; then
    echo 'Running x64 build ... '
    cmake -G Ninja -B$BUILD_DIR \
    -DCMAKE_BUILD_TYPE=${BUILD_CONFIG} .
fi
cd $BUILD_DIR && ninja && cd ..

if [ $BUILD_PLATFORM == "wasm" ]; then
    cp $BUILD_DIR/cpplex.wasm* web/
    cp $BUILD_DIR/cpplex.js* web/
fi

