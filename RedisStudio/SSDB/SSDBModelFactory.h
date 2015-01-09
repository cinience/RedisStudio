#ifndef SSDBModelFactory_INCLUDED
#define SSDBModelFactory_INCLUDED
#include <map>

namespace ssdb {
    class Client;
}
class AbstractSSDBModel;

class SSDBModelFactory
{
public:
    SSDBModelFactory(ssdb::Client* client);

    ~SSDBModelFactory();

    void Register(const std::string& type, AbstractSSDBModel* p);

    void UnregisterModel(const std::string& type);

    AbstractSSDBModel* GetSSDBModel(const std::string& type);
private:
    typedef std::map<std::string, AbstractSSDBModel*> TDicModels;
    TDicModels m_oModels;
};

#endif
