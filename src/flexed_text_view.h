/**
 * @file flexed_text_view.h
 */

#ifndef FLEXED_TEXT_VIEW_H_
#define FLEXED_TEXT_VIEW_H_

#include <gtksourceviewmm.h>

//#include "flexed_text_buffer.h"

namespace flexed {

    class text_buffer;

    /**
     * Represents a text view in the editor.
     */
    class text_view : public Gsv::View {

    public:
        text_view();

        virtual ~text_view();

        /**
         * Returns the editors buffer.
         * The pointer may be a nullptr. Since it is possible that the
         * text views buffer is a Gsv::Buffer, and not a text_buffer.
         */
        Glib::RefPtr<text_buffer> get_text_buffer();
    };
};

#endif // FLEXED_TEXT_VIEW_H_
