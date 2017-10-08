#!/bin/bash

# Script to create a template for flexed mode.

if [ "$1" = "" ]; then
    echo "Usage:" $0 "MODE_NAME"
fi

MODE_UPPERCASE=$(echo "$1"| awk '{print toupper($0)}')
MODE_UPPERCASE=$(echo $MODE_UPPERCASE)$(echo "_FLEXED_H_")

cat > "$1_flexed.cpp" << EOF
#include <gtkmm.h>

#include "$1_flexed.h"

namespace flexed {

void $1_end() {
    g_print("$1_end() called\n");
}

void $1_start() {
    g_print("$1_start() called\n");
}

void $1_buffer_end() {
    g_print("$1_buffer_end() called\n");
}

void $1_buffer_start() {
    g_print("$1_buffer_start() called\n");
}

}
EOF

cat > "$1_flexed.h" << EOF
#ifndef $MODE_UPPERCASE
#define $MODE_UPPERCASE

namespace flexed {

extern "C" {
    void $1_end();
    void $1_start();
    void $1_buffer_start();
    void $1_buffer_end();
}

}

#endif // $MODE_UPPERCASE
EOF
