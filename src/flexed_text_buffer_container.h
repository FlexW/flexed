/**
 * @file flexed_text_buffer_container.h
 */

#ifndef FLEXED_TEXT_BUFFER_CONTAINER_H_
#define FLEXED_TEXT_BUFFER_CONTAINER_H_

#include <gtkmm.h>

#include "flexed_text_buffer.h"
#include "orderd_container.h"
#include "flexed_global_text_buffer_container.h"

extern "C" {

namespace flexed {

class global_text_buffer_container;

/**
 * Text buffer container that is connected to a TextView.
 * Through this container a TextView can always know it's direction of
 * opend text buffers.
 * This container gets notified from global_text_buffer_container if a new text buffer is added.
 */
class text_buffer_container : public orderd_container< Glib::RefPtr<text_buffer> > {

public:
  /**
   * @param global_container Container that notifes this container if new buffer added.
   * @param view Corresponding TextView.
   */
    text_buffer_container(std::shared_ptr<global_text_buffer_container> global_container,
                          Gtk::TextView* view);

  virtual ~text_buffer_container();

  /**
   * Gets called from global_text_buffer_container if a new buffer is added.
   */
  void on_text_buffer_added(Glib::RefPtr<text_buffer> buffer);

  /**
   * Gets called from global_text_buffer_container if a buffer was removed.
   */
  void on_text_buffer_removed(Glib::RefPtr<text_buffer> buffer);

  /**
   * Takes over the list from an other text buffer.
   * @param c Text buffer container.
   */
  void take_over_text_buffer_container_list(orderd_container<Glib::RefPtr <text_buffer> >& c);

  /**
   * Sets the corresponding TextView buffer to the active one.
   */
  void set_buffer();

protected:

  Gtk::TextView* text_view = nullptr;

  /**
   * Sets the text_view buffer to the active one.
   */
  virtual void on_next() override;

  /**
   * Sets the text_view buffer to the active one.
   */
  virtual void on_previous() override;
};

}

}

#endif // TEXT_BUFFER_CONTAINER_H_
