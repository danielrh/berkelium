#ifndef _BERKELIUM_CONTEXTIMPL_HPP_
#define _BERKELIUM_CONTEXTIMPL_HPP_
#include "berkelium/Window.hpp"
namespace Berkelium {
class WindowImpl :public Window {

public:
    WindowImpl *getImpl();
    WindowImpl();
    WindowImpl(const Context*otherContext);
    virtual ~WindowImpl();
    
};

}

#endif
