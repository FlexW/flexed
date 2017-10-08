/**
 * @file flexed_mode_loader.cpp
 */

#include <cstring>
#include <dlfcn.h>
#include <gtkmm.h>

#include "flexed_mode_loader.h"

namespace flexed {

mode_loader::mode_loader() {
}

mode_loader::~mode_loader() {
}

bool mode_loader::load_mode(std::string& buffer_name, std::string& mode_name) {
  void* mode_handle = nullptr;
  if (is_mode_in_buffer_open(mode_name, buffer_name)) {
      g_print("mode in buffer already open\n");
      return false;
  }
  auto iter_mode_handle = mode_handle_map.find(mode_name);
  if (iter_mode_handle != mode_handle_map.end()) {
      g_print("mode already loaded\n");
      mode_handle = iter_mode_handle->second;
      mode_handle_map.insert(
          std::pair<std::string, void*>(mode_name, mode_handle));
      mode_buffer_map.insert(
          std::pair<std::string, std::string>(mode_name, buffer_name));
  }
  else {
      std::string real_name = mode_name;
      std::string fpath = FLEXED_MODE_PATH;
      real_name = replace_minus_with_underscore(real_name);
      fpath += mode_name_to_lib_name(real_name);
      g_print("try to load: %s\n", fpath.c_str());
      mode_handle = dlopen(fpath.c_str(), RTLD_LAZY);
      if (mode_handle == nullptr) {
          g_print("mode not loaded\n");
          return false;
      }
      mode_handle_map.insert(
          std::pair<std::string, void*>(mode_name, mode_handle));
      mode_buffer_map.insert(
          std::pair<std::string, std::string>(mode_name, buffer_name));
      g_print("mode handle: %p\n", mode_handle);
      g_print("saved mode as: %s\n", mode_name.c_str());
      g_print("mode loaded\n");
      call_mode_start_function(mode_name);
  }
  call_mode_buffer_start_function(mode_name);
  return true;
}

void mode_loader::unload_mode(std::string& buffer_name,
                              std::string& mode_name) {
    if (!is_mode_in_buffer_open(mode_name, buffer_name)) {
        g_print("mode in buffer not open\n");
        return;
    }
    call_mode_buffer_end_function(mode_name);
    if (mode_handle_map.count(mode_name) == 1) {
        g_print("unload shared lib\n");
        call_mode_end_function(mode_name);
        dlclose(mode_handle_map.find(mode_name)->second);
    }
    g_print("call buffer end func\n");
    auto iter_mode_handle = mode_handle_map.find(mode_name);
    auto iter_mode_buffer = mode_buffer_map.find(mode_name);
    erase_mode_buffer_map(buffer_name, mode_name);
    mode_handle_map.erase(iter_mode_handle);
}

void mode_loader::call_function(std::list<std::string>& mode_list,
                                std::string& name) {
    g_print("mode_loader::call_function\n");
    for (auto mode : mode_list) {
        g_print("try to find function %s in mode: %s\n", name.c_str(), mode.c_str());
        auto iter = mode_handle_map.find(mode);
        if (iter != mode_handle_map.end()) {
            g_print("mode handle: %p\n", iter->second);
            func_type* ffunction = (func_type*)dlsym(iter->second, name.c_str());
            if (!ffunction) {
                g_print("mode_loader::call_function func not found\n");
                continue;
            }
            g_print("call function %s\n", name.c_str());
            ffunction();
            return;
        }
    }
    g_print("function not found\n");
}

void mode_loader::call_mode_start_function(std::string& mode_name) {
    std::string fname = "_start";
    std::string real_mode_name = mode_name;
    real_mode_name = replace_minus_with_underscore(real_mode_name);
    fname = real_mode_name + fname;

    std::list<std::string> l = {mode_name};
    g_print("call start func: %s in mode: %s\n", fname.c_str(), mode_name.c_str());
    call_function(l, fname);
}

bool mode_loader::call_mode_buffer_start_function(std::string& mode_name) {
    std::string fname = "_buffer_start";
    std::string real_mode_name = mode_name;
    real_mode_name = replace_minus_with_underscore(real_mode_name);
    fname = real_mode_name + fname;

    std::list<std::string> l = {mode_name};
    g_print("call buffer start func: %s in mode: %s\n", fname.c_str(),
            mode_name.c_str());
    call_function(l, fname);
}

void mode_loader::call_mode_buffer_end_function(std::string& mode_name) {
    std::string fname = "_buffer_end";
    std::string real_mode_name = mode_name;
    real_mode_name = replace_minus_with_underscore(real_mode_name);
    fname = real_mode_name + fname;

    std::list<std::string> l = {mode_name};
    g_print("call buffer end func: %s in mode: %s\n", fname.c_str(),
            mode_name.c_str());
    call_function(l, fname);
}

void mode_loader::call_mode_end_function(std::string& mode_name) {
    std::string fname = "_end";
    std::string real_mode_name = mode_name;
    real_mode_name = replace_minus_with_underscore(real_mode_name);
    fname = real_mode_name + fname;

    std::list<std::string> l = {mode_name};
    g_print("call end func: %s in mode: %s\n", fname.c_str(), mode_name.c_str());
    call_function(l, fname);
}

std::string& mode_loader::replace_minus_with_underscore(std::string& name) {
  g_print("replace min with us\n");
  int size = name.size();
  for (int i = 0; i < size; i++) {
      if (name[i] == '-') {
          char c = '_';
          name.replace(i, 1, "_");
      }
    }
  g_print("replaced min with us\n");
  return name;
}

std::string& mode_loader::mode_name_to_lib_name(std::string& name) {
    g_print("mode name %s to lib name\n", name.c_str());
    std::string beg = "libflexed_";
    std::string end = ".so";
    std::string fullname = beg + name + end;
    name = fullname;
    g_print("mode name to lib name\n");
    return name;
}

    bool mode_loader::is_mode_in_buffer_open(std::string& mode_name,
                                             std::string& buffer_name) {
    auto range = mode_buffer_map.equal_range(mode_name);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second == buffer_name) {
            return true;
        }
    }
    return false;
}

void mode_loader::erase_mode_buffer_map(std::string& buffer_name,
                                        std::string& mode_name) {
    auto range = mode_buffer_map.equal_range(mode_name);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second == buffer_name) {
            g_print("erase element in mode_buffer_map\n");
            mode_buffer_map.erase(i);
        }
    }
}

}
