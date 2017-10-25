/**
 * @file flexed_paned.h
 */

#ifndef FLEXED_PANED_H_
#define FLEXED_PANED_H_

#include <gtkmm.h>

namespace flexed {

/**
 * Paned widget.
 */
class paned : public Gtk::Paned {
public:

    paned();

    paned(Gtk::Orientation orientation);

    virtual ~paned();

    /**
     * Sets the divider of the paned relative.
     * Orientation horizontal: 0 means complete left,
     * 100 means complete right.
     * Orientation vertical: 0 means at top, 100 means at bottom.
     * @param pos Position.
     */
    void set_divider_pos_relative(int pos);

    /**
     * Returns the relative position of the divider.
     * @returns Relative position.
     */
    int get_divider_pos_relative();

protected:

    /**
     * Gets called on paneds draw signal.
     */
    bool on_draw_event(const ::Cairo::RefPtr< ::Cairo::Context>& cr);

private:
    int divider_pos = 50;

    /**
     * Calls init routines.
     */
    void init();
};

}

#endif // FLEXED_PANED_H_
