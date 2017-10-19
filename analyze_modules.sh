#!/bin/bash

export CXX=clang++
export CC=clang

mkdir -v analyze_modules
cd analyze_modules
cmake ../
scan-build make
