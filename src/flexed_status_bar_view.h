/**
 * @file flexed_status_bar_view.h
 */

#ifndef FLEXED_STATUS_BAR_VIEW_H_
#define FLEXED_STATUS_BAR_VIEW_H_

#include <gtkmm.h>
#include <gtksourceviewmm.h>

#include "flexed_text_view.h"

namespace flexed {

    class editor_window;
    class text_buffer;

    class status_bar_view : public text_view {
    public:

        status_bar_view(editor_window* editor_win);

        virtual ~status_bar_view();

        /**
         * Returns the views main box that holds other widgets.
         * @returns Box.
         */
        Gtk::Box& get_main_box();

        /* Sets the file name to the active buffer. */
        void set_filename();

        /** Sets the file stats to the active buffer. */
        void set_file_stats();

    private:

        editor_window* editor_win;

        Gtk::Box main_box;

        Gtk::TextView filename_text_view;

        Gtk::TextView file_stats_text_view;

        Glib::RefPtr<text_buffer> status_bar_buffer;
    };
};

#endif // FLEXED_STATUS_BAR_VIEW_H_
