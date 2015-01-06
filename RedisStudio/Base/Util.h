#ifndef Base_Util_INCLUDED
#define Base_Util_INCLUDED

#include <string>
#include <sstream>

namespace Base {

class Util {

public:
    static std::string convertHumanTimeDuration(long long seconds);

    static std::string getPCID(); 
    };

} // namespace Base

#endif
