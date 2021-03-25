#include <iostream>
#include"IocpManager.h"
#include"SessionManager.h"
#include"BaseServer.h"

#include"User.h"
#include"ChatServer.h"

/*
��Ģ

- ��� ������ �׻� m_ ���̱�
- ������ �Ҹ��ڵ� ������ ����Լ�(Get/set)�� inline �� ������Ͽ� �ۼ�
- �ϳ��� ����� �ϴ� ��ü�� ���� ������, ���� ���Կ����� ����
- �Լ��� �׻� �빮�ڷ� ����
- �ڷ����� �� �ѹ� ���� �� �Լ��� �ִ��� �� ���߱�
*/


/*
���� ����
- ���� ������ IOCP Server -> ä�� ���� �� �ƴ϶� �ٸ� �������� ��밡���ϵ��� �߻�ȭ �� ���ȭ
- ��Ӱ� �������� ��� But �ִ��� ���� ->���յ�, Ȯ�强 ��� ������ ���� �� ����
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