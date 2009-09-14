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
    char buffer[WIDTH][HEIGHT][3];
    
    int retval=Berkelium::renderToBuffer(&buffer[0][0][0],WIDTH,HEIGHT);
    delete win;
    return retval;
}
