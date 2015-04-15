#include "StdAfx.h"
#include "RedisListModel.h"

RedisListModel::RedisListModel( RedisClient* client ) : AbstractRedisModel(client)
{

}

bool RedisListModel::GetData( const std::string& key, RedisResult& results )
{
    bool retVal = false;
    ScopedRedisReply lenreply = GetClient()->Command("LLEN %s", key.c_str());
    if (lenreply.IsNull())
    {
        return false;
    }
    if (lenreply->type == REDIS_REPLY_INTEGER)
    {
         long long llen = lenreply->integer;
		 results.NewColumn("Value");
         for (int idx=0; idx<llen; )
         {
             redisReply* reply = GetClient()->Command("LRANGE %s %d %d", key.c_str(), idx, idx+1000 > llen ? idx+1000 : llen - idx);
             if (!reply)  return retVal;
            
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
             idx+=1000;
         }
    }

    return retVal;
}

bool RedisListModel::UpdateData(  const std::string& key, 
    const std::string& oldValue, 
    const std::string& newValue,
    int idx,
    const std::string& field)
{
    ScopedRedisReply reply(GetClient()->Command("LSET %s %d %s", key.c_str(), idx, newValue.c_str()));

    if (reply.IsNull() || reply->type != REDIS_REPLY_STATUS)  return false;
    return true;
}
