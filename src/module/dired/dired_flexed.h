#ifndef DIRED_FLEXED_H_
#define DIRED_FLEXED_H_

namespace flexed {

class editor;

extern "C" {
    void dired_end();
    void dired_start(editor* ed);
    void dired_buffer_start();
    void dired_buffer_end();
}

    class dired {
    public:
        dired(editor* ed);
        virtual ~dired();

        void find_file();

    private:
        editor* ed;;
    };

}

#endif // DIRED_FLEXED_H_
