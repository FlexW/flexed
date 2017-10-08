#include <gtkmm/application.h>

#include "flexed_editor.h"

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "com.flexed.editor");
  //Shows the window and returns when it is closed.
  return app->run(flexed::editor::get_instance());
}
