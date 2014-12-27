#ifndef Base_Http_INCLUDED
#define Base_Http_INCLUDED

#include "ScopedLock.h"

namespace Base {

class Http
{
public:
    class Response {
    public:
        int status;
        std::string data;
    };

    Http(const std::string& hostname, int port);
    
    ~Http();
    
    bool ping();
    
    Response post(const std::string& path, const std::string& data);

private:
    std::string _host;
    std::string _ip;
    int         _port;
};

} // namespace Base

#endif