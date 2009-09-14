// Berkelium headers
#include "berkelium/Platform.hpp"
#include "Root.hpp"
// Chromium headers
#include "base/at_exit.h"
#include "base/path_service.h"
#include "base/thread.h"
#include "base/command_line.h"
#include "base/icu_util.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/browser/browser_process_impl.h"
#include "app/resource_bundle.h"
#include "app/app_paths.h"
//icu_util::Initialize()

AUTO_SINGLETON_INSTANCE(Berkelium::Root);
namespace Berkelium {

Root::Root (){
    new base::AtExitManager();
/*
    base::Thread *coreThread = new base::Thread("CoreThread");
	coreThread->StartWithOptions(base::Thread::Options(MessageLoop::TYPE_UI, 0));
*/
    {
        const char* argv[] = { "berkelium" };
        CommandLine::Init(1, argv);
        new BrowserProcessImpl(*CommandLine::ForCurrentProcess());
        assert(g_browser_process);
    }
	mMessageLoop = new MessageLoop(MessageLoop::TYPE_UI);
//    mNotificationService=new NotificationService();
//    ChildProcess* coreProcess=new ChildProcess;
//    coreProcess->set_main_thread(new ChildThread);
    chrome::RegisterPathProvider();
    app::RegisterPathProvider();
    FilePath homedirpath;
    PathService::Get(chrome::DIR_USER_DATA,&homedirpath);

#if !defined(OS_MACOSX)
    ResourceBundle::InitSharedInstance(L"en-US");// FIXME: lookup locale
    // We only load the theme dll in the browser process.
    ResourceBundle::GetSharedInstance().LoadThemeResources();
#endif  // !defined(OS_MACOSX)
    
    mProf = Profile::CreateProfile(homedirpath);
}
Root::~Root(){
    delete mProf;
//    delete mNotificationService;
    delete mMessageLoop;
    delete g_browser_process;
}


}
