#include "StdAfx.h"
#include "SSDBQueueModel.h"

SSDBQueueModel::SSDBQueueModel(ssdb::Client* client):AbstractSSDBModel(client)
{

}

bool SSDBQueueModel::GetData( const std::string& key, RedisResult& results )
{
    bool retVal = false;


    return retVal;
}

bool SSDBQueueModel::UpdateData(  const std::string& key, 
    const std::string& oldValue, 
    const std::string& newValue,
    int idx,
    const std::string& field)
{

    return true;
}
