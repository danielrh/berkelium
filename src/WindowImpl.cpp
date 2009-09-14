#include "berkelium/Platform.hpp"
#include "WindowImpl.hpp"
namespace Berkelium {
WindowImpl::WindowImpl() {

}
WindowImpl::~WindowImpl() {

}

WindowImpl* WindowImpl::getImpl(){
    return this;
}
}
