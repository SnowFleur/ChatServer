#include <iostream>
#include<memory>
#include"ChatServer.h"

/*
- Send�� �߷��� ���� �� �ִ�...
- ���ι��۰� �� ���� Server�� ���� �� ������ ī���͸� ������ Ƚ���� ����.
- 
*/
class CIocpServer;

int main() {

	std::unique_ptr<CIocpServer> iocpServerHandle = std::make_unique<CChatServer>();

	// IOCP �ʱ�ȭ �� ����� ���� ���
	if (iocpServerHandle->ServerInitialize() == false) {
		std::cout << "Error: IOCP Initializtion false\n";
	}

	//worker Thread ���� �� ���
	if (iocpServerHandle->StartWorkerThread() == false) {
		std::cout << "Error: StartWorkerThread false\n";
	}

}