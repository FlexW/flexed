/**
 * @file keyboard_handler.h
 */

#ifndef KEYBOARD_HANDLER_H_
#define KEYBOARD_HANDLER_H_

#include <memory>
#include <string>
#include <gtkmm.h>

#include "keyboard_map.h"

/**
 * Handels keyboard input.
 */
class keyboard_handler {
public:
  bool on_key_pressed(GdkEventKey* key_event);

  /**
   * Sets the keyboard map.
   * @param map Keyboard map.
   */
  void set_keyboard_map(std::shared_ptr<keyboard_map> k_map);

private:

  /** Keyboard map for key bindings. */
  std::shared_ptr<keyboard_map> k_map = nullptr;

  std::string g_mask = "";
  int last_ret_code = 0;

  void reset_states();
};

#endif
