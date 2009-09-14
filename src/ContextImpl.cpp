#include "berkelium/Platform.hpp"

#include "chrome/browser/renderer_host/site_instance.h"
#include "Root.hpp"
#include "ContextImpl.hpp"

namespace Berkelium {
ContextImpl::ContextImpl(const ContextImpl&other) {
    other.mSiteInstance->AddRef();
    mSiteInstance=other.mSiteInstance;
}
ContextImpl::ContextImpl(SiteInstance*si) {
    mSiteInstance = si;
    mSiteInstance->AddRef();
}
ContextImpl::~ContextImpl() {
    mSiteInstance->Release();
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

}
