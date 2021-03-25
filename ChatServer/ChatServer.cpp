#include"ChatServer.h"
#include"SessionManager.h"
#include"Session.h"
#include"Room.h"
#include"Protocol.h"
#include"WinSocketHeader.h"

CChatServer::~CChatServer() {}

void CChatServer::Init() {
    m_clientIndex=0;
}

void CChatServer::Run(){
    while(true){}
}

void CChatServer::ProcessAsyncAccpet(const CSession& user) {

  // Lock Guard
  {

    LockGuard lockGuard(m_usersLock);
    //해당 유저를 users에 등록
    m_users.push_back(const_cast<CSession *>(&user));
  }
  
    //Send Login Ok
    Send_LoginOK(&user);
}

void CChatServer::ProcessSocketIO(void* packet,const  PacketType packIoType,const CSession& session){

    //스위치 말고 다른거 사용
    switch (packIoType) {
    case  PACKET_TYPE::CS_SEND_MESSAGE: {

      break;
    }
    default:
        std::cout<<"Error: ProcessSocketID\n";
      break;
    }
}

void CChatServer::Send_LoginOK(const CSession* user){
    sc_loginOk_packet packet;
    {
      // Win CAS API 지원 X
      LockGuard lockGuard(m_clientIndexLock);
      packet.myNumber = m_clientIndex;
      ++m_clientIndex;
    }

    const_cast<CSession*>(user)->DoSend(&packet);    

    //새로운 유저가 접속했다는 정보를 보냄
    // for(UsersIter iter=m_users.begin();iter!=m_users.end();++iter){
    //     (*iter)->DoSend();
    // }
}

void CChatServer::Send_Message(const CSession* user){

  sc_message_packet packet;

  


}

