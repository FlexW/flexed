#include <gtkmm.h>
#include <flexed_editor.h>
#include <flexed_text_view.h>

#include "settings_flexed.h"

namespace flexed {

    static settings* instance = nullptr;

    void settings_end() {
        if (instance != nullptr) {
            delete instance;
        }
        g_print("settings_end() called\n");
    }

    void settings_start(editor* ed) {
        if (instance == nullptr) {
            instance = new settings(ed);
        }
        g_print("settings_start() called\n");
    }

    void settings_buffer_end() {
        g_print("settings_buffer_end() called\n");
    }

    void settings_buffer_start() {
        g_print("settings_buffer_start() called\n");
    }

    void settings_set_linnum() {
        instance->set_linnum();
    }

    void settings_unset_linnum() {
        instance->unset_linnum();
    }

    settings::settings(editor* ed) {
        this->ed = ed;
        global_linnum_mode = false;
        ed->signal_text_view_created()
            .connect(sigc::mem_fun(*this, &settings::on_text_view_created));
    }

    settings::~settings() {
    }

    void settings::set_linnum() {
        ed->get_active_text_view()->set_show_line_numbers(true);
    }

    void settings::unset_linnum() {
        ed->get_active_text_view()->set_show_line_numbers(false);
    }

    void settings::set_linnum_global() {
        auto views = ed->get_text_views();
        for (auto view : views) {
            view->set_show_line_numbers(true);
        }
        global_linnum_mode = true;
    }

    void settings::unset_linnum_global() {
        auto views = ed->get_text_views();
        for (auto view : views) {
            view->set_show_line_numbers(false);
        }
        global_linnum_mode = false;
    }

    void settings::on_text_view_created(text_view& view) {
        if (global_linnum_mode) {
            view.set_show_line_numbers();
        }
    }
}
