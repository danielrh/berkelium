#ifndef _BERKELIUM_CONTEXT_HPP_
#define _BERKELIUM_CONTEXT_HPP_

class SiteInstance;
class BrowsingInstance;

namespace Berkelium {
class ContextImpl;

class BERKELIUM_EXPORT Context {
public:

    Context();
    ~Context();
    static Context* create();
    virtual ContextImpl *getImpl()=0;
};

}

#endif
