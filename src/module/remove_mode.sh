#!/bin/bash
# Script to remove a mode.

if [ "$1" = "" ]; then
    echo "Usage:" $0 "MODE_NAME"
    exit
fi

rm -Rf "$1"
sed -i "/$1/d" CMakeLists.txt
