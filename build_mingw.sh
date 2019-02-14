#!/usr/bin/env bash

set -e

# Notes: install required libraries
#sudo apt-get install libboost-all-dev
#sudo apt-get install libwebsocketpp-dev

mkdir -p build_mingw
pushd build_mingw
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-mingw-w64-ubuntu.cmake ..
cmake --build . --target install
popd

