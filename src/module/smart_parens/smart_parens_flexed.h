#ifndef SMART_PARENS_FLEXED_H_
#define SMART_PARENS_FLEXED_H_

namespace flexed {

extern "C" {
    void smart_parens_end();
    void smart_parens_start();
    void smart_parens_buffer_start();
    void smart_parens_buffer_end();
}

class smart_parens {
public:
    static void init_key_bindings();
    static void remove_key_bindings();
    void add_closing_paren();
};


}

#endif // SMART_PARENS_FLEXED_H_
