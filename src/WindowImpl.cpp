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
#include "base/values.h"
#include "net/base/net_util.h"
#include "chrome/browser/renderer_host/render_view_host.h"
#include "chrome/browser/renderer_host/site_instance.h"
#include "chrome/common/render_messages.h"
#include "chrome/browser/tab_contents/tab_contents.h"
#include "chrome/browser/dom_ui/dom_ui.h"
#include "chrome/browser/dom_ui/dom_ui_factory.h"
#include "chrome/browser/renderer_host/render_widget_host_view.h"
#include "chrome/browser/renderer_host/render_view_host_factory.h"
#include "chrome/browser/browser_url_handler.h"
#include "chrome/common/native_web_keyboard_event.h"
#include "chrome/browser/renderer_host/render_process_host.h"
#include "chrome/common/bindings_policy.h"
namespace Berkelium {
//WindowImpl temp;
void WindowImpl::init(SiteInstance*site, int routing_id) {
    mRenderViewHost = RenderViewHostFactory::Create(
        site,
        this,
        routing_id);
    host()->AllowBindings(
        BindingsPolicy::DOM_UI);
    CreateRenderViewForRenderManager(host());
}

WindowImpl::WindowImpl(const Context*otherContext):
        Window(otherContext),
        mController(this, profile())
{
    mMouseX = 0;
    mMouseY = 0;
    mCurrentURL = GURL("about:blank");
    zIndex = 0;
    init(mContext->getImpl()->getSiteInstance(), MSG_ROUTING_NONE);
}
WindowImpl::WindowImpl(const Context*otherContext, int routing_id):
        Window(otherContext),
        mController(this, profile())
{
    mMouseX = 0;
    mMouseY = 0;
    mCurrentURL = GURL("about:blank");
    zIndex = 0;
    init(mContext->getImpl()->getSiteInstance(), routing_id);
}
WindowImpl::~WindowImpl() {
    RenderViewHost* render_view_host = mRenderViewHost;
    mRenderViewHost = NULL;
    render_view_host->Shutdown();
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

SiteInstance *WindowImpl::GetSiteInstance() {
    return mContext->getImpl()->getSiteInstance();
}

void WindowImpl::SetIsCrashed(bool state) {
  if (state == is_crashed_)
    return;

  is_crashed_ = state;
}

void WindowImpl::SetIsLoading(bool is_loading) {
    host()->SetIsLoading(is_loading);
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
        wid->mouseWheel(xScroll, yScroll);
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
    mController.LoadEntry(CreateNavigationEntry(
        newURL,
        referrerURL,
        PageTransition::TYPED));
    NavigateToPendingEntry(reload);

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

bool WindowImpl::NavigateToPendingEntry(bool reload) {
    const NavigationEntry& entry = *mController.pending_entry();

    if (!host()) {
        return false;  // Unable to create the desired render view host.
    }

    // Navigate in the desired RenderViewHost.
    ViewMsg_Navigate_Params navigate_params;
    MakeNavigateParams(entry, reload, &navigate_params);
    host()->Navigate(navigate_params);
    if (view()) {
        view()->Show();
    }

    return true;
}

void WindowImpl::UpdateMaxPageID(int32 page_id) {
  // Ensure both the SiteInstance and RenderProcessHost update their max page
  // IDs in sync. Only TabContents will also have site instances, except during
  // testing.
  if (GetSiteInstance())
    GetSiteInstance()->UpdateMaxPageID(page_id);
  process()->UpdateMaxPageID(page_id);
}

int32 WindowImpl::GetMaxPageID() {
  if (GetSiteInstance())
    return GetSiteInstance()->max_page_id();
  return 0;
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
        if (mDelegate) {
            mDelegate->onWidgetDestroyed(this, wid);
        }
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

    SetIsLoading(true);

    if (mDelegate) {
//        mDelegate->onStartLoading(this);
    }
}
void WindowImpl::DidStopLoading(
    RenderViewHost* render_view_host) {

    SetIsLoading(false);

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

void WindowImpl::UpdateHistoryForNavigation(
    const GURL& virtual_url,
    const NavigationController::LoadCommittedDetails& details,
    const ViewHostMsg_FrameNavigate_Params& params) {
  if (profile()->IsOffTheRecord())
    return;

  // Add to history service.
  HistoryService* hs = profile()->GetHistoryService(Profile::IMPLICIT_ACCESS);
  if (hs) {
    if (PageTransition::IsMainFrame(params.transition) &&
        virtual_url != params.url) {
      // Hack on the "virtual" URL so that it will appear in history. For some
      // types of URLs, we will display a magic URL that is different from where
      // the page is actually navigated. We want the user to see in history
      // what they saw in the URL bar, so we add the virtual URL as a redirect.
      // This only applies to the main frame, as the virtual URL doesn't apply
      // to sub-frames.
      std::vector<GURL> redirects = params.redirects;
      if (!redirects.empty())
        redirects.back() = virtual_url;
      hs->AddPage(virtual_url, this, params.page_id, params.referrer,
                  params.transition, redirects, details.did_replace_entry);
    } else {
      hs->AddPage(params.url, this, params.page_id, params.referrer,
                  params.transition, params.redirects,
                  details.did_replace_entry);
    }
  }
}

bool WindowImpl::UpdateTitleForEntry(NavigationEntry* entry,
                                      const std::wstring& title) {
  // For file URLs without a title, use the pathname instead. In the case of a
  // synthesized title, we don't want the update to count toward the "one set
  // per page of the title to history."
  std::wstring final_title;
  bool explicit_set;
  if (entry->url().SchemeIsFile() && title.empty()) {
    final_title = UTF8ToWide(entry->url().ExtractFileName());
    explicit_set = false;  // Don't count synthetic titles toward the set limit.
  } else {
    TrimWhitespace(title, TRIM_ALL, &final_title);
    explicit_set = true;
  }

  if (final_title == UTF16ToWideHack(entry->title()))
    return false;  // Nothing changed, don't bother.

  entry->set_title(WideToUTF16Hack(final_title));

  // Update the history system for this page.
  if (!profile()->IsOffTheRecord() && !received_page_title_) {
    HistoryService* hs =
        profile()->GetHistoryService(Profile::IMPLICIT_ACCESS);
    if (hs)
      hs->SetPageTitle(entry->virtual_url(), final_title);

    // Don't allow the title to be saved again for explicitly set ones.
    received_page_title_ = explicit_set;
  }

  // Lastly, set the title for the view.
  //view()->SetPageTitle(final_title);

  return true;
}



void WindowImpl::RendererUnresponsive(RenderViewHost* rvh,
                                  bool is_during_unload) {
  if (rvh != host()) {
    return;
  }
  if (mDelegate) mDelegate->onUnresponsive(this);
}
void WindowImpl::RendererResponsive(RenderViewHost* rvh) {
  if (rvh != host()) {
    return;
  }
  if (mDelegate) mDelegate->onResponsive(this);
}

void WindowImpl::RenderViewReady(RenderViewHost* rvh) {
  if (rvh != host()) {
    // Don't notify the world, since this came from a renderer in the
    // background.
    return;
  }

  bool was_crashed = is_crashed();
  SetIsCrashed(false);

  // Restore the focus to the tab (otherwise the focus will be on the top
  // window).
  if (was_crashed)
      view()->Focus();
}

void WindowImpl::RenderViewGone(RenderViewHost* rvh) {
  // Ask the print preview if this renderer was valuable.
  if (rvh != host()) {
    // The pending page's RenderViewHost is gone.
    return;
  }

  SetIsLoading(false);
  SetIsCrashed(true);

  // Tell the view that we've crashed so it can prepare the sad tab page.
  //view()->OnTabCrashed();
  if (mDelegate) mDelegate->onCrashed(this);
}

void WindowImpl::ProcessDOMUIMessage(
    const std::string& message, const Value* content,
    int request_id, bool has_callback)
{
    std::vector<std::string> argsVector;

    /* Callbacks don't work for DOMUI, so we just process message and content.
       (request_id and has_callback are hardcoded to false.)
       In addition, content is currently hardcoded as an array of strings.
    */
    if (content->GetType() == Value::TYPE_LIST) {
        const ListValue* argsListValue = static_cast<const ListValue*>(content);
        for (size_t i = 0; i < argsListValue->GetSize(); ++i) {
            argsVector.push_back(std::string());
            argsListValue->GetString(i, &argsVector.back());
        }
    }

    if (mDelegate) {
        mDelegate->onChromeSend(this, message, argsVector);
    }
}


void WindowImpl::DidNavigate(RenderViewHost* rvh,
                              const ViewHostMsg_FrameNavigate_Params& params) {
  int extra_invalidate_flags = 0;

  // Update the site of the SiteInstance if it doesn't have one yet.
  if (!GetSiteInstance()->has_site())
    GetSiteInstance()->SetSite(params.url);

  // Need to update MIME type here because it's referred to in
  // UpdateNavigationCommands() called by RendererDidNavigate() to
  // determine whether or not to enable the encoding menu.
  // It's updated only for the main frame. For a subframe,
  // RenderView::UpdateURL does not set params.contents_mime_type.
  // (see http://code.google.com/p/chromium/issues/detail?id=2929 )
  // TODO(jungshik): Add a test for the encoding menu to avoid
  // regressing it again.

  /*if (PageTransition::IsMainFrame(params.transition))
        contents_mime_type_ = params.contents_mime_type;
  */

  NavigationController::LoadCommittedDetails details;
  bool did_navigate = mController.RendererDidNavigate(
      params, extra_invalidate_flags, &details);

  // Update history. Note that this needs to happen after the entry is complete,
  // which WillNavigate[Main,Sub]Frame will do before this function is called.
  if (params.should_update_history) {
    // Most of the time, the displayURL matches the loaded URL, but for about:
    // URLs, we use a data: URL as the real value.  We actually want to save
    // the about: URL to the history db and keep the data: URL hidden. This is
    // what the TabContents' URL getter does.
    UpdateHistoryForNavigation(GetURL(), details, params);
  }

  if (!did_navigate)
    return;  // No navigation happened.

  // DO NOT ADD MORE STUFF TO THIS FUNCTION! Your component should either listen
  // for the appropriate notification (best) or you can add it to
  // DidNavigateMainFramePostCommit / DidNavigateAnyFramePostCommit (only if
  // necessary, please).

  // Run post-commit tasks.
  if (details.is_main_frame) {
// FIXME(pathorn): Look into what TabContents uses these functions for.
      //DidNavigateMainFramePostCommit(details, params);
  }
  //DidNavigateAnyFramePostCommit(details, params);
}

void WindowImpl::UpdateState(RenderViewHost* rvh,
                              int32 page_id,
                              const std::string& state) {
  DCHECK(rvh == host());

  // We must be prepared to handle state updates for any page, these occur
  // when the user is scrolling and entering form data, as well as when we're
  // leaving a page, in which case our state may have already been moved to
  // the next page. The navigation controller will look up the appropriate
  // NavigationEntry and update it when it is notified via the delegate.

  int entry_index = mController.GetEntryIndexWithPageID(
      GetSiteInstance(), page_id);
  if (entry_index < 0)
    return;
  NavigationEntry* entry = mController.GetEntryAtIndex(entry_index);

  if (state == entry->content_state())
    return;  // Nothing to update.
  entry->set_content_state(state);
  mController.NotifyEntryChanged(entry, entry_index);
}

void WindowImpl::UpdateTitle(RenderViewHost* rvh,
                              int32 page_id, const std::wstring& title) {
  // If we have a title, that's a pretty good indication that we've started
  // getting useful data.
  //SetNotWaitingForResponse();

  DCHECK(rvh == host());
  NavigationEntry* entry = mController.GetEntryWithPageID(GetSiteInstance(),
                                                            page_id);
  if (!entry || !UpdateTitleForEntry(entry, title))
    return;
}

void WindowImpl::ShowRepostFormWarningDialog() {
}

void WindowImpl::RunFileChooser(bool multiple_files,
                                 const string16& title,
                                 const FilePath& default_file) {
/* Don't have access to a top level window, since this could be run in a
   windowless environment. Perhaps we need a function to return a native
   window handle or NULL if it does not want to allow file choosers.
*/
/*
  if (!mSelectFileDialog.get())
    mSelectFileDialog = SelectFileDialog::Create(this);
  SelectFileDialog::Type dialog_type =
    multiple_files ? SelectFileDialog::SELECT_OPEN_MULTI_FILE :
                     SelectFileDialog::SELECT_OPEN_FILE;
  mSelectFileDialog->SelectFile(dialog_type, title, default_file,
                                  NULL, 0, FILE_PATH_LITERAL(""),
                                  view()->GetTopLevelNativeWindow(), NULL);
*/
}

void WindowImpl::RequestOpenURL(const GURL& url, const GURL& referrer,
                                 WindowOpenDisposition disposition) {
    /* if (disposition == NEW_WINDOW) {
     */
// if (disposition == CURRENT_TAB)
    doNavigateTo(url, referrer, false);
}

void WindowImpl::DomOperationResponse(const std::string& json_string,
                                       int automation_id) {
}


void WindowImpl::RunJavaScriptMessage(
    const std::wstring& message,
    const std::wstring& default_prompt,
    const GURL& frame_url,
    const int flags,
    IPC::Message* reply_msg,
    bool* did_suppress_message)
{
    host()->JavaScriptMessageBoxClosed(reply_msg, false, std::wstring());
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
    NavigationEntry *entry;
    if (page_id == -1)
        entry = mController.pending_entry();
    else
        entry = mController.GetEntryWithPageID(GetSiteInstance(), page_id);
    if (!entry || entry->url() != source_url)
        return;
    entry->set_url(target_url);
}

void WindowImpl::DidRedirectResource(ResourceRequestDetails* details) {
    // Only accessor function:
    // details->new_url();
}

void WindowImpl::Close(RenderViewHost* rvh) {
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

void WindowImpl::UpdatePreferredSize(const gfx::Size&) {
}

}
