#ifndef _BERKELIUM_CONTEXT_HPP_
#define _BERKELIUM_CONTEXT_HPP_

class SiteInstance;
class BrowsingInstance;

namespace Berkelium {
class ContextImpl;

class BERKELIUM_EXPORT Context {
  protected:
    Context();

public:

    virtual ~Context();
    virtual Context* clone()const=0;
    virtual ContextImpl *getImpl()=0;
    virtual const ContextImpl *getImpl()const=0;
};

}

#endif
