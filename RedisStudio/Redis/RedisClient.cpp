#include "StdAfx.h"
#include "RedisClient.h"
#include <stdarg.h>
#include "../Base/CharacterSet.h"
#include "RedisResult.h"
#include "AbstractRedisModel.h"

RedisClient::RedisClient() : m_bReConnect(true),m_bConnected(false),
m_pClient(NULL)
{    
}


RedisClient::~RedisClient()
{
    
}

bool RedisClient::Connect(const std::string& ip, int port, const std::string& auth)
{
    if (IsConnected())
    {
        Quit();
    }
    struct timeval timeout = { 5, 500000 }; 
    m_pClient = redisConnectWithTimeout((char*)ip.c_str(), 
        port, 
        timeout);
    if (m_pClient->err) {        
        SetLastError(m_pClient->errstr);
        //m_fnDisConnect(GetLastError());
        return false;
    }
    if (!auth.empty())
    {
        redisReply* reply = Command("AUTH %s", auth.c_str());
        if (!(reply && reply->type == REDIS_REPLY_STATUS))
        {
            SetLastError(reply->str);
            return false;
        }
    }
    m_bConnected = true;
    //if (m_isConnected && DatabasesNum(m_Databases)) return true;
    return false;
}

bool RedisClient::Ping()
{
    ScopedRedisReply reply(Command("ping"));
    if (!reply.IsNull() && reply->type == REDIS_REPLY_STATUS) 
    {
        return true;
    }
    return false;
}

bool RedisClient::IsConnected()
{
    return m_bConnected;
}

bool RedisClient::Info(std::string& results)
{
    bool retVal = false;

    if (!IsConnected()) return retVal;
    redisReply* reply = Command("info");    
    if (!reply) return retVal;

    if (reply->type == REDIS_REPLY_STRING)
    {
        retVal = true;
        results = reply->str;
    }
    else if (reply->type == REDIS_REPLY_STATUS)
    {
        /// ssdb 返回正常状态
        retVal = true;
    }
    freeReplyObject(reply);

    return retVal;
}

bool RedisClient::keys(const std::string& matchstr, TSeqArrayResults& results)
{
    bool retVal = false;

    if (!IsConnected()) return retVal;

    redisReply* reply = Command("KEYS %s", matchstr.c_str());    
    if (!reply) return retVal;

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        retVal = true;
        std::size_t i = 0;
        redisReply* tmpReply ;
        while (i < reply->elements)
        {
            tmpReply = reply->element[i];
            results.push_back(tmpReply->str);
            i++;
        }
    }
    results.sort();
    freeReplyObject(reply);
    return retVal;
}

bool RedisClient::Exists(const std::string& key)
{
    ScopedRedisReply reply(Command("EXISTS %s", key.c_str()));
    if (reply.IsNull() || reply->type!=REDIS_REPLY_INTEGER || reply->integer==0) return false;
    return true;
}

bool RedisClient::Type(const std::string& key, string& type)
{
    bool retVal = false;
    redisReply* reply = Command("TYPE %s", key.c_str());
    if (!reply) return retVal;
    if (reply->type == REDIS_REPLY_STATUS)
    {
        type = reply->str;
        retVal = true;
    }
    freeReplyObject(reply);
    return true;
}

long long RedisClient::TTL(const std::string& key)
{
    long long ttl = 0;
    ScopedRedisReply reply(Command("TTL %s", key.c_str()));
    if (!reply.IsNull() && reply->type == REDIS_REPLY_INTEGER)
    {
        ttl = reply->integer;
    }
    return ttl;
}

void RedisClient::Quit()
{
    Command("quit");
    m_bConnected = false;
}

redisReply* RedisClient::Command( const char* fmt, ... )
{
    if (!m_pClient) NULL;
    Base::Mutex::ScopedLock scopedLock(m_oMutex);
    redisReply* reply = NULL;
    va_list ap;
    va_start(ap, fmt);
    reply = (redisReply*) redisvCommand(m_pClient, fmt, ap);
    va_end(ap);
    if (reply && reply->type == REDIS_REPLY_ERROR)
    {
        SetLastError(reply->str);
    }
    if (reply==NULL)
    {
        if (reply != NULL) {
             //m_fnDisConnect(GetLastError());
        }
        m_bConnected = false;
    }

    return reply;
}

bool RedisClient::DatabasesNum(int& num)
{
    bool retVal = false;
    redisReply* reply = Command("config get databases");
    if (!reply) {   
        return retVal;
    }

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements>1 )
    {
        retVal = true;
        num = atoi(reply->element[1]->str);
        freeReplyObject(reply);
    } else {
        freeReplyObject(reply);
        for (int idx = 0; idx<16; ++idx) {
            reply = Command("SELECT %d", idx);
            if (!reply) return false;

            if (reply->type == REDIS_REPLY_STATUS)
            {
                retVal = true;
                num = idx + 1;
                freeReplyObject(reply);
            } else {
                freeReplyObject(reply);
                break;
            }   
        }
    }
    return retVal;
}

int RedisClient::DatabasesNum()
{
    m_iDatabases = 0;
    DatabasesNum(m_iDatabases);
    return m_iDatabases;
}

bool RedisClient::SelectDB( int dbindex )
{
    redisReply* reply = Command("SELECT %d", dbindex);
    if (!reply) return false;
    if (reply->type != REDIS_REPLY_STATUS)
    {
        freeReplyObject(reply);
        return false;
    }
    freeReplyObject(reply);
    return true;
}

long long RedisClient::DbSize() 
{
    long long dbsize = 0;
    ScopedRedisReply reply(Command("DBSIZE"));
    if (!reply.IsNull() && reply->type == REDIS_REPLY_INTEGER)
    {
        dbsize = reply->integer;
    }
    return dbsize;
}

bool RedisClient::GetConfig(TDicConfig& dicConfig)
{
    dicConfig.clear();
    redisReply* reply = Command("CONFIG GET *");
    if (!reply) return false;
    if (reply->type != REDIS_REPLY_ARRAY)
    {
        freeReplyObject(reply);
        return false;
    }
    std::size_t i = 0;
    std::string configKey , configValue;
    redisReply* tmpReply ;
    while (i < reply->elements)
    {
        tmpReply = reply->element[i++];
        configKey.assign(tmpReply->str, tmpReply->len);
        tmpReply = reply->element[i++];
        configValue.assign(tmpReply->str, tmpReply->len);
        dicConfig.insert(std::make_pair(configKey, configValue));
    }
    freeReplyObject(reply);
    return true;
}

bool RedisClient::SetConfig(const TDicConfig& dicConfig)
{
    TDicConfig::const_iterator it = dicConfig.begin();
    TDicConfig::const_iterator itend = dicConfig.end();

    for (; it!=itend; ++it)
    {
        std::string key = it->first;
        std::string val = it->second;
        redisReply* reply = Command("CONFIG SET %s %s", key.c_str(), val.c_str());
        if (!reply) return false;
        if (reply->type != REDIS_REPLY_STATUS)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
    }

    return true;
}

bool RedisClient::ReWriteConfig()
{
    redisReply* reply = Command("CONFIG REWRITE");
    if (!reply) return false;

    if (reply->type != REDIS_REPLY_STATUS)
    {
        freeReplyObject(reply);
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool RedisClient::GetData( const std::string& key, std::string& type, RedisResult& results )
{
    redisReply* reply = NULL;
    if (!Type(key, type)) return false;

    if (type == "none") return false;
    m_pModelFactory.reset(new RedisModelFactory(this));
    return m_pModelFactory->GetRedisModel(type)->GetData(key, results);
}

bool RedisClient::DelKey( const std::string& key )
{
    ScopedRedisReply reply(Command("DEL %s", key.c_str()));
    if (reply.IsNull() || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0)
    {
        return false;
    }
    return true;
}

bool RedisClient::UpdateData( const std::string& key, 
                              const std::string& oldValue, 
                              const std::string& newValue,
                              int idx,
                              const std::string& field)
{
    redisReply* reply = NULL;
    std::string type;
    if (!Type(key, type)) return false;

    if (type == "none") return false;
    m_pModelFactory.reset(new RedisModelFactory(this));
    return m_pModelFactory->GetRedisModel(type)->UpdateData(key, oldValue, newValue, idx, field);
}
