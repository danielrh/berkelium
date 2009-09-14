#include "berkelium/Platform.hpp"

#include "chrome/browser/renderer_host/site_instance.h"
#include "Root.hpp"
#include "ContextImpl.hpp"

namespace Berkelium {
ContextImpl::ContextImpl() {
    Profile * profile=Root::getSingleton().getProfile();
    mBrowsingInstance = new BrowsingInstance(profile);
    mBrowsingInstance->AddRef();
}
ContextImpl::~ContextImpl() {
    mBrowsingInstance->Release();
}
ContextImpl* ContextImpl::getImpl() {
    return this;
}
SiteInstance *ContextImpl::getSiteInstance(const std::string &url) {
    return mBrowsingInstance->GetSiteInstanceForURL(GURL(url));
}

}
