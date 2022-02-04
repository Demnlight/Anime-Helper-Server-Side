#include "Globals.h"
bool is_empty1(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}
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

std::string C_Tools::CheckUserToken(std::string strToken, std::string strHwid)
{
	std::string strUsername = g_Tools->GetUsernameFromHWID(strHwid);
	std::ifstream fFile("ServerData/Users/UserList/" + strUsername + ".usr", std::ios::in);
	if (!fFile.good())
		return "User not found.";

	nlohmann::json jUserfile;
	fFile >> jUserfile;
	fFile.close();

	if (jUserfile["Data"]["Name"].get< std::string >() != strUsername)
		return "User not found.";

	if (jUserfile["Data"]["Hwid"].get< std::string >() != strHwid)
		return "Hwid incorrect.";

	if (jUserfile["Data"]["Token"].get< std::string >() != strToken)
		return "Token incorrect.";

	return "OK";
}


std::string C_Tools::GetTokenByHwid(std::string strHwid)
{
	std::string strUsername = g_Tools->GetUsernameFromHWID(strHwid);
	std::ifstream fFile("ServerData/Users/UserList/" + strUsername + ".usr", std::ios::in);
	if (!fFile.good())
		return "User not found.";

	nlohmann::json jUserfile;
	fFile >> jUserfile;
	fFile.close();

	return jUserfile["Data"]["Token"].get< std::string >();
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
	std::ifstream pHWIDFileIn("ServerData/Users/Hwid/Hwid.hwd", std::ios::in);
	if (!pHWIDFileIn.is_open())
		return "failed";
	if (is_empty1(pHWIDFileIn))
		return "failed";

	nlohmann::json jHwidfile;
	pHWIDFileIn >> jHwidfile;
	pHWIDFileIn.close();

	std::string strUsername = "";
	for (auto& jUser : jHwidfile["Hwid"].get< nlohmann::json::object_t >())
	{
		if (jUser.second.get< std::string >() != strHWID)
			continue;

		strUsername = jUser.first;
		break;
	}

	return strUsername;
}

std::string C_Tools::GetHWID()
{
	HANDLE hFile = CreateFileA(("\\\\.\\PhysicalDrive0"), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return { };

	std::unique_ptr< std::remove_pointer <HANDLE >::type, void(*)(HANDLE) > hDevice
	{
		hFile, [](HANDLE handle)
		{
			CloseHandle(handle);
		}
	};

	STORAGE_PROPERTY_QUERY PropertyQuery;
	PropertyQuery.PropertyId = StorageDeviceProperty;
	PropertyQuery.QueryType = PropertyStandardQuery;

	STORAGE_DESCRIPTOR_HEADER DescHeader;
	DWORD dwBytesReturned = 0;
	if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &PropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
		&DescHeader, sizeof(STORAGE_DESCRIPTOR_HEADER), &dwBytesReturned, NULL))
		return { };

	const DWORD dwOutBufferSize = DescHeader.Size;
	std::unique_ptr< BYTE[] > pOutBuffer{ new BYTE[dwOutBufferSize] { } };

	if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &PropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
		pOutBuffer.get(), dwOutBufferSize, &dwBytesReturned, NULL))
		return { };

	STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(pOutBuffer.get());
	if (!pDeviceDescriptor)
		return { };

	const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
	if (!dwSerialNumberOffset)
		return { };

	std::string sResult = reinterpret_cast<const char*>(pOutBuffer.get() + dwSerialNumberOffset);
	sResult.erase(std::remove_if(sResult.begin(), sResult.end(), std::isspace), sResult.end());
	return sResult;
}