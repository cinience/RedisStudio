#ifndef SSDBStringModel_INCLUDED
#define SSDBStringModel_INCLUDED

#include "AbstractSSDBModel.h"

class SSDBStringModel : public AbstractSSDBModel
{
public:
    SSDBStringModel(ssdb::Client* client);

    virtual bool GetData(const std::string& key, RedisResult& results);

    virtual bool UpdateData(const std::string& key, 
        const std::string& oldValue,  
        const std::string& newValue,
        int idx,
        const std::string& field="");
};

#endif
