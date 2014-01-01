#pragma once
#include "hiredis/hiredis.h"
#include <list>
#include <map>
#include <string>
#include "Mutex.h"


typedef void (* callback)(const CDuiString& );


class RedisResult;

class RedisClient
{
public:
	typedef std::list<std::string> TSeqArrayResults;
	typedef std::map<std::string, std::string> TDicConfig;
public:
	RedisClient();

	~RedisClient();

	static RedisClient& GetInstance();

	void SetServerInfo(const CDuiString& name, const string& ip, int port,const string& auth);

	bool Connect();

	bool IsConnected();

	void NeedReConnect();

	void Quit();

	bool Info(std::string& results);

	bool keys(TSeqArrayResults& results);

	bool Type(const std::string& key, string& type);

	bool DatabasesNum(int& num);

	int DatabasesNum();

	bool SelectDB(int dbindex);

	bool GetConfig(TDicConfig& dicConfig);

	bool SetConfig(const TDicConfig& dicConfig);

	bool ReWriteConfig();

	bool GetData(const std::string& key, std::string& type, RedisResult& results);

	bool GetStringData(const std::string& key, RedisResult& result);

	bool GetListData(const std::string& key, RedisResult& results);

	bool GetSetData(const std::string& key, RedisResult& results);

	bool GetZSetData(const std::string& key, RedisResult& results);

	bool GetHashData(const std::string& key, RedisResult& results);

	redisReply* Command(const char* fmt, ...);

	void SetLastError(const std::string& err);

	CDuiString GetLastError();

	CDuiString GetName();
public:
	void SetDisConnectCallback(callback fn)
	{
		m_fnDisConnect = fn;
	}

public:
	redisContext* m_pClient;
	bool          m_bReConnect;
	CDuiString    m_strName;
	string        m_strIP;
	int           m_iPort;
	string        m_strAuth;
	int           m_Databases;
	CDuiString    m_StrErr;
	bool          m_isConnected;
    callback      m_fnDisConnect;
	Mutex         m_mutex;
};