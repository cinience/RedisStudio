#ifndef RedisStringModel_INCLUDED
#define RedisStringModel_INCLUDED

#include "AbstractRedisModel.h"

class RedisStringModel : public AbstractRedisModel
{
public:
    RedisStringModel(RedisClient* client);

    virtual bool GetData(const std::string& key, RedisResult& results);

    virtual bool UpdateData(const std::string& key, 
        const std::string& oldValue,  
        const std::string& newValue,
        int idx,
        const std::string& field="");
};

#endif
