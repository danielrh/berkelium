#ifndef _BERKELIUM_WINDOW_DELEGATE_HPP_
#define _BERKELIUM_WINDOW_DELEGATE_HPP_

namespace Berkelium {

class WindowImpl;
class Window;

class RenderWidget; // FIXME: private interface.

class BERKELIUM_EXPORT WindowDelegate {
public:
    virtual ~WindowDelegate() {}

    // DidStartProvisionalLoadForFrame
    // DidRedirectProvisionalLoad
    virtual void onAddressBarChanged(Window *win, const std::string &newURL)=0;

    // DidStartProvisionalLoadForFrame
    virtual void onStartLoading(Window *win, const std::string &newURL)=0;
    // DocumentLoadedInFrame
    virtual void onLoad(Window *win)=0;
    virtual void onLoadError(Window *win, const std::string &error)=0;


    virtual void onPaint(Window *win)=0;

    virtual void onBeforeUnload(Window *win, bool *proceed)=0;
    virtual void onCancelUnload(Window *win)=0;
    virtual void onCrashed(Window *win)=0;

    virtual void onCreatedWindow(Window *win, Window *newWindow)=0;
//    virtual void onCreatedWidget(Window *win, RenderWidget *newWidget)=0;

/**************************
   Might want messages for:
- ShowContextMenu(const ContextMenuParams& params);
- StartDragging(const WebDropData& drop_data,
                WebKit::WebDragOperationsMask allowed_ops);
- UpdateDragCursor(WebKit::WebDragOperation operation);
- TakeFocus(bool reverse)

- Renderer[Un]Responsive

- RequestOpenURL
- DocumentAvailableInMainFrame
- DidStartLoading
- DidStopLoading
- Close
- UpdateTargetURL
- UpdateTitle
- UpdateEncoding
- UpdateState????
- DidNavigate

   Also, may want API to bind javascript functions.
**************************/

};

}

#endif
