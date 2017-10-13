/**
 * @file flexed_text_buffer.cpp
 */
#include <cstring>

#include "flexed_text_buffer.h"
#include "flexed_editor.h"
#include "keyboard_map.h"
#include "flexed_mode.h"

namespace flexed {

text_buffer::text_buffer() {
    kmap = std::make_shared<keyboard_map>();
}

text_buffer::~text_buffer() {
}

void text_buffer::set_name(const Glib::ustring& name) {
  this->name = name;
}

Glib::ustring& text_buffer::get_name() {
  return name;
}

void text_buffer::set_keyboard_map(std::shared_ptr<keyboard_map> k_map) {
    kmap = k_map;
}

std::shared_ptr<keyboard_map> text_buffer::get_keyboard_map() {
  return kmap;
}

void text_buffer::add_mode(std::string& m) {
  g_print("add mode: %s\n", m.c_str());
  mode_list.push_back(m);
}

void text_buffer::unset_mode(std::string& m ) {
    // NOTE: This does not really unload the mode.
    // It just removes it from the mode list,
    // so that when the user tries to enter a function,
    // it will not be found anymore.
    // The mode stays in memory. It gets not unloaded from memory.
    auto iter = std::find(mode_list.begin(), mode_list.end(), m);
    if (iter != mode_list.end()) {
        mode_list.erase(iter);
    }
}

std::list<std::string>& text_buffer::get_mode_list() {
  return mode_list;
}

}
