#pragma once
#include "Protocol.h"
#include "WinSocketHeader.h"


namespace SERVER_NETWORK {

    void SendPacket(int type,void* packet);

    









// // Standard Send Function
// void SendPacket(SOCKET socket, void *packet);

// // Standard Recv Function
// void Recv(SOCKET socket,OVER_EX& overEx);

// void Send_Message(SOCKET socket,ClientID key,char *message);

// void Send_LoginOK(SOCKET socket,ClientID key);

// void Send_NewUser(SOCKET socket,ClientID key);

// void Send_ChangedRoom(SOCKET socket, RoomUsers& room,Lock& roomLock);

// void Send_OutUser(SOCKET socket,ClientID key);

} // namespace SERVER_NETWORK