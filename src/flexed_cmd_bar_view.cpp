/**
 * @file flexed_cmd_bar_view.cpp
 */

#include "flexed_cmd_bar_view.h"
#include "keyboard_map.h"
#include "log.h"

namespace flexed {

    cmd_bar_view::cmd_bar_view(editor_window *editor_win) {
        init(editor_win);
    }

    cmd_bar_view::~cmd_bar_view() {
    }

    void cmd_bar_view::abort_cmd() {
        reset_prompt_callback();
        clear_cmd_bar();
        cmd_text_view->set_editable(false);
        editor_win->set_focus(*(editor_win->get_active_text_view()));
    }

    void cmd_bar_view::execute_cmd() {
        cmd_text_view->set_editable(false);
        auto text = cmd_text_view->get_buffer()->get_text(true);
        clear_cmd_bar();
        editor_win->set_focus(*(editor_win->get_active_text_view()));
        if (prompt_callback != nullptr) {
            FILE_LOG(LOG_DEBUG1) << "Callback execute cmd bar";
            auto callback = *prompt_callback;
            editor_win->set_focus(*(editor_win->get_active_text_view()));
            reset_prompt_callback();
            callback.second(callback.first, text);
        }
    }

    void cmd_bar_view::clear_cmd_bar() {
        cmd_text_view->get_buffer()->set_text("");
        prompt_text_view->get_buffer()->set_text("");
    }

    void cmd_bar_view::set_cmd_bar_msg(const Glib::ustring& msg) {
        prompt_text_view->get_buffer()->set_text(msg);
    }

    void cmd_bar_view::init(editor_window* editor_win) {
        this->editor_win = editor_win;
        prompt_text_view = std::make_shared<text_view>();
        cmd_text_view = std::make_shared<text_view>();
        cmd_text_view_buffer = Glib::RefPtr<text_buffer>(new text_buffer());
        cmd_text_view_kmap = std::make_shared<keyboard_map>();

        cmd_text_view_kmap
            ->set_key_binding<cmd_bar_view, &cmd_bar_view::abort_cmd>(
                "Cg", this);
        cmd_text_view_kmap
            ->set_key_binding<cmd_bar_view, &cmd_bar_view::execute_cmd>(
                "N", this);
        cmd_text_view->set_buffer(cmd_text_view_buffer);
        cmd_text_view_buffer->set_keyboard_map(cmd_text_view_kmap);
        //cmd_text_view->signal_focus_out_event().connect(
        //    sigc::mem_fun(*this, &cmd_bar_view::on_focus_out_event), false);

        set_editable(false);
        add(main_box);
        main_box.pack_start(*prompt_text_view, Gtk::PACK_EXPAND_WIDGET);
        main_box.pack_start(*cmd_text_view, Gtk::PACK_EXPAND_WIDGET);
        prompt_text_view->set_right_margin(5);
        prompt_text_view->set_editable(false);
        prompt_text_view->set_can_focus(false);
        cmd_text_view->set_editable(false);
        show_all_children();
    }

    void cmd_bar_view::reset_prompt_callback() {
        if (prompt_callback != nullptr) {
            prompt_callback.reset();
        }
        prompt_callback = nullptr;
    }

    bool cmd_bar_view::on_focus_out_event(/*Gdk::EventFocus& gdk_event*/) {
        abort_cmd();
        return false;
    }
}
