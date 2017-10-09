/**
 * @file flexed_editor.h
 */

#ifndef FLEXED_EDITOR_H_
#define FLEXED_EDITOR_H_

#include <functional>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <gtkmm.h>
#include <gtksourceviewmm.h>
#include "flexed_text_buffer.h"
#include "flexed_paned.h"
#include "flexed_mode_loader.h"
#include "keyboard_handler.h"
#include "orderd_container.h"
#include "flexed_global_text_buffer_container.h"
#include "flexed_text_buffer_container.h"

#define CMD_BAR_PROMPT_SEPERATOR_LEN 4
#define CMD_BAR_PROMPT_SEPERATOR     " >> "

namespace flexed {

    /**
     * Represents the editor window.
     */
    class editor : public Gtk::Window {

    public:

        static editor& get_instance();

        virtual ~editor();

        editor(editor const&) = delete;

        void operator = (editor const&) = delete;

        /**
         * Gets the signal buffer changed. This signal will be emitted,
         * every time a buffer changes.
         * Sometimes the changed buffer will be the same.
         * @returns Signal buffer changed.
         */
        sigc::signal<void>& signal_buffer_changed();

        /**
         * Gets the signal text view created.
         * This signal will be emitted,
         * every time a text view gets created.
         * @returns Signal text view created.
         */
        sigc::signal<Gsv::View&>& signal_text_view_created();

        /**
         * Gets the signal text view removed.
         * This signal will be emitted,
         * every time a text view gets removed.
         * @returns Signal text view removed.
         */
        sigc::signal<Gsv::View&>& signal_text_view_removed();

        /**
         * Gets the buffer of the status bar.
         * @returns Buffer.
         */
        Glib::RefPtr<Gtk::TextBuffer> get_status_bar_buffer();

        /**
         * Gets the buffer of the active text view.
         * @returns Buffer.
         */
        Glib::RefPtr<text_buffer> get_active_text_view_buffer();

        /**
         * Gets input from the command bar.
         * A callback must be specified which gets called if the user pressed RETURN.
         * To this callback the user input gets routed.
         * @param prompt A String that gets prompted to the user in the command bar.
         */
        template<class C, void (C::*callback)(Glib::ustring)>
        void get_cmd_bar_input(const Glib::ustring& prompt, C* instance) {
            focus_cmd_bar();
            auto s = new stub(instance, &class_method_stub<C, callback>);
            cmd_bar.get_buffer()->insert_with_tag(cmd_bar.get_buffer()->begin(),
                                                  prompt + CMD_BAR_PROMPT_SEPERATOR,
                                                  no_editable_tag);
            cmd_bar_callback_stub = s;
        }

        /**
         * Returns the editors mode loader.
         * @returns Mode loader.
         */
        mode_loader& get_mode_loader();

        /**
         * Returns the application main_box.
         * This main_box contains all other widgets.
         * @returns main_box.
         */
        Gtk::Box* get_main_box();

        /**
         * Returns the editors command bar.
         * @returns Command bar.
         */
        Gsv::View* get_cmd_bar();

        /**
         * Returns the editors status bar.
         * @returns Status bar.
         */
        Gsv::View* get_status_bar();

        /**
         * Gets the command bars buffer.
         * @returns Command bar buffer.
         */
        Glib::RefPtr<text_buffer> get_cmd_bar_buffer();

        /**
         * Returns the editors keyboard map.
         * @returns Keyboard map.
         */
        std::shared_ptr<keyboard_map> get_keyboard_map();

        /**
         * Sets the divider of the active paned widget.
         * @param pos Divider position. Must be between 0 and 100.
         */
        void set_divider_from_active_paned(int pos);

        /**
         * Sets a message in the command bar.
         */
        void set_cmd_bar_msg(const Glib::ustring& msg);

        /**
         * Opens a text file.
         * Creates a buffer for the file,
         * takes care that not one file is opend twice.
         * Gets input string from cmd_bar text view.
         */
        void open_file_prompt();

        /**
         * Opens a text file.
         * @param fname File name.
         */
        void open_file(Glib::ustring fname);

        /**
         * Prompts the user for loading a mode.
         */
        void load_mode_prompt();

        /**
         * Loads a mode after the user typed in a mode.
         * @name Mode name.
         */
        void load_mode(Glib::ustring name);

        /**
         * Prompts the user for calling a function of a mode.
         */
        void call_mode_function_prompt();

        /**
         * Calls a function of mode after the user typed in a mode.
         * @name Function name
         */
        void call_mode_function(Glib::ustring name);

        /**
         * Prompts the user for unloading a mode.
         */
        void unload_mode_prompt();

        /**
         * Unloads a mode after the user typed in one.
         * @param name Mode name.
         */
        void unload_mode(Glib::ustring name);

        /**
         * Saves a file under current name.
         */
        void save_file();

        /**
         * Exits the editor.
         */
        void quit();

        /**
         * Clears the command bar.
         */
        void clear_cmd_bar();

        /**
         * Aborts current command. Clears command bar.
         */
        void abort_cmd();

        /**
         * Focuses the command bar.
         */
        void focus_cmd_bar();

                /**
         * Inserts new paned in horizontal direction.
         */
        void insert_paned_horizontal();

        /**
         * Inserts new paned in vertical direction.
         */
        void insert_paned_vertical();

        /**
         * Inserts a new paned widget.
         * @param orientation Split direction.
         */
        void insert_paned(Gtk::Orientation orientation);

        /**
         * Inserts the first paned widget if no paned wiget has insert yet.
         * This method gets called from insert_paned().
         * @param active Active ScrolledWindow.
         * @param Split direction.
         */
        void insert_first_paned(Gtk::ScrolledWindow* active,
                                Gtk::Orientation orientation);

        /**
         * Inserts any other paned widget if a paned wiget has already placed.
         * This method gets called from insert_paned().
         * @param active Active ScrolledWindow.
         * @param Split direction.
         */
        void insert_other_paned(Gtk::ScrolledWindow* active,
                                Gtk::Orientation orientation);

        /**
         * Places the divider from active paned widget in the middle.
         */
        void set_divider_from_active_paned_middle();

        /**
         * Creates a new text view with scrolled window.
         * @param old_tv The TextView that created this view.
         * Needed for taking over buffer_container.
         * @returns Text view in scrolled window.
         */
        Gtk::ScrolledWindow* create_text_view(Gsv::View* old_tv);

        /**
         * Creates a new text view with scrolled window.
         * Takes g_text_buffer_container to over take buffer list.
         * @returns Text view in scrolled window.
         */
        Gtk::ScrolledWindow* create_text_view();

        /**
         * Creates a new paned layout container, with two text views.
         * @param left_side Widget on the left side.
         * @param orientation Horizontal or vertical splitting.
         */
        paned* create_paned(Gtk::ScrolledWindow* left_side,
                            Gtk::Orientation orientation);

        /**
         * If there are paned widgets, then this method will remove the active one.
         */
        void remove_paned();

        /**
         * Removes the last paned widget.
         * This method gets called from remove_paned().
         * @param active_paned Active paned widget.
         * @param active_sw Active ScrolledWindow.
         * @param active_tv Active TextView.
         */
        void remove_last_paned(paned* active_paned,
                               Gtk::ScrolledWindow* active_sw,
                               Gsv::View* active_tv);

        /**
         * Removes any other paned widget.
         * This method gets called from remove_paned().
         * @param active_paned Active paned widget.
         * @param active_sw Active ScrolledWindow.
         * @param active_tv Active TextView.
         */
        void remove_other_paned(paned* active_paned,
                                Gtk::ScrolledWindow* active_sw,
                                Gsv::View* active_tv);

        /**
         * Switches active editor_text_view to the previous buffer.
         */
        void switch_to_previous_buffer();

        /**
         * Switches active editor_text_view to the next buffer.
         */
        void switch_to_next_buffer();

        /**
         * If a text view upper the active text view exists,
         * it switches to that.
         */
        void switch_paned_up();

        /**
         * If a text view lower the active text view exists,
         * it switches to that.
         */
        void switch_paned_down();

        /**
         * If a text view left of the active text view exists,
         * it switches to that.
         */
        void switch_paned_left();

        /**
         * If a text view right of the active text view exists,
         * it switches to that.
         */
        void switch_paned_right();

    private:
        typedef void *instance_ptr;
        typedef void (*internal_function)(instance_ptr, Glib::ustring);
        typedef std::pair<instance_ptr, internal_function>  stub;

        /** Indicates if the editor is starting. */
        bool start = true;

        mode_loader fmode_loader;

        sigc::signal<void> sig_buffer_changed;

        sigc::signal<Gsv::View&> sig_text_view_created;

        sigc::signal<Gsv::View&> sig_text_view_removed;

        stub* cmd_bar_callback_stub = nullptr;
        //std::function<void(Glib::ustring)> cmd_bar_callback = nullptr;
        keyboard_handler keyboard;

        /** Global keyboard map. */
        std::shared_ptr<keyboard_map> g_keyboard_map;

        /** Command bar keyboard map. */
        std::shared_ptr<keyboard_map> cmd_bar_keyboard_map;

        Gtk::Box  main_box;
        Gsv::View cmd_bar;
        Gsv::View status_bar;

        Glib::RefPtr<Gsv::Buffer::Tag> no_editable_tag;

        Glib::RefPtr<text_buffer> first_buffer;
        Glib::RefPtr<text_buffer>   cmd_bar_buffer;

        /** Counts current opend text views. */
        int text_view_count = 1;

        /** Stores all open buffers.*/
        std::shared_ptr<global_text_buffer_container> g_text_buffer_container;

        /** Stores all open TextViews with their buffer container. */
        std::map<Gsv::View*, text_buffer_container*> text_view_map;

        /** Tracks active text view. This may be a nullptr. */
        Gsv::View* active_text_view = nullptr;

        editor();

        /**
         * This is the editors entry point.
         * Set ups the window for editing.
         */
        void init_editor();

        bool on_key_pressed(GdkEventKey* event);

        /**
         * Gets called when the active buffer changes.
         */
        void on_buffer_changed();

        /**
         * Removes a text view entry from the text_view_map.
         * Frees the buffer_container. TextView gets not freed.
         * @param view TextView to remove.
         */
        void remove_text_view_from_map(Gsv::View* view);

        /**
         * Adds a new TextView to map. Creates text_buffer_container for view.
         * Overtakes the buffer list from an other container.
         * @param view New TextView.
         * @param old_container Container from TextView,
         * that opend this TextView.
         */
        void add_text_view_to_map(Gsv::View*view,
                                  orderd_container<Glib::RefPtr <text_buffer> >&
                                  old_container);

        /**
         * This signal handler is connected with every editor_text_view.
         * Every time the focus changed on a editor_text_view,
         * this handler gets called.
         * With that it is possible to track which the active text_buffer is.
         * @param event_focus Focus event.
         * @returns Always false.
         */
        bool on_text_view_focus_changed(GdkEventFocus* event_focus);

        /**
         * Checks if a buffer with the same name is already open.
         * @param buffer_name Buffer name.
         * @returns true if already open. False if not.
         */
        bool is_buffer_open(std::string& buffer_name);

        /**
         * Creates a new text_buffer. Sets the name of the buffer and adds it
         * to the global text buffer container (g_text_buffer_container).
         * @param buffer_name Text buffer name (filepath).
         * @returns text_buffer.
         */
        Glib::RefPtr<text_buffer> create_text_buffer(std::string& buffer_name);

        /**
         * Resets the active text view to nullptr if tv matches.
         * @param tv Text view.
         */
        void reset_active_text_view(Gtk::Widget* tv);

        /**
         * Searches for a text view in a paneds child1.
         * @param paned Paned.
         * @returns Gsv::View if found, nullptr if not found.
         */
        Gsv::View* find_text_view_in_paned_child1(paned* ppaned);

        /**
         * Searches for a text view in a paneds child2.
         * @param paned Paned.
         * @returns Gsv::View if found, nullptr if not found.
         */
        Gsv::View* find_text_view_in_paned_child2(paned* ppaned);

        /**
         * Gets the paneds orientation.
         * @param paned Paned
         * @returns Paneds orientation.
         */
        Gtk::Orientation get_paned_orientation(paned* ppaned);

        /**
         * Determines if widget is a Gtk::ScrolledWindow.
         * @param widget Widget.
         * @returns true if is Gtk::ScrolledWindow, false if not.
         */
        bool is_scrolled_window(Gtk::Widget* widget);

        /**
         * Determines if text_view is child1 of a paned.
         * @param text_view Text view.
         * @returns true if is , false if not.
         */
        bool is_paned_child1(Gsv::View* text_view);

        /**
         * Determines if paned is child1 of a paned.
         * @param text_view Text view.
         * @returns true if is, false if not.
         */
        bool is_paned_child1(paned* ppaned);

        /**
         * Determines if text_view is child2 of a paned.
         * @param text_view Text view.
         * @returns true if is , false if not.
         */
        bool is_paned_child2(Gsv::View* text_view);

        /**
         * Determines if paned is child2 of a paned.
         * @param text_view Text view.
         * @returns true if is, false if not.
         */
        bool is_paned_child2(paned* ppaned);

        /**
         * Gets the paned of a text view, if it has one.
         * @param text_view Text view.
         * @returns Corresponding paned. If it has no paned, nullptr.
         */
        paned* get_paned(Gsv::View* text_view);

        /**
         * Gets the parent paned of a paned, if it has one.
         * @param paned Paned.
         * @returns Corresponding parent paned.
         * If it has no parent parent paned, nullptr.
         */
        paned* get_parent_paned(paned* ppaned);

        /**
         * Executes a command in the command bar.
         */
        void execute_cmd();

        /** Turns a member function into internal function stub. */
        template <class C, void (C::*handler)(Glib::ustring)>
        static __attribute__((always_inline))
        void class_method_stub(instance_ptr instance, Glib::ustring data) {
            return (static_cast<C*>(instance)->*handler)(data);
        }
    };
}

#endif // FLEXED_EDITOR_H_
