#ifndef _BERKELIUM_WINDOW_HPP_
#define _BERKELIUM_WINDOW_HPP_

#include "berkelium/Context.hpp"

namespace Berkelium {

class WindowImpl;

class BERKELIUM_EXPORT Window {
protected:
    Window();
public:
    static Window* create();
    virtual ~Window();

    inline Context *getContext() {
        return mContext;
    }
    virtual WindowImpl*getImpl()=0;
private:
    Context *mContext;
};

}

#endif
