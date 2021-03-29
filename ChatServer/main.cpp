#include <iostream>
#include"IocpManager.h"
#include"SessionManager.h"
#include"BaseServer.h"

#include"User.h"
#include"ChatServer.h"

/*
���� ����
- ���� ������ IOCP Server -> ä�� ���� �� �ƴ϶� �ٸ� �������� ��밡���ϵ��� �߻�ȭ �� ���ȭ
- ��Ӱ� �������� ��� But �ִ��� ���� ->���յ�, Ȯ�强 ��� ������ ���� �� ����
- �̱��� ���� �� �ٸ� ������ ���� ���� ��ũ
*/

int main() {

  //IOCP �ʱ�ȭ �� ����� ���� ���
  if (CIocpManager::GetInstance()->Initialize(new CChatServer) == false) {
    std::cout << "Error: IOCP Initializtion false\n";
  }

  //worker Thread ���� �� ����� ���� ����
  if (CIocpManager::GetInstance()->StartWorkerThread() == false) {
    std::cout << "Error: StartWorkerThread false\n";
  }
}