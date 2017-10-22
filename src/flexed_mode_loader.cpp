/**
 * @file flexed_mode_loader.cpp
 */

#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <gtkmm.h>

#include "flexed_mode_loader.h"
#include "flexed_editor.h"
#include "log.h"

namespace flexed {

    mode_loader::mode_loader(editor* ed) {
        this->ed = ed;
        mode_search_path = "/usr/lib/flexed";
        ed->signal_text_buffer_created()
            .connect(sigc::mem_fun(*this, &mode_loader::on_buffer_created));
        ed->signal_file_opened()
            .connect(sigc::mem_fun(*this, &mode_loader::on_file_opened));
    }

    mode_loader::~mode_loader() {
    }

    void mode_loader::set_mode_search_path(std::string& path) {
        mode_search_path = path;
    }

    bool mode_loader::load_mode(Glib::RefPtr<text_buffer> buffer,
                                std::string& mode_name) {
        void* mode_handle = nullptr;
        if (buffer->is_mode_open(mode_name)) {
            //if (is_mode_in_buffer_open(mode_name, buffer_name)) {
            FILE_LOG(LOG_INFO) << "Mode in buffer already open";
            return false;
        }
        if (is_mode_loaded(mode_name)) {
            //auto iter_mode_handle = mode_handle_map.find(mode_name);
            //if (iter_mode_handle != mode_handle_map.end()) {
            FILE_LOG(LOG_INFO) << "Mode already loaded";
            mode_handle = get_mode_handle(mode_name);
            //mode_handle = iter_mode_handle->second;
            mode_handle_map.insert(
                std::pair<std::string, void*>(mode_name, mode_handle));
            //mode_buffer_map.insert(
            //    std::pair<std::string, std::string>(mode_name, buffer_name));
        }
        else {
            //std::string real_name = mode_name;
            //std::string fpath = mode_search_path;
            //real_name = replace_minus_with_underscore(real_name);
            //fpath += real_name + "/";
            //fpath += "/";
            //std::string lib_name = real_name;
            //fpath += mode_name_to_lib_name(lib_name);
            //FILE_LOG(LOG_DEBUG1) << "Try to load: " << fpath;
            //mode_handle = dlopen(fpath.c_str(), RTLD_NOW);
            mode_handle = load_mode_in_memory(mode_name);
            if (mode_handle == nullptr) {
                FILE_LOG(LOG_INFO) << "Mode not loaded";
                return false;
            }
            mode_handle_map.insert(
                std::pair<std::string, void*>(mode_name, mode_handle));
            //mode_buffer_map.insert(
            //    std::pair<std::string, std::string>(mode_name, buffer_name));
            FILE_LOG(LOG_DEBUG1) << "Mode handle: " << mode_handle;
            FILE_LOG(LOG_DEBUG1) << "Saved mode as: " <<  mode_name;
            FILE_LOG(LOG_INFO) << "Mode loaded";
            //call_mode_start_function(mode_handle, mode_name);
        }
        add_mode(buffer, mode_name);
        call_hooks(buffer, mode_name);
        //call_mode_buffer_start_function(mode_name);
        return true;
    }

    void mode_loader::unload_mode(Glib::RefPtr<text_buffer> buffer,
                                  std::string& mode_name) {
        if (!buffer->is_mode_open(mode_name)) {
            FILE_LOG(LOG_INFO) << "Mode in buffer not open";
            return;
        }
        call_mode_buffer_end_function(mode_name);
        buffer->unset_mode(mode_name);
        if (mode_handle_map.count(mode_name) == 1) {
            //FILE_LOG(LOG_INFO) << "Unload shared lib";
            //call_mode_end_function(mode_name);
            //dlclose(mode_handle_map.find(mode_name)->second);
        }

        auto iter_mode_handle = mode_handle_map.find(mode_name);
        buffer->unset_mode(mode_name);
        mode_handle_map.erase(iter_mode_handle);
    }

    bool mode_loader::load_mode_global(std::string& mode_name) {
        // First check if mode is in every buffer loaded, if yes return
        // Then check if mode is already loaded into memory, if not load it
        // Add mode to every buffer
        // Add mode to global_mode_list
        /*if (is_mode_in_every_buffer_open(mode_name)) {
            return true;
        }
        if (!is_mode_loaded(mode_name)) {
            auto mode_handle = load_mode_in_memory(mode_name);
            if (mode_handle == nullptr) {
                return false;
            }
            call_mode_start_function(mode_handle, mode_name);
        }
        add_mode_to_every_buffer(mode_name);
        */
        auto buffers = ed->get_global_text_buffer_container()->get_obj_vec();
        for (auto buffer : buffers) {
            load_mode(buffer, mode_name);
        }
        global_mode_list.push_back(mode_name);
        return true;
    }

    void mode_loader::unload_mode_global(std::string& mode_name) {
        auto buffers = ed->get_global_text_buffer_container()->get_obj_vec();
        for (auto buffer : buffers) {
            unload_mode(buffer, mode_name);
        }
        auto iter = std::find(
            global_mode_list.begin(), global_mode_list.end(), mode_name);
        if (iter != global_mode_list.end()) {
            global_mode_list.erase(iter);
        }
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

    void mode_loader::add_mode_hook(const std::string& mode_name,
                                    const std::string& hook_mode_name) {
        mode_hook_map.insert(
            std::pair<std::string, std::string>(mode_name, hook_mode_name));
    }

    void mode_loader::add_mode_load_on_file_open(
        const std::string& file_extension, const std::string& mode_name) {
        file_ext_mode_map.insert(
            std::pair<std::string, std::string>(file_extension, mode_name));
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
        return true;
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

    void mode_loader::on_buffer_created(Glib::RefPtr<text_buffer> buffer) {
        for (auto mode : global_mode_list) {
            load_mode(buffer, mode);
        }
    }

    void mode_loader::on_file_opened(Glib::RefPtr<text_buffer> buffer,
                                     std::string& fname) {
        auto extension = get_file_extension(fname);
        FILE_LOG(LOG_DEBUG2) << "File extension is: " << extension;
        load_modes_on_file_extension(buffer, extension);
    }

    bool mode_loader::is_mode_loaded(std::string& mode_name) {
        auto iter_mode_handle = mode_handle_map.find(mode_name);
        if (iter_mode_handle != mode_handle_map.end()) {
            FILE_LOG(LOG_INFO) << "Mode already loaded";
            return true;
        }
        return false;
    }

    void* mode_loader::load_mode_in_memory(std::string& mode_name) {
        std::string fpath = mode_search_path;
        fpath += "/";

        std::string lib_name = mode_name;
        fpath += mode_name_to_lib_name(lib_name);

        FILE_LOG(LOG_DEBUG1) << "Try to load: " << fpath;
        auto mode_handle = dlopen(fpath.c_str(), RTLD_NOW);
        if (mode_handle == nullptr) {
            FILE_LOG(LOG_INFO) << "Mode not loaded";
            return nullptr;
        }
        mode_handle_map.insert(
            std::pair<std::string, void*>(mode_name, mode_handle));
        call_mode_start_function(mode_handle, mode_name);
        return mode_handle;
    }

    void* mode_loader::get_mode_handle(std::string& mode_name) {
        auto iter = mode_handle_map.find(mode_name);
        if (iter != mode_handle_map.end()) {
            return iter->second;
        }
        return nullptr;
    }

    void mode_loader::add_mode(Glib::RefPtr<text_buffer> buffer,
                               std::string& mode_name) {
        buffer->add_mode(mode_name);
        call_mode_buffer_start_function(mode_name);
    }

    void mode_loader::call_hooks(Glib::RefPtr<text_buffer> buffer,
                                 std::string& mode_name) {
        auto ret = mode_hook_map.equal_range(mode_name);
        for (auto iter = ret.first; iter != ret.second; iter++) {
            load_mode(buffer, iter->second);
        }
    }

    void mode_loader::load_modes_on_file_extension(
        Glib::RefPtr<text_buffer> buffer, std::string& extension) {
        auto ret = file_ext_mode_map.equal_range(extension);
        for (auto iter = ret.first; iter != ret.second; iter++) {
            load_mode(buffer,  iter->second);
        }
    }

    std::string mode_loader::get_file_extension(std::string& filename) {
        std::string ext = "";
        int len = filename.length();
        for (int i = len - 1; i >= 0; i--) {
            if (filename[i] == '.') {
                return ext;
            }
            ext.insert(ext.begin(), filename[i]);
        }
        return ext;
    }
}
