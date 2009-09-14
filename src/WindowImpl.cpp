#include "berkelium/Platform.hpp"
#include "WindowImpl.hpp"
namespace Berkelium {
WindowImpl::WindowImpl() {
    
}
WindowImpl::WindowImpl(const Context*otherContext):Window(otherContext) {
}
WindowImpl::~WindowImpl() {

}

WindowImpl* WindowImpl::getImpl(){
    return this;
}
}
