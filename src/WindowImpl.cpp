#include "berkelium/Platform.hpp"
#include "ContextImpl.hpp"

#include "WindowImpl.hpp"
#include "Root.hpp"
#include "chrome/browser/tab_contents/tab_contents.h"
#include "chrome/browser/dom_ui/dom_ui.h"
#include "chrome/browser/dom_ui/dom_ui_factory.h"
#include "chrome/browser/renderer_host/render_widget_host_view.h"
namespace Berkelium {
WindowImpl temp;
void WindowImpl::init(SiteInstance*site) {
    width=1024;
    height=768;
    origin_x=0;
    origin_y=0;
    render_manager_.reset(new RenderViewHostManager(this,this));
/*
    mTabContents  = new TabContents(Root::getSingleton().getProfile(),
                                    site,
                                    MSG_ROUTING_NONE,
                                    NULL);*/
}
WindowImpl::WindowImpl() :controller_(NULL,Root::getSingleton().getProfile()){
    init(NULL);
//FIXME
    //mContext=new ContextImpl(mTabContents->GetSiteInstance());
}
WindowImpl::WindowImpl(const Context*otherContext):Window(otherContext),controller_(NULL,Root::getSingleton().getProfile()) {
    init(mContext->getImpl()->getSiteInstance());
}
WindowImpl::~WindowImpl() {
}
RenderWidgetHostView* WindowImpl::CreateViewForWidget(RenderWidgetHost*render_widget_host){
    UNIMPLEMENTED();
    return NULL;
}

bool WindowImpl::CreateRenderViewForRenderManager(
    RenderViewHost* render_view_host) {
  // If the pending navigation is to a DOMUI, tell the RenderView about any
  // bindings it will need enabled.
  if (render_manager_->pending_dom_ui())
    render_view_host->AllowBindings(
        render_manager_->pending_dom_ui()->bindings());

  RenderWidgetHostView* rwh_view = this->CreateViewForWidget(render_view_host);
  if (!render_view_host->CreateRenderView())
    return false;

  // Now that the RenderView has been created, we need to tell it its size.
  rwh_view->SetSize(this->GetContainerSize());

  UpdateMaxPageIDIfNecessary(render_view_host->site_instance(),
                             render_view_host);
  return true;
}



void WindowImpl::UpdateMaxPageIDIfNecessary(SiteInstance* site_instance,
                                             RenderViewHost* rvh) {
  // If we are creating a RVH for a restored controller, then we might
  // have more page IDs than the SiteInstance's current max page ID.  We must
  // make sure that the max page ID is larger than any restored page ID.
  // Note that it is ok for conflicting page IDs to exist in another tab
  // (i.e., NavigationController), but if any page ID is larger than the max,
  // the back/forward list will get confused.
    int max_restored_page_id = controller_.max_restored_page_id();
  if (max_restored_page_id > 0) {
    int curr_max_page_id = site_instance->max_page_id();
    if (max_restored_page_id > curr_max_page_id) {
      // Need to update the site instance immediately.
      site_instance->UpdateMaxPageID(max_restored_page_id);

      // Also tell the renderer to update its internal representation.  We
      // need to reserve enough IDs to make all restored page IDs less than
      // the max.
      if (curr_max_page_id < 0)
        curr_max_page_id = 0;
      rvh->ReservePageIDRange(max_restored_page_id - curr_max_page_id);
    }
  }
}
int WindowImpl::GetBrowserWindowID() const {
    return controller_.window_id().id();
}
ViewType::Type WindowImpl::GetRenderViewType()const {
    return ViewType::TAB_CONTENTS;
}
WindowImpl* WindowImpl::getImpl(){
    return this;
}
}
