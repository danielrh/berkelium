#include "berkelium/Platform.hpp"

#include "chrome/browser/renderer_host/site_instance.h"
#include "Root.hpp"
#include "ContextImpl.hpp"
#include "chrome/browser/profile.h"

namespace Berkelium {
ContextImpl::ContextImpl(const ContextImpl&other) {
    other.mSiteInstance->AddRef();
    mSiteInstance=other.mSiteInstance;
    mProfile = other.mProfile;
}
ContextImpl::ContextImpl(Profile *prof, SiteInstance*si) {
    mSiteInstance = si;
    mSiteInstance->AddRef();
    mProfile = prof;
}
ContextImpl::ContextImpl(Profile *prof) {
    mSiteInstance = SiteInstance::CreateSiteInstance(prof);
    mSiteInstance->AddRef();
    mProfile = prof;
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
