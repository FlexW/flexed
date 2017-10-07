#ifndef FLEXED_MODE_H_
#define FLEXED_MODE_H_

namespace flexed {

class mode {

public:
    mode() {}
    virtual ~mode() {}
    virtual bool start() = 0;
    virtual void end() = 0;
};
}

#endif // FLEXED_MODE_H_
