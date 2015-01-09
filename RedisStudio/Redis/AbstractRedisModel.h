#ifndef AbstractRedisModel_INCLUDED
#define AbstractRedisModel_INCLUDED

#include "RedisResult.h"
#include "RedisClient.h"

class AbstractRedisModel
{
public:
    AbstractRedisModel(RedisClient* client) : m_pClient(client) {}

    virtual ~AbstractRedisModel() {}

    virtual bool GetData(const std::string& key, RedisResult& results) =  0;

    virtual bool UpdateData(const std::string& key, 
        const std::string& oldValue,  
        const std::string& newValue,
        int idx = 0,
        const std::string& field="") = 0;

    RedisClient* GetClient()
    {
        return m_pClient;
    }

private:
    RedisClient* m_pClient;
};

#endif
