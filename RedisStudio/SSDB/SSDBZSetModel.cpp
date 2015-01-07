#include "StdAfx.h"
#include "SSDBZSetModel.h"

SSDBZSetModel::SSDBZSetModel(ssdb::Client* client ) : AbstractSSDBModel(client)
{

}

bool SSDBZSetModel::GetData( const std::string& key, RedisResult& results )
{

	
    return true;
}

bool SSDBZSetModel::UpdateData(const std::string& key, 
                                const std::string& oldValue, 
                                const std::string& newValue,
                                int idx,
                                const std::string& field)
{
    return true;
}
