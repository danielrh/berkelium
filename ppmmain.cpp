//#include "chrome/browser/tab_contents/tab_contents.h"

#include "berkelium/Berkelium.hpp"
#include "berkelium/Window.hpp"
#include "berkelium/Context.hpp"

#define WIDTH 1024
#define HEIGHT 768
int main () {
    using namespace Berkelium;
    Berkelium::init();
    Window* win=Window::create();
    win->resize(800,600);
    win->navigateTo("http://sirikata.com");
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
