#include <iostream>
#include"ChatServer.h"
/*
- Send�� �߷��� ���� �� �ִ�...
- ���ι��۰� �� ���� Server�� ���� �� ������ ī���͸� ������ Ƚ���� ����.
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


  // IOCP �ʱ�ȭ �� ����� ���� ���
  if (iocpServerHandle->ServerInitialize() == false) {
    std::cout << "Error: IOCP Initializtion false\n";
  }

  //worker Thread ���� �� ���
  if (iocpServerHandle->StartWorkerThread() == false) {
    std::cout << "Error: StartWorkerThread false\n";
  }

  if(iocpServerHandle!=NULL)
  delete iocpServerHandle;

}