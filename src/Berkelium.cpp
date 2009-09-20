
#include "chrome/browser/tab_contents/tab_contents.h"
#include "berkelium/Berkelium.hpp"
#include "Root.hpp"
namespace Berkelium {

void init() {
    new Root();
}
void destroy() {
    Root::destroy();
}
void run() {
    Root::getSingleton().runUIMessageLoop();
}

int renderToBuffer(char * buffer, int width, int height) {
    return 0;
}

}
