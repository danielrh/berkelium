/*  Berkelium Implementation
 *  MemoryRenderViewHost.cpp
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

#include "berkelium/Platform.hpp"
#include "WindowImpl.hpp"
#include "MemoryRenderViewHost.hpp"
#include <stdio.h>

#include "chrome/browser/renderer_host/render_process_host.h"
#include "chrome/common/notification_service.h"
#include "chrome/common/render_messages.h"

namespace Berkelium {

MemoryRenderViewHost::MemoryRenderViewHost(
        SiteInstance* instance,
        RenderViewHostDelegate* delegate,
        int routing_id,
        base::WaitableEvent* modal_dialog_event)
    : RenderViewHost(instance, delegate, routing_id, modal_dialog_event) {

    mWindow = static_cast<WindowImpl*>(delegate);
}

MemoryRenderViewHost::~MemoryRenderViewHost() {
}

void MemoryRenderViewHost::OnMessageReceived(const IPC::Message& msg) {
  bool msg_is_ok = true;
  IPC_BEGIN_MESSAGE_MAP_EX(MemoryRenderViewHost, msg, msg_is_ok)
    IPC_MESSAGE_HANDLER(ViewHostMsg_PaintRect, Memory_OnMsgPaintRect)
    IPC_MESSAGE_UNHANDLED(RenderViewHost::OnMessageReceived(msg))
  IPC_END_MESSAGE_MAP_EX()

      ;
  if (msg.type() == ViewHostMsg_PaintRect::ID) {
      RenderViewHost::OnMessageReceived(msg);
  }
          
  if (!msg_is_ok) {
    // The message had a handler, but its de-serialization failed.
    // Kill the renderer.
    process()->ReceivedBadMessage(msg.type());
  }
}

void MemoryRenderViewHost::Memory_OnMsgPaintRect(
    const ViewHostMsg_PaintRect_Params&params)
{
/*
    // Update our knowledge of the RenderWidget's size.
    current_size_ = params.view_size;

    bool is_resize_ack =
        ViewHostMsg_PaintRect_Flags::is_resize_ack(params.flags);

    // resize_ack_pending_ needs to be cleared before we call DidPaintRect, since
    // that will end up reaching GetBackingStore.
    if (is_resize_ack) {
        DCHECK(resize_ack_pending_);
        resize_ack_pending_ = false;
        in_flight_size_.SetSize(0, 0);
    }

    bool is_repaint_ack =
        ViewHostMsg_PaintRect_Flags::is_repaint_ack(params.flags);
    if (is_repaint_ack) {
        repaint_ack_pending_ = false;
        TimeDelta delta = TimeTicks::Now() - repaint_start_time_;
        UMA_HISTOGRAM_TIMES("MPArch.RWH_RepaintDelta", delta);
    }
*/
    DCHECK(!params.bitmap_rect.IsEmpty());
    DCHECK(!params.view_size.IsEmpty());

    const size_t size = params.bitmap_rect.height() *
        params.bitmap_rect.width() * 4;
    TransportDIB* dib = process()->GetTransportDIB(params.bitmap);
    if (dib) {
        if (dib->size() < size) {
            DLOG(WARNING) << "Transport DIB too small for given rectangle";
            process()->ReceivedBadMessage(ViewHostMsg_PaintRect__ID);
        } else {
            // Paint the backing store. This will update it with the renderer-supplied
            // bits. The view will read out of the backing store later to actually
            // draw to the screen.
            Memory_PaintBackingStoreRect(dib, params.bitmap_rect, params.view_size);
        }
    }

/*
    Send(new ViewMsg_PaintRect_ACK(routing_id_));
*/
}

void MemoryRenderViewHost::Memory_PaintBackingStoreRect(
    TransportDIB* bitmap,
    const gfx::Rect& bitmap_rect,
    const gfx::Size& view_size)
{
    Rect updateRect;
    updateRect.mTop = bitmap_rect.y();
    updateRect.mLeft = bitmap_rect.x();
    updateRect.mWidth = bitmap_rect.width();
    updateRect.mHeight = bitmap_rect.height();

    //const uint32_t* bitmap_in =
    //    static_cast<const uint32_t*>(bitmap->memory());

    mWindow->onPaint(static_cast<const unsigned char *>(bitmap->memory()), updateRect);
}


MemoryRenderViewHostFactory::MemoryRenderViewHostFactory() {
    RenderViewHostFactory::RegisterFactory(this);
}

MemoryRenderViewHostFactory::~MemoryRenderViewHostFactory() {
    RenderViewHostFactory::UnregisterFactory();
}

RenderViewHost* MemoryRenderViewHostFactory::CreateRenderViewHost(
    SiteInstance* instance,
    RenderViewHostDelegate* delegate,
    int routing_id,
    base::WaitableEvent* modal_dialog_event)
{
    return new MemoryRenderViewHost(instance, delegate,
                                    routing_id, modal_dialog_event);
}

}

