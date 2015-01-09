#include "stdafx.h"
#include "DBClient.h"

#include "Redis/RedisClient.h"
#include "SSDB/SSDBClient.h"

DBClient* DBClient::Create(const std::string& ip, int port, const std::string& auth)
{
    std::string info;
    DBClient* dbcli = new RedisClient();
    dbcli->Connect(ip, port, auth);
    if (!dbcli->IsConnected()) {
        delete dbcli;
        return NULL;
    }
    if (!dbcli->Info(info)) {
        delete dbcli;
        return NULL;
    }
    dbcli->SetDBType("redis");
    if (info.empty()) {
        delete dbcli;
        dbcli = new SSDBClient();
        if (!dbcli->Connect(ip, port, auth)) 
        {
            delete dbcli;
            dbcli = NULL;
        }
        dbcli->SetDBType("ssdb");
    }
    return dbcli;
}


void DBClient::SetDBType(const std::string& dbtype)
{
    m_DBType = dbtype;
}

std::string DBClient::DBType()
{
    return m_DBType;
}

bool DBClient::DatabasesNum(int& num)
{
    num = 1;
    return true;
}

int DBClient::DatabasesNum()
{
    return 1;
}

bool DBClient::SelectDB(int dbindex)
{
    return false;
}

bool DBClient::GetConfig(TDicConfig& dicConfig)
{
    return false;
}

bool DBClient::SetConfig(const TDicConfig& dicConfig)
{
    return false;
}

bool DBClient::ReWriteConfig()
{
    return false;
}

void DBClient::SetLastError(const std::string& err)
{
    m_Error = err;
}

std::string DBClient::GetLastError()
{
    return m_Error;
}
