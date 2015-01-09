#include "StdAfx.h"
#include "Environment.h"
#include "DBClient.h"


Environment::Environment():m_DbClient(NULL)
{

}

Environment::~Environment()
{
    if (m_DbClient) delete m_DbClient;
}

void Environment::SetDBClient(DBClient* cli)
{
    m_DbClient = cli;
}

DBClient* Environment::GetDBClient()
{
    return m_DbClient;
}

void Environment::SetDBName(const CDuiString& name)
{
    m_DbName = name;
}

DuiLib::CDuiString Environment::GetDBName()
{
    return m_DbName;
}

void Environment::SetDBServerInfo(const std::string& addr, int port, const std::string& auth/*=""*/)
{
    m_sIP = addr;
    m_iPort = port;
    m_sAuth = auth;
}

std::string Environment::GetDBIP()
{
    return m_sIP;
}

int Environment::GetDBPort()
{
    return m_iPort;
}

std::string Environment::GetDBPasswd()
{
    return m_sAuth;
}
