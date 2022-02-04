#include <iostream>
#include <thread>

#include "Backend/Helpers/Logs.hpp"
#include "Backend/Globals.h"

void ThreadReceiveClients()
{
    g_Globals->ReceiveClients();
}
void ThreadHandleClients()
{
    g_Globals->HandleClients();
}

int main()
{
    LOG("Start up");

    WSADATA pWSAData;
    if (WSAStartup(MAKEWORD(2, 2), &pWSAData))
    {
        LOG("Unable to initializate the WSA");
        return 1;
    }

    LOG("WSA successfully inited");

    g_Data->m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (!g_Data->m_ServerSocket)
    {
        LOG("Unable to create the server socket");
        return 1;
    }

    g_Data->m_SocketAddress.sin_family = AF_INET;
    g_Data->m_SocketAddress.sin_port = htons(14312);
    g_Data->m_SocketAddress.sin_addr.S_un.S_addr = INADDR_ANY;

    int32_t iBindResult = bind(g_Data->m_ServerSocket, (sockaddr*)(&g_Data->m_SocketAddress), 16);
    if (iBindResult < 0)
    {
        LOG("Server cannot binded");
        return 1;
    }

    int32_t iListenResult = listen(g_Data->m_ServerSocket, SOMAXCONN);
    if (iListenResult < 0)
    {
        LOG("Cannot listen clients");
        return 1;
    }

    LOG("Server started up");

    CreateDirectory("ServerData/", NULL);
    CreateDirectory("ServerData/Users/", NULL);
    CreateDirectory("ServerData/Users/UserData/", NULL);
    CreateDirectory("ServerData/Users/UserList/", NULL);

    std::thread hReceiveClient(ThreadReceiveClients);
    hReceiveClient.detach();

    std::thread hHandleClients(ThreadHandleClients);
    hHandleClients.detach();

    while (true)
    {
        Sleep(200);
    }

    return 0;
}