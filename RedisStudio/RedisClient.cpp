#include "StdAfx.h"
#include "RedisClient.h"


#include "stdarg.h"

#include "RedisResult.h"

RedisClient::RedisClient() : m_bReConnect(true),m_isConnected(false),
m_pClient(NULL)
{
	
}


RedisClient::~RedisClient()
{
	
}


RedisClient& RedisClient::GetInstance()
{
	static RedisClient client;
    return client;
}

void RedisClient::SetServerInfo( const CDuiString& name, const string& ip, int port,const string& auth )
{
	m_strName = name;
	m_strIP = ip;
	m_iPort = port;
	m_strAuth = auth;
}

bool RedisClient::Connect()
{
	if (IsConnected())
	{
		Quit();
	}
	if (m_strName.IsEmpty()) return false;
	struct timeval timeout = { 5, 500000 }; // 1.5 seconds 
	m_pClient = redisConnectWithTimeout((char*)m_strIP.c_str(), 
		m_iPort, 
		timeout);
	if (m_pClient->err) {
		printf("Connection error: %s\n", m_pClient->errstr);
		SetLastError(m_pClient->errstr);
		return false;
	}
	if (!m_strAuth.empty())
	{
		redisReply* reply = Command("AUTH %s", m_strAuth.c_str());
		if (!(reply && reply->type==REDIS_REPLY_STATUS))
		{
			return false;
		}
	}
	m_isConnected = true;
	if (m_isConnected && DatabasesNum(m_Databases)) return true;
	return false;
}


bool RedisClient::IsConnected()
{
	//redisReply* reply = Command("ping");
	//if (reply->type == REDIS_REPLY_STATUS)
	//{
	//	return true;
	//}
	//return false;
	return m_isConnected;
}

void RedisClient::NeedReConnect()
{
	m_bReConnect = true;
}


bool RedisClient::Info(std::string& results)
{
	bool retVal = false;

	if (!IsConnected()) return retVal;
	redisReply* reply = Command("info");	
	if (!reply) return retVal;

    if (reply->type == REDIS_REPLY_STRING)
	{
		retVal = true;
		results = reply->str;
	}
	freeReplyObject(reply);

	return retVal;
}

bool RedisClient::keys(TSeqArrayResults& results)
{
	bool retVal = false;

	if (!IsConnected()) return retVal;

	redisReply* reply = Command("KEYS *");	
	if (!reply) return retVal;

	if (reply->type == REDIS_REPLY_ARRAY)
	{
		retVal = true;
		std::size_t i = 0;
		redisReply* tmpReply ;
		while (i < reply->elements)
		{
			tmpReply = reply->element[i];
			results.push_back(tmpReply->str);
			i++;
		}
	}
	freeReplyObject(reply);
	return retVal;
}

bool RedisClient::Type(const std::string& key, string& type)
{
	bool retVal = false;
	redisReply* reply = Command("TYPE %s", key.c_str());
	if (!reply) return retVal;
	if (reply->type == REDIS_REPLY_STATUS)
	{
		type = reply->str;
		retVal = true;
	}
	freeReplyObject(reply);
	return true;
}

void RedisClient::Quit()
{
	Command("quit");
	m_isConnected = false;
}

redisReply* RedisClient::Command( const char* fmt, ... )
{
	if (!m_pClient) NULL;
	Mutex::ScopedLock scopedLock(m_mutex);
	redisReply* reply = NULL;
	va_list ap;
	va_start(ap, fmt);
	reply = (redisReply*) redisvCommand(m_pClient, fmt, ap);
	va_end(ap);
	if (reply==NULL || reply->type==REDIS_REPLY_ERROR)
	{
		m_fnDisConnect(m_strName);
		m_isConnected = false;
	}
	return reply;
}

void RedisClient::SetLastError( const std::string& err )
{
	//m_StrErr = m_StrErr.Append()
}

DuiLib::CDuiString RedisClient::GetLastError()
{
	return m_StrErr;
}

DuiLib::CDuiString RedisClient::GetName()
{
	return m_strName;
}

bool RedisClient::DatabasesNum(int& num)
{
	bool retVal = false;
	redisReply* reply = Command("config get databases");
	if (!reply) return retVal;

	if (reply->type == REDIS_REPLY_ARRAY)
	{
		retVal = true;
		num = atoi(reply->element[1]->str);
	}

	return retVal;
}

int RedisClient::DatabasesNum()
{
	return m_Databases;
}

bool RedisClient::SelectDB( int dbindex )
{
	redisReply* reply = Command("SELECT %d", dbindex);
	if (!reply) return false;
	if (reply->type != REDIS_REPLY_STATUS)
	{
		freeReplyObject(reply);
		return false;
	}
	freeReplyObject(reply);
	return true;
}

bool RedisClient::GetConfig(TDicConfig& dicConfig)
{
	dicConfig.clear();
	redisReply* reply = Command("CONFIG GET *");
	if (!reply) return false;
	if (reply->type != REDIS_REPLY_ARRAY)
	{
		freeReplyObject(reply);
		return false;
	}
	std::size_t i = 0;
	std::string configKey , configValue;
	redisReply* tmpReply ;
	while (i < reply->elements)
	{
		tmpReply = reply->element[i++];
		configKey.assign(tmpReply->str, tmpReply->len);
		tmpReply = reply->element[i++];
		configValue.assign(tmpReply->str, tmpReply->len);
		dicConfig.insert(std::make_pair(configKey, configValue));
	}
	freeReplyObject(reply);
	return true;
}

bool RedisClient::SetConfig(const TDicConfig& dicConfig)
{
	TDicConfig::const_iterator it = dicConfig.begin();
	TDicConfig::const_iterator itend = dicConfig.end();

	for (; it!=itend; ++it)
	{
		std::string key = it->first;
		std::string val = it->second;
		redisReply* reply = Command("CONFIG SET %s %s", key.c_str(), val.c_str());
		if (!reply) return false;
		if (reply->type != REDIS_REPLY_STATUS)
		{
			freeReplyObject(reply);
			return false;
		}
		freeReplyObject(reply);
	}

	return true;
}

bool RedisClient::ReWriteConfig()
{
	redisReply* reply = Command("CONFIG REWRITE");
	if (!reply) return false;

	if (reply->type != REDIS_REPLY_STATUS)
	{
		freeReplyObject(reply);
		return false;
	}
	freeReplyObject(reply);
	return true;
}

bool RedisClient::GetData( const std::string& key, std::string& type, RedisResult& results )
{
	redisReply* reply = NULL;
	if (! Type(key, type)) return false;

	if (type == "none")
	{
		return false;
	}
	
	if (type == "string")
	{
		if (!GetStringData(key, results)) return false;
	} 
	else if (type == "list")
	{
		if (!GetListData(key, results)) return false;
	}
	else if (type == "set")
	{
		if (!GetSetData(key, results)) return false;
	}
	else if (type == "zset")
	{
		if (!GetZSetData(key, results)) return false;
	}
	else if (type == "hash")
	{
		if (!GetHashData(key, results)) return false;
	}
	return true;
}

bool RedisClient::GetStringData( const std::string& key, RedisResult& results)
{
	bool retVal = false;
	redisReply* reply = Command("GET %s", key.c_str());
	if (!reply)  return retVal;
	results.NewColumn("Value");
	if (reply->type == REDIS_REPLY_STRING)
	{
		results.NewRow();
		string& myvalue = results.Value(results.RowSize()-1, 0);
		myvalue.assign(reply->str, reply->len);
		retVal = true;
	}
	return retVal;
}

bool RedisClient::GetListData( const std::string& key, RedisResult& results )
{
	bool retVal = false;
	redisReply* reply = Command("LRANGE %s %d %d", key.c_str(), 0, -1);
	if (!reply)  return retVal;
    results.NewColumn("Value");
	if (reply->type == REDIS_REPLY_ARRAY)
	{
		std::size_t i = 0;
		redisReply* tmpReply ;
		while (i < reply->elements)
		{
			tmpReply = reply->element[i];
			results.NewRow();
			string& myvalue = results.Value(results.RowSize()-1, 0);
			myvalue.assign(tmpReply->str, tmpReply->len);
			i++;
		}
		retVal = true;
	}
	freeReplyObject(reply);

	return retVal;
}

bool RedisClient::GetSetData( const std::string& key, RedisResult& results )
{
	bool retVal = false;
	redisReply* reply = Command("SMEMBERS %s", key.c_str());
	if (!reply)  return retVal;
	results.NewColumn("Value");
	if (reply->type == REDIS_REPLY_ARRAY)
	{
		std::size_t i = 0;
		redisReply* tmpReply ;
		while (i < reply->elements)
		{
			tmpReply = reply->element[i];
			results.NewRow();
			string& myvalue = results.Value(results.RowSize()-1, 0);
			myvalue.assign(tmpReply->str, tmpReply->len);
			i++;
		}
		retVal = true;
		
	}
	freeReplyObject(reply);
	return retVal;
}

bool RedisClient::GetZSetData( const std::string& key, RedisResult& results )
{
	bool retVal = false;
	redisReply* reply = Command("ZREVRANGE %s %d %d WITHSCORES", key.c_str(), 0, -1);
	if (!reply)  return retVal;
	
	results.NewColumn("Score");
	results.NewColumn("Value");
	if (reply->type == REDIS_REPLY_ARRAY)
	{
		std::size_t i = 0;
		redisReply* tmpReply ;
		while (i < reply->elements)
		{
			tmpReply = reply->element[i];
			if (i%2 == 0)
			{
				results.NewRow();
				
				string& myvalue = results.Value(results.RowSize()-1, 1);
				myvalue.assign(tmpReply->str, tmpReply->len);
			}
			else 
			{
				string& myvalue = results.Value(results.RowSize()-1, 0);
				myvalue.assign(tmpReply->str, tmpReply->len);
			}			
			i++;
		}
		retVal = true;
	}
	freeReplyObject(reply);
	return retVal;
}

bool RedisClient::GetHashData( const std::string& key, RedisResult& results )
{
	bool retVal = false;
	redisReply* reply = Command("HKEYS %s", key.c_str());
	if (!reply)  return retVal;
	results.NewColumn("Field ");
	results.NewColumn("Value");
	if (reply->type == REDIS_REPLY_ARRAY)
	{
		std::size_t i = 0;
		redisReply* tmpReply ;
		bool isOK = true;
		while (i < reply->elements)
		{
			tmpReply = reply->element[i];
			redisReply* theReply = Command("HGET %s %s", key.c_str(), tmpReply->str);
			if (!theReply) break;
			if (theReply->type != REDIS_REPLY_STRING) 
			{
				isOK = false;
				freeReplyObject(theReply);
				break;
			}
			results.NewRow();

			string& myFiled = results.Value(results.RowSize()-1, 0) ;
			myFiled.assign(tmpReply->str, tmpReply->len);

			string& myvalue = results.Value(results.RowSize()-1, 1) ;
			myvalue.assign(theReply->str, theReply->len);
			freeReplyObject(theReply);
			i++;
		}
		if (isOK)
		retVal = true;
	}
	freeReplyObject(reply);
	return retVal;
}
