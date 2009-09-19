#ifndef _BERKELIUM_CONTEXTIMPL_HPP_
#define _BERKELIUM_CONTEXTIMPL_HPP_
#include "berkelium/Context.hpp"
#include "chrome/browser/browsing_instance.h"
namespace Berkelium {
class ContextImpl :public Context{
public:
    ContextImpl(const ContextImpl&other);
    ContextImpl(Profile*prof, SiteInstance*si);
    ContextImpl(Profile*prof);
    ~ContextImpl();

    SiteInstance *getSiteInstance(){
        return mSiteInstance;
    }
    ContextImpl *getImpl();
    const ContextImpl *getImpl()const;

    Profile*profile()const {
        return mProfile;
    }

    Context*clone()const;
private:
    SiteInstance *mSiteInstance;
    Profile *mProfile;
    
};

}

#endif
