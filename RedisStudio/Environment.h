#pragma once

class DBClient;

class Environment {
public:
	Environment();

	void SetDBClient(DBClient* cli);

	void SetDBServerInfo(const std::string& addr, int port, const std::string& auth="");

	void SetDBName(const CDuiString& name);

	DBClient* GetDBClient();
	
	CDuiString GetDBName();

	std::string GetDBIP();

	int GetDBPort();

	std::string GetDBPasswd();

private:
	CDuiString m_DbName;
	std::string m_sIP;
	int         m_iPort;
	std::string m_sAuth;
	DBClient*  m_DbClient;
};
