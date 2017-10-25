/**
 * @file flexed_text_buffer.h
 */

#ifndef FLEXED_TEXT_BUFFER_H_
#define FLEXED_TEXT_BUFFER_H_

#include <memory>
#include <list>
#include <gtkmm.h>
#include <gtksourceviewmm.h>

namespace flexed {

    class keyboard_map;

/**
 * Represents a text buffer in a text view.
 */
class text_buffer : public Gsv::Buffer {

public:
  text_buffer();
  virtual ~text_buffer();

  /**
   * Sets the name of text buffer,
   * this is useful for identifying the buffer.
   * @param name Name.
   */
  void  set_name(const Glib::ustring& name);

  /**
   * Gets the name of a buffer.
   * @returns Name of buffer.
   */
  Glib::ustring& get_name();

  /**
   * Sets the keyboard map of the buffer.
   * @param kmap Keyboard map.
   */
  void set_keyboard_map(std::shared_ptr<keyboard_map> k_map);

  /**
   * Gets the keyboard map of the buffer.
   * @returns Keyboard map.
   */
  std::shared_ptr<keyboard_map> get_keyboard_map();

  /**
   * Adds a mode to the buffer. Takes care of not adding a mode twice.
   * @param m Mode name.
   */
  void add_mode(std::string& m);

  /**
   * Gets a list of all opened modes in the buffer.
   * @returns List of modes.
   */
  std::list<std::string>& get_mode_list();

  /**
   * Unset a mode in the buffer.
   * @param m Mode name.
   */
  void unset_mode(std::string& m);


    /**
     * Determines if a mode is open.
     * @param mode_name Name of mode.
     */
    bool is_mode_open(std::string& mode_name);

private:
  Glib::ustring name;

  std::shared_ptr<keyboard_map> kmap;

  /** Holds all opened modes, with the handle to the dynamic loaded library. */
  std::list<std::string> mode_list;
};

}

#endif // FLEXED_TEXT_BUFFER_H_
