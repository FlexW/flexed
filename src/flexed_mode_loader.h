/**
 * @file flexed_mode_loader.h
 */

#ifndef FLEXED_MODE_LOADER_H_
#define FLEXED_MODE_LOADER_H_

#include <list>
#include <map>
#include <string>

// The path after which the loader searches for modules.
#define FLEXED_MODE_PATH "/home/felix/Develop/flexed/build_debug/src/module/"

namespace flexed {
    typedef void func_type();

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
    mode_loader();
    virtual ~mode_loader();

    /**
     * Loads a mode into memory. If the mode is already loaded,
     * it does nothing.
     * @param name Name of the mode.
     * @returns true if mode successfully loaded. false if not.
     */
    bool load_mode(std::string& buffer_name, std::string& mode_name);

    void unload_mode(std::string& buffer_name, std::string& mode_name);

    /**
     * Calls a function of a loaded mode.
     * @param mode_list A list of all modes that should be searched after the function.
     * @param name Name of the function that should be called.
     */
    void call_function(std::list<std::string>& mode_list, std::string& name);

private:
    /** Stores the handles to all open modes. */
    std::multimap<std::string, void*> mode_handle_map;

    /** Stores the name of modes that are open in buffers. */
    std::multimap<std::string, std::string> mode_buffer_map;

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
    void call_mode_start_function(std::string &mode_name);

    bool call_mode_buffer_start_function(std::string& mode_name);

    void call_mode_buffer_end_function(std::string& mode_name);

    /**
     * Calls the end function of the mode.
     * This function must always be in the form void MODE_NAME_end(void).
     * If it's not in this form, the loader will be unable to find it.
     * @param mode_name Name of the mode.
     */
    void call_mode_end_function(std::string &mode_name);

    bool is_mode_in_buffer_open(std::string& mode_name, std::string& buffer_name);

    void erase_mode_buffer_map(std::string& buffer_name, std::string& mode_name);
};

}

#endif // FLEXED_MODE_LOADER_H_
