#ifndef INIT_FLEXED_H_
#define INIT_FLEXED_H_

namespace flexed {

extern "C" {
    void init_end();
    void init_start(editor *ed);
    void init_buffer_start();
    void init_buffer_end();
}

}

#endif // INIT_FLEXED_H_
