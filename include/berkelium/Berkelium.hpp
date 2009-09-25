#ifndef _BERKELIUM_HPP_
#define _BERKELIUM_HPP_
#include "berkelium/Platform.hpp"
namespace Berkelium {

/* TODO: Allow forkedProcessHook to be called without requiring the
   library to be initialized/in memory (unless this is a sub-process).
   i.e. an inline function that first searches for "--type=" in argv,
   then uses dlopen or GetProcAddress.
*/

void BERKELIUM_EXPORT forkedProcessHook(int argc, char **argv);
//void BERKELIUM_EXPORT forkedProcessHook(char *szCmdLine);

void BERKELIUM_EXPORT init();
void BERKELIUM_EXPORT destroy();

void BERKELIUM_EXPORT run();

int BERKELIUM_EXPORT renderToBuffer(char *buffer, int width, int height);

}
#endif
