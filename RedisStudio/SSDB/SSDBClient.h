#ifndef SSDBClient_INCLUDED
#define SSDBClient_INCLUDED

#include "hiredis/hiredis.h"
#include <list>
#include <map>
#include <string>
#include "../Base/Mutex.h"
#include "SSDBModelFactory.h"
#include "../DBClient.h"
#include "SSDB.h"

class RedisResult;
 
class SSDBClient : public DBClient
{
public:
    SSDBClient();

    ~SSDBClient();

    bool Connect(const std::string& ip, int port, const std::string& auth);

    bool Ping();

    bool IsConnected();

    void Quit();

    bool Info(std::string& results);

    bool keys(const std::string &, TSeqArrayResults& results);

    bool Exists(const std::string& key);

    bool Type(const std::string& key, string& type);

    long long TTL(const std::string& key);

    bool DatabasesNum(int& num);

    int  DatabasesNum();

    bool SelectDB(int dbindex);

    long long DbSize();

    bool GetConfig(TDicConfig& dicConfig);

    bool SetConfig(const TDicConfig& dicConfig);

    bool ReWriteConfig();

    bool GetData(const std::string& key, std::string& type, RedisResult& results);

    bool UpdateData(const std::string& key, 
                    const std::string& oldValue, 
                    const std::string& newValue,
                    int idx,
                    const std::string& field="");

    bool DelKey(const std::string& key);

    std::string RealKey(const std::string& key);

private:
    ssdb::Client* m_Client;

    std::map<std::string, std::string> m_mapKeys;
    string        m_strIP;
    int           m_iPort;
    string        m_strAuth;
    bool          m_bConnected;
    
    Base::Mutex         m_oMutex;
    std::auto_ptr<SSDBModelFactory> m_pModelFactory; 
};

#endif
