#include "StdAfx.h"
#include "Util.h"

namespace Base {

std::string Util::HumanReadableTimeDuration(long long seconds)
{
    std::stringstream sb;
    if (seconds == 0) {
        return "0 secs";
    }        
    if (seconds >= 86400) {
        sb << seconds / 86400;
        sb << " days";
        seconds -= (seconds / 86400) * 86400;
    }
    if (seconds >= 3600) {
        if (sb.str().size() > 0) {
            sb << " ";
        }
        sb << seconds / 3600;
        sb << "hrs" ;
        seconds -= (seconds / 3600) * 3600;
    }
    if (seconds >= 60) {
        if (sb.str().size() > 0) {
            sb << " ";
        }
        sb << seconds / 60 << "mins";
        seconds -= (seconds / 60) * 60;
    }
    if (seconds > 0) {
        if (sb.str().size() > 0) {
            sb << " ";
        } 
        sb << seconds;
        sb << "secs";
    }

    return sb.str();
}

} // namespace Base