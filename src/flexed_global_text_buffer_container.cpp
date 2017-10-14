/**
 * @file global_text_buffer_container.cpp
 */

#include "flexed_global_text_buffer_container.h"

namespace flexed {

void global_text_buffer_container::register_text_buffer_added(
    text_buffer_container* h) {
  handler_list.push_back(h);
}

void global_text_buffer_container::remove_text_buffer_handler(
    text_buffer_container* h) {
  auto pos = 0;
  auto found = false;

  for (auto handler : handler_list) {
    if (handler == h) {
      found = true;
      break;
    }
    pos++;
  }
  if (found) {
    auto handler_iter = handler_list.begin();
    std::advance(handler_iter, pos);
    handler_list.erase(handler_iter);
  }
}

void global_text_buffer_container::on_added(Glib::RefPtr<text_buffer> obj) {
  text_buffer_added(obj);
}

void global_text_buffer_container::before_removed(
    Glib::RefPtr<text_buffer> obj) {
  text_buffer_removed(obj);
}

void global_text_buffer_container::text_buffer_added(
    Glib::RefPtr<text_buffer> buffer) {
  for (auto handler : handler_list) {
    handler->on_text_buffer_added(buffer);
  }
}

void global_text_buffer_container::text_buffer_removed(
    Glib::RefPtr<text_buffer> buffer) {
  for (auto handler : handler_list) {
    handler->on_text_buffer_removed(buffer);
  }
}

}
