#ifndef RedisHashModel_INCLUDED
#define RedisHashModel_INCLUDED

#include "AbstractRedisModel.h"

class RedisHashModel : public AbstractRedisModel
{
public:
    RedisHashModel(RedisClient* client);

    virtual bool GetData(const std::string& key, RedisResult& results);

    virtual bool UpdateData(const std::string& key,      
        const std::string& oldValue,  
        const std::string& newValue,
        int idx,
        const std::string& field="");
};

#endif
