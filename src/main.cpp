#include <gtkmm/application.h>

#include "flexed_editor.h"

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "com.flexed.editor");

  flexed::editor editor;

  //Shows the window and returns when it is closed.
  return app->run(editor);
}
