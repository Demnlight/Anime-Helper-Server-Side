#pragma once

#include <WS2tcpip.h>
#include <string>

#include "Helpers/Logs.hpp"
#include "Helpers/Json.hpp"
#include "Helpers/Message.hpp"
#include "Helpers/base64.h"

#pragma comment( lib, "ws2_32.lib")

struct C_Client
{
	bool m_bAuthenticated;
	bool m_bIsAdmin;
	bool m_bHasSubscription;

	std::string m_Username;
	std::string m_Token;
	std::string m_HWID;
	std::string m_FullAddress;
	char m_Address[64];

	SOCKET m_Socket;
	sockaddr_in m_SocketAddress;

	int32_t m_iAuthMode;
	int32_t m_iClientIndex;

	void SetUserData(std::string strName, std::string strValue)
	{
		std::ifstream pUserfileIn("ServerData/Users/UserList/" + m_Username + ".usr", std::ios::in);

		nlohmann::json jUserfile;
		pUserfileIn >> jUserfile;
		pUserfileIn.close();

		jUserfile["Data"][strName] = strValue;

		std::ofstream pUserfileOut("ServerData/Users/UserList/" + m_Username + ".usr", std::ios::out | std::ios::trunc);
		pUserfileOut << jUserfile.dump(4);
		pUserfileOut.close();
	}

	void AddUserData(std::string strData, std::string strName, std::string strValue)
	{
		std::ifstream pUserfileIn("ServerData/Users/UserList/" + m_Username + ".usr", std::ios::in);

		nlohmann::json jUserfile;
		pUserfileIn >> jUserfile;
		pUserfileIn.close();

		jUserfile["Data"][strData][strName] = strValue;

		std::ofstream pUserfileOut("ServerData/Users/UserList/" + m_Username + ".usr", std::ios::out | std::ios::trunc);
		pUserfileOut << jUserfile.dump(4);
		pUserfileOut.close();
	}
};

class C_Data
{
public:
	SOCKET m_ServerSocket;
	sockaddr_in m_SocketAddress;

	std::vector< C_Client > m_Clients;
};

class C_Globals
{
public:
	void ReceiveClients();
	void HandleClients();
	void HandleSubs();
};

class C_Events
{
public:
	void OnAccountCreation(C_Client& Client, C_Message& Message);
	void OnAccountLogin(C_Client& Client, C_Message& Message);
	void OnAccountLoginToken(C_Client& Client, C_Message& Message);
	void OnPushAnimeList(C_Client& Client, C_Message& Message);
	void OnSaveAnimeList(C_Client& Client, C_Message& Message);
};
class C_Tools
{
public:
	bool IsUserHaveSubscription(std::string strName);
	std::string GetUsernameFromHWID(std::string strHWID);
	void KickClient(C_Client& Client);
	bool SendNetMessage(C_Client& Client, C_Message jMessage);
	bool SendNetMessage(C_Client& Client, std::string jMessage);
	bool IsValidUser(std::string strUsername);
	std::string CheckUser(std::string strUsername, std::string strPassword);
	std::string CheckUserToken(std::string strToken, std::string strHwid);
	std::string GetTokenByHwid(std::string strHwid);
	bool IsValidNewHWID(std::string strHWID);
	std::string IsValidUsernameAndPassword(std::string strName, std::string strPassword);
	std::string GetHWID();
};

inline C_Tools* g_Tools = new C_Tools();
inline C_Events* g_Events = new C_Events();
inline C_Data* g_Data = new C_Data();
inline C_Globals* g_Globals = new C_Globals();
