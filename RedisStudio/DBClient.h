#pragma once

#include <string>
#include <list>
#include <map>

class RedisResult;

class DBClient {
public:
	typedef std::list<std::string> TSeqArrayResults;
	typedef std::map<std::string, std::string> TDicConfig;
public:
	static DBClient* Create(const std::string& ip, int port, const std::string& auth);

	std::string DBType();

	virtual bool Connect(const std::string& ip, int port, const std::string& auth) = 0;

	virtual bool IsConnected() = 0;

	virtual void Quit() = 0;

	virtual bool Info(std::string& results) = 0;

	virtual bool keys(const std::string &, TSeqArrayResults& results) = 0;

	virtual bool Exists(const std::string& key) = 0;

	virtual bool Type(const std::string& key, string& type) = 0;

	virtual long long TTL(const std::string& key) = 0;

	virtual bool DatabasesNum(int& num);

	virtual int  DatabasesNum();

	virtual bool SelectDB(int dbindex);

	virtual long long DbSize() = 0;

	virtual bool GetConfig(TDicConfig& dicConfig);

	virtual bool SetConfig(const TDicConfig& dicConfig);

	virtual bool ReWriteConfig();

	virtual bool GetData(const std::string& key, std::string& type, RedisResult& results) = 0;

	virtual bool UpdateData(const std::string& key, 
		const std::string& oldValue, 
		const std::string& newValue,
		int idx,
		const std::string& field="") = 0;

	virtual bool DelKey(const std::string& key) = 0;

	void SetLastError(const std::string& err);

	std::string GetLastError();
protected:
	std::string m_IP;
	int         m_Port;
	std::string m_Auth;
private:
	std::string m_Error;

};