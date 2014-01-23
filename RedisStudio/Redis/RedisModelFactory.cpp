#include "StdAfx.h"
#include "RedisModelFactory.h"
#include "AbstractRedisModel.h"
#include "RedisStringModel.h"
#include "RedisListModel.h"
#include "RedisSetModel.h"
#include "RedisZSetModel.h"
#include "RedisHashModel.h"

RedisModelFactory::RedisModelFactory(RedisClient* client)
{
    Register("string", new RedisStringModel(client));
    Register("list",   new RedisListModel(client));
    Register("set",    new RedisSetModel(client));
    Register("zset",   new RedisZSetModel(client));
    Register("hash",   new RedisHashModel(client));
}

RedisModelFactory::~RedisModelFactory()
{
    TDicModels::iterator it = m_oModels.begin();
    for (; it!=m_oModels.end(); ++it)
    {
        delete it->second;
    }
}

void RedisModelFactory::Register( const std::string& type, AbstractRedisModel* p )
{
    if (m_oModels.find(type) != m_oModels.end())
    {
        UnregisterModel(type);
    }
    m_oModels.insert(std::make_pair(type, p));
}

void RedisModelFactory::UnregisterModel( const std::string& type )
{
    TDicModels::iterator it = m_oModels.find(type);
    if (it != m_oModels.end())
    {
        delete it->second;
        m_oModels.erase(it);
    }
}

AbstractRedisModel* RedisModelFactory::GetRedisModel( const std::string& type )
{
    if (m_oModels.find(type) != m_oModels.end())
    {
        return m_oModels[type];
    }
    return NULL;
}
