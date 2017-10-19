/**
 * @file orderd_container.h
 * @brief Orderd container.
 */

#ifndef ORDERD_CONTAINER_H_
#define ORDERD_CONTAINER_H_

#include <functional>
#include <vector>
#include <gtkmm.h>

#include "flexed_text_buffer.h"

namespace flexed {
class text_buffer;
}

/**
 * Container class to store objects orderd and iterate over them.
 * If there are elements in the container then there will always be one active element.
 * If a element gets added, this element is the active element.
 * To change the active element, one must call next() or previous(). Then the
 * next or previous element will become active.
 */
template<class T>
class orderd_container {

public:
  orderd_container() {}
  virtual ~orderd_container() {}

  /**
   * Adds a element at the first position.
   * @param obj Text buffer to add.
   */
  void add(T obj) {
    obj_vec.insert(obj_vec.begin(), obj);
    on_added(obj);
  }

  /**
   * Removes the element.
   * @param obj Element to remove.
   */
  void remove(T obj) {
    int pos = 0;
    int size = obj_vec.size();
    for (; pos < size; pos++) {
      if (obj_vec[pos] == obj)
    break;
    }
    if (pos == size)
      return;

    before_removed(obj_vec[pos]);
    obj_vec.erase(obj_vec.begin() + pos);
  }

  /**
   * Returns the element at the given position.
   * @param pos Position of element.
   * @returns Element, on error: nullptr
   */
  T get(int pos) {
    return obj_vec.at(obj_vec.begin() + pos);
  }

  /**
   * Returns the active element.
   * @returns Element, if no elements in container: nullptr.
   */
  T active() {
    return obj_vec.front();
  }

  /**
   * Sets the active element to the next element in the container.
   */
  void next() {
    int size = obj_vec.size();
    for (int i = 1; i < size; i++) {
      swap_element(i, i - 1);
    }
    on_next();
  }

  /**
   * Sets the active element to the previous element in the container.
   */
  void previous() {
    int size = obj_vec.size();
    for(int i = size - 1 - 1; i >= 0; i--) {
      swap_element(i, i + 1);
    }
    on_previous();
  }

  /**
   * Returns the number of elements.
   * @returns Number of elements.
   */
  int size() {
    return obj_vec.size();
  }

  /**
   * Returns the containers obj_vec.
   * @returns obj_vec.
   */
  std::vector<T>& get_obj_vec() {
    return obj_vec;
  }

  /**
   * Iterates over all elements in the container.
   * @param func Function to which every object gets passed.
   * If the function returns false, iterating will be aborted.
   * @returns False if iterating was aborted. True if not.
   */
  bool for_each(std::function<bool (Glib::RefPtr<flexed::text_buffer>)> func) {
    for (auto obj : obj_vec) {
      if (!func(obj)) {
    return false;
      }
    }
    return true;
  }

protected:

  /**
   * Container to store elements.
   * The element at position 0 will always be the active element.
   */
  std::vector<T> obj_vec;

  /**
   * This method can be overwritten by child classes,
   * to react if the container was a element added.
   * @param obj Element that was added.
   */
  virtual void on_added(T obj) {}

  /**
   * This method can be overwritten by child classes,
   * to react if the container is going to remove a element.
   * @param pos Position of element to removed.
   */
  virtual void before_removed(T obj) {}

  /**
   * This method can be overwritten by child class to react if
   * the container marks the next item as active.
   * This gets called after the elements swapped.
   */
  virtual void on_next() {}

  /**
   * This method can be overwritten by child class to react if
   * the container marks the previous item as active.
   * This gets called after the elements swapped.
   */
  virtual void on_previous() {}

private:

  /**
   * Swaps two elements from obj_vec. If one position out of range, nothing happens.
   * @param pos1 Position element 1.
   * @param pos2 Position element 2.
   */
  void swap_element(int pos1, int pos2) {
    if (pos1 >= obj_vec.size() || pos2 >= obj_vec.size())
      return;
    auto tmp = obj_vec[pos1];
    obj_vec[pos1] = obj_vec[pos2];
    obj_vec[pos2] = tmp;
  }
};

#endif // ORDERD_CONTAINER_H_
