#include "berkelium/Platform.hpp"
#include "berkelium/Singleton.hpp"
#include "chrome/browser/profile.h"
#include "chrome/common/notification_service.h"
namespace Berkelium {
//singleton class that contains chromium singletons. Not visible outside of Berkelium library core
class Root:public AutoSingleton<Root> {
    Profile*mProf;
    MessageLoop* mMessageLoop;
    NotificationService*mNotificationService;
public:
    Root();
    ~Root();
};

}
