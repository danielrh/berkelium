/*  Berkelium Implementation
 *  MemoryRenderViewHost.hpp
 *
 *  Copyright (c) 2009, Patrick Reiter Horn
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
    void Memory_OnMsgScrollRect(const ViewHostMsg_ScrollRect_Params&params);
    void Memory_OnMsgPaintRect(const ViewHostMsg_PaintRect_Params&params);
    void Memory_ScrollBackingStoreRect(TransportDIB* bitmap,
                                      const gfx::Rect& bitmap_rect,
                                      int dx, int dy,
                                      const gfx::Rect& clip_rect);
    void Memory_PaintBackingStoreRect(TransportDIB* bitmap,
                                      const gfx::Rect& bitmap_rect);

    WindowImpl *mWindow;
    gfx::Size current_size_;

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
