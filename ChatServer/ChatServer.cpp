#include"ChatServer.h"
#include"Session.h"
#include"Room.h"
#include"Protocol.h"
#include"WinSocketHeader.h"
#include"User.h"

void CChatServer::Init() {
  m_clientIndex=0;

  //생성할 방 개수
  m_roomHandle=new CRoom(CHAT_ROOM_COUNT);

  // 패킷 프로세스 등록
  // 최초에 등록만 진행하고 Read만 하기 때문에 Lock 필요 X
  m_processPacket.insert(PairProcessPacket(PACKET_TYPE::CS_SEND_MESSAGE,&CChatServer::Send_Message));
  m_processPacket.insert(PairProcessPacket(PACKET_TYPE::CS_CHANGED_ROOM,&CChatServer::Send_ChangedRoom));
  m_processPacket.insert(PairProcessPacket(PACKET_TYPE::CS_WHISPER_MESSAGE,&CChatServer::Send_WhisperMessage));

}

void CChatServer::Run(){
    while(true){}
}

void CChatServer::ProcessAsyncAccpet(CSession& user) {
  // Lock Guard
  {
    LockGuard lockGuard(m_usersLock);
    //해당 유저를 users에 등록
    m_users.push_back(const_cast<CSession *>(&user));
    user.SetClientID(m_clientIndex++);
  }
    //해당 유저를 rooms에 등록 Default 0번
    m_roomHandle->InsertUserInRoom(&user);

    //Send Login Ok
    Send_LoginOK(&user);
    //Send NewUser
    Send_NewUser(&user);
    //Send UserList
    Send_RoomUserList(&user);

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

  // 자기가 들어가 있는 방에 Broad Cast
  m_roomHandle->SendAllMessageInRoom(user,&packet);

}

 //나중에 다시 수정하기 로직이 넘 이상
void CChatServer::Send_ChangedRoom(const CSession* user,void* packetBuffer){

  cs_changedRoom_packet *changedRoom_packet =
      reinterpret_cast<cs_changedRoom_packet *>(packetBuffer);

    RoomNumber newRoom=changedRoom_packet->changed_room_number;

      //해당 유저의 퇴장을 보냄(OLD) -->버그
      //Send_OutUser(user);
      //Send_RoomUserList(user);

    if(m_roomHandle->DeletedUserInRoom(const_cast<CSession*>(user))==false){
      std::cout<<"Error: Not Find user\n";
    } 
    
    else {
      if (m_roomHandle->InsertUserInRoom(const_cast<CSession *>(user),newRoom) == true) {
        //해당 유저의 접속을 보냄(NEW)
        Send_RoomUserList(user);
        Send_NewUser(user);
      }
    }
}

void CChatServer::Send_WhisperMessage(const CSession* user,void* packetBuffer){}

void CChatServer::Send_NewUser(const CSession *user, void *packetBuffer) {
  sc_newUser_packet packet;
  packet.new_userNumber = user->GetClientID();
  // 자기가 들어가 있는 방에 Broad Cast
  m_roomHandle->SendAllMessageInRoom(user,&packet);
}

void CChatServer::Send_OutUser(const CSession* user,void* packetBuffer){
  sc_outUser_packet packet;
  packet.out_userNumber= user->GetClientID();

  // 자기가 들어가 있는 방에 Broad Cast
  m_roomHandle->SendAllMessageInRoom(user,&packet);
}

void CChatServer::Send_RoomUserList(const CSession* user,void* packetBuffer){
    sc_changedRoom_packet packet;
    m_roomHandle->CopyAllUserInRoom(user,packet.roomClientids,packet.countOfclient);
    m_roomHandle->SendAllMessageInRoom(user,&packet);

    const_cast<CSession *>(user)->DoSend(&packet);
}