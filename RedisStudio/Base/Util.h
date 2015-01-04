#ifndef Base_Util_INCLUDED
#define Base_Util_INCLUDED

#include <string>
#include <sstream>

namespace Base {

class Util {

public:
    static std::string HumanReadableTimeDuration(long long seconds);
    };

} // namespace Base

#endif