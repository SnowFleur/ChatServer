#include <iostream>
#include<memory>
#include"ChatServer.h"

/*
- Send는 잘려서 보낼 수 있다...
- 내부버퍼가 꽉 차면 Server가 죽을 수 있으니 카운터를 가지고 횟수를 제한.
- 
*/
class CIocpServer;

int main() {

	std::unique_ptr<CIocpServer> iocpServerHandle = std::make_unique<CChatServer>();

	// IOCP 초기화 및 사용할 서버 등록
	if (iocpServerHandle->ServerInitialize() == false) {
		std::cout << "Error: IOCP Initializtion false\n";
	}

	//worker Thread 생성 및 등록
	if (iocpServerHandle->StartWorkerThread() == false) {
		std::cout << "Error: StartWorkerThread false\n";
	}

}