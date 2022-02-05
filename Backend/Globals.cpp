#include "Globals.h"

void C_Globals::ReceiveClients()
{
    LOG("Awaiting new clients... ");
    while (true)
    {
        Sleep(50);

        sockaddr_in pSocketAddress;
        int32_t iClientSize = sizeof(pSocketAddress);

        SOCKET pClientSocket = accept(g_Data->m_ServerSocket, (sockaddr*)(&pSocketAddress), &iClientSize);
        if (!pClientSocket)
        {
            LOG("Non accepted");
            continue;
        }

        C_Client Client;
        Client.m_bAuthenticated = false;
        Client.m_bIsAdmin = false;
        Client.m_Username = "";
        Client.m_HWID = "";
        Client.m_Socket = pClientSocket;
        Client.m_SocketAddress = pSocketAddress;
        Client.m_iClientIndex = g_Data->m_Clients.size();

        inet_ntop(AF_INET, &Client.m_SocketAddress.sin_addr, Client.m_Address, 64);
        Client.m_FullAddress += Client.m_Address;
        Client.m_FullAddress += ":" + std::to_string(ntohs(Client.m_SocketAddress.sin_port));

        LOG("New Client received");
        g_Data->m_Clients.emplace_back(Client);
    }
}

void C_Globals::HandleClients()
{
    while (true)
    {
        Sleep(10);
        if (g_Data->m_Clients.empty())
            Sleep(10);

        if (g_Data->m_Clients.size() > 0)
        {
            std::string ClientSize = "Client size: ";
            ClientSize += std::to_string(g_Data->m_Clients.size());
            LOG(ClientSize.c_str());

            for (C_Client& Client : g_Data->m_Clients)
            {
                char aReceived[512000];
                ZeroMemory(aReceived, 512000);

                int32_t iRecvBytes = recv(Client.m_Socket, aReceived, 512000, 0);
                if (iRecvBytes < 0)
                {
                    g_Tools->KickClient(Client);
                    continue;
                }

                C_Message jAcceptedMessage;
                if (!jAcceptedMessage.Accept(aReceived))
                {
                    g_Tools->KickClient(Client);
                    continue;
                }

                auto iType = std::stoi(jAcceptedMessage.GetJSON()["Data"]["Type"].get< std::string >());
                if (iType == MSG_REGISTER)
                    g_Events->OnAccountCreation(Client, jAcceptedMessage);
                else if (iType == MSG_LOGIN)
                    g_Events->OnAccountLogin(Client, jAcceptedMessage);
                else if (iType == MSG_PUSHLIST)
                    g_Events->OnPushAnimeList(Client, jAcceptedMessage);
                else if (iType == MSG_SAVELIST)
                    g_Events->OnSaveAnimeList(Client, jAcceptedMessage);
                else if (iType == MSG_LOGINTOKEN)
                    g_Events->OnAccountLoginToken(Client, jAcceptedMessage);
                else
                    g_Tools->KickClient(Client);
            }
        }
    }
}