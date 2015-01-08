#include "StdAfx.h"
#include "SSDBStringModel.h"
#include "../Redis/RedisResult.h"

SSDBStringModel::SSDBStringModel(ssdb::Client* client ) : AbstractSSDBModel(client)
{

}

bool SSDBStringModel::GetData( const std::string& key, RedisResult& results )
{
	std::string val;
	if (GetClient()->get(key, &val).ok()) 
	{
		results.NewColumn("Value");
		results.NewRow();
		string& myvalue = results.Value(results.RowSize()-1, 0);
		myvalue = val;
		return true;
	}
	else 
	{
		return false;
	}
    return true;
}

bool SSDBStringModel::UpdateData( const std::string& key, 
                                   const std::string& oldValue, 
                                   const std::string& newValue,
                                   int idx,
                                   const std::string& field )
{
    return true;
}

