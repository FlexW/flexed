#include <vector>
#include <gtkmm.h>

#include "keyboard_map.h"
#include "log.h"

namespace flexed {

void keyboard_map::remove_key_binding(const std::string& key_binding) {
  auto kb_split = split_key_bindings(key_binding);
  k_map_state1.erase(kb_split[0]);
  k_map_state2.erase(kb_split[0] + kb_split[1]);
  k_map_state3.erase(kb_split[0] + kb_split[1] + kb_split[2]);
  // free stub ?
}

int keyboard_map::call_handler(const std::string& key_binding) {
    if (key_binding.size() <= 2) {
    auto iter = k_map_state1.find(key_binding);
    if (iter == k_map_state1.end())
      return -1;
    auto func = iter->second;
    if (func == nullptr) {
      return 1;
    }
    func->second(func->first);
    return 0;
  }
  if (key_binding.size() <= 4) {
    auto iter = k_map_state2.find(key_binding);
    if (iter == k_map_state2.end())
      return -1;
    auto func = iter->second;
    if (func == nullptr) {
      return 2;
    }
    FILE_LOG(LOG_DEBUG2) << "Func p: " << func;
    FILE_LOG(LOG_DEBUG2) << "Func->second p: " << func->second;
    FILE_LOG(LOG_DEBUG2) << "Func->first p: " << func->first;
    func->second(func->first);
    return 0;
  }
  auto iter = k_map_state3.find(key_binding);
  if (iter == k_map_state3.end())
    return -1;
  auto func = iter->second;
  if (func == nullptr) {
    return 3;
  }
  func->second(func->first);
  return 0;
}

std::vector<std::string> keyboard_map::split_key_bindings(
    const std::string& key_binding) {
  int kb_size = key_binding.size();
  std::string kb_state1 = "";
  std::string kb_state2 = "";
  std::string kb_state3 = "";

  for (int i = 0; i < kb_size && i < 6; i++) {
    if (i < 2) {
      kb_state1 += key_binding[i];
    }
    else if (i < 4) {
      kb_state2 += key_binding[i];
    }
    else {
      kb_state3 += key_binding[i];
    }
  }
  std::vector<std::string> vec = {kb_state1, kb_state2, kb_state3};
  return vec;
}

}
