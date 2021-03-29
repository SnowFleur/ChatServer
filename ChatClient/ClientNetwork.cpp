#include "ClientNetwork.h"
#include "ChatClient.h"

namespace CLIENT_NETWORK {

void Recv(CChatClient &client) {
  DWORD iobyte=0, flag=0;
	int		  recvReturn = 0;

  client.clinetBuffer.m_recvWsabuf.buf=client.clinetBuffer.m_recvBuffer.GetReadIndex();
  client.clinetBuffer.m_recvWsabuf.len=BUFFER_SIZE;

  recvReturn=WSARecv(client.clinetSocket_, &client.clinetBuffer.m_recvWsabuf, 1, &iobyte, &flag,
          NULL, NULL);
  if (recvReturn == SOCKET_ERROR) {
    if (WSAGetLastError() != WSA_IO_PENDING) {
      std::cout << "Error: Recv  " << WSAGetLastError() << "\n";
    }
  }

  int rest = iobyte;
  char* ptr=client.clinetBuffer.m_recvWsabuf.buf;
  PacketSize packet_size = 0;

  if (client.clinetBuffer.m_prevSize > 0) {
    packet_size = client.clinetBuffer.m_prevSize;
  }

  while (0 < rest) {

    if (0 == packet_size){
      packet_size = ptr[0];
    }

    //받아야 하는 패킷 크기
    int required = packet_size -  client.clinetBuffer.m_prevSize;

    //[21.03.25] 여기부분은 진짜 네트워크 타 봐야 버그 체크 가능
    if (required <= rest) {
      //유저 버퍼에 링버퍼의 값을 복사해온다
      memcpy_s(client.clinetBuffer.m_userBuffer+ client.clinetBuffer.m_prevSize,BUFFER_SIZE,ptr,required);

      //BUFFSZE_SIZE 크기만큼 링버퍼 Index 이동
      client.clinetBuffer.m_recvBuffer.SetReadIndex(BUFFER_SIZE);
      
      client.ProcessPakcet();

      rest -= required;
      ptr += required;
      packet_size=client.clinetBuffer.m_prevSize=0;
    }

    else {
      memcpy_s(client.clinetBuffer.m_userBuffer + packet_size, BUFFER_SIZE, ptr,rest);
      client.clinetBuffer.m_prevSize+=rest;
      //받은 크기만큼 링버퍼 Index 이동
      client.clinetBuffer.m_recvBuffer.SetReadIndex(rest);
      rest = 0;
    }
  }
}

void SendPacket(CChatClient &client, void *packet) {
  DWORD   dwBytes = 0, dwFlags = 0;
  char *p = reinterpret_cast<char *>(packet);
	int		  sendReturn = 0;


  //링버퍼에서 사용 가능한 버퍼 가져옴
  client.clinetBuffer.m_sendWsabuf.buf=client.clinetBuffer.m_sendBuffer.GetWriteIndex();
  //패킷 사이즈
  client.clinetBuffer.m_sendWsabuf.len=p[0];
  // 보내는 패킷사이즈 만큼 Index 이동
  client.clinetBuffer.m_sendBuffer.SetWriteIndex(p[0]);

  memcpy_s(client.clinetBuffer.m_sendWsabuf.buf,client.clinetBuffer.m_sendWsabuf.len,
  packet,p[0]);

  sendReturn=WSASend(client.clinetSocket_, &client.clinetBuffer.m_sendWsabuf, 1, &dwBytes, 0,NULL, NULL);
  if (sendReturn == SOCKET_ERROR) {
    if (WSAGetLastError() != WSA_IO_PENDING) {
      std::cout << "Error: Send  " << WSAGetLastError() << "\n";
    }
  }
}

void SendChatPacket(CChatClient &client, const char *message) {
  cs_message_packet packet;
  memcpy_s(packet.message, MESSAGE_SIZE, message, MESSAGE_SIZE);
  SendPacket(client, &packet);
}

void SendChagnedRoom(CChatClient &client,const ClientID roomNumber){
  cs_changedRoom_packet packet;
  packet.changed_room_number=roomNumber;
  SendPacket(client, &packet);
}

void SendWhisperMessage(CChatClient &client,const ClientID userNumber,const char* message){
  cs_whisperMessage_packet packet;
  packet.userNumber=userNumber;
  memcpy_s(packet.message, MESSAGE_SIZE, message, MESSAGE_SIZE);
  SendPacket(client, &packet);
}


} // namespace CLEINT_NETWORK
