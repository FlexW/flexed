#ifndef SMART_PARENS_FLEXED_H_
#define SMART_PARENS_FLEXED_H_

namespace flexed {

    class editor;

extern "C" {
    void smart_parens_end();
    void smart_parens_start(editor *ed);
    void smart_parens_buffer_start();
    void smart_parens_buffer_end();
}

class smart_parens {
public:
    smart_parens(editor* ed);

    void init_key_bindings();
    void remove_key_bindings();
    void add_closing_paren();

private:
    editor* ed;
};


}

#endif // SMART_PARENS_FLEXED_H_
