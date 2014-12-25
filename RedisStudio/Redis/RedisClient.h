#ifndef RedisClient_INCLUDED
#define RedisClient_INCLUDED

#include "hiredis/hiredis.h"
#include <list>
#include <map>
#include <string>
#include "../Base/Mutex.h"
#include "ScopedRedisReply.h"
#include "RedisModelFactory.h"

typedef void (* callback)(const CDuiString& );


class RedisResult;

class RedisClient
{
public:
    typedef std::list<std::string> TSeqArrayResults;
    typedef std::map<std::string, std::string> TDicConfig;
public:
    RedisClient();

    ~RedisClient();

    static RedisClient& GetInstance();

    void SetServerInfo(const CDuiString& name, const string& ip, int port,const string& auth);

    bool Connect();

    bool IsConnected();

    void NeedReConnect();

    void Quit();

    bool Info(std::string& results);

    bool keys(TSeqArrayResults& results);

    bool Exists(const std::string& key);

    bool Type(const std::string& key, string& type);

	long long TTL(const std::string& key);

    bool DatabasesNum(int& num);

    int  DatabasesNum();

    bool SelectDB(int dbindex);

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

    redisReply* Command(const char* fmt, ...);

    void SetLastError(const std::string& err);

    CDuiString GetLastError();

    CDuiString GetName();
public:
    void SetDisConnectCallback(callback fn)
    {
        m_fnDisConnect = fn;
    }

private:
    redisContext* m_pClient;
    bool          m_bReConnect;
    CDuiString    m_strName;
    string        m_strIP;
    int           m_iPort;
    string        m_strAuth;
    int           m_Databases;
    CDuiString    m_StrErr;
    bool          m_isConnected;
    callback      m_fnDisConnect;
    Base::Mutex         m_mutex;
    std::auto_ptr<RedisModelFactory> m_ModelFactory;
    
};

#endif
