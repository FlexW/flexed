#include <gtkmm.h>

#include "c_mode_flexed.h"

namespace flexed {

char* c_mode_end() {
    return NULL;
}

void c_mode_start() {
    g_print("c_mode start_mode() called\n");
}

void c_mode_dummy1() {
    g_print("c_mode_dummy1() called\n");
}

void c_mode_dummy2() {
    g_print("c_mode_dummy2() called\n");
}

c_mode::c_mode() {
}

c_mode::~c_mode() {
}

bool c_mode::start() {
  g_print("start c mode\n");
  return true;
}

void c_mode::end() {
}

}
