/**
 * @file flexed_text_buffer_container.cpp
 */

#include "flexed_text_buffer_container.h"
#include "log.h"

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
  FILE_LOG(LOG_INFO) << "Buffer removing";
  int pos = 0;
  int size = obj_vec.size();
  for(; pos < size; pos++) {
    if (obj_vec[pos] == buffer) {
      obj_vec.erase(obj_vec.begin() + pos);
      break;
    }
  }
  set_buffer();
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

    void text_buffer_container::switch_buffer(std::string& buffer_name) {
        auto index = find_buffer_by_name(buffer_name);
        FILE_LOG(LOG_DEBUG2) << "Index: " << index;
        if (index == -1) {
            return;
        }
        swap_element(0, index);
        set_buffer();
    }

void text_buffer_container::on_next() {
  set_buffer();
}

void text_buffer_container::on_previous() {
  set_buffer();
}

    int text_buffer_container::find_buffer_by_name(std::string& buffer_name) {
        int index = 0;
        FILE_LOG(LOG_DEBUG2) << "buffer name to search: " << buffer_name;
        for (auto buffer : obj_vec) {
            FILE_LOG(LOG_DEBUG2) << "buffer name: " << buffer->get_name();
            if (buffer->get_name() == buffer_name) {
                return index;
            }
            index++;
        }
        return -1;
    }

}
