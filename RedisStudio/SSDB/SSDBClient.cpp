#include "StdAfx.h"
#include "SSDBClient.h"
#include <stdarg.h>

#include "../Redis/RedisResult.h"
#include "AbstractSSDBModel.h"

#include "strings.h"


SSDBClient::SSDBClient() : m_Client(NULL),m_isConnected(false)
{  
}


SSDBClient::~SSDBClient()
{
    
}

bool SSDBClient::Connect(const std::string& ip, int port, const std::string& auth)
{
	m_isConnected = false;
	m_Client = ssdb::Client::connect(ip.c_str(), port);
	if (!m_Client) return false;

	if (!auth.empty()) 
	{
		const std::vector<std::string>* rsp = m_Client->request("AUTH", auth);
		ssdb::Status status(rsp);
		if (status.ok()) 
		{
			m_isConnected = true;
		}
		else
		{
			delete m_Client;
			m_Client = NULL;
		}
	} 
	else 
	{
		m_isConnected = true;
	}
    return m_isConnected;
}


bool SSDBClient::IsConnected()
{
    return m_isConnected;
}


bool SSDBClient::Info(std::string& results)
{
    bool retVal = false;

    return retVal;
}

bool SSDBClient::keys(const std::string& matchstr, TSeqArrayResults& results)
{
	int keyNum = 20000;
	std::string key ;
	/// SSDB的key可以重复
	std::vector<std::string> allkeys ;
	m_Client->keys("", "", keyNum, &allkeys);
	for (std::size_t idx=0; idx<allkeys.size(); ++idx)
	{
		key = allkeys[idx] + " ";
		m_Keys[key] = "string"; 
		results.push_back(key);
	}

	const std::vector<std::string>* rsp = m_Client->request("hlist", "", "", str(keyNum));
	ssdb::Status hlistStatus(rsp);
	for (std::size_t idx=1; hlistStatus.ok() && idx<rsp->size(); ++idx)
	{
		key = (*rsp)[idx] + "  ";
		m_Keys[key] = "hash"; 
		results.push_back(key);
	}

	rsp = m_Client->request("zlist", "", "", str(keyNum));
	ssdb::Status zlistStatus(rsp);
	for (std::size_t idx=1; zlistStatus.ok() && idx<rsp->size(); ++idx)
	{
		key = (*rsp)[idx] + "   ";
		m_Keys[key] = "zset"; 
		results.push_back(key);
	}

	rsp = m_Client->request("qlist", "", "", str(keyNum));
	ssdb::Status qlistStatus(rsp);
	for (std::size_t idx=1; qlistStatus.ok() && idx<rsp->size(); ++idx)
	{
		key = (*rsp)[idx] + "    ";
		m_Keys[key] = "queue"; 
		results.push_back(key);
	}
	results.sort();

    return true;
}

bool SSDBClient::Exists(const std::string& key)
{
	return (m_Keys.find(key) != m_Keys.end());
}

bool SSDBClient::Type(const std::string& key, string& type)
{
	type = m_Keys[key];
    return true;
}

long long SSDBClient::TTL(const std::string& key)
{
	long long ttl = 0;
	const std::vector<std::string>* rsp = m_Client->request("ttl", RealKey(key));
	ssdb::Status status(rsp);
	if (status.ok() && rsp->size() >= 2) 
	{
		ttl = str_to_int64(rsp->at(1));
	}
    return ttl;
}

void SSDBClient::Quit()
{
    m_isConnected = false;
}


bool SSDBClient::DatabasesNum(int& num)
{
    return 1;
}

int SSDBClient::DatabasesNum()
{
    return 1;
}

bool SSDBClient::SelectDB(int dbindex )
{
    return true;
}

long long SSDBClient::DbSize() 
{   
    return m_Keys.size();
}

bool SSDBClient::GetConfig(TDicConfig& dicConfig)
{

    return true;
}

bool SSDBClient::SetConfig(const TDicConfig& dicConfig)
{

    return true;
}

bool SSDBClient::ReWriteConfig()
{

    return true;
}

bool SSDBClient::GetData( const std::string& key, std::string& type, RedisResult& results )
{
	if (!Type(key, type)) return false;

	std::string realKey = RealKey(key);

	m_ModelFactory.reset(new SSDBModelFactory(this->m_Client));
	return m_ModelFactory->GetSSDBModel(type)->GetData(realKey, results);
}

bool SSDBClient::DelKey( const std::string& key )
{

    return true;
}

bool SSDBClient::UpdateData( const std::string& key, 
                              const std::string& oldValue, 
                              const std::string& newValue,
                              int idx,
                              const std::string& field)
{
	return true;
}

std::string SSDBClient::RealKey(const std::string& key)
{
	std::string type = m_Keys[key];
	std::string realKey;
	if (type == "string") 
	{
		realKey.append(key.begin(), key.begin()+key.size()-1);
	} 
	else if  (type == "hash")
	{
		realKey.append(key.begin(), key.begin()+key.size()-2);
	}
	else if (type == "zset")
	{
		realKey.append(key.begin(), key.begin()+key.size()-3);
	}
	else if (type == "queue")
	{
		realKey.append(key.begin(), key.begin()+key.size()-4);
	} 
	else 
	{
		realKey = key;
	}
	return realKey;
}
