/**
 * @file flexed_status_bar_view.h
 */

#ifndef FLEXED_STATUS_BAR_VIEW_H_
#define FLEXED_STATUS_BAR_VIEW_H_

#include <gtksourceviewmm.h>

#include "flexed_text_buffer.h"

namespace flexed {

    class text_buffer;

    class status_bar_view : public Gsv::View {
    public:

        status_bar_view();

        virtual ~status_bar_view();

        Gtk::Box& get_main_box();

        void set_filename();

        void set_file_stats();

    private:
        Gtk::Box main_box;
        Gtk::TextView filename_text_view;
        Gtk::TextView file_stats_text_view;

        Glib::RefPtr<text_buffer> status_bar_buffer;
    };
};

#endif // FLEXED_STATUS_BAR_VIEW_H_
