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

void mode_loader::load_mode(std::string& name) {
  if (mode_map.find(name) != mode_map.end()) {
      g_print("mode already loaded\n");
      return;
  }
  std::string real_name = name;
  std::string fpath = FLEXED_MODE_PATH;
  real_name = replace_minus_with_underscore(real_name);
  fpath += mode_name_to_lib_name(real_name);
  g_print("try to load: %s\n", fpath.c_str());

  void* mode_handle = dlopen(fpath.c_str(), RTLD_NOW);
  mode_map[name] = mode_handle;
  g_print("mode handle: %p\n", mode_handle);
  g_print("saved mode as: %s\n", name.c_str());

  if (mode_map[name] != nullptr) {
    g_print("mode loaded\n");
    call_mode_start_function(name);
    return;
  }
  mode_map.erase(name);
  g_print("mode not loaded\n");
}

void mode_loader::call_function(std::list<std::string>& mode_list, std::string& name) {
    g_print("mode_loader::call_function\n");
    for (auto mode : mode_list) {
        g_print("try to find function %s in mode: %s\n", name.c_str(), mode.c_str());
        auto iter = mode_map.find(mode);
        if (iter != mode_map.end()) {
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

}
