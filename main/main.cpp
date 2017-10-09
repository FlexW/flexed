#include <gtkmm/application.h>

#include "../src/flexed_editor.h"

int main(int argc, char *argv[]) {
  auto app = Gtk::Application::create(argc, argv, "com.flexed.editor");
  //Shows the window and returns when it is closed.
  //flexed::editor ed;
  //return app->run(ed);
  return app->run(*flexed::editor::get_instance());
}
