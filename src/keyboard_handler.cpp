#include <cstring>

#include "keyboard_handler.h"

bool keyboard_handler::on_key_pressed(GdkEventKey* event_key) {
  //static int skip_times = 0;

  /*
  if (skip_times > 0) {
    skip_times--;
    return false;
  }
  */
  std::string mask = "";
  //int cnt_key_pressed = 0;
  if (event_key->state & GDK_MOD1_MASK) {
    g_print("ALT LEFT pressed\n");
    mask += "A";
    //cnt_key_pressed++;
  }
  if (event_key->state & GDK_SHIFT_MASK) {
    g_print("SHIFT pressed\n");
    mask += "S";
    //cnt_key_pressed++;
  }
  if (event_key->state & GDK_CONTROL_MASK) {
    g_print("CTRL pressed\n");
    mask += "C";
    //cnt_key_pressed++;
  }
  if (std::isprint(event_key->keyval)) {
    g_print("ASCII pressed\n");
    mask += event_key->keyval;
    //cnt_key_pressed++;
  }
  if (event_key->keyval == GDK_KEY_Up) {
    g_print("Up pressed\n");
    mask += "U";
  }
  if (event_key->keyval == GDK_KEY_Down) {
    g_print("Down pressed\n");
    mask += "D";
  }
  if (event_key->keyval == GDK_KEY_Left) {
    g_print("Left pressed\n");
    mask += "L";
  }
  if (event_key->keyval == GDK_KEY_Right) {
    g_print("Right pressed\n");
    mask += "R";
  }
  if (event_key->keyval == GDK_KEY_Return) {
      g_print("Return pressed\n");
      mask += "N";
  }

  //skip_times = cnt_key_pressed - 1;
  if (k_map == nullptr)
    return false;

  if (last_ret_code == 0) {
    g_print("call key handler: %s\n", mask.c_str());
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
    g_print("call key handler: %s\n", g_mask.c_str());
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
    g_print("call key handler: %s\n", g_mask.c_str());
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
