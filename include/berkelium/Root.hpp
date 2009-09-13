#include "berkelium/Platform.hpp"
#include "berkelium/Singleton.hpp"
namespace Berkelium {
//singleton class that contains chromium singletons. Not visible outside of Berkelium library core
class Root:public AutoSingleton<Root> {
public:
    Root();
    ~Root();
};

}
