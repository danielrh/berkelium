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
#ifndef _WIN32
#include <sys/time.h>
#include <time.h>
#endif


namespace Berkelium {
//WindowImpl temp;
void WindowImpl::init(SiteInstance*site) {
    mRenderViewHost = RenderViewHostFactory::Create(
        site,
        this,
        MSG_ROUTING_NONE,
        NULL);
    CreateRenderViewForRenderManager(host());
}

WindowImpl::WindowImpl(const Context*otherContext):Window(otherContext) {
    mCurrentURL = GURL("about:blank");
    mLastNavEntry = NULL;
    mNavEntry = NULL;
    mWindowX=mWindowY=0;
    mModifiers=0;
    init(mContext->getImpl()->getSiteInstance());
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

void WindowImpl::resize(int width, int height) {
    SetContainerBounds(gfx::Rect(0, 0, width, height));
}

void WindowImpl::focus() {
    if (host()) host()->Focus();
}
void WindowImpl::unfocus() {
    if (host()) host()->Blur();
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
        myhost->WasResized();
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

void WindowImpl::onPaint(const unsigned char *sourceBuffer, const Rect &rect,
                         int dx, int dy, const Rect &scrollRect) {
    if (mDelegate) {
        mDelegate->onPaint(this, sourceBuffer, rect, dx, dy, scrollRect);
    }
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
    return new RenderWidget(this, render_widget_host);
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
    // TODO: Support multiple windows.
}
void WindowImpl::CreateNewWidget(int route_id, bool activatable) {
    // TODO: Support multiple windows.
}
void WindowImpl::ShowCreatedWindow(int route_id,
                                   WindowOpenDisposition disposition,
                                   const gfx::Rect& initial_pos,
                                   bool user_gesture,
                                   const GURL& creator_url) {
    WindowImpl *win = new WindowImpl(getContext());
    win->resize(initial_pos.width(), initial_pos.height());
    if (mDelegate) {
        mDelegate->onCreatedWindow(this, win);
    }
}
void WindowImpl::ShowCreatedWidget(int route_id,
                                   const gfx::Rect& initial_pos) {
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
template<class T>
void zeroWebEvent(T &event, int modifiers, WebKit::WebInputEvent::Type t) {
    memset(&event,0,sizeof(T));
    event.type=t;
    event.size=sizeof(T);
    event.modifiers=modifiers;
#ifdef _WIN32
    event.timeSTampSeconds=GetTickCount()/1000.0;
#else
    timeval tv;
    gettimeofday(&tv,NULL);
    event.timeStampSeconds=((double)tv.tv_usec)/1000000.0;
    event.timeStampSeconds+=tv.tv_sec;
#endif
}

void WindowImpl::mouseMoved(int xPos, int yPos) {
    if (!view()) return;
    WebKit::WebMouseEvent event;
    zeroWebEvent(event,mModifiers,WebKit::WebInputEvent::MouseMove);
	event.x = xPos;
	event.y = yPos;
	event.globalX = xPos+mWindowX;
	event.globalY = yPos+mWindowY;
    mMouseX=xPos;
    mMouseY=yPos;
	event.button = WebKit::WebMouseEvent::ButtonNone;
    view()->GetRenderWidgetHost()->ForwardMouseEvent(event);
}

void WindowImpl::mouseWheel(int scrollX, int scrollY) {
    if (!view()) return;
	WebKit::WebMouseWheelEvent event;
	zeroWebEvent(event, mModifiers, WebKit::WebInputEvent::MouseWheel);
	event.x = mMouseX;
	event.y = mMouseY;
	event.windowX = mMouseX; // PRHFIXME: Window vs Global position?
	event.windowY = mMouseY;
	event.globalX = mWindowX+mMouseX;
	event.globalY = mWindowY+mMouseY;
	event.button = WebKit::WebMouseEvent::ButtonNone;
	event.deltaX = scrollX; // PRHFIXME: want x and y scroll.
	event.deltaY = scrollY;
	event.wheelTicksX = scrollX; // PRHFIXME: want x and y scroll.
	event.wheelTicksY = scrollY;
	event.scrollByPage = false;

    view()->GetRenderWidgetHost()->ForwardMouseEvent(event);
}

void WindowImpl::mouseButton(unsigned int mouseButton, bool down) {
    if (!view()) return;
    unsigned int buttonChangeMask=0;
    switch(mouseButton) {
      case 0:
        buttonChangeMask = WebKit::WebInputEvent::LeftButtonDown;
        break;
      case 1:
        buttonChangeMask = WebKit::WebInputEvent::MiddleButtonDown;
        break;
      case 2:
        buttonChangeMask = WebKit::WebInputEvent::RightButtonDown;
        break;
    }
    if (down) {
        mModifiers|=buttonChangeMask;
    }else {
        mModifiers&=(~buttonChangeMask);
    }
    WebKit::WebMouseEvent event;
    zeroWebEvent(event,mModifiers,down?WebKit::WebInputEvent::MouseDown:WebKit::WebInputEvent::MouseUp);
    switch(mouseButton) {
      case 0:
        event.button = WebKit::WebMouseEvent::ButtonLeft;
        break;
      case 1:
        event.button = WebKit::WebMouseEvent::ButtonMiddle;
        break;
      case 2:
        event.button = WebKit::WebMouseEvent::ButtonRight;
        break;
    }
    if (down){
        event.clickCount=1;
    }
	event.x = mMouseX;
	event.y = mMouseY;
	event.globalX = mMouseX+mWindowX;
	event.globalY = mMouseY+mWindowY;
    view()->GetRenderWidgetHost()->ForwardMouseEvent(event);
}

void WindowImpl::keyEvent(bool pressed, int modifiers, int vk_code, int scancode){
	if (!view()) return;
	NativeWebKeyboardEvent event;
	zeroWebEvent(event, mModifiers, pressed?WebKit::WebInputEvent::RawKeyDown:WebKit::WebInputEvent::KeyUp);
	event.windowsKeyCode = vk_code;
	event.nativeKeyCode = scancode;
	event.text[0]=0;
	event.unmodifiedText[0]=0;
	event.isSystemKey = (modifiers & Berkelium::SYSTEM_KEY)?true:false;

	event.modifiers=0;
	if (modifiers & Berkelium::ALT_MOD)
		event.modifiers |= WebKit::WebInputEvent::AltKey;
	if (modifiers & Berkelium::CONTROL_MOD)
		event.modifiers |= WebKit::WebInputEvent::ControlKey;
	if (modifiers & Berkelium::SHIFT_MOD)
		event.modifiers |= WebKit::WebInputEvent::ShiftKey;
	if (modifiers & Berkelium::META_MOD)
		event.modifiers |= WebKit::WebInputEvent::MetaKey;
	if (modifiers & Berkelium::KEYPAD_KEY)
		event.modifiers |= WebKit::WebInputEvent::IsKeyPad;
	if (modifiers & Berkelium::AUTOREPEAT_KEY)
		event.modifiers |= WebKit::WebInputEvent::IsAutoRepeat;

	event.setKeyIdentifierFromWindowsKeyCode();

	view()->GetRenderWidgetHost()->ForwardKeyboardEvent(event);

	// keep track of persistent modifiers.
    unsigned int test=(WebKit::WebInputEvent::LeftButtonDown|WebKit::WebInputEvent::MiddleButtonDown|WebKit::WebInputEvent::RightButtonDown);
	mModifiers = ((mModifiers&test) |  (event.modifiers& (Berkelium::SHIFT_MOD|Berkelium::CONTROL_MOD|Berkelium::ALT_MOD|Berkelium::META_MOD)));    
}


void WindowImpl::textEvent(std::wstring text) {
	if (!view()) return;
	// generate one of these events for each lengthCap chunks.
	// 1 less because we need to null terminate.
	const size_t lengthCap = WebKit::WebKeyboardEvent::textLengthCap-1;
	NativeWebKeyboardEvent event;
	zeroWebEvent(event,mModifiers, WebKit::WebInputEvent::Char);
	event.isSystemKey = false;
	event.windowsKeyCode = 0;
	event.nativeKeyCode = 0;
	event.keyIdentifier[0]=0;
	size_t i;
	while (text.size() > lengthCap) {

	}
	for (i = 0; i + lengthCap < text.size(); i+=lengthCap) {
		memcpy(event.text, text.data()+i, lengthCap*sizeof(WebKit::WebUChar));
		event.text[lengthCap]=0;
		memcpy(event.unmodifiedText, text.data()+i, lengthCap*sizeof(WebKit::WebUChar));
		event.unmodifiedText[lengthCap]=0;
        view()->GetRenderWidgetHost()->ForwardKeyboardEvent(event);
	}
	if (i < text.size()) {
		assert(text.size()-i <= lengthCap);
		memcpy(event.unmodifiedText, text.data()+i, (text.size()-i)*sizeof(WebKit::WebUChar));
		memcpy(event.text, text.data()+i, (text.size()-i)*sizeof(WebKit::WebUChar));
		event.text[text.size()-i]=0;
		event.unmodifiedText[text.size()-i]=0;
        view()->GetRenderWidgetHost()->ForwardKeyboardEvent(event);
	}
}

}
