#include <gtkmm.h>
#include <flexed_editor.h>

#include "dired_flexed.h"

namespace flexed {

    static dired* dired_instance = nullptr;

    void dired_end() {
        g_print("dired_end() called\n");
        delete dired_instance;
    }

    void dired_start(editor* ed) {
        g_print("dired_start() called\n");
        dired_instance = new dired(ed);
    }

    void dired_buffer_end() {
        g_print("dired_buffer_end() called\n");
    }

    void dired_buffer_start() {
        g_print("dired_buffer_start() called\n");
    }

    dired::dired(editor* ed) {
        this->ed = ed;
    }

    dired::~dired() {
    }

    void dired::find_file() {
    }
}
