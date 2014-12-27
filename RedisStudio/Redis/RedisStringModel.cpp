#include "StdAfx.h"
#include "RedisStringModel.h"

RedisStringModel::RedisStringModel( RedisClient* client ) : AbstractRedisModel(client)
{

}

bool RedisStringModel::GetData( const std::string& key, RedisResult& results )
{
    bool retVal = false;
    redisReply* reply = GetClient()->Command("GET %s", key.c_str());
    if (!reply)  return retVal;
    results.NewColumn("Value");
    if (reply->type == REDIS_REPLY_STRING)
    {
        results.NewRow();
        string& myvalue = results.Value(results.RowSize()-1, 0);
        myvalue.assign(reply->str, reply->len);
        retVal = true;
    }
    freeReplyObject(reply);
    return retVal;
}

bool RedisStringModel::UpdateData( const std::string& key, 
                                   const std::string& oldValue, 
                                   const std::string& newValue,
                                   int idx,
                                   const std::string& field )
{
    ScopedRedisReply reply(GetClient()->Command("SET %s %s", key.c_str(), newValue.c_str()));
    if (reply.IsNull() || reply->type != REDIS_REPLY_STATUS)  return false;
    return true;
}

