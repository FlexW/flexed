/**
 * @file flexed_global_text_buffer_container.h
 */

#ifndef FLEXED_GLOBAL_TEXT_BUFFER_CONTAINER_H_
#define FLEXED_GLOBAL_TEXT_BUFFER_CONTAINER_H_

#include <list>
#include <gtkmm.h>

#include "flexed_text_buffer.h"
#include "orderd_container.h"
#include "flexed_text_buffer_container.h"

namespace flexed {

//class text_buffer;
class text_buffer_container;

/**
 * Stores all open text buffers.
 * Notifies registerd containers if a new buffer was added.
 */
class global_text_buffer_container :  public orderd_container< Glib::RefPtr<text_buffer> > {

public:
  global_text_buffer_container() {}
  virtual ~global_text_buffer_container() {}

  /**
   * Register handler method to notify if buffer added.
   * This method does not check if the handler was already registerd.
   * In the case the handler is added multiple times, it will be called multiple times.
   * @param h Handler to notify.
   */
  void register_text_buffer_added(text_buffer_container* h);

  /**
   * Remove a registered handler.
   * @param h Handler to remove.
   */
  void remove_text_buffer_handler(text_buffer_container* h);

protected:
  void on_added(Glib::RefPtr<text_buffer> obj) override;
  void before_removed(Glib::RefPtr<text_buffer> obj) override;

private:

  /** Stores handlers for buffer added event. */
  std::list<text_buffer_container*> handler_list;

  /**
   * Calls all handler that wants a notification if a buffer was added.
   * @param buffer TextBuffer that was added.
   */
  void text_buffer_added(Glib::RefPtr<text_buffer> buffer);

  /**
   * Calls all handler that wants a notification if a buffer was removed.
   * @param buffer TextBuffer that gets removed.
   */
  void text_buffer_removed(Glib::RefPtr<text_buffer> buffer);
};

}

#endif /* FLEXED_GLOBAL_TEXT_BUFFER_CONTAINER_H_ */
