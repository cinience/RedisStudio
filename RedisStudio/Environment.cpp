#include "StdAfx.h"
#include "Environment.h"
#include "DBClient.h"

Environment::Environment():m_DbClient(NULL),m_ConnIdx(0)
{
}

Environment::~Environment()
{
    if (m_DbClient) delete m_DbClient;
}

void Environment::SetDBClient(DBClient* cli)
{
    Base::Mutex::ScopedLock scopedLock(m_Mutex);
    m_DbClient = cli;
    m_ConnIdx++;
}

DBClient* Environment::GetDBClient()
{
    Base::Mutex::ScopedLock scopedLock(m_Mutex);
    return m_DbClient;
}

int Environment::GetDBIdx()
{
    Base::Mutex::ScopedLock scopedLock(m_Mutex);
    return m_ConnIdx;
}

void Environment::SetDBName(const CDuiString& name)
{
    Base::Mutex::ScopedLock scopedLock(m_Mutex);
    m_DbName = name;
}

DuiLib::CDuiString Environment::GetDBName()
{
    Base::Mutex::ScopedLock scopedLock(m_Mutex);
    return m_DbName;
}

void Environment::SetDBServerInfo(const std::string& addr, int port, const std::string& auth/*=""*/)
{
    Base::Mutex::ScopedLock scopedLock(m_Mutex);
    m_IP = addr;
    m_Port = port;
    m_Auth = auth;
}

std::string Environment::GetDBIP()
{
    Base::Mutex::ScopedLock scopedLock(m_Mutex);
    return m_IP;
}

int Environment::GetDBPort()
{
    Base::Mutex::ScopedLock scopedLock(m_Mutex);
    return m_Port;
}

std::string Environment::GetDBPasswd()
{
    Base::Mutex::ScopedLock scopedLock(m_Mutex);
    return m_Auth;
}
