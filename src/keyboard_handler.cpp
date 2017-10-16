#include <cstring>

#include "keyboard_handler.h"
#include "log.h"

namespace flexed {

bool keyboard_handler::on_key_pressed(GdkEventKey* event_key) {
  std::string mask = "";

  if (event_key->state & GDK_MOD1_MASK) {
      FILE_LOG(LOG_DEBUG2) << "ALT LEFT pressed";
    mask += "A";
  }
  if (event_key->state & GDK_SHIFT_MASK) {
      FILE_LOG(LOG_DEBUG2) << "SHIFT pressed";
    mask += "S";
    //cnt_key_pressed++;
  }
  if (event_key->state & GDK_CONTROL_MASK) {
      FILE_LOG(LOG_DEBUG2) << "CTRL pressed";
    mask += "C";
    //cnt_key_pressed++;
  }
  if (std::isprint(event_key->keyval)) {
      FILE_LOG(LOG_DEBUG2) << "ASCII pressed";
    mask += event_key->keyval;
    //cnt_key_pressed++;
  }
  if (event_key->keyval == GDK_KEY_Up) {
      FILE_LOG(LOG_DEBUG2) << "Up pressed\n";
    mask += "U";
  }
  if (event_key->keyval == GDK_KEY_Down) {
      FILE_LOG(LOG_DEBUG2) << "Down pressed";
    mask += "D";
  }
  if (event_key->keyval == GDK_KEY_Left) {
      FILE_LOG(LOG_DEBUG2) << "Left pressed";
      mask += "L";
  }
  if (event_key->keyval == GDK_KEY_Right) {
      FILE_LOG(LOG_DEBUG2) << "Right pressed";
    mask += "R";
  }
  if (event_key->keyval == GDK_KEY_Return) {
      FILE_LOG(LOG_DEBUG2) << "Return pressed";
      mask += "N";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F1 pressed";
      mask += "F1";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F2 pressed";
      mask += "F2";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F3 pressed";
      mask += "F3";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F4 pressed";
      mask += "F4";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F5 pressed";
      mask += "F5";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F6 pressed";
      mask += "F6";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F7 pressed";
      mask += "F7";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F8 pressed";
      mask += "F8";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F9 pressed";
      mask += "F9";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F10 pressed";
      mask += "F10";
  }
  if (event_key->keyval == GDK_KEY_F11) {
      FILE_LOG(LOG_DEBUG2) << "F11 pressed";
      mask += "F11";
  }

  //skip_times = cnt_key_pressed - 1;
  if (k_map == nullptr)
    return false;

  if (last_ret_code == 0) {
      FILE_LOG(LOG_INFO) << "Call key handler: " << mask;
    int ret_code = k_map->call_handler(mask);
    if (ret_code == 1) {
      last_ret_code = 1;
      g_mask += mask;
      return true;
    }
    reset_states();
    if (ret_code == 0) {
      return true;
    }
    return false;
  }
  else if (last_ret_code == 1) {
    g_mask += mask;
    FILE_LOG(LOG_INFO) << "Call key handler: " << g_mask;
    int ret_code = k_map->call_handler(g_mask);
    if (ret_code == 2) {
      last_ret_code = 2;
      return true;
    }
    reset_states();
    if (ret_code == 0) {
      return true;
    }
    return false;
  }
  else if (last_ret_code == 2) {
    g_mask += mask;
    FILE_LOG(LOG_INFO) << "Call key handler: " << g_mask;
    int ret_code = k_map->call_handler(g_mask);
    reset_states();
    if (ret_code == 0) {
      return true;
    }
  }
  return false;
}

void keyboard_handler::set_keyboard_map(std::shared_ptr<keyboard_map> k_map) {
  this->k_map = k_map;
}

void keyboard_handler::reset_states() {
  last_ret_code = 0;
  g_mask = "";
}

}
