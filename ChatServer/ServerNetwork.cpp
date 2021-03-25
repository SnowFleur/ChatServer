//#include "ServerNetwork.h"
//#include "Server.h"
//#include <iostream>
//
//namespace SERVER_NETWORK {
//
//// Standard Send Function
//void SendPacket(SOCKET socket, void *packet) {
//  char *p = reinterpret_cast<char *>(packet);
//  OVER_EX *overEX = new OVER_EX();
//  DWORD ioByte = 0, flags = 0;
//  overEX->m_wsabuf.len = p[0];
//  overEX->m_ioEvent = IO_SEND;
//
//  memcpy_s(overEX->m_buffer, overEX->m_wsabuf.len, packet, p[0]);
//
//  int result=WSASend(socket, &overEX->m_wsabuf, 1, &ioByte, flags, &overEX->m_overlapped,NULL);
//  if (result == SOCKET_ERROR) {
//    if (WSAGetLastError() != WSA_IO_PENDING) {
//      std::cout << "Error: Send  " << WSAGetLastError() << "\n";
//    }
//  }
//}
//
//// Standard Recv Function
//void Recv(SOCKET socket, OVER_EX &overEx) {
//  DWORD flag = 0;
//  int result;
//  overEx.m_ioEvent = IO_RECV;
//
//  result = WSARecv(socket, &overEx.m_wsabuf, 1, NULL, &flag, &(overEx.m_overlapped), 0);
//  if (result == SOCKET_ERROR) {
//    if (WSAGetLastError() != WSA_IO_PENDING) {
//      std::cout << "Error: Recv  " << WSAGetLastError() << "\n";
//    }
//  }
//}
//
//void Send_Message(SOCKET socket, ClientID key, char *message) {
//  sc_message_packet packet;
//  packet.type = SC_SEND_MESSAGE;
//  packet.size = sizeof(packet);
//  packet.id = key;
//  memcpy_s(&packet.message, MESSAGE_SIZE, message, MESSAGE_SIZE);
//  SendPacket(socket, &packet);
//}
//
//void Send_LoginOK(SOCKET socket, ClientID key) {
//  sc_loginOk_packet packet;
//  packet.type = SC_LOGIN_OK;
//  packet.size = sizeof(packet);
//  packet.myNumber = key;
//  SendPacket(socket, &packet);
//}
//
//void Send_NewUser(SOCKET socket, ClientID key) {
//  sc_newUser_packet packet;
//  packet.type = SC_NEW_USER;
//  packet.size = sizeof(packet);
//  packet.new_userNumber = key;
//  SendPacket(socket, &packet);
//}
//
//void Send_ChangedRoom(SOCKET socket, RoomUsers &room,Lock& roomLock) {
//  sc_changedRoom_packet packet;
//  size_t roomSize=room.size();
//
//  roomLock.AcquiredLock();
//  for (size_t i = 0; i < roomSize; ++i) {
//    packet.roomClientids[i] = room[i];
//  }
//  roomLock.ReleaseLock();
//
//  packet.size = sizeof(packet);
//  packet.type = SC_CHANGED_ROOM;
//  packet.coundOfclient = roomSize;
//  SendPacket(socket, &packet);
//}
//
//void Send_OutUser(SOCKET socket, ClientID key) {
//  sc_outUser_packet packet;
//  packet.type=SC_OUT_USER;
//  packet.size=sizeof(packet);
//  packet.out_userNumber=key;
//  SendPacket(socket, &packet);
//}
//
//} // namespace SERVER_NETWORK