#include <iostream>
#include"ChatServer.h"
/*
- Send는 잘려서 보낼 수 있다...
- 내부버퍼가 꽉 차면 Server가 죽을 수 있으니 카운터를 가지고 횟수를 제한.
- 
*/
class CIocpServer;

// #include"ObjectPool.h"

// struct OverlappedEx:public CObjectManager<CFixedMemoryBlock>{
//   OVERLAPPED    overlapped;
//   WSABUF        wsabuf;
//   IO_EVENT      ioEvent;
//   CSession*     session;

//   OverlappedEx(CSession* s,IO_EVENT io) : session(NULL), ioEvent(io){
//     ZeroMemory(&overlapped, sizeof(overlapped));
//     session=s;
//   }
// };

// struct OverlappedAcceptEx : public OverlappedEx {
//   OverlappedAcceptEx(CSession* s) : OverlappedEx(s,IO_ACCEPT){
//     wsabuf.buf = NULL;
//     wsabuf.len = 0;
//   }
// };


int main() {


  CIocpServer *iocpServerHandle = NULL;
  iocpServerHandle = new CChatServer();


  // IOCP 초기화 및 사용할 서버 등록
  if (iocpServerHandle->ServerInitialize() == false) {
    std::cout << "Error: IOCP Initializtion false\n";
  }

  //worker Thread 생성 및 등록
  if (iocpServerHandle->StartWorkerThread() == false) {
    std::cout << "Error: StartWorkerThread false\n";
  }

  if(iocpServerHandle!=NULL)
  delete iocpServerHandle;

}