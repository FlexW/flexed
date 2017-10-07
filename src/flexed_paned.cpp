#include "flexed_paned.h"

namespace flexed {

paned::paned() : Gtk::Paned() {
    init();
}

paned::paned(Gtk::Orientation orientation) : Gtk::Paned(orientation) {
    init();
}

void paned::init() {
    signal_draw()
        .connect(sigc::mem_fun(*this, &paned::on_draw_event), false);
}

paned::~paned() {
}

void paned::set_divider_pos_relative(int pos) {
    if (pos >= 0 && pos <= 100) {
        divider_pos = pos;
    }
}

int paned::get_divider_pos_relative() {
    return divider_pos;
}

bool paned::on_draw_event(const ::Cairo::RefPtr< ::Cairo::Context>& cr) {
    auto orientation = get_orientation();
    switch (orientation) {
    case Gtk::ORIENTATION_VERTICAL:
    {
        int h = get_height();
        set_position((int)((float)h / 100.0 * (float)divider_pos));
    }
        break;
    case Gtk::ORIENTATION_HORIZONTAL:
    {
        int w = get_width();
        set_position((int)((float)w / 100.0 * (float)divider_pos));
    }
        break;
    }
    return false;
}

}
