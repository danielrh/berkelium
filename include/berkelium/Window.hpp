#ifndef _BERKELIUM_WINDOW_HPP_
#define _BERKELIUM_WINDOW_HPP_

#include "berkelium/Context.hpp"

namespace Berkelium {

class WindowImpl;

class BERKELIUM_EXPORT Window {
protected:
    Window();
    Window (const Context*otherContext);
public:
    static Window* create();
    static Window* create(const Context&otherContext);
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
