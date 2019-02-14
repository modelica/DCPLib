#!/usr/bin/env bash

set -e

# Notes: install required libraries
#sudo apt-get install libboost-all-dev
#sudo apt-get install libwebsocketpp-dev

mkdir -p build
pushd build
cmake ..
cmake --build . --target install
popd

