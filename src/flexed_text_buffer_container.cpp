/**
 * @file flexed_text_buffer_container.cpp
 */

#include "flexed_text_buffer_container.h"

namespace flexed {

text_buffer_container::text_buffer_container(
    std::shared_ptr<global_text_buffer_container> global_container,
    Gtk::TextView* text_view) {
  global_container->register_text_buffer_added(this);
  this->text_view = text_view;
}

text_buffer_container::~text_buffer_container() {
}

void text_buffer_container::on_text_buffer_added(
    Glib::RefPtr<text_buffer> buffer) {
  obj_vec.push_back(buffer);
}

void text_buffer_container::on_text_buffer_removed(
    Glib::RefPtr<text_buffer> buffer) {
  int pos = 0;
  int size = obj_vec.size();
  for(; pos < size; pos++) {
    if (obj_vec[pos] == buffer) {
      obj_vec.erase(obj_vec.begin() + pos);
      break;
    }
  }
}

void text_buffer_container::take_over_text_buffer_container_list(
    orderd_container& c) {
  auto c_obj_vec = c.get_obj_vec();
  int size = c_obj_vec.size();
  for (int i = size - 1; i >= 0; i--) {
    add(c_obj_vec[i]);
  }
  set_buffer();
}

void text_buffer_container::set_buffer() {
  if (text_view == nullptr || obj_vec.size() == 0)
    return;
  text_view->set_buffer(active());
}

void text_buffer_container::on_next() {
  set_buffer();
}

void text_buffer_container::on_previous() {
  set_buffer();
}

}
