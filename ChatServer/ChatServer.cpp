#include"ChatServer.h"
#include"Session.h"
#include"Room.h"
#include"Protocol.h"
#include"WinSocketHeader.h"
#include"User.h"

void CChatServer::Init() {
  m_clientIndex=0;

  //������ �� ����
  m_roomHandle=new CRoom(CHAT_ROOM_COUNT);

  // ��Ŷ ���μ��� ���
  // ���ʿ� ��ϸ� �����ϰ� Read�� �ϱ� ������ Lock �ʿ� X
  m_processPacket.insert(PairProcessPacket(PACKET_TYPE::CS_SEND_MESSAGE,&CChatServer::Send_Message));
  m_processPacket.insert(PairProcessPacket(PACKET_TYPE::CS_CHANGED_ROOM,&CChatServer::Send_ChangedRoom));
  m_processPacket.insert(PairProcessPacket(PACKET_TYPE::CS_WHISPER_MESSAGE,&CChatServer::Send_WhisperMessage));

}

void CChatServer::Run(){
  std::cout<<"Run Server\n";
    while(true){}
}

void CChatServer::ProcessAsyncAccpet(CSession& user) {
  // Lock Guard
  {
    LockGuard lockGuard(m_usersLock);
    //�ش� ������ users�� ���
    m_users.push_back(const_cast<CSession *>(&user));
    user.SetClientID(m_clientIndex++);
  }
    //�ش� ������ rooms�� ��� Default 0��
    m_roomHandle->InsertUserInRoom(&user);

    //Send Login Ok
    Send_LoginOK(&user);

    //���ο� ������ ���� �˸� �� ����Ʈ ����
    m_roomHandle->SendNewUserInRoom(&user);

}

void CChatServer::ProcessSocketIO(void* packet,const CSession& session){

  PacketType type = reinterpret_cast<char *>(packet)[1];

  MFP fp = m_processPacket[type];
  (this->*fp)(&session, packet);

  //call Recv
  const_cast<CSession&>(session).DoRecv();
}

void CChatServer::Send_LoginOK(const CSession *user, void *packetBuffer) {
  sc_loginOk_packet packet;
  packet.myNumber=user->GetClientID();
  const_cast<CSession *>(user)->DoSend(&packet);
}

void CChatServer::Send_Message(const CSession *user, void *packetBuffer) {

  cs_message_packet *message_packet =
      reinterpret_cast<cs_message_packet *>(packetBuffer);

#ifdef _DEBUG
  for (int i = 0; message_packet->message[i] != '\n'; ++i) {
    std::cout << message_packet->message[i];
  }
  std::cout << "\n";
#endif

  sc_message_packet packet;
  // message Copy
  memcpy_s(packet.message, MESSAGE_SIZE, message_packet->message, MESSAGE_SIZE);
  // Insert User number
  packet.id = user->GetClientID();

  // �ڱⰡ �� �ִ� �濡 Broad Cast
  m_roomHandle->SendAllMessageInRoom(user, &packet);
}

 //���߿� �ٽ� �����ϱ� ������ �� �̻�
void CChatServer::Send_ChangedRoom(const CSession* user,void* packetBuffer){

  cs_changedRoom_packet *changedRoom_packet =
      reinterpret_cast<cs_changedRoom_packet *>(packetBuffer);

  //�ش� ������ ������ ����(OLD) -->����
  m_roomHandle->ChangedRoom(user, changedRoom_packet->changed_room_number);
}

void CChatServer::Send_WhisperMessage(const CSession* user,void* packetBuffer){}
