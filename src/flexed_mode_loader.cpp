/**
 * @file flexed_mode_loader.cpp
 */

#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <gtkmm.h>

#include "flexed_mode_loader.h"
#include "log.h"

namespace flexed {

    mode_loader::mode_loader(editor* ed) {
        this->ed = ed;
        mode_search_path = getenv("HOME");
        mode_search_path += "/.flexed/";
    }

    mode_loader::~mode_loader() {
    }

    void mode_loader::set_mode_search_path(std::string& path) {
        mode_search_path = path;
    }

    bool mode_loader::load_mode(std::string& buffer_name,
                                std::string& mode_name) {
        void* mode_handle = nullptr;
        if (is_mode_in_buffer_open(mode_name, buffer_name)) {
            FILE_LOG(LOG_INFO) << "Mode in buffer already open";
            return false;
        }
        auto iter_mode_handle = mode_handle_map.find(mode_name);
        if (iter_mode_handle != mode_handle_map.end()) {
            FILE_LOG(LOG_INFO) << "Mode already loaded";
            mode_handle = iter_mode_handle->second;
            mode_handle_map.insert(
                std::pair<std::string, void*>(mode_name, mode_handle));
            mode_buffer_map.insert(
                std::pair<std::string, std::string>(mode_name, buffer_name));
        }
        else {
            std::string real_name = mode_name;
            std::string fpath = mode_search_path;
            real_name = replace_minus_with_underscore(real_name);
            fpath += real_name + "/";
            std::string lib_name = real_name;
            fpath += mode_name_to_lib_name(lib_name);
            FILE_LOG(LOG_DEBUG1) << "Try to load: " << fpath;
            mode_handle = dlopen(fpath.c_str(), RTLD_NOW);
            if (mode_handle == nullptr) {
                FILE_LOG(LOG_INFO) << "Mode not loaded";
                return false;
            }
            mode_handle_map.insert(
                std::pair<std::string, void*>(mode_name, mode_handle));
            mode_buffer_map.insert(
                std::pair<std::string, std::string>(mode_name, buffer_name));
            FILE_LOG(LOG_DEBUG1) << "Mode handle: " << mode_handle;
            FILE_LOG(LOG_DEBUG1) << "Saved mode as: " <<  mode_name;
            FILE_LOG(LOG_INFO) << "Mode loaded";
            call_mode_start_function(mode_handle, mode_name);
        }
        call_mode_buffer_start_function(mode_name);
        return true;
    }

void mode_loader::unload_mode(std::string& buffer_name,
                              std::string& mode_name) {
    if (!is_mode_in_buffer_open(mode_name, buffer_name)) {
        FILE_LOG(LOG_INFO) << "Mode in buffer not open";
        return;
    }
    call_mode_buffer_end_function(mode_name);
    if (mode_handle_map.count(mode_name) == 1) {
        FILE_LOG(LOG_INFO) << "Unload shared lib";
        call_mode_end_function(mode_name);
        dlclose(mode_handle_map.find(mode_name)->second);
    }
    FILE_LOG(LOG_INFO) << "Call buffer end func";
    auto iter_mode_handle = mode_handle_map.find(mode_name);
    auto iter_mode_buffer = mode_buffer_map.find(mode_name);
    erase_mode_buffer_map(buffer_name, mode_name);
    mode_handle_map.erase(iter_mode_handle);
}

void mode_loader::call_function(std::list<std::string>& mode_list,
                                std::string& name) {
    for (auto mode : mode_list) {
        FILE_LOG(LOG_DEBUG1) << "Try to find function " << name
                             << " in mode: " << mode;
        auto iter = mode_handle_map.find(mode);
        if (iter != mode_handle_map.end()) {
            FILE_LOG(LOG_DEBUG1) << "Mode handle: " << iter->second;
            key_binding_func_t* ffunction
                = (key_binding_func_t*)dlsym(iter->second, name.c_str());
            if (!ffunction) {
                FILE_LOG(LOG_INFO) << "Function not found";
                continue;
            }
            FILE_LOG(LOG_INFO) << "Call function " << name;
            ffunction();
            return;
        }
    }
    FILE_LOG(LOG_INFO) << "Function not found";
}

    void mode_loader::call_mode_start_function(void *handle,
                                               std::string& mode_name) {
    std::string fname = "_start";
    std::string real_mode_name = mode_name;
    real_mode_name = replace_minus_with_underscore(real_mode_name);
    fname = real_mode_name + fname;

    std::list<std::string> l = {mode_name};
    FILE_LOG(LOG_DEBUG1) << "Call start func: " << fname
                         << " in mode: " << mode_name;
    start_func_t* ffunction
        = (start_func_t*)dlsym(handle, fname.c_str());
    if (!ffunction) {
        FILE_LOG(LOG_INFO) <<  "Start function not found";
        return;
    }
    ffunction(ed);
}

bool mode_loader::call_mode_buffer_start_function(std::string& mode_name) {
    std::string fname = "_buffer_start";
    std::string real_mode_name = mode_name;
    real_mode_name = replace_minus_with_underscore(real_mode_name);
    fname = real_mode_name + fname;

    std::list<std::string> l = {mode_name};
    FILE_LOG(LOG_DEBUG1) << "Call buffer start func: " << fname
                         << " in mode: " << mode_name;
    call_function(l, fname);
}

void mode_loader::call_mode_buffer_end_function(std::string& mode_name) {
    std::string fname = "_buffer_end";
    std::string real_mode_name = mode_name;
    real_mode_name = replace_minus_with_underscore(real_mode_name);
    fname = real_mode_name + fname;

    std::list<std::string> l = {mode_name};
    FILE_LOG(LOG_DEBUG1) << "Call mode end func: " << fname
                         << " in mode: " << mode_name;
    call_function(l, fname);
}

void mode_loader::call_mode_end_function(std::string& mode_name) {
    std::string fname = "_end";
    std::string real_mode_name = mode_name;
    real_mode_name = replace_minus_with_underscore(real_mode_name);
    fname = real_mode_name + fname;

    std::list<std::string> l = {mode_name};
    FILE_LOG(LOG_DEBUG1) << "Call buffer end func: " << fname
                         << " in mode: " << mode_name;
    call_function(l, fname);
}

std::string& mode_loader::replace_minus_with_underscore(std::string& name) {
  FILE_LOG(LOG_DEBUG2) << "Replace min with underscore";
  int size = name.size();
  for (int i = 0; i < size; i++) {
      if (name[i] == '-') {
          char c = '_';
          name.replace(i, 1, "_");
      }
    }
  return name;
}

std::string& mode_loader::mode_name_to_lib_name(std::string& name) {
    FILE_LOG(LOG_DEBUG1) << "Mode name " << name << " to lib name";
    std::string beg = "libflexed_";
    std::string end = ".so";
    std::string fullname = beg + name + end;
    name = fullname;
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
            FILE_LOG(LOG_DEBUG1) << "Erase element in mode_buffer_map";
            mode_buffer_map.erase(i);
        }
    }
}

}
