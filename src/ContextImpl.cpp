#include "berkelium/Platform.hpp"

#include "chrome/browser/renderer_host/site_instance.h"
#include "Root.hpp"
#include "ContextImpl.hpp"

namespace Berkelium {
ContextImpl::ContextImpl(const ContextImpl&other) {
    other.mBrowsingInstance->AddRef();
    mBrowsingInstance=other.mBrowsingInstance;
}
ContextImpl::ContextImpl() {
    Profile * profile=Root::getSingleton().getProfile();
    mBrowsingInstance = new BrowsingInstance(profile);
    mBrowsingInstance->AddRef();
}
ContextImpl::~ContextImpl() {
    mBrowsingInstance->Release();
}
Context* ContextImpl::clone() const{
    return new ContextImpl(*this);
}
ContextImpl* ContextImpl::getImpl() {
    return this;
}
const ContextImpl* ContextImpl::getImpl() const{
    return this;
}
SiteInstance *ContextImpl::getSiteInstance(const std::string &url) {
    return mBrowsingInstance->GetSiteInstanceForURL(GURL(url));
}

}
