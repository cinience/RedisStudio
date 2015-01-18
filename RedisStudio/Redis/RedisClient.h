#ifndef RedisClient_INCLUDED
#define RedisClient_INCLUDED

#include "hiredis/hiredis.h"
#include <list>
#include <map>
#include <string>
#include "../Base/Mutex.h"
#include "ScopedRedisReply.h"
#include "RedisModelFactory.h"
#include "../DBClient.h"

class RedisResult;
 
class RedisClient : public DBClient
{
public:
    RedisClient();

     ~RedisClient();

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

    redisReply* Command(const char* fmt, ...);

private:
    redisContext* m_pClient;
    bool          m_bReConnect;
    int           m_iDatabases;
    bool          m_bConnected;
    Base::Mutex         m_oMutex;
    std::auto_ptr<RedisModelFactory> m_pModelFactory;
};

#endif
