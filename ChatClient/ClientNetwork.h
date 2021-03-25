#pragma once
#include "..\ChatServer\Protocol.h"
#include "..\ChatServer\WinSocketHeader.h"

class CChatClient;

namespace CLIENT_NETWORK {

void Recv(CChatClient &client);

void SendPacket(CChatClient &client, void *packet);

void SendChatPacket(CChatClient &client,const char* message);

void SendChagnedRoom(CChatClient &client,const ClientID roomNumber);

void SendWhisperMessage(CChatClient &client,const ClientID userNumber,const char* message);

} // namespace CLEINT_NETWORK
