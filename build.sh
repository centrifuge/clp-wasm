#!/bin/bash

source /emsdk/emsdk_env.sh

BUILD_CONFIG=$1
if [ -z "$1" ] ; then
    BUILD_CONFIG=debug
fi
BUILD_DIR=build_wasm_$BUILD_CONFIG

cmake -G Ninja -B$BUILD_DIR \
   -DCMAKE_BUILD_TYPE=${BUILD_CONFIG} \
   -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
   -DCMAKE_MODULE_PATH=$EMSDK/upstream/emscripten/cmake \
   -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake .

cd $BUILD_DIR && ninja && cd ..
cp $BUILD_DIR/cpplex.wasm* web/
cp $BUILD_DIR/cpplex.js web/

