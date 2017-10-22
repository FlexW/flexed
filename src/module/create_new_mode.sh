#!/bin/bash

# Script to create a template for flexed mode.

if [ "$1" = "" ]; then
    echo "Usage:" $0 "MODE_NAME"
    exit
fi

mkdir "$1"
cd "$1"

MODE_UPPERCASE=$(echo "$1"| awk '{print toupper($0)}')
MODE_UPPERCASE=$(echo $MODE_UPPERCASE)$(echo "_FLEXED_H_")

cat > "$1_flexed.cpp" << EOF
#include <gtkmm.h>
#include <flexed_editor.h>

#include "$1_flexed.h"

namespace flexed {

void $1_end() {
    g_print("$1_end() called\n");
}

void $1_start(editor* ed) {
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

class editor;

extern "C" {
    void $1_end();
    void $1_start(editor* ed);
    void $1_buffer_start();
    void $1_buffer_end();
}

}

#endif // $MODE_UPPERCASE
EOF

cat > "CMakeLists.txt" << EOF
find_package(PkgConfig)

SET(GCC_COVERAGE_LINK_FLAGS "-fPIC")
SET(
  CMAKE_EXE_LINKER_FLAGS
  "\${CMAKE_EXE_LINKER_FLAGS} \${GCC_COVERAGE_LINK_FLAGS}"
  )

SET(CMAKE_CXX_FLAGS
  ${CMAKE_CXX_FLAGS} "-Wno-#warnings"
  )

pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(GTKSOURCEVIEWMM gtksourceviewmm-3.0)

link_directories(
  \${GTKMM_LIBRARY_DIRS}
  \${GTKSOURCEVIEWMM_LIBRARY_DIRS}
  )

include_directories(
  \${GTKMM_INCLUDE_DIRS}
  \${GTKSOURCEVIEWMM_INCLUDE_DIRS}
  "/usr/include/flexed"
  )

add_library(flexed_$1 SHARED $1_flexed.cpp)

target_link_libraries(
  flexed_$1
  \${GTKMM_LIBRARIES}
  \${GTKSOURCEVIEWMM_LIBRARIES}
  flexed
  )

install(
  TARGETS flexed_$1
  DESTINATION "/usr/lib/flexed"
  )
EOF

echo "add_subdirectory($1)" >> ../CMakeLists.txt
