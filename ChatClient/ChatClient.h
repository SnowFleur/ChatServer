#pragma once

#include "ClientNetwork.h"
#include"ScreenManager.h"
#include"..\ChatServer\RingBuffer.h"

//static const char MainConsoleTitle[5];

struct Buffer {
    WSABUF                m_recvWsabuf;
    WSABUF                m_sendWsabuf;
    CRingBuffer<char>     m_recvBuffer;
    CRingBuffer<char>     m_sendBuffer;
    char                  m_userBuffer[BUFFER_SIZE]; 
    DWORD                 m_prevSize;
    Buffer();
};


class CChatClient {
private:
  ClientID    myNumber_;
  WSAEVENT    event_;
  RoomUsers   roomUserList_;
  RoomNumber  currentRoomNumber_;
  
  void InitSocket(const SOCKET_TYPE);
  void ProcessChatting();
  void SendPacket(void* packet);
public:
  SOCKET        clinetSocket_;
  Buffer        clinetBuffer;

  CChatClient();
  void Run(const SOCKET_TYPE);
  void ProcessPakcet();
};

