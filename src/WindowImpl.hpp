/*  Berkelium Implementation
 *  WindowImpl.hpp
 *
 *  Copyright (c) 2009, Daniel Reiter Horn
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

#ifndef _BERKELIUM_WINDOWIMPL_HPP_
#define _BERKELIUM_WINDOWIMPL_HPP_
#include "berkelium/Widget.hpp"
#include "berkelium/Window.hpp"
#include "base/gfx/rect.h"
#include "base/gfx/size.h"
#include "chrome/browser/renderer_host/render_widget_host.h"
#include "chrome/browser/renderer_host/render_view_host.h"
#include "chrome/browser/renderer_host/render_view_host_delegate.h"
#include "chrome/browser/tab_contents/navigation_controller.h"

class RenderProcessHost;
class Profile;

namespace Berkelium {
class WindowView;
class RenderWidget;
class MemoryRenderViewHost;
class Rect;
class WindowImpl :
        public Window,
        public RenderViewHostDelegate,
        public RenderViewHostDelegate::Resource,
        public RenderViewHostDelegate::View
{

    void init(SiteInstance *, int routingId);
    NavigationEntry* CreateNavigationEntry(
        const GURL&url,
        const GURL&referrer,
        PageTransition::Type transition);

    WindowImpl(const Context*otherContext, int routingId);

public:
    WindowImpl *getImpl();
//    WindowImpl();
    WindowImpl(const Context*otherContext);
    virtual ~WindowImpl();

    const GURL &getCurrentURL() {
        return mCurrentURL;
    }

    void executeJavascript(const std::wstring &javascript);
    bool navigateTo(const std::string &url);

    Profile *profile() const;
    RenderProcessHost *process() const;
    RenderWidgetHostView *view() const;
    RenderViewHost *host() const;

    virtual Widget* getWidget() const;

    virtual void setTransparent(bool istrans);

    virtual void focus();
    virtual void unfocus();
    virtual void mouseMoved(int xPos, int yPos);
    virtual void mouseButton(unsigned int buttonID, bool down);
    virtual void mouseWheel(int xScroll, int yScroll);
    virtual void textEvent(std::wstring evt);
    virtual void keyEvent(bool pressed, int mods, int vk_code, int scancode);

    virtual void refresh();
    virtual void cut();
    virtual void copy();
    virtual void paste();
    virtual void undo();
    virtual void redo();
    virtual void del();
    virtual void selectAll();

    bool doNavigateTo(
        const GURL &newURL,
        const GURL &referrerURL,
        bool reload);

    void SetContainerBounds(const gfx::Rect &rc);
    void resize(int width, int height);

    void onPaint(Widget *wid,
                 const unsigned char *sourceBuffer, const Rect &rect,
                 int dx, int dy, const Rect &scrollRect);
    void onWidgetDestroyed(Widget *wid);
protected:
    ContextImpl *getContextImpl() const;

    bool CreateRenderViewForRenderManager(RenderViewHost* render_view_host);

protected: /******* RenderViewHostDelegate *******/

    virtual RenderViewHostDelegate::View* GetViewDelegate();
    virtual RenderViewHostDelegate::Resource* GetResourceDelegate();
    virtual void DidDisplayInsecureContent(){}
    virtual void DidRunInsecureContent(const std::string&){}
    virtual void CreateNewWindow(int){}
    virtual void DidStartLoading(RenderViewHost* render_view_host);
    virtual void DidStopLoading(RenderViewHost* render_view_host);

  // Functions for managing switching of Renderers. For TabContents, this is
  // implemented by the RenderViewHostManager
//  virtual RendererManagement* GetRendererManagementDelegate();
  // Functions that integrate with other browser services.
//  virtual BrowserIntegration* GetBrowserIntegrationDelegate();

    virtual int GetBrowserWindowID() const;
    ViewType::Type GetRenderViewType()const;

protected: /******* RenderViewHostDelegate::Resource *******/

    void GetContainerBounds(gfx::Rect* rc) const{
        rc->SetRect(mRect.x(), mRect.y(), mRect.width(), mRect.height());
    }
    gfx::Size GetContainerSize()const {
        gfx::Rect rc;
        GetContainerBounds(&rc);
        return gfx::Size(rc.width(),rc.height());
    }

    virtual RenderWidgetHostView* CreateViewForWidget(RenderWidgetHost*render_widget_host);
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

private:

    GURL mCurrentURL;
    int zIndex;

    int mMouseX;
    int mMouseY;

    gfx::Rect mRect;
    NavigationEntry *mLastNavEntry;
    NavigationEntry *mNavEntry;

    std::map<int, WindowImpl*> mNewlyCreatedWindows;
    std::map<int, RenderWidget*> mNewlyCreatedWidgets;

    // Manages creation and swapping of render views.
    RenderViewHost *mRenderViewHost;

};

}

#endif
