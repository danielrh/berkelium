//#include "chrome/browser/tab_contents/tab_contents.h"

#include "berkelium/Berkelium.hpp"
#include "berkelium/Window.hpp"
#include "berkelium/WindowDelegate.hpp"
#include "berkelium/Context.hpp"

#include <iostream>

using namespace Berkelium;

class TestDelegate : public WindowDelegate {
    std::string mURL;
public:

    virtual void onAddressBarChanged(Window *win, const std::string &newURL) {
        mURL = newURL;
        std::cout << "*** onAddressChanged to "<<mURL<<std::endl;
    }

    virtual void onStartLoading(Window *win, const std::string &newURL) {
        std::cout << "*** Start loading "<<newURL<<" from "<<mURL<<std::endl;
    }
    virtual void onLoad(Window *win) {
        sleep(3);
        std::cout << "*** onLoad "<<mURL<<std::endl;
        if (mURL.find("yahoo") != std::string::npos) {
            return;
        }
        if (mURL.find("google") == std::string::npos) {
            win->navigateTo("http://google.com");
            return;
        }
        if (mURL.find("yahoo") == std::string::npos) {
            win->navigateTo("http://yahoo.com");
        }
    }
    virtual void onLoadError(Window *win, const std::string &error) {
        std::cout << "*** onLoadError "<<mURL<<": "<<error<<std::endl;
    }

    virtual void onPaint(Window *win) {
        std::cout << "*** onPaint "<<mURL<<std::endl;
    }

    virtual void onBeforeUnload(Window *win, bool *proceed) {
        std::cout << "*** onBeforeUnload "<<mURL<<std::endl;
        *proceed = true;
    }
    virtual void onCancelUnload(Window *win) {
        std::cout << "*** onCancelUnload "<<mURL<<std::endl;
    }
    virtual void onCrashed(Window *win) {
        std::cout << "*** onCrashed "<<mURL<<std::endl;
    }

    virtual void onCreatedWindow(Window *win, Window *newWindow) {
        std::cout << "*** onCreatedWindow from source "<<mURL<<std::endl;
        newWindow->setDelegate(new TestDelegate);
    }
};

#define WIDTH 1024
#define HEIGHT 768
int main () {
    printf("RUNNING MAIN!\n");
    Berkelium::init();
    Window* win=Window::create();
    win->resize(800,600);
    win->setDelegate(new TestDelegate);
    win->navigateTo("http://xkcd.com");
    //win->navigateTo("http://google.com");

/*
    Window* win2=Window::create();
    win2->resize(800,600);
    win2->navigateTo("http://slashdot.org");
    Window* win3=Window::create();
    win3->resize(800,600);
    win3->navigateTo("http://vegastrike.sourceforge.net");

    Window* win4=Window::create();
    win4->resize(800,600);
    win4->navigateTo("http://xkcd.com");

    Window* win5=Window::create();
    win5->resize(800,600);
    win5->navigateTo("http://sirikata.com");
*/
    char buffer[WIDTH][HEIGHT][3];
    Berkelium::run();
    int retval=Berkelium::renderToBuffer(&buffer[0][0][0],WIDTH,HEIGHT);
    delete win;
    //delete win2;
    //delete win3;
    //delete win4;
    //delete win5;
    return retval;
}
