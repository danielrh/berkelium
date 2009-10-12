/*  Berkelium Implementation
 *  WindowImpl.cpp
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

#include "berkelium/Platform.hpp"
#include "ContextImpl.hpp"
#include "RenderWidget.hpp"
#include "WindowImpl.hpp"
#include "MemoryRenderViewHost.hpp"
#include "Root.hpp"
#include "berkelium/WindowDelegate.hpp"

#include "base/file_util.h"
#include "net/base/net_util.h"
#include "chrome/browser/renderer_host/render_view_host.h"
#include "chrome/common/render_messages.h"
#include "chrome/browser/tab_contents/tab_contents.h"
#include "chrome/browser/dom_ui/dom_ui.h"
#include "chrome/browser/dom_ui/dom_ui_factory.h"
#include "chrome/browser/renderer_host/render_widget_host_view.h"
#include "chrome/browser/renderer_host/render_view_host_factory.h"
#include "chrome/browser/browser_url_handler.h"
#include "chrome/common/native_web_keyboard_event.h"

namespace Berkelium {
//WindowImpl temp;
void WindowImpl::init(SiteInstance*site, int routing_id) {
    mRenderViewHost = RenderViewHostFactory::Create(
        site,
        this,
        routing_id,
        NULL);
    CreateRenderViewForRenderManager(host());
}

WindowImpl::WindowImpl(const Context*otherContext):Window(otherContext) {
    mMouseX = 0;
    mMouseY = 0;
    mCurrentURL = GURL("about:blank");
    mLastNavEntry = NULL;
    mNavEntry = NULL;
    zIndex = 0;
    init(mContext->getImpl()->getSiteInstance(), MSG_ROUTING_NONE);
}
WindowImpl::WindowImpl(const Context*otherContext, int routing_id):Window(otherContext) {
    mMouseX = 0;
    mMouseY = 0;
    mCurrentURL = GURL("about:blank");
    mLastNavEntry = NULL;
    mNavEntry = NULL;
    zIndex = 0;
    init(mContext->getImpl()->getSiteInstance(), routing_id);
}
WindowImpl::~WindowImpl() {
    RenderViewHost* render_view_host = mRenderViewHost;
    mRenderViewHost = NULL;
    render_view_host->Shutdown();

    delete mLastNavEntry;
    delete mNavEntry;
}

RenderProcessHost *WindowImpl::process() const {
    return host()->process();
}
RenderWidgetHostView *WindowImpl::view() const {
    if (!host()) {
        return NULL;
    }
    return host()->view();
}
RenderViewHost *WindowImpl::host() const {
    return mRenderViewHost;
}

int WindowImpl::GetBrowserWindowID() const {
    // FIXME!!!!
    return 0;
}

ViewType::Type WindowImpl::GetRenderViewType()const {
    return ViewType::TAB_CONTENTS;
}

void MakeNavigateParams(const NavigationEntry& entry, bool reload,
                        ViewMsg_Navigate_Params* params) {
  params->page_id = entry.page_id();
  params->url = entry.url();
  params->referrer = entry.referrer();
  params->transition = entry.transition_type();
  params->state = entry.content_state();
  params->reload = reload;
  params->request_time = base::Time::Now();
}

Widget *WindowImpl::getWidget() const {
    return static_cast<RenderWidget*>(view());
}


void WindowImpl::setTransparent(bool istrans) {
    SkBitmap bg;
    int bitmap = 0;
    if (istrans) {
        bg.setConfig(SkBitmap::kA1_Config, 1, 1);
        bg.setPixels(&bitmap);
    }
    host()->Send(new ViewMsg_SetBackground(host()->routing_id(),bg));
}

void WindowImpl::focus() {
    FrontToBackIter iter = frontIter();
    if (iter != frontEnd()) {
        (*iter)->focus();
        ++iter;
    }
    while (iter != frontEnd()) {
        (*iter)->unfocus();
        ++iter;
    }
    getWidget()->focus();
}
void WindowImpl::unfocus() {
    FrontToBackIter iter = frontIter();
    while (iter != frontEnd()) {
        (*iter)->unfocus();
        ++iter;
    }
}

void WindowImpl::mouseMoved(int xPos, int yPos) {
    mMouseX = xPos;
    mMouseY = yPos;
    for (FrontToBackIter iter = frontIter(); iter != frontEnd(); ++iter) {
        Rect r = (*iter)->getRect();
        (*iter)->mouseMoved(xPos - r.left(), yPos - r.top());
    }
}
void WindowImpl::mouseButton(unsigned int buttonID, bool down) {
    FrontToBackIter iter = frontIter();
    if (iter != frontEnd()) {
        (*iter)->mouseButton(buttonID, down);
    }
}
void WindowImpl::mouseWheel(int xScroll, int yScroll) {
    Widget *wid = getWidgetAtPoint(mMouseX, mMouseY, true);
    if (wid) {
        wid->mouseButton(xScroll, yScroll);
    }
}

void WindowImpl::textEvent(std::wstring evt) {
    FrontToBackIter iter = frontIter();
    if (iter != frontEnd()) {
        (*iter)->textEvent(evt);
    }
}
void WindowImpl::keyEvent(bool pressed, int mods, int vk_code, int scancode) {
    FrontToBackIter iter = frontIter();
    if (iter != frontEnd()) {
        (*iter)->keyEvent(pressed, mods, vk_code, scancode);
    }
}



void WindowImpl::resize(int width, int height) {
    SetContainerBounds(gfx::Rect(0, 0, width, height));
}

void WindowImpl::cut() {
    if (host()) host()->Cut();
}
void WindowImpl::copy() {
    if (host()) host()->Copy();
}
void WindowImpl::paste() {
    if (host()) host()->Paste();
}
void WindowImpl::undo() {
    if (host()) host()->Undo();
}
void WindowImpl::redo() {
    if (host()) host()->Redo();
}
void WindowImpl::del() {
    if (host()) host()->Delete();
}
void WindowImpl::selectAll() {
    if (host()) host()->SelectAll();
}

void WindowImpl::refresh() {
    doNavigateTo(mCurrentURL, GURL(), true);
}

void WindowImpl::SetContainerBounds (const gfx::Rect &rc) {
    mRect = rc;
    RenderWidgetHostView* myview = view();
    if (myview) {
        myview->SetSize(this->GetContainerSize());
    }
    RenderViewHost* myhost = host();
    if (myhost) {
        static_cast<MemoryRenderViewHost*>(myhost)->Memory_WasResized();        
    }
}

void WindowImpl::executeJavascript(const std::wstring &javascript) {
    if (host()) {
        host()->ExecuteJavascriptInWebFrame(std::wstring(), javascript);
    }
}

bool WindowImpl::navigateTo(const std::string &url) {
    this->mCurrentURL = GURL(url);
    return doNavigateTo(this->mCurrentURL, GURL(), false);
}
bool WindowImpl::doNavigateTo(
        const GURL &newURL,
        const GURL &referrerURL,
        bool reload)
{
    if (view()) {
        view()->Hide();
    }
    if (mLastNavEntry) {
        delete mLastNavEntry;
    }
    mLastNavEntry = mNavEntry;
    mNavEntry = CreateNavigationEntry(
        newURL,
        referrerURL,
        PageTransition::TYPED);

    if (!mNavEntry) {
        mNavEntry = mLastNavEntry;
        return false;
    }

    if (!host()) {
        return false;  // Unable to create the desired render view host.
    }

    // Navigate in the desired RenderViewHost.
    ViewMsg_Navigate_Params navigate_params;
    MakeNavigateParams(*mNavEntry, reload, &navigate_params);
    host()->Navigate(navigate_params);
    if (view()) {
        view()->Show();
    }
    return true;
}

NavigationEntry* WindowImpl::CreateNavigationEntry(
    const GURL& url, const GURL& referrer, PageTransition::Type transition) {
  // Allow the browser URL handler to rewrite the URL. This will, for example,
  // remove "view-source:" from the beginning of the URL to get the URL that
  // will actually be loaded. This real URL won't be shown to the user, just
  // used internally.
  GURL loaded_url(url);
  BrowserURLHandler::RewriteURLIfNecessary(&loaded_url, profile());

  NavigationEntry* entry = new NavigationEntry(NULL, -1, loaded_url, referrer,
                                               string16(), transition);
  entry->set_virtual_url(url);
  entry->set_user_typed_url(url);
  if (url.SchemeIsFile()) {
    entry->set_title(WideToUTF16Hack(
        file_util::GetFilenameFromPath(net::FormatUrl(url, L"en_US"))));
  }
  return entry;
}



Profile* WindowImpl::profile() const{
    //return Root::getSingleton().getProfile();
    return getContextImpl()->profile();
}
WindowImpl* WindowImpl::getImpl() {
    return this;
}

ContextImpl *WindowImpl::getContextImpl() const {
    return static_cast<ContextImpl*>(getContext());
}

void WindowImpl::onPaint(Widget *wid,
                         const unsigned char *sourceBuffer, const Rect &rect,
                         int dx, int dy, const Rect &scrollRect) {
    if (mDelegate) {
        if (wid) {
            mDelegate->onWidgetPaint(
                this, wid, sourceBuffer, rect, dx, dy, scrollRect);
        } else {
            mDelegate->onPaint(
                this, sourceBuffer, rect, dx, dy, scrollRect);
        }
    }
}

void WindowImpl::onWidgetDestroyed(Widget *wid) {
    if (wid != getWidget()) {
        mDelegate->onWidgetDestroyed(this, wid);
    }
    removeWidget(wid);
}



/******* RenderViewHostManager::Delegate *******/

bool WindowImpl::CreateRenderViewForRenderManager(
    RenderViewHost* render_view_host) {

  RenderWidget* rwh_view = 
      static_cast<RenderWidget*>(this->CreateViewForWidget(render_view_host));

  if (!render_view_host->CreateRenderView())
    return false;

  // Now that the RenderView has been created, we need to tell it its size.
  rwh_view->SetSize(this->GetContainerSize());
  render_view_host->set_view(rwh_view);

  appendWidget(rwh_view);

//  UpdateMaxPageIDIfNecessary(render_view_host->site_instance(),
//                             render_view_host);
  return true;

}



// See 'Browser::BeforeUnloadFired' in chrome/browser/browser.cc
// for an example of how to properly handle beforeUnload and unload
// in the case of the user closing a whole window.
/*
void WindowImpl::BeforeUnloadFiredFromRenderManager(
    bool proceed,
    bool* proceed_to_fire_unload)
{
    *proceed_to_fire_unload = proceed;
    if (mDelegate) {
        if (proceed) {
            mDelegate->onBeforeUnload(this, proceed_to_fire_unload);
        } else {
            mDelegate->onCancelUnload(this);
        }
    }
}
*/

void WindowImpl::DidStartLoading(
    RenderViewHost* render_view_host) {

    render_view_host->SetIsLoading(true);

    if (mDelegate) {
//        mDelegate->onStartLoading(this);
    }
}
void WindowImpl::DidStopLoading(
    RenderViewHost* render_view_host) {

    render_view_host->SetIsLoading(true);

/*
    if (mDelegate) {
        mDelegate->onStopLoading(this);
    }
*/
}
/*
void WindowImpl::RenderViewGoneFromRenderManager(
    RenderViewHost* render_view_host) {
    if (render_view_host != static_cast<RenderViewHost*>(host())) {
        return; // The pending destination page crashed: don't care.
    }
    if (mDelegate) {
        mDelegate->onCrashed(this);
    }
}
*/

/******* RenderViewHostDelegate *******/

RenderViewHostDelegate::View* WindowImpl::GetViewDelegate() {
    return this;
}
RenderViewHostDelegate::Resource* WindowImpl::GetResourceDelegate() {
    return this;
}

/******* RenderViewHostDelegate::Resource *******/

RenderWidgetHostView* WindowImpl::CreateViewForWidget(RenderWidgetHost*render_widget_host) {
    RenderWidget *wid = new RenderWidget(this);
    wid->setHost(render_widget_host);
    return wid;
}

void WindowImpl::DidStartProvisionalLoadForFrame(
        RenderViewHost* render_view_host,
        bool is_main_frame,
        const GURL& url) {
    if (!is_main_frame) {
        return;
    }
    if (render_view_host != static_cast<RenderViewHost*>(host())) {
        return;
    }
    if (mDelegate) {
        this->mCurrentURL = url;
        mDelegate->onStartLoading(this, url.spec());
        mDelegate->onAddressBarChanged(this, url.spec());
    }
}

void WindowImpl::DidStartReceivingResourceResponse(
        ResourceRequestDetails* details) {
    // See "chrome/browser/renderer_host/resource_request_details.h"
    // for list of accessor functions.
}

void WindowImpl::DidRedirectProvisionalLoad(
    int32 page_id,
    const GURL& source_url,
    const GURL& target_url)
{
    // should use page_id to lookup in history
    NavigationEntry *entry = mNavEntry;
    if (entry->url() == source_url) {
        entry->set_url(target_url);
    }
}

void WindowImpl::DidRedirectResource(ResourceRequestDetails* details) {
    // Only accessor function:
    // details->new_url();
}

void WindowImpl::DidLoadResourceFromMemoryCache(
        const GURL& url,
        const std::string& frame_origin,
        const std::string& main_frame_origin,
        const std::string& security_info) {
}

void WindowImpl::DidFailProvisionalLoadWithError(
        RenderViewHost* render_view_host,
        bool is_main_frame,
        int error_code,
        const GURL& url,
        bool showing_repost_interstitial) {
}

void WindowImpl::DocumentLoadedInFrame() {
    if (mDelegate) {
        mDelegate->onLoad(this);
    }
}

/******* RenderViewHostDelegate::View *******/
void WindowImpl::CreateNewWindow(int route_id,
                                 base::WaitableEvent* modal_dialog_event) {
    std::cout<<"Created window "<<route_id<<std::endl;
    mNewlyCreatedWindows.insert(
        std::pair<int, WindowImpl*>(route_id, new WindowImpl(getContext(), route_id)));
}
void WindowImpl::CreateNewWidget(int route_id, bool activatable) {
    std::cout<<"Created widget "<<route_id<<std::endl;
    RenderWidget* wid = new RenderWidget(this);
    wid->setHost(new MemoryRenderWidgetHost(this, wid, process(), route_id));
    wid->set_activatable(activatable);
    mNewlyCreatedWidgets.insert(
        std::pair<int, RenderWidget*>(route_id, wid));
}
void WindowImpl::ShowCreatedWindow(int route_id,
                                   WindowOpenDisposition disposition,
                                   const gfx::Rect& initial_pos,
                                   bool user_gesture,
                                   const GURL& creator_url) {
    std::cout<<"Show Created window "<<route_id<<std::endl;
    std::map<int, WindowImpl*>::iterator iter = mNewlyCreatedWindows.find(route_id);
    assert(iter != mNewlyCreatedWindows.end());
    WindowImpl *newwin = iter->second;
    mNewlyCreatedWindows.erase(iter);
    newwin->host()->Init();

    newwin->resize(initial_pos.width(), initial_pos.height());

    if (!newwin->process()->HasConnection()) {
        // The view has gone away or the renderer crashed. Nothing to do.
        // Fixme: memory leak?
        std::cout<<"Show window process fail "<<route_id<<std::endl;
        return;
    }

    if (mDelegate) {
        mDelegate->onCreatedWindow(this, newwin);
    }
}
void WindowImpl::ShowCreatedWidget(int route_id,
                                   const gfx::Rect& initial_pos) {
    std::cout<<"Show Created widget "<<route_id<<std::endl;
    std::map<int, RenderWidget*>::iterator iter = mNewlyCreatedWidgets.find(route_id);
    assert(iter != mNewlyCreatedWidgets.end());
    RenderWidget *wid = iter->second;
    appendWidget(wid);
    mNewlyCreatedWidgets.erase(iter);

    if (!wid->GetRenderWidgetHost()->process()->HasConnection()) {
        // The view has gone away or the renderer crashed. Nothing to do.
        // Fixme: memory leak?
        std::cout<<"Show widget process fail "<<route_id<<std::endl;
        return;
    }

    int thisZ = ++zIndex;

    wid->InitAsPopup(view(), initial_pos);
    wid->GetRenderWidgetHost()->Init();

    wid->SetSize(gfx::Size(initial_pos.width(), initial_pos.height()));
    wid->setPos(initial_pos.x(), initial_pos.y());
    if (mDelegate) {
        mDelegate->onWidgetCreated(this, wid, thisZ);
        mDelegate->onWidgetResize(this, wid,
                                  initial_pos.width(), initial_pos.height());
        mDelegate->onWidgetMove(this, wid,
                                initial_pos.x(), initial_pos.y());
    }

}
void WindowImpl::ShowContextMenu(const ContextMenuParams& params) {
    // TODO: Add context menu event
}
void WindowImpl::StartDragging(const WebDropData& drop_data,
                               WebKit::WebDragOperationsMask allowed_ops) {
    // TODO: Add dragging event
}
void WindowImpl::UpdateDragCursor(WebKit::WebDragOperation operation) {
    // TODO: Add dragging event
}
void WindowImpl::GotFocus() {
    // Useless: just calls this when we hand it an input event.
}
void WindowImpl::TakeFocus(bool reverse) {
}
void WindowImpl::HandleKeyboardEvent(const NativeWebKeyboardEvent& event) {
    // Useless: just calls this when we hand it an input event.
}
void WindowImpl::HandleMouseEvent() {
    // Useless: just calls this when we hand it an input event.
}
void WindowImpl::HandleMouseLeave() {
    // Useless: just calls this when we hand it an input event.
}
void WindowImpl::UpdatePreferredWidth(int pref_width) {
}

}
