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

bool TabContents::CreateRenderViewForRenderManager(
    RenderViewHost* render_view_host) {
  // If the pending navigation is to a DOMUI, tell the RenderView about any
  // bindings it will need enabled.
  if (render_manager_.pending_dom_ui())
    render_view_host->AllowBindings(
        render_manager_.pending_dom_ui()->bindings());

  RenderWidgetHostView* rwh_view = view_->CreateViewForWidget(render_view_host);
  if (!render_view_host->CreateRenderView())
    return false;

  // Now that the RenderView has been created, we need to tell it its size.
  rwh_view->SetSize(view_->GetContainerSize());

  UpdateMaxPageIDIfNecessary(render_view_host->site_instance(),
                             render_view_host);
  return true;
}

WindowImpl* WindowImpl::getImpl(){
    return this;
}
}
