#pragma once
#include <iostream>
#include "ClientNetwork.h"
#include "ScreenManager.h"

static char MainConsoleTitle;

struct Buffer {
    WSABUF              recvWsabuf;
    char                recvBuffer[BUFFER_SIZE]; 
    WSABUF              sendWsabuf;
    char                sendBuffer[BUFFER_SIZE]; 
    char                packetBuffer[BUFFER_SIZE]; 
    int                 prevSize;
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
  void JoinInRoom();
  void OutInRoom();
public:
  SOCKET        clinetSocket_;
  Buffer        clinetBuffer;

  CChatClient();
  void Run(const SOCKET_TYPE);
  void ProcessPakcet();
};

