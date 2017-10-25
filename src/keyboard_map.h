/**
 * @file keyboard_map.h
 */

#ifndef KEYBOARD_MAP_H_
#define KEYBOARD_MAP_H_

#include <map>
#include <utility>
#include <string>

typedef void *instance_ptr;
typedef void (*internal_function)(instance_ptr);
typedef std::pair<instance_ptr, internal_function> stub;

/**
 * Handels key bindings.
 * This class can be plugged into keyboard_handler. keyboard_handler
 * will then call the call_handler() method every time keyboard input is detected.
 * If a matching entry is found, the corresponding function will be called.
 * Key bindings are specified as strings, without whitespaces or tabs etc.
 * The order of symbols matters A should always be first S second then C and at last ASCII.
 * C = Ctrl, A = Alt (Meta), S = Shift, L = Arrow left, R = Arrow right, U = Arrow up, D = Arrow down, N = Return.
 * Example: "Cx" means Ctrl-x, "CxCs" means Ctrl-x Ctrl-s.
 *
 * The key map has three states and one state can contain at maximum 2 signs.
 * That means the longest possible key binding sequnce is C-x C-x C-x "CxCxCx".
 * To check if a key binding is part of a longer sequence, pass the part key binding to
 * call_handler(). call_handler() will then return a code where one can determine if the key binding is
 * part of a longer sequence.
 * Example:
 * We want to match the key sequence C-x C-y C-z "CxCyCz".
 * User presses keys (c-x).
 * We call call_handler("Cx");
 * call_handler() returns 1, that means we matched a keybinding in state 1.
 * Next user presses again keys (C-y).
 * We call call_handler("CxCy");
 * call_handler() returns 2, taht means we matched a keybinding in state 2.
 * Again the user presses keys (C-z).
 * We call again call_handler("CxCyCz").
 * call_handler() returns 0. That means the key binding matched a function.
 */

namespace flexed {

    class keyboard_map {

public:

  /**
   * Sets a key binding.
   * @param key_binding Key binding.
   * @returns If key binding is unique and element could be insert, true else false.
   */
  template<class C, void (C::*handler)()>
  bool set_key_binding(const std::string& key_binding, C *instance) {
    stub* val_p = new stub(instance, &class_method_stub<C, handler>);
    //auto key_val_p = std::pair<std::string, stub*>(key_binding, val_p);
    insert_key_binding(key_binding, val_p);
    return true;
  }

  template<void (*handler)()>
  bool set_key_binding(const std::string& key_binding) {
    stub* val_p = new stub(nullptr, &function_stub<handler>);
    //auto key_val_p = std::pair<std::string, stub*>(key_binding, val_p);
    insert_key_binding(key_binding, val_p);
    return true;
  }

  /**
   * Removes a key binding.
   * @param key_binding Key binding.
   */
  void remove_key_binding(const std::string& key_binding);

  /**
   * Calls the handler if the key_binding has a match.
   * Returns code that one may use to indicate
   * if a binding was matched.
   * @param key_binding Key binding.
   * @returns 0 = Matched function, 1 = Matched a binding in state1 but
   * no function called yet, 2 = Matched a binding in state2 but
   * no function called yet, -1 = no binding.
   */
  int call_handler(const std::string& key_binding);

private:

  /** Saves key bindings with functions. */
  std::map<std::string, stub*> k_map;

  /** Saves key bindings that are possible in the states. */
  std::multimap<std::string, stub*> k_map_state1;
  std::multimap<std::string, stub*> k_map_state2;
  std::multimap<std::string, stub*> k_map_state3;

  /** Turns a free function into internal function stub. */
  template <void (*handler)()>
  static void function_stub(instance_ptr) {
    return (handler)();
  }

  /** Turns a member function into internal function stub. */
  template <class C, void (C::*handler)()>
  static void class_method_stub(instance_ptr instance) {
    return (static_cast<C*>(instance)->*handler)();
  }

  /**
   * Split key binding into state1, state2, state3.
   */
  std::vector<std::string> split_key_bindings(const std::string& key_binding);

void insert_key_binding(const std::string& key_binding, stub* val_p) {
  auto kb_split = split_key_bindings(key_binding);
  if (kb_split[0].size() >= 1) {
    if (kb_split[1].size() >= 1) {
      k_map_state1.insert(std::pair<std::string, stub*>(kb_split[0], nullptr));
    }
    else {
      k_map_state1.insert(std::pair<std::string, stub*>(kb_split[0], val_p));
    }
  }
  if (kb_split[1].size() >= 1) {
    if (kb_split[2].size() >= 1) {
      k_map_state2.insert(
          std::pair<std::string, stub*>(kb_split[0] + kb_split[1], nullptr));
    }
    else {
      k_map_state2.insert(
          std::pair<std::string, stub*>(kb_split[0] + kb_split[1], val_p));
    }
  }
  if (kb_split[2].size() >= 1) {
    k_map_state3.insert(
        std::pair<std::string, stub*>(kb_split[0] + kb_split[1] + kb_split[2],
                                      val_p));
  }
}

};

}

#endif /* KEYBOARD_MAP_H_ */
