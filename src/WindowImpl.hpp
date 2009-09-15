#ifndef _BERKELIUM_WINDOWIMPL_HPP_
#define _BERKELIUM_WINDOWIMPL_HPP_
#include "berkelium/Window.hpp"

#include "chrome/browser/renderer_host/render_widget_host.h"
#include "chrome/browser/renderer_host/render_view_host.h"
#include "chrome/browser/renderer_host/render_view_host_delegate.h"
#include "chrome/browser/tab_contents/render_view_host_manager.h"


namespace Berkelium {
class WindowImpl :
        public Window,
        public RenderViewHostManager::Delegate,
        public RenderViewHostDelegate,
        public RenderViewHostDelegate::Resource,
        public RenderViewHostDelegate::View
{
    void init(SiteInstance *);
public:
    WindowImpl *getImpl();
    WindowImpl();
    WindowImpl(const Context*otherContext);
    virtual ~WindowImpl();

protected: /******* RenderViewHostManager::Delegate *******/

    virtual bool CreateRenderViewForRenderManager(
        RenderViewHost* render_view_host);
    virtual void BeforeUnloadFiredFromRenderManager(
        bool proceed, bool* proceed_to_fire_unload);
    virtual void DidStartLoadingFromRenderManager(
        RenderViewHost* render_view_host);
    virtual void RenderViewGoneFromRenderManager(
        RenderViewHost* render_view_host);
    virtual void UpdateRenderViewSizeForRenderManager();
    virtual void NotifySwappedFromRenderManager();
    virtual NavigationController& GetControllerForRenderManager();
    virtual DOMUI* CreateDOMUIForRenderManager(const GURL& url);
    virtual NavigationEntry*
        GetLastCommittedNavigationEntryForRenderManager();

protected: /******* RenderViewHostDelegate *******/

  virtual View* GetViewDelegate();
  virtual Resource* GetResourceDelegate();

  // Functions for managing switching of Renderers. For TabContents, this is
  // implemented by the RenderViewHostManager
//  virtual RendererManagement* GetRendererManagementDelegate();
  // Functions that integrate with other browser services.
//  virtual BrowserIntegration* GetBrowserIntegrationDelegate();

protected: /******* RenderViewHostDelegate::Resource *******/

    virtual void DidStartProvisionalLoadForFrame(
        RenderViewHost* render_view_host,
        bool is_main_frame,
        const GURL& url);

    virtual void DidStartReceivingResourceResponse(
        ResourceRequestDetails* details);

    virtual void DidRedirectProvisionalLoad(int32 page_id,
                                            const GURL& source_url,
                                            const GURL& target_url);

    virtual void DidRedirectResource(ResourceRequestDetails* details);

    virtual void DidLoadResourceFromMemoryCache(
        const GURL& url,
        const std::string& frame_origin,
        const std::string& main_frame_origin,
        const std::string& security_info);

    virtual void DidFailProvisionalLoadWithError(
        RenderViewHost* render_view_host,
        bool is_main_frame,
        int error_code,
        const GURL& url,
        bool showing_repost_interstitial);

    virtual void DocumentLoadedInFrame();

protected: /******* RenderViewHostDelegate::View *******/
    virtual void CreateNewWindow(int route_id,
                                 base::WaitableEvent* modal_dialog_event);
    virtual void CreateNewWidget(int route_id, bool activatable);
    virtual void ShowCreatedWindow(int route_id,
                                   WindowOpenDisposition disposition,
                                   const gfx::Rect& initial_pos,
                                   bool user_gesture,
                                   const GURL& creator_url);
    virtual void ShowCreatedWidget(int route_id,
                                   const gfx::Rect& initial_pos);
    virtual void ShowContextMenu(const ContextMenuParams& params);
    virtual void StartDragging(const WebDropData& drop_data,
                               WebKit::WebDragOperationsMask allowed_ops);
    virtual void UpdateDragCursor(WebKit::WebDragOperation operation);
    virtual void GotFocus();
    virtual void TakeFocus(bool reverse);
    virtual void HandleKeyboardEvent(const NativeWebKeyboardEvent& event);
    virtual void HandleMouseEvent();
    virtual void HandleMouseLeave();
    virtual void UpdatePreferredWidth(int pref_width);

};

}

#endif
