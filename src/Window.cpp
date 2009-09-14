#include "berkelium/Platform.hpp"
#include "WindowImpl.hpp"
namespace Berkelium {
Window* Window::create() {
    return new WindowImpl();
}


Window::Window() {
}

Window::~Window() {
}

}

