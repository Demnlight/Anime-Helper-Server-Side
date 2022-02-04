#include "Globals.h"

void C_Tools::KickClient(C_Client& Client)
{
	std::string strLogMsg = "";
	if (Client.m_bAuthenticated)
	{
		strLogMsg += Client.m_Username;
		strLogMsg += " ( ";
	}
	strLogMsg += Client.m_FullAddress;
	if (Client.m_bAuthenticated)
	{
		strLogMsg += " )";
	}

	strLogMsg += " disconnected from the server";
	LOG(strLogMsg.c_str());
	closesocket(Client.m_Socket);

	g_Data->m_Clients.erase(g_Data->m_Clients.begin() + Client.m_iClientIndex);

	for (int32_t iClient = 0; iClient < g_Data->m_Clients.size(); iClient++)
	{
		if (iClient < Client.m_iClientIndex)
			continue;

		g_Data->m_Clients[iClient].m_iClientIndex--;
	}
}

bool C_Tools::IsUserHaveSubscription(std::string strName)
{
	std::ifstream pUserfileIn("ServerData/Users/UserList/" + strName + ".usr", std::ios::in);

	nlohmann::json jUserfile;
	pUserfileIn >> jUserfile;
	pUserfileIn.close();

	return jUserfile["Data"]["Sub-key"] != "";
}
bool C_Tools::SendNetMessage(C_Client& Client, C_Message jMessage)
{
	std::string strDump = jMessage.GetJSON().dump(4);
	int32_t iLength = strDump.length();
	return send(Client.m_Socket, strDump.c_str(), iLength, 0) > 0;
}
bool C_Tools::SendNetMessage(C_Client& Client, std::string jMessage)
{
	std::string strDump = jMessage;
	int32_t iLength = strDump.length();
	return send(Client.m_Socket, strDump.c_str(), iLength, 0) > 0;
}
bool C_Tools::IsValidUser(std::string strUsername)
{
	std::ifstream fFile(strUsername, std::ios::in);
	if (fFile.good())
		return false;

	return true;
}
bool C_Tools::IsValidNewHWID(std::string strHWID)
{
	std::ifstream fFile("ServerData/Users/hwid/hwid.hwd", std::ios::in);
	if (!fFile.good())
		return false;

	nlohmann::json jHWIDs;
	fFile >> jHWIDs;
	fFile.close();

	for (auto& jObject : jHWIDs.get< nlohmann::json::object_t >())
	{
		if (!jObject.second.is_object())
			continue;

		for (auto& jHWID : jObject.second.get< nlohmann::json::object_t >())
		{
			if (jHWID.second.get< std::string >() != strHWID)
				continue;

			return false;
		}
	}

	return true;
}

std::string C_Tools::CheckUser(std::string strUsername, std::string strPassword)
{
	std::ifstream fFile("ServerData/Users/UserList/" + strUsername + ".usr", std::ios::in);
	if (!fFile.good())
		return "User not found.";

	nlohmann::json jUserfile;
	fFile >> jUserfile;
	fFile.close();

	if (jUserfile["Data"]["Name"].get< std::string >() != strUsername)
		return "User not found.";

	if (jUserfile["Data"]["Password"].get< std::string >() != strPassword)
		return "Password incorrect.";

	return "OK";
}
std::string C_Tools::IsValidUsernameAndPassword(std::string strName, std::string strPassword)
{
	if (strName.length() < 3)
		return "You should enter atleast 3 symbols in your nickname.";

	if (strPassword.length() < 6)
		return "You should enter atleast 6 symbols in your password.";

	if (strstr(strPassword.c_str(), strName.c_str()))
		return "Name should have a difference from password.";

	if (
		strstr(strName.c_str(), "/") ||
		strstr(strName.c_str(), "\n") ||
		strstr(strName.c_str(), "|") ||
		strstr(strName.c_str(), ",") ||
		strstr(strName.c_str(), "!") ||
		strstr(strName.c_str(), "@") ||
		strstr(strName.c_str(), "#") ||
		strstr(strName.c_str(), "$") ||
		strstr(strName.c_str(), "%") ||
		strstr(strName.c_str(), "^") ||
		strstr(strName.c_str(), "&") ||
		strstr(strName.c_str(), "*") ||
		strstr(strName.c_str(), "(") ||
		strstr(strName.c_str(), "{") ||
		strstr(strName.c_str(), "}") ||
		strstr(strName.c_str(), ")") ||
		strstr(strName.c_str(), "-") ||
		strstr(strName.c_str(), "=") ||
		strstr(strName.c_str(), "+") ||
		strstr(strName.c_str(), "¹") ||
		strstr(strName.c_str(), ";") ||
		strstr(strName.c_str(), ":") ||
		strstr(strName.c_str(), "?") ||
		strstr(strName.c_str(), "'") ||
		strstr(strName.c_str(), " "))
		return "Name has an invalid symbols.";

	if (
		strstr(strPassword.c_str(), ".") ||
		strstr(strPassword.c_str(), "/") ||
		strstr(strPassword.c_str(), "\n") ||
		strstr(strPassword.c_str(), "|") ||
		strstr(strPassword.c_str(), ",") ||
		strstr(strPassword.c_str(), "!") ||
		strstr(strPassword.c_str(), "@") ||
		strstr(strPassword.c_str(), "#") ||
		strstr(strPassword.c_str(), "$") ||
		strstr(strPassword.c_str(), "%") ||
		strstr(strPassword.c_str(), "^") ||
		strstr(strPassword.c_str(), "&") ||
		strstr(strPassword.c_str(), "*") ||
		strstr(strPassword.c_str(), "(") ||
		strstr(strPassword.c_str(), ")") ||
		strstr(strPassword.c_str(), "-") ||
		strstr(strPassword.c_str(), "=") ||
		strstr(strPassword.c_str(), "+") ||
		strstr(strPassword.c_str(), "¹") ||
		strstr(strPassword.c_str(), ";") ||
		strstr(strPassword.c_str(), ":") ||
		strstr(strPassword.c_str(), "?") ||
		strstr(strPassword.c_str(), "'") ||
		strstr(strPassword.c_str(), " "))
		return "Password has an invalid symbols.";
	else if (strstr(strPassword.c_str(), "123") || strstr(strPassword.c_str(), "111") || strstr(strPassword.c_str(), "321"))
		return "Password must have no 123/321/111 combinations.";

	return "OK";
}
std::string C_Tools::GetUsernameFromHWID(std::string strHWID)
{
	std::ifstream pHWIDFileIn("ServerData/Users/hwid/Hwid.hwd", std::ios::in);

	nlohmann::json jHwidfile;
	pHWIDFileIn >> jHwidfile;
	pHWIDFileIn.close();

	std::string strUsername = "";
	for (auto& jUser : jHwidfile["HWID"].get< nlohmann::json::object_t >())
	{
		if (jUser.second.get< std::string >() != strHWID)
			continue;

		strUsername = jUser.first;
		break;
	}

	return strUsername;
}