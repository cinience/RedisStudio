#pragma once

#include "Base/Mutex.h"

class DBClient;

class Environment {
public:
    Environment();

    ~Environment();
    
    void SetDBClient(DBClient* cli);

    void SetDBServerInfo(const std::string& addr, int port, const std::string& auth="");

    void SetDBName(const CDuiString& name);

    DBClient* GetDBClient();

    int  GetDBIdx();

    CDuiString GetDBName();

    std::string GetDBIP();

    int GetDBPort();

    std::string GetDBPasswd();

private:
    CDuiString  m_DbName;
    std::string m_IP;
    int         m_Port;
    std::string m_Auth;
    DBClient*   m_DbClient;
    Base::Mutex m_Mutex;
    int    m_ConnIdx;
};
