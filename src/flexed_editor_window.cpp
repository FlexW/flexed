#include <cstring>
#include <fstream>
#include <iostream>
#include <utility>
#include <string>
#include <boost/filesystem.hpp>

#include "flexed_editor_window.h"
#include "flexed_paned.h"
#include "flexed_status_bar_view.h"
#include "flexed_cmd_bar_view.h"
#include "flexed_paned.h"
#include "flexed_text_view.h"
#include "keyboard_map.h"
#include "log.h"

namespace flexed {

    editor_window::editor_window() {
        init_editor();
    }

    editor_window::~editor_window() {
    }

    sigc::signal<void>& editor_window::signal_buffer_changed() {
        return sig_buffer_changed;
    }

    sigc::signal<void, text_view&>& editor_window::signal_text_view_created() {
        return sig_text_view_created;
    }

    sigc::signal<void, text_view&>& editor_window::signal_text_view_removed() {
        return sig_text_view_removed;
    }

    sigc::signal< void, Glib::RefPtr<text_buffer> >&
    editor_window::signal_text_buffer_created() {
        return sig_text_buffer_created;
    }

    sigc::signal< void, Glib::RefPtr<text_buffer> >&
    editor_window::signal_text_buffer_removed() {
        return sig_text_buffer_removed;
    }

    Glib::RefPtr<text_buffer> editor_window::get_active_text_view_buffer() {
        auto buffer = active_text_view->get_text_buffer();
        //auto tbuffer = Glib::RefPtr<text_buffer>::cast_dynamic(buffer);
        return buffer;
    }

    std::shared_ptr<mode_loader> editor_window::get_mode_loader() {
        return fmode_loader;
    }

    Gtk::Box* editor_window::get_main_box() {
        return &main_box;
    }

    std::shared_ptr<cmd_bar_view> editor_window::get_cmd_bar() {
        return cmd_bar;
    }

    std::shared_ptr<status_bar_view> editor_window::get_status_bar() {
        return status_bar;
    }

    std::shared_ptr<keyboard_map> editor_window::get_keyboard_map() {
        return g_keyboard_map;
    }

    std::list<text_view*> editor_window::get_text_views() {
        std::list<text_view*> list;
        for (auto p : text_view_map) {
            list.push_back(p.first);
        }
        return list;
    }

    std::shared_ptr<global_text_buffer_container>
    editor_window::get_global_text_buffer_container() {
        return g_text_buffer_container;
    }

    text_view* editor_window::get_active_text_view() {
        return active_text_view;
    }

    keyboard_handler& editor_window::get_keyboard_handler() {
        return keyboard;
    }

    void editor_window::set_divider_from_active_paned(int pos) {
        if (pos > 100 || pos < 0)
            return;

        auto tv = get_focus();
        if (tv == nullptr)
            return;

        auto sw = tv->get_parent();
        if (auto ppaned = dynamic_cast<paned*>(sw->get_parent())) {
            ppaned->set_divider_pos_relative(pos);
            return;
        }
        FILE_LOG(LOG_INFO) << "No Paned. Can not place divider";
    }

    void
    editor_window::set_ask_for_save_file_buffer(Glib::RefPtr<text_buffer> buffer) {
        ask_for_save_file_buffer = buffer;
    }

    void editor_window::set_text_view_focus(text_view* text_view) {
    }

    void editor_window::switch_fullscreen() {
        FILE_LOG(LOG_INFO) << "Request fullscreen now";
        if (is_fullscreen) {
            is_fullscreen = false;
            unfullscreen();
            return;
        }
        is_fullscreen = true;
        fullscreen();
    }

    void editor_window::open_file_prompt() {
        cmd_bar->prompt_cmd_bar<editor_window, &editor_window::open_file>("Open file",
                                                            this);
    }

    void editor_window::open_file(Glib::ustring fname) {
        if (!fname.compare("")) {
            FILE_LOG(LOG_INFO) << "No file name";
            return;
        }
        FILE_LOG(LOG_INFO) << "Open file: " << fname;
        auto fname_path = boost::filesystem::path(fname);
        auto abs_path = boost::filesystem::absolute(fname_path);
        auto abs_path_str = abs_path.string();
        if (is_buffer_open(abs_path_str)) {
            FILE_LOG(LOG_INFO) << "Buffer already open";
            return;
        }
        auto file = new std::ifstream(fname);
        if (!file) {
            FILE_LOG(LOG_INFO) << "Error opening file";
            delete file;
            return;
        }
        auto new_buffer = create_text_buffer(abs_path_str);
        Glib::ustring utext = "";
        std::string text = "";
        if (!(file->fail())) {
            file->seekg(0, std::ios::end);
            text.reserve(file->tellg());
            file->seekg(0, std::ios::beg);
            text.assign((std::istreambuf_iterator<char>(*file)),
                        (std::istreambuf_iterator<char>()));
        }
        file->close();
        delete file;

        utext = text;
        new_buffer->set_text(text);
        if (active_text_view == nullptr) {
            return;
        }
        auto buf_container = text_view_map.find(active_text_view)->second;
        buf_container->previous();

        new_buffer->set_modified(false);
        status_bar->set_filename();
        status_bar->set_file_stats();
    }

    void editor_window::load_mode_prompt() {
        cmd_bar->prompt_cmd_bar<editor_window, &editor_window::load_mode>("Load mode",
                                                            this);
    }

    void editor_window::load_mode(Glib::ustring name) {
        FILE_LOG(LOG_INFO) << "Try to load mode now";
        if (fmode_loader
            ->load_mode(get_active_text_view_buffer(), (std::string&)name)) {
            //get_active_text_view_buffer()->add_mode((std::string&)name);
            cmd_bar->set_cmd_bar_msg(name + " loaded");
            return;
        }
        cmd_bar->set_cmd_bar_msg(name + " not loaded");
    }

    void editor_window::load_mode_global_prompt() {
        cmd_bar->prompt_cmd_bar<editor_window, &editor_window::load_mode_global>(
            "Load mode global", this);
    }

    void editor_window::load_mode_global(Glib::ustring name) {
        if (fmode_loader
            ->load_mode_global((std::string&)name)) {
            cmd_bar->set_cmd_bar_msg(name + " loaded global");
            return;
        }
        cmd_bar->set_cmd_bar_msg(name + " not loaded global");
    }

    void editor_window::call_mode_function_prompt() {
        cmd_bar->prompt_cmd_bar<editor_window, &editor_window::call_mode_function>(
            "Execute", this);
    }

    void editor_window::call_mode_function(Glib::ustring name) {
        FILE_LOG(LOG_INFO) << "Try to call function now";
        auto mode_list = get_active_text_view_buffer()->get_mode_list();
        for(auto mode : mode_list) {
            g_print("mode in mode_list: %s\n", mode.c_str());
        }
        fmode_loader->call_function(mode_list, (std::string&)name);
    }

    void editor_window::unload_mode_prompt() {
        cmd_bar->prompt_cmd_bar<editor_window, &editor_window::unload_mode>(
            "Unload mode", this);
    }

    void editor_window::unload_mode(Glib::ustring name) {
        FILE_LOG(LOG_INFO) << "Try to unload mode now";
        //get_active_text_view_buffer()->unset_mode((std::string&)name);
        fmode_loader
            ->unload_mode(get_active_text_view_buffer(), (std::string&)name);
        cmd_bar->set_cmd_bar_msg(name + " unloaded");
    }

    void editor_window::unload_mode_global_prompt() {
        cmd_bar->prompt_cmd_bar<editor_window, &editor_window::unload_mode_global>(
            "Unload mode global", this);
    }

    void editor_window::unload_mode_global(Glib::ustring name) {
        fmode_loader->unload_mode_global((std::string&)name);
        cmd_bar->set_cmd_bar_msg(name + " unloaded global");
    }

    void editor_window::ask_for_save_prompt() {
        cmd_bar->prompt_cmd_bar<editor_window, &editor_window::ask_for_save>(
            ask_for_save_file_buffer->get_name()
            + " | File modified. Save? [y/n]", this);
    }

    void editor_window::ask_for_save(Glib::ustring y_or_n) {
        int y_or_n_flag = check_yes_or_no(y_or_n);
        if (y_or_n_flag == 1) {
            //if (ask_for_save_file_buffer == nullptr) {
            //    save_file();
            //}
            //else {
            save_file(ask_for_save_file_buffer);
                //}
            //ask_for_save_file_buffer = nullptr;
        }
        else if (y_or_n_flag == 0) {
            ask_for_save_file_buffer->set_modified(false);
        }
        else if (y_or_n_flag == -1) {
            FILE_LOG(LOG_INFO) << "Not y or no";
            ask_for_save_prompt();
            return;
        }
        FILE_LOG(LOG_DEBUG1) << "exit_editor_flag: " << exit_editor_flag;
        if (exit_editor_flag) {
            FILE_LOG(LOG_DEBUG1) << "call quit again";
            quit();
        }
    }

    int editor_window::check_yes_or_no(std::string y_or_n) {
        if (y_or_n == "yes") {
            return 1;
        }
        else if (y_or_n == "y") {
            return 1;
        }
        else if (y_or_n == "no") {
            return 0;
        }
        if (y_or_n == "n") {
            return 0;
        }
        else {
            return -1;
        }
    }

    void editor_window::save_file() {
        save_file(get_active_text_view_buffer());
    }

    void editor_window::save_file(Glib::RefPtr<text_buffer> buffer) {
        auto fname = buffer->get_name();
        FILE_LOG(LOG_INFO) << "Save file as: %s" << fname;
        std::ofstream tfile(fname);
        if (tfile.is_open()) {
            tfile << buffer->get_text();
            tfile.close();
        cmd_bar->set_cmd_bar_msg("Save successfully");
        FILE_LOG(LOG_INFO) << "File saved";
            get_active_text_view_buffer()->set_modified(false);
            status_bar->set_file_stats();
            return;
        }
        cmd_bar->set_cmd_bar_msg("Save failed!");
        FILE_LOG(LOG_INFO) << "File not saved";
    }

    void editor_window::quit() {
        FILE_LOG(LOG_INFO) << "Exit editor now";
        exit_editor_flag = true;
        if (!check_buffers_saved()) {
            return;
        }
        close();
    }

    void editor_window::insert_paned_horizontal() {
        insert_paned(Gtk::ORIENTATION_HORIZONTAL);
    }

    void editor_window::insert_paned_vertical() {
        insert_paned(Gtk::ORIENTATION_VERTICAL);
    }

    void editor_window::insert_paned(Gtk::Orientation orientation) {
        text_view* active_tv = (text_view*)get_focus();
        if (active_tv == nullptr
            || active_tv == (text_view*)cmd_bar.get()
            || active_tv == (text_view*)status_bar.get()) {
            return;
        }
        Gtk::ScrolledWindow* active =
            (Gtk::ScrolledWindow*)active_tv->get_parent();
        if (text_view_count == 1) {
            insert_first_paned(active, orientation);
            return;
        }
        insert_other_paned(active, orientation);
    }

    void editor_window::insert_first_paned(Gtk::ScrolledWindow *active,
                                    Gtk::Orientation orientation) {
        main_box.remove(*cmd_bar);
        main_box.remove(*status_bar);
        main_box.remove(*active);

        paned* n_paned = create_paned(active, orientation);
        main_box.pack_start(*n_paned);
        main_box.pack_start(*status_bar, Gtk::PACK_SHRINK);
        main_box.pack_start(*cmd_bar, Gtk::PACK_SHRINK);
        show_all_children();

        active = (Gtk::ScrolledWindow*)n_paned->get_child2();
        set_focus(*active->get_child());
        text_view_count++;
    }

    void editor_window::insert_other_paned(Gtk::ScrolledWindow* active,
                                    Gtk::Orientation orientation) {
        paned* active_paned = (paned*)active->get_parent();
        active_paned->remove(*active);

        paned* n_paned = create_paned(active, orientation);
        if (active_paned->get_child1() == nullptr) {
            active_paned->add1(*n_paned);
        }
        else {
            active_paned->add2(*n_paned);
        }
        show_all_children();

        active = (Gtk::ScrolledWindow*)n_paned->get_child2();
        set_focus(*active->get_child());
        text_view_count++;
    }

    void editor_window::set_divider_from_active_paned_middle() {
        set_divider_from_active_paned(50);
    }

    Gtk::ScrolledWindow* editor_window::create_editing_text_view(text_view* old_tv) {
        auto w = construct_editing_text_view();
        add_text_view_to_map((text_view*)w->get_child(),
                             *text_view_map.find(old_tv)->second);
        active_text_view = (text_view*)w->get_child();
        return w;
    }

    Gtk::ScrolledWindow* editor_window::create_editing_text_view() {
        auto w = construct_editing_text_view();
        add_text_view_to_map((text_view*)w->get_child(),
                             *g_text_buffer_container);
        active_text_view = (text_view*)w->get_child();
        return w;
    }

    paned* editor_window::create_paned(Gtk::ScrolledWindow* left_side,
                                Gtk::Orientation orientation) {
        paned* p = new paned(orientation);
        p->add1(*left_side);
        p->add2(*create_editing_text_view((text_view*)left_side->get_child()));
        FILE_LOG(LOG_DEBUG1) << "Created paned";
        return p;
    }

    void editor_window::remove_paned() {
        auto active_tv = get_focus();
        if (active_tv == nullptr
            || active_tv == (text_view*)cmd_bar.get()
            || active_tv == (text_view*)status_bar.get()) {
            return;
        }

        auto active_sw = active_tv->get_parent();
        if ((Gtk::Box*)active_sw->get_parent() == &main_box) {
            return;
        }

        sig_text_view_removed.emit(*(text_view*)active_tv);

        auto active_paned = (paned*)active_sw->get_parent();
        if ((Gtk::Box*)active_paned->get_parent() == &main_box) {
            remove_last_paned(active_paned,
                              (Gtk::ScrolledWindow*)active_sw,
                              (text_view*)active_tv);
            reset_active_text_view(active_tv);
            return;
        }
        remove_other_paned(active_paned,
                           (Gtk::ScrolledWindow*)active_sw,
                           (text_view*)active_tv);
        reset_active_text_view(active_tv);
    }

    void editor_window::remove_last_paned(paned* active_paned,
                                   Gtk::ScrolledWindow* active_sw,
                                   text_view* active_tv) {
        active_paned->remove(*active_sw);
        remove_text_view_from_map(active_tv);
        delete active_tv;
        delete active_sw;

        auto replace_sw = active_paned->get_child1();
        if (replace_sw == nullptr)
            replace_sw = active_paned->get_child2();

        active_paned->remove(*replace_sw);
        main_box.remove(*cmd_bar);
        main_box.remove(*status_bar);
        main_box.remove(*active_paned);
        delete active_paned;

        main_box.pack_start(*replace_sw);
        main_box.pack_start(*status_bar, Gtk::PACK_SHRINK);
        main_box.pack_start(*cmd_bar, Gtk::PACK_SHRINK);

        text_view_count--;

        if (auto sw = dynamic_cast<Gtk::ScrolledWindow*>(replace_sw)) {
            FILE_LOG(LOG_DEBUG2) << "Set focus scrolled win";
            set_focus(*sw->get_child());
        }
        else  if (auto p = dynamic_cast<paned*>(replace_sw)) {
            FILE_LOG(LOG_DEBUG2) << "Set focus paned";
            set_focus(*(paned*)p->get_child1());
        }
    }

    void editor_window::remove_other_paned(paned* active_paned,
                                    Gtk::ScrolledWindow* active_sw,
                                    text_view* active_tv) {
        active_paned->remove(*active_sw);
        remove_text_view_from_map(active_tv);
        delete active_tv;
        delete active_sw;

        auto replace_sw = active_paned->get_child1();
        if (replace_sw == nullptr) {
            replace_sw = active_paned->get_child2();
        }
        active_paned->remove(*replace_sw);

        auto active_paned_parent = (paned*)active_paned->get_parent();
        active_paned_parent->remove(*active_paned);
        delete active_paned;

        if (active_paned_parent->get_child1() == nullptr) {
            active_paned_parent->add1(*replace_sw);
        }
        else {
            active_paned_parent->add2(*replace_sw);
        }

        text_view_count--;

        if (auto sw = dynamic_cast<Gtk::ScrolledWindow*>(replace_sw)) {
            FILE_LOG(LOG_DEBUG2) << "Set focus scrolled win";
            set_focus(*sw->get_child());
        }
        else  if (auto p = dynamic_cast<paned*>(replace_sw)) {
            FILE_LOG(LOG_DEBUG2) << "Set focus paned";
            auto c = (Gtk::ScrolledWindow*)p->get_child1();
            set_focus(*c->get_child());
        }
    }

    void editor_window::switch_to_previous_buffer() {
        if (active_text_view == nullptr) {
            return;
        }
        FILE_LOG(LOG_INFO) << "Switch previous buffer";
        auto bcontainer = text_view_map.find(active_text_view)->second;
        bcontainer->previous();
        signal_buffer_changed().emit();
    }

    void editor_window::switch_to_next_buffer() {
        if (active_text_view == nullptr) {
            return;
        }
        FILE_LOG(LOG_INFO) << "Switch next buffer";
        auto bcontainer = text_view_map.find(active_text_view)->second;
        bcontainer->next();
        signal_buffer_changed().emit();
    }

    void editor_window::switch_paned_up() {
        auto ppaned = get_paned(active_text_view);
        if (ppaned == nullptr) {
            FILE_LOG(LOG_DEBUG2) << "paned == nullptr";
            return;
        }
        if (get_paned_orientation(ppaned) == Gtk::ORIENTATION_VERTICAL
            && is_paned_child2(active_text_view)) {
            set_focus(*find_text_view_in_paned_child1(ppaned));
            return;
        }
        auto parent_paned = ppaned;
        do {
            FILE_LOG(LOG_DEBUG2) << "search paned";
            ppaned = parent_paned;
            parent_paned = get_parent_paned(ppaned);
            if (parent_paned == nullptr) {
                FILE_LOG(LOG_DEBUG2) << "No upper text view";
                cmd_bar->set_cmd_bar_msg("No upper text view");
                return;
            }
        } while (((get_paned_orientation(parent_paned)
                   == Gtk::ORIENTATION_VERTICAL)
                  && is_paned_child1(ppaned))
                 || (get_paned_orientation(parent_paned)
                     == Gtk::ORIENTATION_HORIZONTAL));

    set_focus(*find_text_view_in_paned_child1(parent_paned));
}

    void editor_window::switch_paned_down() {
        auto ppaned = get_paned(active_text_view);
        if (ppaned == nullptr) {
            FILE_LOG(LOG_DEBUG2) << "paned == nullptr";
            return;
        }
        if (get_paned_orientation(ppaned) == Gtk::ORIENTATION_VERTICAL
            && is_paned_child1(active_text_view)) {
            set_focus(*find_text_view_in_paned_child2(ppaned));
            return;
        }
        auto parent_paned = ppaned;
        do {
            FILE_LOG(LOG_DEBUG2) << "Search paned";
            ppaned = parent_paned;
            parent_paned = get_parent_paned(ppaned);
            if (parent_paned == nullptr) {
                FILE_LOG(LOG_DEBUG2) << "No lower text view";
                cmd_bar->set_cmd_bar_msg("No lower text view");
                return;
            }
        } while (((get_paned_orientation(parent_paned)
                   == Gtk::ORIENTATION_VERTICAL)
                  && is_paned_child2(ppaned))
                 || (get_paned_orientation(parent_paned)
                     == Gtk::ORIENTATION_HORIZONTAL));

        set_focus(*find_text_view_in_paned_child2(parent_paned));
    }

    void editor_window::switch_paned_left() {
        auto ppaned = get_paned(active_text_view);
        if (ppaned == nullptr) {
            FILE_LOG(LOG_DEBUG2) << "paned == nullptr\n";
            return;
        }
        if (get_paned_orientation(ppaned) == Gtk::ORIENTATION_HORIZONTAL
            && is_paned_child2(active_text_view)) {
            set_focus(*find_text_view_in_paned_child1(ppaned));
            return;
        }
        auto parent_paned = ppaned;
        do {
            FILE_LOG(LOG_DEBUG2) << "Search paned\n";
            ppaned = parent_paned;
            parent_paned = get_parent_paned(ppaned);
            if (parent_paned == nullptr) {
                FILE_LOG(LOG_DEBUG2) << "No left text view";
                cmd_bar->set_cmd_bar_msg("No left text view");
                return;
            }
        } while (((get_paned_orientation(parent_paned)
                   == Gtk::ORIENTATION_HORIZONTAL)
                  && is_paned_child1(ppaned))
                 || (get_paned_orientation(parent_paned)
                     == Gtk::ORIENTATION_VERTICAL));

        set_focus(*find_text_view_in_paned_child1(parent_paned));
    }

    void editor_window::switch_paned_right() {
        auto ppaned = get_paned(active_text_view);
        if (ppaned == nullptr) {
            FILE_LOG(LOG_DEBUG2) << "paned == nullptr";
            return;
        }
        if (get_paned_orientation(ppaned) == Gtk::ORIENTATION_HORIZONTAL
            && is_paned_child1(active_text_view)) {
            FILE_LOG(LOG_DEBUG2) << "Set focus";
            set_focus(*find_text_view_in_paned_child2(ppaned));
            return;
        }
        auto parent_paned = ppaned;
        do {
            FILE_LOG(LOG_DEBUG2) << "Search paned";
            ppaned = parent_paned;
            parent_paned = get_parent_paned(ppaned);
            if (parent_paned == nullptr) {
                FILE_LOG(LOG_DEBUG2) << "No right text view";
                cmd_bar->set_cmd_bar_msg("No right text view");
                return;
            }
        } while (((get_paned_orientation(parent_paned)
                   == Gtk::ORIENTATION_HORIZONTAL)
                  && is_paned_child2(ppaned))
                 || (get_paned_orientation(parent_paned)
                     == Gtk::ORIENTATION_VERTICAL));

        set_focus(*find_text_view_in_paned_child2(parent_paned));
    }

    void editor_window::remove_active_text_buffer() {
        auto buffer = active_text_view->get_text_buffer();
        if (buffer->get_modified() == false) {
            remove_buffer(buffer);
            if (g_text_buffer_container->size() == 0) {
                FILE_LOG(LOG_INFO) << "Quit editor since all buffers removed";
                quit();
            }
            return;
        }
        set_ask_for_save_file_buffer(buffer);
        ask_for_save_prompt();
    }

    void editor_window::remove_buffer(Glib::RefPtr<text_buffer> buffer) {
        sig_text_buffer_removed.emit(buffer);
        g_text_buffer_container->remove(buffer);
    }

    void editor_window::init_editor() {
        fmode_loader = std::make_shared<mode_loader>(this);
        g_text_buffer_container
                       = std::make_shared<global_text_buffer_container>();
        g_keyboard_map = std::make_shared<keyboard_map>();
        //status_bar     = std::make_shared<status_bar_view>(this);
        //cmd_bar        = std::make_shared<cmd_bar_view>(this);

        setup_main_window();
        setup_welcome_text_buffer();
        setup_main_box();
        setup_signals();
        show_all_children();
    }

    void editor_window::setup_main_window() {
        set_title("flexed");
        set_default_size(900, 300);
    }

    void editor_window::setup_main_box() {
        main_box = Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2);
        add(main_box);

        main_box.pack_start(*create_editing_text_view());
        main_box.pack_start(*status_bar, Gtk::PACK_SHRINK);
        main_box.pack_start(*cmd_bar, Gtk::PACK_SHRINK);
    }

    void editor_window::setup_signals() {
        add_events(Gdk::KEY_PRESS_MASK);
        add_events(Gdk::KEY_RELEASE_MASK);
        add_events(Gdk::STRUCTURE_MASK);
        add_events(Gdk::EventMask::FOCUS_CHANGE_MASK);

        signal_key_press_event()
            .connect(sigc::mem_fun(*this, &editor_window::on_key_pressed), false);
        signal_buffer_changed()
            .connect(sigc::mem_fun(*this, &editor_window::on_buffer_changed));
        signal_delete_event()
            .connect(sigc::mem_fun(*this, &editor_window::on_delete_event), false);
        signal_window_state_event()
            .connect(sigc::mem_fun(
                         *this, &editor_window::on_window_state_event), false);
    }

    void editor_window::setup_welcome_text_buffer() {
        std::string first_buffer_name = "*HOME*";
        first_buffer = Glib::RefPtr<text_buffer>(new text_buffer());
        //first_buffer = create_text_buffer(first_buffer_name);
        first_buffer->set_text("Welcome to the flexed text editor.");
        first_buffer->set_modified(false);
        first_buffer->set_name(first_buffer_name);
        first_buffer->set_keyboard_map(g_keyboard_map);
        auto cur_pos_prop = first_buffer->property_cursor_position();
        cur_pos_prop.signal_changed().connect(
            sigc::mem_fun(*status_bar, &status_bar_view::set_file_stats));
        g_text_buffer_container->add(first_buffer);
    }

    void editor_window::remove_text_view_from_map(text_view* view) {
        auto iter = text_view_map.find(view);
        if (iter == text_view_map.end()) {
            return;
        }
        text_view_map.erase(iter);
    }

    void editor_window::add_text_view_to_map(text_view* view,
                                      orderd_container<
                                      Glib::RefPtr <text_buffer> >&
                                      old_container) {
        text_buffer_container* c
            = new text_buffer_container(g_text_buffer_container, view);
        text_view_map.insert(std::pair<text_view*,
                             text_buffer_container*>(view, c));
        c->take_over_text_buffer_container_list(old_container);

        view->signal_focus_in_event()
            .connect(sigc::mem_fun(*this, &editor_window::on_text_view_focus_changed),
                     false);
    }

    bool editor_window::on_text_view_focus_changed(GdkEventFocus* focus_event) {
        if (start) {
            std::string init_mode_name = "init";
            std::string init_buffer_name = "*INIT*";
            fmode_loader
                ->load_mode(get_active_text_view_buffer(), init_mode_name);
            start = false;
            FILE_LOG(LOG_INFO) << "init mode loading finished";
        }
        active_text_view = (text_view*)get_focus();
        if (active_text_view == nullptr) {
            return false;
        }
        Glib::RefPtr<text_buffer> tbuffer = active_text_view->get_text_buffer();
        keyboard.set_keyboard_map(tbuffer->get_keyboard_map());
        signal_buffer_changed().emit();
        FILE_LOG(LOG_INFO) << "Text view focus changed";
        return false;
    }

    bool editor_window::is_buffer_open(std::string& buffer_name) {
        auto func = [&buffer_name](Glib::RefPtr<text_buffer> buf)->bool {
            if (!buffer_name.compare(buf->get_name())) {
                FILE_LOG(LOG_INFO) << "Buffer name matched";
                return false;
            }
            return true;
        };
        if (g_text_buffer_container->for_each(func)) {
            return false;
        }
        return true;
    }

    Glib::RefPtr<text_buffer> editor_window::create_text_buffer(
        std::string& buffer_name) {
        auto new_buffer = Glib::RefPtr<text_buffer>(new text_buffer());
        new_buffer->set_name(buffer_name);
        g_text_buffer_container->add(
            static_cast< Glib::RefPtr<text_buffer> >(new_buffer));
        new_buffer->set_keyboard_map(g_keyboard_map);

        auto cur_pos_prop = new_buffer->property_cursor_position();
        cur_pos_prop.signal_changed().connect(
            sigc::mem_fun(*status_bar, &status_bar_view::set_file_stats));
        sig_text_buffer_created.emit(new_buffer);
        return new_buffer;
    }

    void editor_window::reset_active_text_view(Gtk::Widget* tv) {
        if (tv == active_text_view) {
            FILE_LOG(LOG_DEBUG1) << "Reset active_text_view";
            active_text_view = nullptr;
        }
    }

    text_view* editor_window::find_text_view_in_paned_child1(paned* ppaned) {
        switch (get_paned_orientation(ppaned)) {
        case Gtk::ORIENTATION_VERTICAL:
            if (is_scrolled_window(ppaned->get_child1())) {
                auto sw = (Gtk::ScrolledWindow*)ppaned->get_child1();
                return (text_view*)sw->get_child();
            }
            return find_text_view_in_paned_child1((paned*)ppaned->get_child1());
            break;

        case Gtk::ORIENTATION_HORIZONTAL:
            if (is_scrolled_window(ppaned->get_child1())) {
                auto sw = (Gtk::ScrolledWindow*)ppaned->get_child1();
                return (text_view*)sw->get_child();
            }
            return find_text_view_in_paned_child1((paned*)ppaned->get_child1());
            break;
        }
    }

    text_view* editor_window::find_text_view_in_paned_child2(paned* ppaned) {
        switch (get_paned_orientation(ppaned)) {
        case Gtk::ORIENTATION_VERTICAL:
            if (is_scrolled_window(ppaned->get_child2())) {
                auto sw = (Gtk::ScrolledWindow*)ppaned->get_child2();
                return (text_view*)sw->get_child();
            }
            return find_text_view_in_paned_child2((paned*)ppaned->get_child2());
            break;

        case Gtk::ORIENTATION_HORIZONTAL:
            if (is_scrolled_window(ppaned->get_child2())) {
                auto sw = (Gtk::ScrolledWindow*)ppaned->get_child2();
                return (text_view*)sw->get_child();
            }
            return find_text_view_in_paned_child2((paned*)ppaned->get_child2());
            break;
        }
    }

    Gtk::Orientation editor_window::get_paned_orientation(paned* ppaned) {
        return ppaned->get_orientation();
    }

    bool editor_window::is_scrolled_window(Gtk::Widget* widget) {
        auto sw = dynamic_cast<Gtk::ScrolledWindow*>(widget);
        if (sw == nullptr) {
            return false;
        }
        return true;
    }

    bool editor_window::is_paned_child1(text_view* text_view) {
        auto sw = dynamic_cast<Gtk::ScrolledWindow*>(text_view->get_parent());
        auto ppaned = dynamic_cast<paned*>(sw->get_parent());
        if (ppaned->get_child1() == sw) {
            return true;
        }
        return false;
    }

    bool editor_window::is_paned_child1(paned* ppaned) {
        auto parent_paned = dynamic_cast<paned*>(ppaned->get_parent());
        if (parent_paned == nullptr) {
            return false;
        }
        if (parent_paned->get_child1() == ppaned) {
            return true;
        }
        return false;
    }

    bool editor_window::is_paned_child2(text_view* text_view) {
        auto sw = dynamic_cast<Gtk::ScrolledWindow*>(text_view->get_parent());
        auto ppaned = dynamic_cast<paned*>(sw->get_parent());
        if (ppaned->get_child2() == sw) {
            return true;
        }
        return false;
    }

    bool editor_window::is_paned_child2(paned* ppaned) {
        auto parent_paned = dynamic_cast<paned*>(ppaned->get_parent());
        if (parent_paned == nullptr) {
            return false;
        }
        if (parent_paned->get_child2() == ppaned) {
            return true;
        }
        return false;
    }

    paned* editor_window::get_paned(text_view* text_view) {
        auto sw = dynamic_cast<Gtk::ScrolledWindow*>(text_view->get_parent());
        auto ppaned = dynamic_cast<paned*>(sw->get_parent());
        return ppaned;
    }

    paned* editor_window::get_parent_paned(paned* ppaned) {
        auto parent_paned = dynamic_cast<paned*>(ppaned->get_parent());
        if (parent_paned == nullptr) {
            return nullptr;
        }
        return parent_paned;
    }

    void editor_window::on_buffer_changed() {
        FILE_LOG(LOG_INFO) << "Signal buffer changed";
        keyboard.set_keyboard_map(
            get_active_text_view_buffer()->get_keyboard_map());
        status_bar->set_file_stats();
        status_bar->set_filename();
    }

    bool editor_window::on_delete_event(GdkEventAny* any_event) {
        FILE_LOG(LOG_DEBUG1) << "User pressed windows X to close";
        if (exit_editor_flag) {
            return false;
        }
        quit();
        return true;
    }

    bool editor_window::on_key_pressed(GdkEventKey* key_event) {
        if (active_text_view == get_focus()) {
            cmd_bar->clear_cmd_bar();
        }
        return keyboard.on_key_pressed(key_event);
    }

    bool
    editor_window::on_window_state_event(GdkEventWindowState* window_state_event) {
        FILE_LOG(LOG_INFO) << "Window state changed";
        /*if (is_fullscreen) {
            is_fullscreen = false;
            return false;
        }
        is_fullscreen = true;*/
        return false;
    }

    bool editor_window::check_buffers_saved() {
        auto buffer_vec = g_text_buffer_container->get_obj_vec();
        for (auto buffer : buffer_vec) {
            FILE_LOG(LOG_DEBUG1) << "Buffer: " << buffer->get_name()
                                 << " modified: " << buffer->get_modified();
            if (buffer->get_modified()) {
                FILE_LOG(LOG_DEBUG1) << "Ask now for save";
                ask_for_save_file_buffer = buffer;
                ask_for_save_prompt();
                return false;
            }
        }
        return true;
    }

    Gtk::ScrolledWindow* editor_window::construct_editing_text_view() {
        auto tv = new text_view();
        auto w = new Gtk::ScrolledWindow();
        w->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
        w->add(*tv);
        sig_text_view_created.emit(*tv);
        return w;
    }
}
