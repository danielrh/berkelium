#include "berkelium/Platform.hpp"
#include "WindowImpl.hpp"
#include "Root.hpp"
#include "ContextImpl.hpp"

#include "chrome/browser/profile.h"

namespace Berkelium {
Window* Window::create() {
    return new WindowImpl(new ContextImpl(Root::getSingleton().getProfile()));
}
Window* Window::create(const Context &otherContext) {
    return new WindowImpl(&otherContext);
}


Window::Window() {
    mContext=NULL;
}
Window::Window(const Context*otherContext) {
    mContext=otherContext->clone();
}

Window::~Window() {
    delete mContext;
}

}

