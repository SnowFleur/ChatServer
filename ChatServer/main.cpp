#include <iostream>
#include<memory>
#include"ChatServer.h"

/*
- Send�� �߷��� ���� �� �ִ�...
- ���ι��۰� �� ���� Server�� ���� �� ������ ī���͸� ������ Ƚ���� ����.
- 
*/
class CIocpServer;

#include"PacketFactory.h"


int main() {


	CPacketPoolFactory factory;

	auto packet=factory.CraetePacket(PACKET_TYPE::SC_LOGIN_OK);


	//std::unique_ptr<CIocpServer> iocpServerHandle = std::make_unique<CChatServer>();

	//// IOCP �ʱ�ȭ �� ����� ���� ���
	//if (iocpServerHandle->ServerInitialize() == false) {
	//	std::cout << "Error: IOCP Initializtion false\n";
	//}

	////worker Thread ���� �� ���
	//if (iocpServerHandle->StartWorkerThread() == false) {
	//	std::cout << "Error: StartWorkerThread false\n";
	//}

}