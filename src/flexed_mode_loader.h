/**
 * @file flexed_mode_loader.h
 */

#ifndef FLEXED_MODE_LOADER_H_
#define FLEXED_MODE_LOADER_H_

#include <list>
#include <map>
#include <string>
#include <gtkmm.h>

extern "C" {

namespace flexed {

    class editor;
    class text_buffer;

    typedef void key_binding_func_t();
    typedef void start_func_t(editor*);
/**
 * Loads a flexed mode (shared library) into memory
 * and calls functions of this mode.
 * Modes for the flexed editor are shared libraries,
 * that can be loaded at run time.
 * The must be saved in a specific location,
 * so that the loader is able to find them.
 */
class mode_loader {
public:
    mode_loader(editor* ed);

    virtual ~mode_loader();

    /**
     * Sets the path where the loader looks after modes.
     * @path Path.
     */
    void set_mode_search_path(std::string& path);

    /**
     * Opens a mode in a buffer.
     * @param buffer Buffer.
     * @param mode_name Name of the mode.
     * @returns true if mode successfully loaded. false if not.
     */
    bool load_mode(Glib::RefPtr<text_buffer> buffer, std::string& mode_name);

    /**
     * Unloads a mode from the given buffer.
     * @param buffer_name Name of the buffer.
     * @param mode_name Name of the mode.
     */
    void unload_mode(Glib::RefPtr<text_buffer> buffer, std::string& mode_name);

    /**
     * Loads a mode into memory and adds it to every open buffer and adds
     * it to every buffer that will be opened in the future.
     * @param mode_name Name of the mode.
     * @returns true if mode successfully loaded. false if not.
     */
    bool load_mode_global(std::string& mode_name);

    /**
     * Unloads a mode from every open buffer.
     * @param mode_name Name of the mode.
     */
    void unload_mode_global(std::string& mode_name);

    /**
     * Calls a function of a loaded mode.
     * @param mode_list A list of all modes that should be searched after the
     * function.
     * @param name Name of the function that should be called.
     */
    void call_function(std::list<std::string>& mode_list, std::string& name);

private:

    editor *ed;

    /** Mode search path. */
    std::string mode_search_path;

    /** Stores the handles to all open modes. */
    std::multimap<std::string, void*> mode_handle_map;

    /** Stores the name of modes that are open in buffers. */
    //std::multimap<std::string, std::string> mode_buffer_map;

    /**
     * The directory in which should be searched after modules.
     */
    std::string mode_load_path;

    /** Saves a list of all modes that are enabled global. */
    std::list<std::string> global_mode_list;

    /**
     * Converts a mode name to the real shared library name.
     * @param name Mode name.
     * @returns Converted mode name.
     */
    std::string& mode_name_to_lib_name(std::string& name);

    /**
     * Replaces the '-' sign with '_'.
     * @param str String.
     * @returns String.
     */
    std::string& replace_minus_with_underscore(std::string& str);

    /**
     * Calls the start function of the mode.
     * This function must always be in the form void MODE_NAME_start(void).
     * If it's not in this form, the loader will be unable to find it.
     * @param mode_name Name of the mode.
     */
    void call_mode_start_function(void *handle, std::string &mode_name);

    bool call_mode_buffer_start_function(std::string& mode_name);

    void call_mode_buffer_end_function(std::string& mode_name);

    /**
     * Calls the end function of the mode.
     * This function must always be in the form void MODE_NAME_end(void).
     * If it's not in this form, the loader will be unable to find it.
     * @param mode_name Name of the mode.
     */
    void call_mode_end_function(std::string &mode_name);

    /**
     * Determines if a mode is in a buffer open.
     * @param mode_name Name of the mode.
     * @param buffer_name Name of the buffer.
     * @returns true if is open, false if not.
     */
    bool is_mode_in_buffer_open(std::string& mode_name,
                                std::string& buffer_name);

    /**
     * Gets called every time a buffer gets created.
     * @param buffer The buffer that was created.
     */
    void on_buffer_created(Glib::RefPtr<text_buffer> buffer);

    /**
     * Determines if the mode is loaded into memory.
     * @param mode_name Name of the mode.
     * @returns true if is loaded, false if not.
     */
    bool is_mode_loaded(std::string& mode_name);

    /**
     * Tries to load the mode into memory. Calls mode start function.
     * @param mode_name Name of the mode.
     * @returns nullptr if could not load. Handle to the shared lib if loaded.
     */
    void* load_mode_in_memory(std::string& mode_name);

    /**
     * Gets the handle of a mode, if the mode is already loaded into memory.
     * @param mode_name Name of mode.
     * @returns Mode handle if mode found, nullptr if mode not found.
     */
    void* get_mode_handle(std::string& mode_name);

    /**
     * Adds a mode to a buffer and calls the buffer start function.
     * @param buffer Buffer.
     * @param mode_name Name of mode.
     */
    void add_mode(Glib::RefPtr<text_buffer> buffer, std::string& mode_name);
};

}

}

#endif // FLEXED_MODE_LOADER_H_
