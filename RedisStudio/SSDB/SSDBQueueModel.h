#ifndef SSDBQueueModel_INCLUDED
#define SSDBQueueModel_INCLUDED

#include "AbstractSSDBModel.h"

class SSDBQueueModel : public AbstractSSDBModel
{
public:
    SSDBQueueModel(ssdb::Client* client);

    virtual bool GetData(const std::string& key, RedisResult& results);

    virtual bool UpdateData(const std::string& key, 
        const std::string& oldValue,  
        const std::string& newValue,
        int idx,
        const std::string& field="");
};

#endif
