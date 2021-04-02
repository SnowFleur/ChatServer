#pragma once 

#include<vector>
#include <map>
#include<iostream>
#include<cstdlib>
#include<iomanip>
#include<ctime>

#include"StressClient.h"
#include"..\ChatServer\WinSocketHeader.h"
#include "..\ChatServer\IocpServer.h"
#include"..\ChatServer\Protocol.h"
#include"..\ChatServer\Lock.h"
#include"..\ChatServer\Session.h"
#include"..\ChatServer\OverlappedEx.h"

const int COUNT_OF_DUMMY = MAX_ROOM_CLIENT - 5;

class StressTest : public CIocpServer {
    typedef std::vector<CSession*>::iterator        dummyIter;
private:
    
    std::vector<CSession*>      m_dummys;
    Lock                        m_dummysLock;

    void InitSubServer()override;
    void RunSubServer()override;
    void ProcessAsyncAccpet(CSession &session)override{}
    void ProcessSocketIO(void *packet, const CSession &session)override;
    DWORD DoThread()override;
    
	CSession* CreateSubSession()override{ return new CDummyClinet(this); }

  void Send_Message(CSession* user,void* packetBuffer=NULL);
  void Changed_Room(CSession* user,void* packetBuffer=NULL);

public:
  StressTest():CIocpServer(this){}

};

/*
****************************************
****************************************
****************************************
*/

void StressTest::InitSubServer(){
  srand((unsigned int)time(NULL));
}


void StressTest::RunSubServer(){

  SOCKADDR_IN serverAddr;
  ZeroMemory(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_port = SERVER_PORT;
  serverAddr.sin_family = PF_INET;
  serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDR);


  static int dummyCount = 0;

  while (true) {

    // 1초에 한번씩 Chat Server연결 시도
    Sleep(1000);

    if (dummyCount < COUNT_OF_DUMMY) {

      CSession *session = CreateSubSession();

      if (connect(session->GetSocket(),
                  reinterpret_cast<SOCKADDR *>(&serverAddr),
                  sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "connect Error: " << WSAGetLastError() << "\n";
      }
      session->SetClientID(DUMMY_ACCEPT);
      RegitSocketInIOCP(session->GetSocket(), GetIOCPHandle());
      session->DoRecv();
      m_dummys.push_back(session);
      ++dummyCount;
    }

      system("cls");
      std::cout << "Count Of Clinet:" << dummyCount << "\n";

	  //출력은 단일이기 때문에 Lock X 
	  for(int i=0;i<m_dummys.size();++i){

		  if(i%5==0)std::cout<<"\n";

		  std::cout<<"Dummy["<<std::setw(3)<<i<<"]:";
		  switch (m_dummys[i]->GetClientID()){
			case DUMMY_RECY:{
				std::cout<<"RECV        ";
				break;
		  }
			case DUMMY_MESSAGE:{
				std::cout<<"MESSAGE     ";
				break;
		  }
			case DUUMY_CHANG_ROOM:{
				std::cout<<"CHANGE ROOM ";
				break;
		  }
			case DUMMY_ACCEPT:{
				std::cout<<"ACCEPT      ";
				break;
		  }

			default:
				std::cout<<"Error\n";
				break;
		  }


	  }

  }

}

void StressTest::ProcessSocketIO(void *packet, const CSession &session) {

  //다시 Recv
  const_cast<CSession &>(session).DoRecv();
  const_cast<CSession&>(session).SetClientID(DUMMY_RECY);
  Sleep(500);

  const_cast<CSession&>(session).SetClientID(DUMMY_MESSAGE);
  //다시 Send
  switch (rand() % 4) {
  case 0: {
    const char temp[25] = "HELLO WORLD\n";
    Send_Message(const_cast<CSession *>(&session), (void *)temp);
    break;
    }
  case 1: {
    const char temp[25] = "Life Is Hard\n";
    Send_Message(const_cast<CSession *>(&session), (void *)temp);
    break;
    }
  case 2: {
    const char temp[25] = "WA SEND!\n";
    Send_Message(const_cast<CSession *>(&session), (void *)temp);
    break;
    }
    case 3:{
       Changed_Room(const_cast<CSession *>(&session));
  const_cast<CSession&>(session).SetClientID(DUUMY_CHANG_ROOM);
      break;
    }
  }

}

void StressTest::Send_Message(CSession *user, void *packetBuffer){
  LockGuard lockGuard(m_dummysLock);

  char* p =reinterpret_cast<char*>(packetBuffer);

  cs_message_packet packet;
  // message Copy
  memcpy_s(packet.message, MESSAGE_SIZE, p, MESSAGE_SIZE);
  user->DoSend(&packet);

}

void StressTest::Changed_Room(CSession *user, void *packetBuffer) {
  cs_changedRoom_packet packet;
  packet.changed_room_number = rand() % 4 + 1;
  user->DoSend(&packet);
}

DWORD StressTest::DoThread(){
  HANDLE iocpHandle = GetIOCPHandle();
  DWORD IoByte;
  LPOVERLAPPED  overlapped=NULL;
  bool          IoEventResult=true;
#ifdef _WIN64
  ULONGLONG Iokey;
#else
  ULONG Iokey;
#endif //  _WIN64
  while (true) {

   //GQCS를 넘어가고 Handle 값이 사라짐? ---> local 하게 해서 해결
   IoEventResult = GetQueuedCompletionStatus(iocpHandle, &IoByte, &Iokey,&overlapped,INFINITE);
    if (IoEventResult == false) {
      if (overlapped == false && WSAGetLastError() != 64) {
        std::cout << WSAGetLastError() << "\n";
        return 0;
      }
    }

    OverlappedEx* overEX=reinterpret_cast<OverlappedEx*>(overlapped);


  //나중에 hash 혹은 다른 자료구조를 이용해 추상화 하기
  switch (overEX->ioEvent) {
    case IO_RECV: {
      overEX->session->ProcessIO();
      break;
    }
    case IO_CONNECT:{
      break;
    }
    case IO_SEND: {
      break;
    }
    default: {
      std::cout << "Error: Not Define Io Event\n";
      IoEventResult = false;
      break;
    }
  } // End Switch

    if (IoEventResult == true) {
      delete overlapped;
      overlapped = NULL;
    }

  } //end While

   _endthreadex( 0 );
  return 0;


}