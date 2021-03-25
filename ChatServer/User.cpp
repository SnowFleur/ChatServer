#include<iostream>
#include "User.h"
#include"ChatServer.h"


void CUser::ProcessIO(){
    ProcessPacket(GetUserBuffer());
}

void CUser::ProcessPacket(void* packet){
    
 char *p = reinterpret_cast<char *>(packet);

  switch (p[1]) {
  case PACKET_TYPE::CS_SEND_MESSAGE: {
    cs_message_packet *message_packet =
        reinterpret_cast<cs_message_packet *>(p);

    for (int i = 0; message_packet->message[i] != '\n'; ++i) {
      std::cout << message_packet->message[i];
    }
    std::cout << "\n";
    m_serverHandle->ProcessSocketIO(packet, PACKET_TYPE::CS_SEND_MESSAGE,*this);
    break;
  }
  case  PACKET_TYPE::CS_CHANGED_ROOM: {
    break;
  }
  case  PACKET_TYPE::CS_WHISER_MESSAGE: {
    break;
  }
  default: {
    std::cout << "Error: Not Defined Packet Type\n";
    break;
  }
  }

  //Call Recv
  DoRecv();

}