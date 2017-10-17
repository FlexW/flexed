#!/bin/bash

mkdir -v build
cd build/
cmake -DCMAKE_BUILD_TYPE=Debug ../
make && make install

cd ..
./build_modules.sh
