#include "berkelium/Platform.hpp"
#include "WindowImpl.hpp"
#include "ContextImpl.hpp"
namespace Berkelium {
Window* Window::create() {
    return new WindowImpl();
}
Window* Window::create(const Context &otherContext) {
    return new WindowImpl(&otherContext);
}


Window::Window() {
    mContext=Context::create();
}
Window::Window(const Context*otherContext) {
    mContext=otherContext->clone();
}

Window::~Window() {
    delete mContext;
}

}

