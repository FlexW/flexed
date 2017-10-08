#include <gtkmm.h>

#include "c_mode_flexed.h"

namespace flexed {

void c_mode_end() {
    g_print("c_mode_end() called\n");
}

void c_mode_start() {
    g_print("c_mode_start() called\n");
}

void c_mode_buffer_end() {
    g_print("c_mode_buffer_end() called\n");
}

void c_mode_buffer_start() {
    g_print("c_mode_buffer_start() called\n");
}

void c_mode_dummy1() {
    g_print("c_mode_dummy1() called\n");
}

void c_mode_dummy2() {
    g_print("c_mode_dummy2() called\n");
}

}
