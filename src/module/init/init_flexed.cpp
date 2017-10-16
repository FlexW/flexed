#include <gtkmm.h>

#include "../../flexed_editor.h"
#include "init_flexed.h"

namespace flexed {

void init_end() {
    g_print("init_end() called\n");
}

void init_start(editor *ed) {
    g_print("init_start() called\n");
    auto g_keyboard_map = ed->get_keyboard_map();
    g_print("init_start() end\n");
    g_keyboard_map->set_key_binding<editor, &editor::insert_paned_horizontal>(
        "Cx2", ed);

    g_keyboard_map->set_key_binding<editor, &editor::insert_paned_vertical>(
        "Cx3", ed);

    g_keyboard_map->set_key_binding<editor, &editor::remove_paned>(
        "Cx1", ed);

    g_keyboard_map->set_key_binding<editor, &editor::open_file_prompt>(
        "CxCf", ed);

    g_keyboard_map->set_key_binding<editor, &editor::switch_to_next_buffer>(
        "CxR", ed);

    g_keyboard_map->set_key_binding<editor, &editor::switch_to_previous_buffer>(
        "CxL", ed);

    g_keyboard_map->set_key_binding<editor, &editor::switch_paned_up>(
        "AU", ed);

    g_keyboard_map->set_key_binding<editor, &editor::switch_paned_down>(
        "AD", ed);

    g_keyboard_map->set_key_binding<editor, &editor::switch_paned_left>(
        "AL", ed);

    g_keyboard_map->set_key_binding<editor, &editor::switch_paned_right>(
        "AR", ed);

    g_keyboard_map->set_key_binding<editor, &editor::call_mode_function_prompt>(
        "Ax", ed);

    g_keyboard_map->set_key_binding<editor, &editor::save_file>(
        "CxCs", ed);

    g_keyboard_map->set_key_binding<editor, &editor::quit>(
        "CxCq", ed);
    g_keyboard_map->set_key_binding<editor, &editor::load_mode_prompt>(
        "CmCl", ed);

    g_keyboard_map->set_key_binding<editor, &editor::unload_mode_prompt>(
        "CmCu", ed);

    g_keyboard_map->set_key_binding<editor, &editor::remove_active_text_buffer>(
        "CxCk", ed);

    g_keyboard_map->set_key_binding<editor, &editor::switch_fullscreen>(
        "F11", ed);
}

void init_buffer_end() {
    g_print("init_buffer_end() called\n");
}

void init_buffer_start() {
    g_print("init_buffer_start() called\n");
}

}
