#include "stdafx.h"
#include "DBClient.h"

#include "Redis/RedisClient.h"
#include "SSDB/SSDBClient.h"

DBClient* DBClient::Create(const std::string& ip, int port, const std::string& auth)
{
	std::string info;
	DBClient* dbcli = new RedisClient();
	dbcli->Connect(ip, port, auth);
	if (!dbcli->IsConnected()) {
		delete dbcli;
		return NULL;
	}
	if (!dbcli->Info(info)) {
		delete dbcli;
		return NULL;
	}
	if (info.empty()) {
		delete dbcli;
		dbcli = new SSDBClient();
		if (!dbcli->Connect(ip, port, auth)) 
		{
			delete dbcli;
			dbcli = NULL;
		}
	}
	return dbcli;
#if 0
	ssdb::Client* cli = ssdb::Client::connect(ip.c_str(), port);
	const std::vector<std::string>* rsp = cli->request("info");
	std::vector<std::string> keys;
	cli->hkeys("sys:blacklist:net:ip", "","", 100, &keys);
#endif
}

bool DBClient::DatabasesNum(int& num)
{
	num = 1;
	return true;
}

int DBClient::DatabasesNum()
{
	return 1;
}

bool DBClient::SelectDB(int dbindex)
{
	return true;
}

bool DBClient::GetConfig(TDicConfig& dicConfig)
{
	return false;
}

bool DBClient::SetConfig(const TDicConfig& dicConfig)
{
	return false;
}

bool DBClient::ReWriteConfig()
{
	return false;
}

void DBClient::SetLastError(const std::string& err)
{
	m_Error = err;
}

std::string DBClient::GetLastError()
{
	return m_Error;
}

