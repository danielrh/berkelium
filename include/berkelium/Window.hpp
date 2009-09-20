#ifndef _BERKELIUM_WINDOW_HPP_
#define _BERKELIUM_WINDOW_HPP_

#include "berkelium/Context.hpp"

namespace Berkelium {

class WindowImpl;

class BERKELIUM_EXPORT Window {
protected:
    Window();
    Window (const Context*otherContext);
    Context *mContext;

public:
    static Window* create();
    static Window* create(const Context&otherContext);
    virtual ~Window();

    inline Context *getContext() const {
        return mContext;
    }

    virtual void resize(int width, int height)=0;
    virtual bool navigateTo(const std::string &url)=0;
    virtual WindowImpl*getImpl()=0;

};

}

#endif
