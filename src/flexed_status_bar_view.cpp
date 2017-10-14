#include "flexed_status_bar_view.h"
#include "flexed_editor.h"
#include "flexed_text_view.h"
#include "flexed_text_buffer.h"

namespace flexed{

    status_bar_view::status_bar_view(editor* ed) : text_view() {
        this->ed = ed;
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
    }

    status_bar_view::~status_bar_view() {
    }

    Gtk::Box& status_bar_view::get_main_box() {
        return main_box;
    }

    void status_bar_view::set_filename() {
        std::string name = ed->get_active_text_view_buffer()->get_name();
        filename_text_view.get_buffer()->set_text(name);
    }

    void status_bar_view::set_file_stats() {
        auto buffer = ed->get_active_text_view_buffer();
        auto cursor_mark = buffer->get_insert();
        auto iter = buffer->get_iter_at_mark(cursor_mark);

        int line_num = iter.get_line() + 1;
        int line_count = buffer->get_line_count();
        float fline_num_procent = ((float)line_num / (float)line_count * 100.0);

        int line_num_procent = (int)fline_num_procent;
        std::string stats = std::to_string(line_num_procent) + " %";

        if (buffer->get_modified()) {
            stats += " *";
        }
        file_stats_text_view.get_buffer()->set_text(stats);
    }
}
