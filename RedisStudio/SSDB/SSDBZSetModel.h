#ifndef SSDBZSetModel_INCLUDED
#define SSDBZSetModel_INCLUDED

#include "AbstractSSDBModel.h"

class SSDBZSetModel : public AbstractSSDBModel
{
public:
    SSDBZSetModel(ssdb::Client* client);

    virtual bool GetData(const std::string& key, RedisResult& results);

    virtual bool UpdateData(const std::string& key, 
        const std::string& oldValue,  
        const std::string& newValue,
        int idx,
        const std::string& field="");
};

#endif
