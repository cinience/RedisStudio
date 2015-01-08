#include "StdAfx.h"
#include "SSDBQueueModel.h"
#include "../Redis/RedisResult.h"

SSDBQueueModel::SSDBQueueModel(ssdb::Client* client):AbstractSSDBModel(client)
{

}

bool SSDBQueueModel::GetData( const std::string& key, RedisResult& results )
{
	results.NewColumn("Value");
	std::vector<std::string> values ;
	if (GetClient()->qslice(key, 0, 20000, &values).ok())
	{
		for (std::size_t idx=0; idx<values.size(); ++idx)
		{
			results.NewRow();
			string& myvalue = results.Value(results.RowSize()-1, 0);
			myvalue = values[idx];
		}
	}
	else 
	{
		return false;
	}

    return true;
}

bool SSDBQueueModel::UpdateData(  const std::string& key, 
    const std::string& oldValue, 
    const std::string& newValue,
    int idx,
    const std::string& field)
{

    return true;
}
