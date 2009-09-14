#ifndef _BERKELIUM_CONTEXTIMPL_HPP_
#define _BERKELIUM_CONTEXTIMPL_HPP_
#include "berkelium/Context.hpp"
#include "chrome/browser/browsing_instance.h"
namespace Berkelium {
class ContextImpl :public Context{
public:
    ContextImpl(const ContextImpl&other);
    ContextImpl(SiteInstance*);
    ~ContextImpl();

    SiteInstance *getSiteInstance(){
        return mSiteInstance;
    }
    ContextImpl *getImpl();
    const ContextImpl *getImpl()const;
    Context*clone()const;
private:
    SiteInstance *mSiteInstance;
    
};

}

#endif
