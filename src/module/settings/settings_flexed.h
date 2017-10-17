#ifndef SETTINGS_FLEXED_H_
#define SETTINGS_FLEXED_H_

namespace flexed {

class editor;

extern "C" {
    void settings_end();
    void settings_start(editor* ed);
    void settings_buffer_start();
    void settings_buffer_end();

    void settings_set_linnum();

    void settings_unset_linnum();
}

    class settings {
    public:
        settings(editor* ed);

        virtual ~settings();

        /**
         * Sets line numbers in the active buffer.
         */
        void set_linnum();

        /**
         * Unset line numbers in the active buffer.
         */
        void unset_linnum();

        /**
         * Sets line numbers in every buffer.
         */
        void set_linnum_global();

        /**
         * Unset line numbers in every buffer.
         */
        void unset_linnum_global();

    private:

        editor* ed;
    };

}

#endif // SETTINGS_FLEXED_H_
