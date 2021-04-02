#include <iostream>
#include"ChatServer.h"
/*
- Send�� �߷��� �� �ִ�.
- ���ι��۰� �� ���� Server�� ���� �� ������ ī���͸� ������ Ƚ���� ����.
- 
*/

class CIocpServer;

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