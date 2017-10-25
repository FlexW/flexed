#include <gtkmm.h>

#include "init_flexed.h"

namespace flexed {

void init_end() {
    g_print("init_end() called\n");
}

void init_start(editor_window *ed) {
    g_print("init_start() called\n");
    auto g_keyboard_map = ed->get_keyboard_map();
    g_print("init_start() end\n");
    g_keyboard_map->set_key_binding<editor_window, &editor_window::insert_paned_horizontal>(
        "Cx2", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::insert_paned_vertical>(
        "Cx3", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::remove_paned>(
        "Cx1", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::open_file_prompt>(
        "CxCf", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::switch_to_next_buffer>(
        "CxR", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::switch_to_previous_buffer>(
        "CxL", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::switch_paned_up>(
        "AU", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::switch_paned_down>(
        "AD", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::switch_paned_left>(
        "AL", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::switch_paned_right>(
        "AR", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::call_mode_function_prompt>(
        "Ax", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::save_file>(
        "CxCs", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::quit>(
        "CxCq", ed);
    g_keyboard_map->set_key_binding<editor_window, &editor_window::load_mode_prompt>(
        "CmCl", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::unload_mode_prompt>(
        "CmCu", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::load_mode_global_prompt>(
        "CgCl", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::unload_mode_global_prompt>(
        "CgCu", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::remove_active_text_buffer>(
        "CxCk", ed);

    g_keyboard_map->set_key_binding<editor_window, &editor_window::switch_fullscreen>(
        "F11", ed);
}

void init_buffer_end() {
    g_print("init_buffer_end() called\n");
}

void init_buffer_start() {
    g_print("init_buffer_start() called\n");
}

}
