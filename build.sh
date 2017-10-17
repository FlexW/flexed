#!/bin/bash

mkdir build_debug
cd build/
cmake -DCMAKE_BUILD_TYPE=Debug ../
make && make install
