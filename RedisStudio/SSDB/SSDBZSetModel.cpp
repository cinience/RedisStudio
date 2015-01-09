#include "StdAfx.h"
#include "SSDBZSetModel.h"
#include "strings.h"
#include "../Redis/RedisResult.h"

SSDBZSetModel::SSDBZSetModel(ssdb::Client* client ) : AbstractSSDBModel(client)
{

}

bool SSDBZSetModel::GetData( const std::string& key, RedisResult& results )
{
    results.NewColumn("Score");
    results.NewColumn("Value");
    std::vector<std::string> values ;
    if (GetClient()->zrange(key, 0, 20000, &values).ok())
    {
        for (std::size_t idx=0; idx<values.size();)
        {
            int64_t score = 0;
            GetClient()->zget(key, values[idx], &score);
            results.NewRow();
            std::string& scorestr = results.Value(results.RowSize()-1, 0);
            std::string& value = results.Value(results.RowSize()-1, 1);			
            value = values[idx++];
            scorestr = values[idx++];
        }
    }
    else 
    {
        return false;
    }
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
