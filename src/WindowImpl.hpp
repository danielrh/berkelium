#ifndef _BERKELIUM_WINDOWIMPL_HPP_
#define _BERKELIUM_WINDOWIMPL_HPP_
#include "berkelium/Window.hpp"
class TabContents;


namespace Berkelium {
class WindowImpl :public Window {
    TabContents*mTabContents;
    void init(SiteInstance*);
public:
    TabContents*getTabContents(){return mTabContents;}
    WindowImpl *getImpl();
    WindowImpl();
    WindowImpl(const Context*otherContext);
    virtual ~WindowImpl();
    
};

}

#endif
