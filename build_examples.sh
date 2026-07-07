#!/usr/bin/env bash
# This shell script is supposed to be run in a docker-container with already installed DCPLib and asio libs.
set -e

if [[ $(basename ${PWD}) != "DCPLib" ]]; then
  echo "Please run this script in DCPLib folder to make sure the right folders are accessed."
  exit -1
fi

pushd example
  for example in $(ls -d */); do
    pushd $example
      echo "In folder $example"
      mkdir -p build
      pushd build
        cmake ..
        cmake --build .
      popd
    popd
  done
popd
