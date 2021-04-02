#include<iostream>
#include <conio.h>
#include "ChatClient.h"

Buffer::Buffer() :m_recvWsabuf(),m_sendWsabuf(),m_recvBuffer(),
m_sendBuffer(),m_userBuffer(),m_prevSize(0) {

}

CChatClient::CChatClient() :
 myNumber_(NO_RECV_NUMBER_TO_SERVER),roomUserList_(MAX_ROOM_CLIENT),currentRoomNumber_(0) {
  for (int i = 0; i < MAX_ROOM_CLIENT; ++i) {
    roomUserList_[i] = NO_RECV_NUMBER_TO_SERVER;
  }
}

void CChatClient::Run(SOCKET_TYPE socketType) {

  //Set Main Console Size
  system("mode con cols=70 lines=20"); 

  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cout << "Error: Can Not Load Winsock.dll\n";
  }
  InitSocket(socketType);

  SOCKADDR_IN serverAddr;
  ZeroMemory(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_port = SERVER_PORT;
  serverAddr.sin_family = PF_INET;
  serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDR);

  if (connect(clinetSocket_, reinterpret_cast<SOCKADDR *>(&serverAddr),
              sizeof(serverAddr)) == SOCKET_ERROR) {
    std::cout << "connect Error: " << WSAGetLastError() << "\n";
  }

  event_ = WSACreateEvent();
  WSAEventSelect(clinetSocket_, event_, FD_READ | FD_WRITE | FD_CLOSE);

  // u_long nonBlockingMode = 1;
  // ioctlsocket(clinetSocket_, FIONBIO, &nonBlockingMode);

  CScreenManager::GetInstance();
  ProcessChatting();
}

void CChatClient::InitSocket(const SOCKET_TYPE socketType) {

  switch (socketType) {
  case TCP_TYPE: {
    clinetSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    break;
  }
  case UDP_TYPE: {
    clinetSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    break;
  }
  default:
    break;
  }

  if (clinetSocket_ == INVALID_SOCKET) {
    std::cout << "Error: clinetSocket Is INVALID\n";
  }
}

void CChatClient::ProcessChatting() {
  WSANETWORKEVENTS wsaNetEvents;
  while (true) {

    if (_kbhit()) {
      _getch();

      char temp[MESSAGE_SIZE];
      fgets(temp, MESSAGE_SIZE - 2, stdin);

      if (temp[0] == '\n') {
        std::cout << "Push \n";
      }

      else if (strcmp(temp, "!Insert\n") == 0) {
        std::cout << "Select Join Room Number\n";
        std::cin >> currentRoomNumber_;

        if (currentRoomNumber_ > 5) {
          std::cout << "INVALID ROOM NUMBER\n";
        } else {
          CLIENT_NETWORK::SendChagnedRoom(*this, currentRoomNumber_);
        }
      }

      else if (strcmp(temp, "!Whisper\n") == 0) {
        std::cout << "Select User Number\n";
        ClientID selectUserNumber;
        std::cin >> selectUserNumber;

        std::cout << "Input Message:";
        getchar();

        char whisPer[MESSAGE_SIZE];
        fgets(whisPer, MESSAGE_SIZE - 2, stdin);

        CLIENT_NETWORK::SendWhisperMessage(*this, selectUserNumber, whisPer);
      }
      else if(strcmp(temp,"!Exit\n")==0){
        closesocket(clinetSocket_);
        WSACleanup();
        return ;
      }

      else {
        CScreenManager::GetInstance()->InsertChat(myNumber_, temp);
        CLIENT_NETWORK::SendChatPacket(*this, temp);
      }

      CScreenManager::GetInstance()->DrawToMynameAndRoomNumber(myNumber_,currentRoomNumber_);
      CScreenManager::GetInstance()->DrawToUserIndex(roomUserList_);
      CScreenManager::GetInstance()->Draw();
    }

    DWORD result =
        WSAWaitForMultipleEvents(clinetSocket_, &event_, FALSE, 0, FALSE);

    if (WSAEnumNetworkEvents(clinetSocket_, event_, &wsaNetEvents) ==
        SOCKET_ERROR)
      std::cout << "Error: WSAEVENT\n";

    // Read
    if (wsaNetEvents.lNetworkEvents & FD_READ) {

      CLIENT_NETWORK::Recv(*this);
      CScreenManager::GetInstance()->DrawToMynameAndRoomNumber(myNumber_,currentRoomNumber_);
      CScreenManager::GetInstance()->DrawToUserIndex(roomUserList_);
      CScreenManager::GetInstance()->Draw();


    } 
    else if (wsaNetEvents.lNetworkEvents & FD_WRITE) {
    }

    else if (wsaNetEvents.lNetworkEvents & FD_CLOSE) {
    }
  }
}

void CChatClient::SendPacket(void *packet) {
  char *p = reinterpret_cast<char *>(packet);

  //링버퍼에서 사용 가능한 버퍼 가져옴
  clinetBuffer.m_sendWsabuf.buf=clinetBuffer.m_sendBuffer.GetWriteIndex();
  //패킷 사이즈
  clinetBuffer.m_sendWsabuf.len=p[0];
  // 보내는 패킷사이즈 만큼 Index 이동
  clinetBuffer.m_sendBuffer.SetWriteIndex(p[0]);

  memcpy_s(clinetBuffer.m_sendWsabuf.buf,clinetBuffer.m_sendWsabuf.len, packet, p[0]);

  WSASend(clinetSocket_, &clinetBuffer.m_sendWsabuf, 1, 0, 0, NULL, NULL);
}

void CChatClient::ProcessPakcet() {

  switch (clinetBuffer.m_userBuffer[1]) {
  case PACKET_TYPE::SC_LOGIN_OK: {
    sc_loginOk_packet *loginOk_packet =
        reinterpret_cast<sc_loginOk_packet *>(clinetBuffer.m_userBuffer);
    myNumber_ = loginOk_packet->myNumber;
    CScreenManager::GetInstance()->DrawToMynameAndRoomNumber(myNumber_,currentRoomNumber_);

    itoa(myNumber_, &MainConsoleTitle, 10);
    SetConsoleTitle(&MainConsoleTitle);
    break;
  }
  case PACKET_TYPE::SC_SEND_MESSAGE: {
    sc_message_packet *message_packet =
        reinterpret_cast<sc_message_packet *>(clinetBuffer.m_userBuffer);

    CScreenManager::GetInstance()->InsertChat(message_packet->id,message_packet->message);
    break;
  }
  case PACKET_TYPE::SC_NEW_USER: {
    sc_newUser_packet *newUser_packet =
        reinterpret_cast<sc_newUser_packet *>(clinetBuffer.m_userBuffer);
    CScreenManager::GetInstance()->InsertChat(newUser_packet->new_userNumber,"Join In Room\n");
    break;
  }
  case PACKET_TYPE::SC_CHANGED_ROOM: {
    sc_changedRoom_packet *changedRoom_packet =
        reinterpret_cast<sc_changedRoom_packet *>(clinetBuffer.m_userBuffer);

    for (int i = 0; i < MAX_ROOM_CLIENT; ++i) {
      roomUserList_[i] = NO_RECV_NUMBER_TO_SERVER;
    }
    for (size_t i = 0; i < changedRoom_packet->countOfclient; ++i) {
      roomUserList_[i] = changedRoom_packet->roomClientids[i];
    }

    break;
  }
  case PACKET_TYPE::SC_OUT_USER: {
    sc_outUser_packet *outUser_packet =
        reinterpret_cast<sc_outUser_packet *>(clinetBuffer.m_userBuffer);
         CScreenManager::GetInstance()->InsertChat(outUser_packet->out_userNumber,"Out In Room\n"); 
    break;
  }
  default:
    std::cout << "Error: Not Defined Packet Type\n";
    break;
  }
}