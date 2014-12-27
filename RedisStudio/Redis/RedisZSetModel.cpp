#include "StdAfx.h"
#include "RedisZSetModel.h"

RedisZSetModel::RedisZSetModel( RedisClient* client ) : AbstractRedisModel(client)
{

}

bool RedisZSetModel::GetData( const std::string& key, RedisResult& results )
{
    bool retVal = false;
    redisReply* reply = GetClient()->Command("ZREVRANGE %s %d %d WITHSCORES", key.c_str(), 0, -1);
    if (!reply)  return retVal;

    results.NewColumn("Score");
    results.NewColumn("Value");
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        std::size_t i = 0;
        redisReply* tmpReply ;
        while (i < reply->elements)
        {
            tmpReply = reply->element[i];
            if (i%2 == 0)
            {
                results.NewRow();

                string& myvalue = results.Value(results.RowSize()-1, 1);
                myvalue.assign(tmpReply->str, tmpReply->len);
            }
            else 
            {
                string& myvalue = results.Value(results.RowSize()-1, 0);
                myvalue.assign(tmpReply->str, tmpReply->len);
            }            
            i++;
        }
        retVal = true;
    }
    freeReplyObject(reply);
    return retVal;
}

bool RedisZSetModel::UpdateData(const std::string& key, 
                                const std::string& oldValue, 
                                const std::string& newValue,
                                int idx,
                                const std::string& field)
{
    ScopedRedisReply reply(GetClient()->Command("ZREM %s %s", key.c_str(), oldValue.c_str()));
    if(reply.IsNull() || reply->type != REDIS_REPLY_INTEGER || reply->integer==0) 
        return false;

    ScopedRedisReply addReply(GetClient()->Command("ZADD %s %s %s", key.c_str(), field.c_str(), newValue.c_str()));
    if (addReply.IsNull() || addReply->type != REDIS_REPLY_INTEGER || reply->integer==0)
        return false;
    return true;
}
