// Berkelium headers
#include "berkelium/Platform.hpp"
#include "Root.hpp"
// Chromium headers
#include "base/at_exit.h"
#include "base/path_service.h"
#include "base/thread.h"
#include "base/command_line.h"
#include "base/icu_util.h"
#include "net/base/cookie_monster.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/browser/browser_prefs.h"
#include "chrome/browser/browser_process_impl.h"
#include "chrome/browser/process_singleton.h"
#include "chrome/browser/profile_manager.h"
#include "chrome/browser/renderer_host/browser_render_process_host.h"
#include "app/resource_bundle.h"
#include "app/app_paths.h"
#include "chrome/common/pref_names.h"
//icu_util::Initialize()

AUTO_SINGLETON_INSTANCE(Berkelium::Root);
namespace Berkelium {

Root::Root (){
    new base::AtExitManager();
/*
    base::Thread *coreThread = new base::Thread("CoreThread");
	coreThread->StartWithOptions(base::Thread::Options(MessageLoop::TYPE_UI, 0));
*/
    chrome::RegisterPathProvider();
    app::RegisterPathProvider();
    FilePath homedirpath;
    PathService::Get(chrome::DIR_USER_DATA,&homedirpath);

    mProcessSingleton= new ProcessSingleton(homedirpath);
    BrowserProcess *browser_process;
    {
        const char* argv[] = { "berkelium" };
        CommandLine::Init(1, argv);
        browser_process=new BrowserProcessImpl(*CommandLine::ForCurrentProcess());
        assert(g_browser_process);
    }
	mMessageLoop = new MessageLoop(MessageLoop::TYPE_UI);
//    mNotificationService=new NotificationService();
//    ChildProcess* coreProcess=new ChildProcess;
//    coreProcess->set_main_thread(new ChildThread);

#if !defined(OS_MACOSX)
    ResourceBundle::InitSharedInstance(L"en-US");// FIXME: lookup locale
    // We only load the theme dll in the browser process.
    ResourceBundle::GetSharedInstance().LoadThemeResources();
#endif  // !defined(OS_MACOSX)
    net::CookieMonster::EnableFileScheme();
    ProfileManager* profile_manager = browser_process->profile_manager();
    mProf = profile_manager->GetDefaultProfile(homedirpath);

    PrefService* user_prefs = mProf->GetPrefs();
    DCHECK(user_prefs);
    
    // Now that local state and user prefs have been loaded, make the two pref
    // services aware of all our preferences.
    browser::RegisterAllPrefs(user_prefs, browser_process->local_state());

    browser_process->local_state()->RegisterStringPref(prefs::kApplicationLocale, L"");
    browser_process->local_state()->RegisterBooleanPref(prefs::kMetricsReportingEnabled, false);
//    browser_process->local_state()->SetString(prefs::kApplicationLocale,std::wstring());
    mProcessSingleton->Create();
}
Root::~Root(){
    //  delete mProcessSingleton;
    delete mProf;
//    delete mNotificationService;
    delete mMessageLoop;
    delete g_browser_process;
}


}
