/*  Berkelium Implementation
 *  Root.hpp
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
#include "berkelium/Singleton.hpp"
#include "chrome/browser/profile.h"
#include "chrome/common/notification_service.h"
#include "base/scoped_nsautorelease_pool.h"

class BrowserRenderProcessHost;
class ProcessSingleton;
class ChromeThread;

namespace Berkelium {

class MemoryRenderViewHostFactory;

//singleton class that contains chromium singletons. Not visible outside of Berkelium library core
class Root:public AutoSingleton<Root> {
    Profile*mProf;
    MessageLoop* mMessageLoop;
    NotificationService*mNotificationService;
    ProcessSingleton *mProcessSingleton;
    ChromeThread *mUIThread;
    MemoryRenderViewHostFactory *mRenderViewHostFactory;
    base::ScopedNSAutoreleasePool autorelease_pool;
public:
    Root();
    ~Root();

//    void runUntilStopped();
//    void stopRunning();
    void update();

    ProcessSingleton *getProcessSingleton(){
        return mProcessSingleton;
    }
    inline Profile* getProfile() {
        return mProf;
    }
};

}
