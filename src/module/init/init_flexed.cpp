#include <gtkmm.h>

#include "../../flexed_editor.h"
#include "init_flexed.h"

namespace flexed {

void init_end() {
    g_print("init_end() called\n");
}

void init_start() {
    g_print("editor instance in init: %p\n", editor::get_instance());
    g_print("init_start() called\n");
    auto g_keyboard_map = editor::get_instance()->get_keyboard_map();
    g_print("init_start() end\n");
    g_keyboard_map->set_key_binding<editor, &editor::insert_paned_horizontal>(
        "Cx2", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::insert_paned_vertical>(
        "Cx3", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::remove_paned>(
        "Cx1", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::open_file_prompt>(
        "CxCf", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::switch_to_next_buffer>(
        "CxR", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::switch_to_previous_buffer>(
        "CxL", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::switch_paned_up>(
        "SU", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::switch_paned_down>(
        "SD", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::switch_paned_left>(
        "SL", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::switch_paned_right>(
        "SR", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::call_mode_function_prompt>(
        "Ax", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::save_file>(
        "CxCs", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::quit>(
        "CxCq", editor::get_instance());
    g_keyboard_map->set_key_binding<editor, &editor::load_mode_prompt>(
        "CmCl", editor::get_instance());

    g_keyboard_map->set_key_binding<editor, &editor::unload_mode_prompt>(
        "CmCu", editor::get_instance());
}

void init_buffer_end() {
    g_print("init_buffer_end() called\n");
}

void init_buffer_start() {
    g_print("init_buffer_start() called\n");
}

}
