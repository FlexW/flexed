#include "flexed_text_view.h"
#include "flexed_text_buffer.h"

namespace flexed {

    text_view::text_view() {
    }

    text_view::~text_view() {
    }

    Glib::RefPtr<text_buffer> text_view::get_text_buffer() {
        return Glib::RefPtr<text_buffer>::cast_dynamic(get_buffer());
    }

}
