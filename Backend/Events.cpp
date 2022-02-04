#include "Globals.h"
bool is_empty2(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}
void C_Events::OnAccountCreation(C_Client& Client, C_Message& Message)
{
	auto strUsername = Message.GetMsgBlank("Username");
	auto strPassword = Message.GetMsgBlank("Password");
	auto strHwid = Message.GetMsgBlank("Hwid");

	auto strAddress = Client.m_FullAddress;

	std::string strUserFileName = "ServerData/Users/UserList/";
	strUserFileName += strUsername;
	strUserFileName += ".usr";

	if (!g_Tools->IsValidUser(strUserFileName))
	{
		g_Tools->SendNetMessage(Client, C_Message(RESULT_FAIL, "Your nickname is already used here."));

		std::string strSomething = strAddress;
		strSomething += " got the Name error. ( Account Creation )";
		strSomething += strUsername;
		LOG(strSomething.c_str());

		return;
	}

	std::string strResult = g_Tools->IsValidUsernameAndPassword(strUsername, strPassword);
	if (strResult != "OK")
	{
		std::string strSomething = strAddress;
		strSomething += " gave invalid credentials. ( Account Creation )";
		LOG(strSomething.c_str());
		LOG(strResult.c_str());

		g_Tools->SendNetMessage(Client, C_Message(RESULT_FAIL, strResult));
		return;
	}

	nlohmann::json jUserFile;
	jUserFile["Data"]["Name"] = strUsername;
	jUserFile["Data"]["Password"] = strPassword;
	jUserFile["Data"]["Hwid"] = strHwid;
	jUserFile["Data"]["Token"] = g_Crypt->base64_encode(strHwid);
	jUserFile["Data"]["Last IP"] = Client.m_FullAddress;
	jUserFile["Data"]["Last Time"] = Date();
	jUserFile["Data"]["Registration date"] = Date();
	jUserFile["Data"]["Registration IP"] = Client.m_FullAddress;
	jUserFile["Data"]["Group"] = "User";

	std::string userdata = "ServerData/Users/UserData/data_" + strUsername + "/";
	CreateDirectory(userdata.c_str(), NULL);
	CreateDirectory("ServerData/Users/Hwid/", NULL);

	std::ofstream pNewFile(strUserFileName, std::ios::out | std::ios::trunc);
	pNewFile.clear();
	pNewFile << jUserFile.dump(4).c_str();
	pNewFile.close();

	nlohmann::json jHWID;
	jHWID["Hwid"][strUsername] = strHwid;

	std::ofstream pHWIDFileOut("ServerData/Users/Hwid/hwid.hwd", std::ios::out | std::ios::trunc);
	pHWIDFileOut << jHWID.dump(4);
	pHWIDFileOut.close();

	std::string strSomething = strAddress;
	strSomething += " has created an account as ";
	strSomething += strUsername;
	LOG(strSomething.c_str());

	g_Tools->SendNetMessage(Client, C_Message(RESULT_OK, "Account was created."));
}

void C_Events::OnAccountLoginToken(C_Client& Client, C_Message& jMessage)
{
	if (Client.m_bAuthenticated)
	{
		g_Tools->SendNetMessage(Client, C_Message(RESULT_FAIL, "You are already logged in."));
		LOG("You are already logged in.");
		return;
	}

	std::string strHwid = jMessage.GetMsgBlank("Hwid");
	std::string strToken = jMessage.GetMsgBlank("UserToken");

	std::string strUsername = g_Tools->GetUsernameFromHWID(strHwid);
	std::string strResult = g_Tools->CheckUserToken(strToken, strHwid);
	if (strResult != "OK")
	{
		g_Tools->SendNetMessage(Client, C_Message(RESULT_FAIL, strResult));
		LOG(strResult.c_str());
		return;
	}

	Client.m_bAuthenticated = true;
	Client.m_iAuthMode = 0;
	Client.m_Username = strUsername;

	nlohmann::json answer;
	answer["Data"]["Text"] = strUsername;
	g_Tools->SendNetMessage(Client, answer.dump(4));

	std::ifstream fFile("ServerData/Users/UserList/" + strUsername + ".usr", std::ios::in);

	nlohmann::json jUserfile;
	fFile >> jUserfile;
	fFile.close();

	Client.m_bIsAdmin = jUserfile["Data"]["Group"].get< std::string >() == "Administrator";
	jUserfile["Data"]["Last IP"] = Client.m_Address;
	jUserfile["Data"]["Last Time"] = Date();

	std::ofstream pHWIDFileOut("ServerData/Users/UserList/" + strUsername + ".usr", std::ios::out | std::ios::trunc);
	pHWIDFileOut << jUserfile.dump(4);
	pHWIDFileOut.close();

	std::string strSomething = Client.m_FullAddress;
	strSomething += " logged in as ";
	strSomething += strUsername;
	LOG(strSomething.c_str());
}

void C_Events::OnAccountLogin(C_Client& Client, C_Message& jMessage)
{
	if (Client.m_bAuthenticated)
	{
		g_Tools->SendNetMessage(Client, C_Message(RESULT_FAIL, "You are already logged in."));
		LOG("You are already logged in.");
		return;
	}

	std::string strUsername = jMessage.GetMsgBlank("Username");
	std::string strPassword = jMessage.GetMsgBlank("Password");
	std::string strHwid = jMessage.GetMsgBlank("Hwid");

	std::string strResult = g_Tools->CheckUser(strUsername, strPassword);
	if (strResult != "OK")
	{
		g_Tools->SendNetMessage(Client, C_Message(RESULT_FAIL, strResult));
		LOG(strResult.c_str());
		return;
	}

	Client.m_bAuthenticated = true;
	Client.m_iAuthMode = 0;
	Client.m_Username = strUsername;

	nlohmann::json answer;
	std::string token = strHwid;
	answer["Data"]["Token"] = g_Crypt->base64_encode(token);
	answer["Data"]["Hwid"] = strHwid;

	g_Tools->SendNetMessage(Client, answer.dump(4));

	std::ifstream fFile("ServerData/Users/UserList/" + strUsername + ".usr", std::ios::in);

	nlohmann::json jUserfile;
	fFile >> jUserfile;
	fFile.close();

	Client.m_bIsAdmin = jUserfile["Data"]["Group"].get< std::string >() == "Administrator";
	jUserfile["Data"]["Last IP"] = Client.m_Address;
	jUserfile["Data"]["Last Time"] = Date();

	std::ofstream pHWIDFileOut("ServerData/Users/UserList/" + strUsername + ".usr", std::ios::out | std::ios::trunc);
	pHWIDFileOut << jUserfile.dump(4);
	pHWIDFileOut.close();

	std::string strSomething = Client.m_FullAddress;
	strSomething += " logged in as ";
	strSomething += strUsername;
	LOG(strSomething.c_str());
}

void C_Events::OnPushAnimeList(C_Client& Client, C_Message& Message)
{
	nlohmann::json ListCfg;
	auto strUsername = Message.GetMsgBlank("Username");
	auto strAddress = Client.m_FullAddress;

	if (!Client.m_bAuthenticated)
	{
		nlohmann::json answer;
		answer["Data"]["Result"] = "Denied";
		answer["Data"]["Reason"] = "Client doesnt authenticated";
		g_Tools->SendNetMessage(Client, C_Message(RESULT_OK, answer.dump(4)));
		LOG("Client.m_bAuthenticated false");
		return;
	}

	std::string strUserFileName = "ServerData/Users/UserData/AllAnimeList.cfg";
	std::ifstream in(strUserFileName);

	if (is_empty2(in))
	{
		g_Tools->SendNetMessage(Client, C_Message(RESULT_OK, "Failed to open file"));
		LOG("Failed to open file");
	}

	if (in.is_open())
	{
		in >> ListCfg;
	}
	in.close();

	std::string jAnswer = ListCfg.dump(4);

	std::string finallog = "Send AllAnimeList.cfg from: ";
	finallog += strUserFileName;
	finallog += " | to ";
	finallog += strUsername;

	LOG(finallog.c_str());
	g_Tools->SendNetMessage(Client, jAnswer);
}