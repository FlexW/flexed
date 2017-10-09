#include "flexed_status_bar_view.h"

namespace flexed{

    status_bar_view::status_bar_view() {
        status_bar_buffer = Glib::RefPtr<text_buffer>();
        set_buffer(status_bar_buffer);
        set_editable(false);
        add(main_box);

        main_box.pack_start(filename_text_view, Gtk::PACK_EXPAND_WIDGET);
        main_box.pack_start(file_stats_text_view, Gtk::PACK_EXPAND_WIDGET);
        show_all_children();

        set_top_margin(2);
        set_bottom_margin(3);

        file_stats_text_view.set_editable(false);
        file_stats_text_view.set_top_margin(2);
        file_stats_text_view.set_bottom_margin(2);
        file_stats_text_view.set_right_margin(7);
        file_stats_text_view.set_left_margin(7);

        filename_text_view.set_editable(false);
        filename_text_view.set_top_margin(2);
        filename_text_view.set_bottom_margin(2);
        filename_text_view.set_right_margin(7);
        filename_text_view.set_left_margin(30);

        std::string s = "file stats";
        set_file_stats(s);
    }

    status_bar_view::~status_bar_view() {
    }

    Gtk::Box& status_bar_view::get_main_box() {
        return main_box;
    }

    void status_bar_view::set_filename(std::string& name) {
        filename_text_view.get_buffer()->set_text(name);
    }

    void status_bar_view::set_file_stats(std::string& stats) {
        file_stats_text_view.get_buffer()->set_text(stats);
    }
}
