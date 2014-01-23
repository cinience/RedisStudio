#include "StdAfx.h"
#include "RedisSetModel.h"

RedisSetModel::RedisSetModel( RedisClient* client ) : AbstractRedisModel(client)
{

}

bool RedisSetModel::GetData( const std::string& key, RedisResult& results )
{
    bool retVal = false;
    redisReply* reply = GetClient()->Command("SMEMBERS %s", key.c_str());
    if (!reply)  return retVal;
    results.NewColumn("Value");
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        std::size_t i = 0;
        redisReply* tmpReply ;
        while (i < reply->elements)
        {
            tmpReply = reply->element[i];
            results.NewRow();
            string& myvalue = results.Value(results.RowSize()-1, 0);
            myvalue.assign(tmpReply->str, tmpReply->len);
            i++;
        }
        retVal = true;

    }
    freeReplyObject(reply);
    return retVal;
}

bool RedisSetModel::UpdateData( const std::string& key, 
                                const std::string& oldValue, 
                                const std::string& newValue,
                                int idx,
                                const std::string& field )
{
    ScopedRedisReply reply(GetClient()->Command("SREM %s %s", key.c_str(), oldValue.c_str()));
    if(reply.IsNull() || reply->type != REDIS_REPLY_INTEGER || reply->integer==0) 
          return false;

    ScopedRedisReply addReply(GetClient()->Command("SADD %s %s", key.c_str(), newValue.c_str()));
    if (addReply.IsNull() || addReply->type != REDIS_REPLY_INTEGER || reply->integer==0)
          return false;
    return true;
}
