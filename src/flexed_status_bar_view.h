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

        void set_filename(std::string& name);

    private:

        Glib::RefPtr<text_buffer> status_bar_buffer;
    };
};

#endif // FLEXED_STATUS_BAR_VIEW_H_
