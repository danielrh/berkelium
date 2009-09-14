#ifndef _BERKELIUM_CONTEXTIMPL_HPP_
#define _BERKELIUM_CONTEXTIMPL_HPP_
#include "berkelium/Context.hpp"
#include "chrome/browser/browsing_instance.h"
namespace Berkelium {
class ContextImpl :public Context{
public:
    ContextImpl();
    ~ContextImpl();

    SiteInstance *getSiteInstance(const std::string &url);
    BrowsingInstance *getBrowsingInstance(){
        return mBrowsingInstance;
    }
    ContextImpl *getImpl();
private:
    BrowsingInstance *mBrowsingInstance;
    
};

}

#endif
