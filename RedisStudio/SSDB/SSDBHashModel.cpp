#include "StdAfx.h"
#include "SSDBHashModel.h"
#include "../Redis/RedisResult.h"

SSDBHashModel::SSDBHashModel(ssdb::Client* client ) : AbstractSSDBModel(client)
{

}

bool SSDBHashModel::GetData( const std::string& key, RedisResult& results )
{
	std::vector<std::string> hmkeys ;
	if (GetClient()->hkeys(key, "", "", 200000, &hmkeys).error())
	{
		return false;
	}
	results.NewColumn("Field ");
	results.NewColumn("Value");
	std::string val;
	for (std::size_t idx=0; idx<hmkeys.size(); ++idx)
	{
		if (GetClient()->hget(key, hmkeys[idx], &val).error())
		{
			return false;
		}
		results.NewRow();
		std::string& field = results.Value(results.RowSize()-1, 0);
		std::string& value = results.Value(results.RowSize()-1, 1);
		field = hmkeys[idx];
		value = val;		
	}
	return true;
}

bool SSDBHashModel::UpdateData( const std::string& key, 
                                 const std::string& oldValue, 
                                 const std::string& newValue,
                                 int idx,
                                 const std::string& field)
{
    
    return true;
}
