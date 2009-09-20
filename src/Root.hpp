#include "berkelium/Platform.hpp"
#include "berkelium/Singleton.hpp"
#include "chrome/browser/profile.h"
#include "chrome/common/notification_service.h"

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
public:
    Root();
    ~Root();

    void runUIMessageLoop();

    ProcessSingleton *getProcessSingleton(){
        return mProcessSingleton;
    }
    inline Profile* getProfile() {
        return mProf;
    }
};

}
