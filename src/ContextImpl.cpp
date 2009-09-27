/*  Berkelium Implementation
 *  ContextImpl.cpp
 *
 *  Copyright (c) 2009, Patrick Reiter Horn
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
