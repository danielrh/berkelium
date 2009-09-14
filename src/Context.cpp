#include "berkelium/Platform.hpp"
#include "ContextImpl.hpp"

namespace Berkelium {

Context* Context::create() {
    return new ContextImpl();
}


Context::Context() {
}
Context::~Context() {
}




}
