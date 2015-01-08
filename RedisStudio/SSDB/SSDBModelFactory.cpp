#include "StdAfx.h"
#include "SSDBModelFactory.h"
#include "AbstractSSDBModel.h"
#include "SSDBStringModel.h"
#include "SSDBQueueModel.h"
#include "SSDBZSetModel.h"
#include "SSDBHashModel.h"

SSDBModelFactory::SSDBModelFactory(ssdb::Client* client)
{
    Register("string", new SSDBStringModel(client));
    Register("queue",  new SSDBQueueModel(client));
    Register("zset",   new SSDBZSetModel(client));
    Register("hash",   new SSDBHashModel(client));
}

SSDBModelFactory::~SSDBModelFactory()
{
    TDicModels::iterator it = m_oModels.begin();
    for (; it!=m_oModels.end(); ++it)
    {
        delete it->second;
    }
}

void SSDBModelFactory::Register( const std::string& type, AbstractSSDBModel* p )
{
    if (m_oModels.find(type) != m_oModels.end())
    {
        UnregisterModel(type);
    }
    m_oModels.insert(std::make_pair(type, p));
}

void SSDBModelFactory::UnregisterModel( const std::string& type )
{
    TDicModels::iterator it = m_oModels.find(type);
    if (it != m_oModels.end())
    {
        delete it->second;
        m_oModels.erase(it);
    }
}

AbstractSSDBModel* SSDBModelFactory::GetSSDBModel(const std::string& type )
{
    if (m_oModels.find(type) != m_oModels.end())
    {
        return m_oModels[type];
    }
    return NULL;
}
