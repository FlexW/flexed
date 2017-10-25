#ifndef DIRED_FLEXED_H_
#define DIRED_FLEXED_H_

namespace flexed {

class editor_window;

extern "C" {
    void dired_end();
    void dired_start(editor_window* ed);
    void dired_buffer_start();
    void dired_buffer_end();
}

    class dired {
    public:
        dired(editor_window* ed);
        virtual ~dired();

        void find_file();

    private:
        editor_window* ed;;
    };

}

#endif // DIRED_FLEXED_H_
