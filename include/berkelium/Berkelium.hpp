#ifndef _BERKELIUM_HPP_
#define _BERKELIUM_HPP_
#include "berkelium/Platform.hpp"
namespace Berkelium {

void BERKELIUM_EXPORT init();
void BERKELIUM_EXPORT destroy();

int BERKELIUM_EXPORT renderToBuffer(char *buffer, int width, int height);

}
#endif
