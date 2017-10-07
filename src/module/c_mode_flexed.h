#ifndef C_MODE_FLEXED_H_
#define C_MODE_FLEXED_H_

#include "../flexed_mode.h"

namespace flexed {

extern "C" {
    char* c_mode_end();
    void c_mode_start();
    void c_mode_buffer_start(buffer)
    void c_mode_dummy1();
    void c_mode_dummy2();
}

class c_mode {
public:
  c_mode();
  virtual ~c_mode();
  bool start();
  void end();
};

}
#endif // C_MODE_FLEXED_H_
