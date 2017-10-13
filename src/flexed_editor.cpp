#include <cstring>
#include <fstream>
#include <iostream>
#include <utility>
#include <string>
#include <boost/filesystem.hpp>

#include "flexed_editor.h"
#include "flexed_paned.h"
#include "flexed_status_bar_view.h"
#include "flexed_cmd_bar_view.h"
#include "flexed_paned.h"
#include "flexed_text_view.h"
#include "keyboard_map.h"

namespace flexed {

    editor::editor() {
        init_editor();
    }

    editor::~editor() {
        //delete status_bar;
    }
/*
    editor* editor::get_instance() {
        static editor instance;
        return &instance;
    }
*/
    sigc::signal<void>& editor::signal_buffer_changed() {
        return sig_buffer_changed;
    }

    sigc::signal<text_view&>& editor::signal_text_view_created() {
        return sig_text_view_created;
    }

    sigc::signal<text_view&>& editor::signal_text_view_removed() {
        return sig_text_view_removed;
    }
/*
    Glib::RefPtr<text_buffer> editor::get_status_bar_buffer() {
        auto buffer = status_bar->get_buffer();
        auto tbuffer = Glib::RefPtr<text_buffer>::cast_dynamic(buffer);
        return tbuffer;
    }
*/
    Glib::RefPtr<text_buffer> editor::get_active_text_view_buffer() {
        auto buffer = active_text_view->get_buffer();
        auto tbuffer = Glib::RefPtr<text_buffer>::cast_dynamic(buffer);
        return tbuffer;
    }

    std::shared_ptr<mode_loader> editor::get_mode_loader() {
        return fmode_loader;
    }

    Gtk::Box* editor::get_main_box() {
        return &main_box;
    }

    std::shared_ptr<cmd_bar_view> editor::get_cmd_bar() {
        return cmd_bar;
    }

    std::shared_ptr<status_bar_view> editor::get_status_bar() {
        return status_bar;
    }
/*
    Glib::RefPtr<text_buffer> editor::get_cmd_bar_buffer() {
        return cmd_bar->get_buffer();
    }
*/
    std::shared_ptr<keyboard_map> editor::get_keyboard_map() {
        return g_keyboard_map;
    }

    std::list<text_view*> editor::get_text_views() {
        std::list<text_view*> list;
        for (auto p : text_view_map) {
            list.push_back(p.first);
        }
        return list;
    }

    std::shared_ptr<global_text_buffer_container>
    editor::get_global_text_buffer_container() {
        return g_text_buffer_container;
    }

    text_view* editor::get_active_text_view() {
        return active_text_view;
    }

    keyboard_handler& editor::get_keyboard_handler() {
        return keyboard;
    }

    void editor::set_divider_from_active_paned(int pos) {
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
        g_print("No Paned. Can not place divider\n");
    }
/*
    void editor::set_cmd_bar_msg(const Glib::ustring& msg) {
        cmd_bar.get_buffer()->set_text(msg);
    }
*/
    void
    editor::set_ask_for_save_file_buffer(Glib::RefPtr<text_buffer> buffer) {
        ask_for_save_file_buffer = buffer;
    }

    void editor::set_text_view_focus(text_view* text_view) {
    }

    void editor::open_file_prompt() {
        cmd_bar->prompt_cmd_bar<editor, &editor::open_file>("Open file",
                                                            this);
    }

    void editor::open_file(Glib::ustring fname) {
        if (!fname.compare("")) {
            g_print("No file name\n");
            return;
        }
        g_print("Open file: %s\n", fname.c_str());
        auto fname_path = boost::filesystem::path(fname);
        auto abs_path = boost::filesystem::absolute(fname_path);
        auto abs_path_str = abs_path.string();
        if (is_buffer_open(abs_path_str)) {
            g_print("Buffer already open\n");
            return;
        }
        auto file = new std::ifstream(fname);
        if (!file) {
            g_print("Error opening file\n");
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
        status_bar->set_file_stats();
    }

        void editor::load_mode_prompt() {
        cmd_bar->prompt_cmd_bar<editor, &editor::load_mode>("Load mode",
                                                            this);
    }

    void editor::load_mode(Glib::ustring name) {
        g_print("try to load mode now\n");
        if (fmode_loader->load_mode(
                (std::string&)get_active_text_view_buffer()->get_name(),
                (std::string&)name)) {
            get_active_text_view_buffer()->add_mode((std::string&)name);
            cmd_bar->set_cmd_bar_msg(name + " loaded");
        }
    }

    void editor::call_mode_function_prompt() {
        cmd_bar->prompt_cmd_bar<editor, &editor::call_mode_function>(
            "Execute", this);
    }

    void editor::call_mode_function(Glib::ustring name) {
        g_print("call function now\n");
        auto mode_list = get_active_text_view_buffer()->get_mode_list();
        for(auto mode : mode_list) {
            g_print("mode in mode_list: %s\n", mode.c_str());
        }
        fmode_loader->call_function(mode_list, (std::string&)name);
    }

    void editor::unload_mode_prompt() {
        cmd_bar->prompt_cmd_bar<editor, &editor::unload_mode>(
            "Unload mode", this);
    }

    void editor::unload_mode(Glib::ustring name) {
        g_print("unload mode now\n");
        get_active_text_view_buffer()->unset_mode((std::string&)name);
        fmode_loader->unload_mode(
            (std::string&)get_active_text_view_buffer()->get_name(),
            (std::string&)name);
        cmd_bar->set_cmd_bar_msg(name + " unloaded");
    }

    void editor::ask_for_save_prompt() {
        cmd_bar->prompt_cmd_bar<editor, &editor::ask_for_save>(
            ask_for_save_file_buffer->get_name()
            + " | File modified. Save? [y/n]", this);
    }

    void editor::ask_for_save(Glib::ustring y_or_n) {
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
            g_print("not y or no \n");
            ask_for_save_prompt();
            return;
        }
        g_print("exit_editor_flag: %d\n", exit_editor_flag);
        if (exit_editor_flag) {
            g_print("call quit again\n");
            quit();
        }
    }

    int editor::check_yes_or_no(std::string y_or_n) {
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

    void editor::save_file() {
        save_file(get_active_text_view_buffer());
    }

    void editor::save_file(Glib::RefPtr<text_buffer> buffer) {
        auto fname = buffer->get_name();
        g_print("save file as: %s\n", fname.c_str());
        std::ofstream tfile(fname);
        if (tfile.is_open()) {
            tfile << buffer->get_text();
            tfile.close();
        cmd_bar->set_cmd_bar_msg("Save successfully");
            g_print("file saved.\n");
            get_active_text_view_buffer()->set_modified(false);
            status_bar->set_file_stats();
            return;
        }
        cmd_bar->set_cmd_bar_msg("Save failed!");
        g_print("file not saved\n");
    }

    void editor::quit() {
        g_print("quit\n");
        exit_editor_flag = true;
        if (!check_buffers_saved()) {
            return;
        }
        close();
    }
/*
    void editor::clear_cmd_bar() {
        cmd_bar.get_buffer()->set_text("");
        }*/
/*
    void editor::abort_cmd() {
        if (cmd_bar_callback_stub != nullptr) {
            free(cmd_bar_callback_stub);
            cmd_bar_callback_stub = nullptr;
        }
        clear_cmd_bar();
        set_focus(*active_text_view);
    }
*//*
    void editor::focus_cmd_bar() {
        keyboard.set_keyboard_map(cmd_bar_keyboard_map);
        cmd_bar.get_buffer()->set_text("");
        set_focus(cmd_bar);
    }
  */
    void editor::insert_paned_horizontal() {
        insert_paned(Gtk::ORIENTATION_HORIZONTAL);
    }

    void editor::insert_paned_vertical() {
        insert_paned(Gtk::ORIENTATION_VERTICAL);
    }

    void editor::insert_paned(Gtk::Orientation orientation) {
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

    void editor::insert_first_paned(Gtk::ScrolledWindow *active,
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

    void editor::insert_other_paned(Gtk::ScrolledWindow* active,
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

    void editor::set_divider_from_active_paned_middle() {
        set_divider_from_active_paned(50);
    }

    Gtk::ScrolledWindow* editor::create_editing_text_view(text_view* old_tv) {
        auto w = construct_editing_text_view();
        add_text_view_to_map((text_view*)w->get_child(),
                             *text_view_map.find(old_tv)->second);
        return w;
    }

    Gtk::ScrolledWindow* editor::create_editing_text_view() {
        auto w = construct_editing_text_view();
        add_text_view_to_map((text_view*)w->get_child(),
                             *g_text_buffer_container);
        return w;
    }

    paned* editor::create_paned(Gtk::ScrolledWindow* left_side,
                                Gtk::Orientation orientation) {
        paned* p = new paned(orientation);
        p->add1(*left_side);
        p->add2(*create_editing_text_view((text_view*)left_side->get_child()));
        g_print("Created paned\n");
        return p;
    }

    void editor::remove_paned() {
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

    void editor::remove_last_paned(paned* active_paned,
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
            g_print("set focus scrolled win\n");
            set_focus(*sw->get_child());
        }
        else  if (auto p = dynamic_cast<paned*>(replace_sw)) {
            g_print("set focus paned\n");
            set_focus(*(paned*)p->get_child1());
        }
    }

    void editor::remove_other_paned(paned* active_paned,
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
            g_print("set focus scrolled win\n");
            set_focus(*sw->get_child());
        }
        else  if (auto p = dynamic_cast<paned*>(replace_sw)) {
            g_print("set focus paned\n");
            auto c = (Gtk::ScrolledWindow*)p->get_child1();
            set_focus(*c->get_child());
        }
    }

    void editor::switch_to_previous_buffer() {
        if (active_text_view == nullptr) {
            return;
        }
        g_print("previous buffer\n");
        auto bcontainer = text_view_map.find(active_text_view)->second;
        bcontainer->previous();
        signal_buffer_changed().emit();
    }

    void editor::switch_to_next_buffer() {
        if (active_text_view == nullptr) {
            return;
        }
        g_print("next buffer\n");
        auto bcontainer = text_view_map.find(active_text_view)->second;
        bcontainer->next();
        signal_buffer_changed().emit();
    }

    void editor::switch_paned_up() {
        auto ppaned = get_paned(active_text_view);
        if (ppaned == nullptr) {
            g_print("paned == nullptr\n");
            return;
        }
        if (get_paned_orientation(ppaned) == Gtk::ORIENTATION_VERTICAL
            && is_paned_child2(active_text_view)) {
            set_focus(*find_text_view_in_paned_child1(ppaned));
            return;
        }
        auto parent_paned = ppaned;
        do {
            g_print("search paned\n");
            ppaned = parent_paned;
            parent_paned = get_parent_paned(ppaned);
            if (parent_paned == nullptr) {
                g_print("no upper text view\n");
                return;
            }
        } while (((get_paned_orientation(parent_paned)
                   == Gtk::ORIENTATION_VERTICAL)
                  && is_paned_child1(ppaned))
                 || (get_paned_orientation(parent_paned)
                     == Gtk::ORIENTATION_HORIZONTAL));

    set_focus(*find_text_view_in_paned_child1(parent_paned));
}

    void editor::switch_paned_down() {
        auto ppaned = get_paned(active_text_view);
        if (ppaned == nullptr) {
            g_print("paned == nullptr\n");
            return;
        }
        if (get_paned_orientation(ppaned) == Gtk::ORIENTATION_VERTICAL
            && is_paned_child1(active_text_view)) {
            set_focus(*find_text_view_in_paned_child2(ppaned));
            return;
        }
        auto parent_paned = ppaned;
        do {
            g_print("search paned\n");
            ppaned = parent_paned;
            parent_paned = get_parent_paned(ppaned);
            if (parent_paned == nullptr) {
                g_print("no lower text view\n");
                return;
            }
        } while (((get_paned_orientation(parent_paned)
                   == Gtk::ORIENTATION_VERTICAL)
                  && is_paned_child2(ppaned))
                 || (get_paned_orientation(parent_paned)
                     == Gtk::ORIENTATION_HORIZONTAL));

        set_focus(*find_text_view_in_paned_child2(parent_paned));
    }

    void editor::switch_paned_left() {
        auto ppaned = get_paned(active_text_view);
        if (ppaned == nullptr) {
            g_print("paned == nullptr\n");
            return;
        }
        if (get_paned_orientation(ppaned) == Gtk::ORIENTATION_HORIZONTAL
            && is_paned_child2(active_text_view)) {
            set_focus(*find_text_view_in_paned_child1(ppaned));
            return;
        }
        auto parent_paned = ppaned;
        do {
            g_print("search paned\n");
            ppaned = parent_paned;
            parent_paned = get_parent_paned(ppaned);
            if (parent_paned == nullptr) {
                g_print("no left text view\n");
                return;
            }
        } while (((get_paned_orientation(parent_paned)
                   == Gtk::ORIENTATION_HORIZONTAL)
                  && is_paned_child1(ppaned))
                 || (get_paned_orientation(parent_paned)
                     == Gtk::ORIENTATION_VERTICAL));

        set_focus(*find_text_view_in_paned_child1(parent_paned));
    }

    void editor::switch_paned_right() {
        auto ppaned = get_paned(active_text_view);
        if (ppaned == nullptr) {
            g_print("paned == nullptr\n");
            return;
        }
        if (get_paned_orientation(ppaned) == Gtk::ORIENTATION_HORIZONTAL
            && is_paned_child1(active_text_view)) {
            g_print("set focus\n");
            set_focus(*find_text_view_in_paned_child2(ppaned));
            return;
        }
        auto parent_paned = ppaned;
        do {
            g_print("search paned\n");
            ppaned = parent_paned;
            parent_paned = get_parent_paned(ppaned);
            if (parent_paned == nullptr) {
                g_print("no right text view\n");
                return;
            }
        } while (((get_paned_orientation(parent_paned)
                   == Gtk::ORIENTATION_HORIZONTAL)
                  && is_paned_child2(ppaned))
                 || (get_paned_orientation(parent_paned)
                     == Gtk::ORIENTATION_VERTICAL));

        set_focus(*find_text_view_in_paned_child2(parent_paned));
    }

    void editor::init_editor() {
        g_print("try init editor\n");
        fmode_loader = std::make_shared<mode_loader>(this);
        g_text_buffer_container
                       = std::make_shared<global_text_buffer_container>();
        g_keyboard_map = std::make_shared<keyboard_map>();
//cmd_bar_buffer = Glib::RefPtr<text_buffer>(new text_buffer(this));
        status_bar     = std::make_shared<status_bar_view>(this);
        cmd_bar        = std::make_shared<cmd_bar_view>(this);

        setup_main_window();
        setup_welcome_text_buffer();
        setup_main_box();
        setup_signals();
        show_all_children();
        g_print("init editor finished\n");
    }

    void editor::setup_main_window() {
        set_title("flexed");
        set_default_size(900, 300);
        g_print("setup main window finished\n");
    }

    void editor::setup_main_box() {
        main_box = Gtk::Box(Gtk::ORIENTATION_VERTICAL, 2);
        add(main_box);

        main_box.pack_start(*create_editing_text_view());
        main_box.pack_start(*status_bar, Gtk::PACK_SHRINK);
        main_box.pack_start(*cmd_bar, Gtk::PACK_SHRINK);
        g_print("setup main box finished\n");
    }

    void editor::setup_signals() {
        add_events(Gdk::KEY_PRESS_MASK);
        add_events(Gdk::KEY_RELEASE_MASK);
        add_events(Gdk::STRUCTURE_MASK);
        add_events(Gdk::EventMask::FOCUS_CHANGE_MASK);

        signal_key_press_event()
            .connect(sigc::mem_fun(*this, &editor::on_key_pressed), false);
        signal_buffer_changed()
            .connect(sigc::mem_fun(*this, &editor::on_buffer_changed));
        g_print("setup signals finished\n");
    }

    void editor::setup_welcome_text_buffer() {
        std::string first_buffer_name("*HOME*");
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

        g_print("setup welcome buffer finished\n");
    }

    void editor::remove_text_view_from_map(text_view* view) {
        auto iter = text_view_map.find(view);
        if (iter == text_view_map.end()) {
            return;
        }
        text_view_map.erase(iter);
    }

    void editor::add_text_view_to_map(text_view* view,
                                      orderd_container<
                                      Glib::RefPtr <text_buffer> >&
                                      old_container) {
        text_buffer_container* c
            = new text_buffer_container(g_text_buffer_container, view);
        text_view_map.insert(std::pair<text_view*,
                             text_buffer_container*>(view, c));
        c->take_over_text_buffer_container_list(old_container);

        view->signal_focus_in_event()
            .connect(sigc::mem_fun(*this, &editor::on_text_view_focus_changed),
                     false);
    }

    bool editor::on_text_view_focus_changed(GdkEventFocus* focus_event) {
        if (start) {
            g_print("editor instance this: %p\n", this);
            std::string init_mode_name = "init";
            std::string init_buffer_name = "*INIT*";
            fmode_loader->load_mode(init_buffer_name, init_mode_name);
            start = false;
            g_print("init mode loading finished\n");
            //fmode_loader->unload_mode(init_buffer_name, init_mode_name);
        }
        active_text_view = (text_view*)get_focus();
        g_print("active_text_view addr: %p\n", active_text_view);
        if (active_text_view == nullptr) {
            g_print("returning: active_text_view == nullptr\n");
            return false;
        }
        g_print("here\n");
        Glib::RefPtr<text_buffer> tbuffer = active_text_view->get_text_buffer();
        //auto tbuffer = Glib::RefPtr<text_buffer>::cast_dynamic(buffer);
        keyboard.set_keyboard_map(tbuffer->get_keyboard_map());
        signal_buffer_changed().emit();
        g_print("TextView focus changed\n");
        return false;
    }

    bool editor::is_buffer_open(std::string& buffer_name) {
        auto func = [&buffer_name](Glib::RefPtr<text_buffer> buf)->bool {
            if (!buffer_name.compare(buf->get_name())) {
                g_print("buffer name matched\n");
                return false;
            }
            return true;
        };
        if (g_text_buffer_container->for_each(func)) {
            return false;
        }
        return true;
    }

    Glib::RefPtr<text_buffer> editor::create_text_buffer(
        std::string& buffer_name) {
        auto new_buffer = Glib::RefPtr<text_buffer>(new text_buffer());
        new_buffer->set_name(buffer_name);
        g_text_buffer_container->add(
            static_cast< Glib::RefPtr<text_buffer> >(new_buffer));
        new_buffer->set_keyboard_map(g_keyboard_map);

        auto cur_pos_prop = new_buffer->property_cursor_position();
        cur_pos_prop.signal_changed().connect(
            sigc::mem_fun(*status_bar, &status_bar_view::set_file_stats));
        return new_buffer;
    }

    void editor::reset_active_text_view(Gtk::Widget* tv) {
        if (tv == active_text_view) {
            g_print("reset active_text_view.\n");
            active_text_view = nullptr;
        }
    }

    text_view* editor::find_text_view_in_paned_child1(paned* ppaned) {
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

    text_view* editor::find_text_view_in_paned_child2(paned* ppaned) {
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

    Gtk::Orientation editor::get_paned_orientation(paned* ppaned) {
        return ppaned->get_orientation();
    }

    bool editor::is_scrolled_window(Gtk::Widget* widget) {
        auto sw = dynamic_cast<Gtk::ScrolledWindow*>(widget);
        if (sw == nullptr) {
            return false;
        }
        return true;
    }

    bool editor::is_paned_child1(text_view* text_view) {
        auto sw = dynamic_cast<Gtk::ScrolledWindow*>(text_view->get_parent());
        auto ppaned = dynamic_cast<paned*>(sw->get_parent());
        if (ppaned->get_child1() == sw) {
            return true;
        }
        return false;
    }

    bool editor::is_paned_child1(paned* ppaned) {
        auto parent_paned = dynamic_cast<paned*>(ppaned->get_parent());
        if (parent_paned == nullptr) {
            return false;
        }
        if (parent_paned->get_child1() == ppaned) {
            return true;
        }
        return false;
    }

    bool editor::is_paned_child2(text_view* text_view) {
        auto sw = dynamic_cast<Gtk::ScrolledWindow*>(text_view->get_parent());
        auto ppaned = dynamic_cast<paned*>(sw->get_parent());
        if (ppaned->get_child2() == sw) {
            return true;
        }
        return false;
    }

    bool editor::is_paned_child2(paned* ppaned) {
        auto parent_paned = dynamic_cast<paned*>(ppaned->get_parent());
        if (parent_paned == nullptr) {
            return false;
        }
        if (parent_paned->get_child2() == ppaned) {
            return true;
        }
        return false;
    }

    paned* editor::get_paned(text_view* text_view) {
        auto sw = dynamic_cast<Gtk::ScrolledWindow*>(text_view->get_parent());
        auto ppaned = dynamic_cast<paned*>(sw->get_parent());
        return ppaned;
    }

    paned* editor::get_parent_paned(paned* ppaned) {
        auto parent_paned = dynamic_cast<paned*>(ppaned->get_parent());
        if (parent_paned == nullptr) {
            return nullptr;
        }
        return parent_paned;
    }
/*
    void editor::execute_cmd() {
        if (cmd_bar_callback_stub != nullptr) {
            g_print("callback execute cmd bar\n");
            auto text = cmd_bar.get_buffer()->get_text(true);
            auto pos = text.find(CMD_BAR_PROMPT_SEPERATOR);
            auto text_sub = text.substr(pos + CMD_BAR_PROMPT_SEPERATOR_LEN);
            //cmd_bar_callback_stub->second(cmd_bar_callback_stub->first,
            //                              text_sub);
            free(cmd_bar_callback_stub);
        }
        clear_cmd_bar();
        set_focus(*active_text_view);
        cmd_bar_callback_stub = nullptr;
    }
*/
    void editor::on_buffer_changed() {
        g_print("signal buffer changed\n");
        //status_bar->set_filename(
        //    (std::string&)get_active_text_view_buffer()->get_name());
        //get_status_bar_buffer()
        //    ->set_text("    " + get_active_text_view_buffer()->get_name());
        keyboard.set_keyboard_map(
            get_active_text_view_buffer()->get_keyboard_map());
        status_bar->set_file_stats();
        status_bar->set_filename();
    }

    bool editor::on_key_pressed(GdkEventKey* key_event) {
        //status_bar->set_file_stats();
        if (active_text_view == get_focus()) {
            cmd_bar->clear_cmd_bar();
        }
        return keyboard.on_key_pressed(key_event);
    }

    bool editor::check_buffers_saved() {
        auto buffer_vec = g_text_buffer_container->get_obj_vec();
        for (auto buffer : buffer_vec) {
            g_print("buffer: %s modified: %d\n", buffer->get_name().c_str(),
                    buffer->get_modified());
            if (buffer->get_modified()) {
                g_print("ask now for save\n");
                ask_for_save_file_buffer = buffer;
                ask_for_save_prompt();
                return false;
            }
        }
        return true;
    }

    Gtk::ScrolledWindow* editor::construct_editing_text_view() {
        auto tv = new text_view();
        auto w = new Gtk::ScrolledWindow();
        w->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
        w->add(*tv);
        return w;
    }
}
