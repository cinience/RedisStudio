#include "StdAfx.h"
#include "RedisHashModel.h"

RedisHashModel::RedisHashModel( RedisClient* client ) : AbstractRedisModel(client)
{

}

bool RedisHashModel::GetData( const std::string& key, RedisResult& results )
{
    bool retVal = false;
    redisReply* reply = GetClient()->Command("HKEYS %s", key.c_str());
    if (!reply)  return retVal;
    results.NewColumn("Field ");
    results.NewColumn("Value");
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        std::size_t i = 0;
        redisReply* tmpReply ;
        bool isOK = true;
        while (i < reply->elements)
        {
            tmpReply = reply->element[i];
            redisReply* theReply = GetClient()->Command("HGET %s %s", key.c_str(), tmpReply->str);
            if (!theReply) break;
            if (theReply->type != REDIS_REPLY_STRING) 
            {
                isOK = false;
                freeReplyObject(theReply);
                break;
            }
            results.NewRow();

            string& myFiled = results.Value(results.RowSize()-1, 0) ;
            myFiled.assign(tmpReply->str, tmpReply->len);

            string& myvalue = results.Value(results.RowSize()-1, 1) ;
            myvalue.assign(theReply->str, theReply->len);
            freeReplyObject(theReply);
            i++;
        }
        if (isOK)
            retVal = true;
    }
    freeReplyObject(reply);
    return retVal;
}

bool RedisHashModel::UpdateData( const std::string& key, 
                                 const std::string& oldValue, 
                                 const std::string& newValue,
                                 int idx,
                                 const std::string& field)
{
    /// 如果哈希表中域 field 已经存在且旧值已被新值覆盖，返回 0 。
    ScopedRedisReply reply(GetClient()->Command("HSET %s %s %s", key.c_str(), field.c_str(), newValue.c_str()));
    if(reply.IsNull() || reply->type != REDIS_REPLY_INTEGER || reply->integer!=0) 
        return false;
    return true;
}
