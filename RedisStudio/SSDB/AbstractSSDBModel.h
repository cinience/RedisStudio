#ifndef AbstractRedisModel_INCLUDED
#define AbstractRedisModel_INCLUDED

#include <string>
#include "SSDB.h"

class RedisResult;

class AbstractSSDBModel
{
public:
    AbstractSSDBModel(ssdb::Client* client) : m_pClient(client) {}

    virtual bool GetData(const std::string& key, RedisResult& results) =  0;

    virtual bool UpdateData(const std::string& key, 
        const std::string& oldValue,  
        const std::string& newValue,
        int idx = 0,
        const std::string& field="") = 0;

    ssdb::Client* GetClient()
    {
        return m_pClient;
    }

private:
    ssdb::Client* m_pClient;
};

#endif
