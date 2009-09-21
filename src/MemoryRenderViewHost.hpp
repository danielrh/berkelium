#ifndef _BERKELIUM_RENDERWIDGET_HPP_
#define _BERKELIUM_RENDERWIDGET_HPP_

#include "chrome/browser/renderer_host/render_view_host.h"
#include "chrome/browser/renderer_host/render_view_host_factory.h"

namespace Berkelium {
class WindowImpl;


class MemoryRenderViewHost : public RenderViewHost {

public:
    MemoryRenderViewHost(
        SiteInstance* instance,
        RenderViewHostDelegate* delegate,
        int routing_id,
        base::WaitableEvent* modal_dialog_event);
    ~MemoryRenderViewHost();

    virtual void OnMessageReceived(const IPC::Message& msg);

private:
    void Memory_OnMsgPaintRect(const ViewHostMsg_PaintRect_Params&params);
    void Memory_PaintBackingStoreRect(TransportDIB* bitmap,
                                      const gfx::Rect& bitmap_rect,
                                      const gfx::Size& view_size);

};

class MemoryRenderViewHostFactory : public RenderViewHostFactory {
public:

    ~MemoryRenderViewHostFactory();
    MemoryRenderViewHostFactory();

    virtual RenderViewHost* CreateRenderViewHost(
        SiteInstance* instance,
        RenderViewHostDelegate* delegate,
        int routing_id,
        base::WaitableEvent* modal_dialog_event);
};

}

#endif
