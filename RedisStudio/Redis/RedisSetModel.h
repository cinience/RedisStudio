#ifndef RedisSetModel_INCLUDED
#define RedisSetModel_INCLUDED

#include "AbstractRedisModel.h"

class RedisSetModel : public AbstractRedisModel
{
public:
    RedisSetModel(RedisClient* client);

    virtual bool GetData(const std::string& key, RedisResult& results);

    virtual bool UpdateData(const std::string& key, 
        const std::string& oldValue,  
        const std::string& newValue,
        int idx,
        const std::string& field="");
};

#endif
