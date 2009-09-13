//#include "chrome/browser/tab_contents/tab_contents.h"

#include "berkelium/Berkelium.hpp"
#define WIDTH 1024
#define HEIGHT 768
int main () {
    Berkelium::init();
    char buffer[WIDTH][HEIGHT][3];
    
    int retval=Berkelium::renderToBuffer(&buffer[0][0][0],WIDTH,HEIGHT);
    return retval;
}
