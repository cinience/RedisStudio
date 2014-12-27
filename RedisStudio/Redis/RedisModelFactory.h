#ifndef RedisModelFactory_INCLUDED
#define RedisModelFactory_INCLUDED
#include <map>

class RedisClient;
class AbstractRedisModel;

class RedisModelFactory
{
public:
    RedisModelFactory(RedisClient* client);

    ~RedisModelFactory();

    void Register(const std::string& type, AbstractRedisModel* p);

    void UnregisterModel(const std::string& type);

    AbstractRedisModel* GetRedisModel(const std::string& type);
private:
    typedef std::map<std::string, AbstractRedisModel*> TDicModels;
    TDicModels m_oModels;
};

#endif
