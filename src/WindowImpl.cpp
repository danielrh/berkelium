#include "berkelium/Platform.hpp"
#include "ContextImpl.hpp"

#include "WindowImpl.hpp"
#include "Root.hpp"
#include "chrome/browser/tab_contents/tab_contents.h"
namespace Berkelium {

void WindowImpl::init(SiteInstance*site) {
    mTabContents  = new TabContents(Root::getSingleton().getProfile(),
                                    site,
                                    MSG_ROUTING_NONE,
                                    NULL);
}
WindowImpl::WindowImpl() {
    init(NULL);
    mContext=new ContextImpl(mTabContents->GetSiteInstance());
}
WindowImpl::WindowImpl(const Context*otherContext):Window(otherContext) {
    init(mContext->getImpl()->getSiteInstance());
}
WindowImpl::~WindowImpl() {

}

WindowImpl* WindowImpl::getImpl(){
    return this;
}
}
