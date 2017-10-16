/**
 * @file flexed_cmd_bar_view.h
 */

#ifndef FLEXED_CMD_BAR_VIEW_H_
#define FLEXED_CMD_BAR_VIEW_H_

#include <vector>
#include <gtkmm.h>
#include <gtksourceviewmm.h>
#include <gdkmm.h>

#include "flexed_text_view.h"
#include "flexed_editor.h"

namespace flexed {

    //class text_view;
    class keyboard_map;
    /**
     * Represents the editors command bar.
     */
    class cmd_bar_view : public text_view {

    public:

        cmd_bar_view(editor* ed);

        virtual ~cmd_bar_view();

        /**
         * Prompts the user for input.
         * A callback must be specified which gets called if the user
         * pressed RETURN.
         * To this callback the user input gets routed.
         * @param prompt A String that gets prompted to the user in the
         * command bar.
         */
        template<class C, void (C::*callback)(Glib::ustring)>
        void prompt_cmd_bar(const Glib::ustring& prompt, C* instance) {
            ed->set_focus(*cmd_text_view);
            //auto s = new stub(instance, &class_method_stub<C, callback>);
            prompt_callback = std::make_shared<stub>(
                instance, &class_method_stub<C, callback>);
            cmd_text_view->set_editable(true);
            prompt_text_view->get_buffer()->set_text(prompt);
            ed->get_keyboard_handler().set_keyboard_map(
                cmd_text_view->get_text_buffer()->get_keyboard_map());
        }

        template<class C, void (C::*callback)(Glib::ustring)>
        void prompt_cmd_bar_y_or_n(const Glib::ustring& prompt,
                                   C* instance) {
        }

        /**
         * Aborts current command. Clears command bar.
         */
        void abort_cmd();

        /**
         * Calls the function that is currently saved in prompt_callback.
         * @see prompt_cmd_bar()
         */
        void execute_cmd();

        /**
         * Clears the command bars text.
         */
        void clear_cmd_bar();

        /**
         * Sets a message in the command bar. This message gets deleted if
         * the user presses any key.
         */
        void set_cmd_bar_msg(const Glib::ustring& msg);

        /**
         * Gets called if the cmd text_view looses focus.
         */
        bool on_focus_out_event(/*Gdk::EventFocus& gdk_event*/);

    private:
        typedef void *instance_ptr;
        typedef void (*internal_function)(instance_ptr, Glib::ustring);
        typedef std::pair<instance_ptr, internal_function>  stub;

        editor *ed;

        /**
         * Saves a function that should be called if the user presses
         * enter after a prompt.
         */
        std::shared_ptr<stub> prompt_callback;

        Gtk::Box main_box;

        std::shared_ptr<text_view> prompt_text_view;

        std::shared_ptr<text_view> cmd_text_view;

        std::shared_ptr<keyboard_map> cmd_text_view_kmap;

        Glib::RefPtr<text_buffer> cmd_text_view_buffer;

        /** Turns a member function into internal function stub. */
        template <class C, void (C::*handler)(Glib::ustring)>
        static __attribute__((always_inline))
        void class_method_stub(instance_ptr instance, Glib::ustring data) {
            return (static_cast<C*>(instance)->*handler)(data);
        }

        /** Calls initialization routines. */
        void init(editor* ed);

        /**
         * Resets the prompt callback.
         */
        void reset_prompt_callback();
    };
};

#endif // FLEXED_CMD_BAR_VIEW_H_
