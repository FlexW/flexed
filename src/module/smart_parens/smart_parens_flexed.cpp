#include "../../flexed_editor.h"
#include "smart_parens_flexed.h"

namespace flexed {

static smart_parens* instance = nullptr;

void smart_parens_end() {
    delete instance;
    g_print("smart_parens_end() called\n");
}

void smart_parens_start() {
    g_print("smart_parens_start() called\n");
    instance = new smart_parens();
}

void smart_parens_buffer_end() {
    g_print("smart_parens_buffer_end() called\n");
    smart_parens::remove_key_bindings();
}

void smart_parens_buffer_start() {
    g_print("smart_parens_buffer_start() called\n");
    smart_parens::init_key_bindings();
}

void smart_parens::init_key_bindings() {
    editor::get_instance()->get_active_text_view_buffer()->get_keyboard_map()
        ->set_key_binding<smart_parens, &smart_parens::add_closing_paren>(
            "S(", instance);
}

void smart_parens::remove_key_bindings() {
    editor::get_instance()->get_active_text_view_buffer()->get_keyboard_map()
        ->remove_key_binding("S(");
}

void smart_parens::add_closing_paren() {
    g_print("add_closing_paren() called\n");
    auto buffer = editor::get_instance()->get_active_text_view_buffer();
    buffer->insert_at_cursor("()");

    auto insert_iter = buffer->get_iter_at_mark(buffer->get_insert());
    insert_iter.backward_char();
    buffer->place_cursor(insert_iter);
}

}
