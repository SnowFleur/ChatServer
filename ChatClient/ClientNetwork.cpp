#include "ClientNetwork.h"
#include "ChatClient.h"

namespace CLIENT_NETWORK {

void Recv(CChatClient &client) {
  DWORD iobyte=0, flag=0;
  WSARecv(client.clinetSocket_, &client.clinetBuffer.recvWsabuf, 1, &iobyte, &flag,
          NULL, NULL);

  int rest = iobyte;
  char* ptr = reinterpret_cast<char*>(client.clinetBuffer.recvBuffer);
  char packet_size = 0;

  if (client.clinetBuffer.prevSize > 0) {
    packet_size = client.clinetBuffer.prevSize;
  }

  while (0 < rest) {
    if (0 == packet_size)
      packet_size = ptr[0];
    int required = packet_size - client.clinetBuffer.prevSize;

    if (required <= rest) {
      memcpy(client.clinetBuffer.packetBuffer + client.clinetBuffer.prevSize, ptr,
             required);
      client.ProcessPakcet();
      rest -= required;
      ptr += required;

      packet_size = 0;
      client.clinetBuffer.prevSize = 0;
    } else {
     
      memcpy(client.clinetBuffer.packetBuffer + client.clinetBuffer.prevSize, ptr,
             rest);
      rest = 0;
      client.clinetBuffer.prevSize += rest;
    }
  }
}

void SendPacket(CChatClient &client, void *packet) {
  char *p = reinterpret_cast<char *>(packet);
  DWORD iobyte;

  client.clinetBuffer.sendWsabuf.len = p[0];

  memcpy_s(client.clinetBuffer.sendBuffer, BUFFER_SIZE, p, p[0]);

  WSASend(client.clinetSocket_, &client.clinetBuffer.sendWsabuf, 1, &iobyte, 0,
          NULL, NULL);
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
