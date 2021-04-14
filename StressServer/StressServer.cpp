#include"StressServer.h"
#include"StressClient.h"
#include"..\ChatServer\Session.h"
#include"..\ChatServer\OverlappedEx.h"
#include"..\ChatServer\Protocol.h"
#include"..\ChatServer\WinSocketHeader.h"

#include<iostream>


void StressTest::InitSubServer() {
	srand((unsigned int)time(NULL));
}

void StressTest::RunSubServer() {

	static int dummyCount = 0;

	while (true) {

		if (dummyCount < COUNT_OF_DUMMY) {

			CSession* session = CreateSubSession();
			OverlappedConnetEx* overEx = new OverlappedConnetEx(session);

			PostQueuedCompletionStatus(GetIOCPHandle(), 5, 0, (LPWSAOVERLAPPED)(overEx));


			session->SetClientID(DUMMY_ACCEPT);
			RegitSocketInIOCP(session->GetSocket(), GetIOCPHandle());
			m_dummys.push_back(session);

			++dummyCount;
		}

		system("cls");
		std::cout << "Count Of Clinet:" << dummyCount << "\n";
		// std::cout << "Count Of Coonect Error:" << ConnectErrorCount<<"Error Code: "<<lastError<< "\n";


		  //출력은 단일이기 때문에 Lock X 
		for (int i = 0;i < m_dummys.size();++i) {

			if (i % 5 == 0)std::cout << "\n";

			std::cout << "Dummy[" << std::setw(3) << i << "]:";
			switch (m_dummys[i]->GetClientID()) {
			case DUMMY_RECY: {
				std::cout << "RECV        ";
				break;
			}
			case DUMMY_MESSAGE: {
				std::cout << "MESSAGE     ";
				break;
			}
			case DUUMY_CHANG_ROOM: {
				std::cout << "CHANGE ROOM ";
				break;
			}
			case DUMMY_ACCEPT: {
				std::cout << "ACCEPT      ";
				break;
			}

			default:
				std::cout << "Error\n";
				break;
			}
		}
	}
}

void StressTest::ProcessSocketIO(void* packet, const CSession& session) {

	const_cast<CSession&>(session).SetClientID(DUMMY_MESSAGE);
	//다시 Send
	switch (rand() % 4) {
	case 0: {
		const char temp[25] = "HELLO WORLD\n";
		Send_Message(const_cast<CSession*>(&session), (void*)temp);
		break;
	}
	case 1: {
		const char temp[25] = "Is it safe?\n";
		Send_Message(const_cast<CSession*>(&session), (void*)temp);
		break;
	}
	case 2: {
		const char temp[25] = "Show me the money!\n";
		Send_Message(const_cast<CSession*>(&session), (void*)temp);
		break;
	}
	case 3: {
		Changed_Room(const_cast<CSession*>(&session));
		const_cast<CSession&>(session).SetClientID(DUUMY_CHANG_ROOM);
		break;
	}
	}

}

CSession* StressTest::CreateSubSession() { return new CDummyClinet(this); }

void StressTest::Send_Message(CSession* user, void* packetBuffer) {
	LockGuard lockGuard(m_dummysLock);

	char* p = reinterpret_cast<char*>(packetBuffer);

	cs_message_packet packet;
	// message Copy
	memcpy_s(packet.message, MESSAGE_SIZE, p, MESSAGE_SIZE);
	user->DoSend(&packet);

}

void StressTest::Changed_Room(CSession* user, void* packetBuffer) {
	cs_changedRoom_packet packet;
	packet.changed_room_number = rand() % 4 + 1;
	user->DoSend(&packet);
}

DWORD StressTest::DoThread() {
	HANDLE iocpHandle = GetIOCPHandle();
	DWORD IoByte;
	LPOVERLAPPED  overlapped = nullptr;
	bool          IoEventResult = true;
#ifdef _WIN64
	ULONGLONG Iokey;
#else
	ULONG Iokey;
#endif //  _WIN64

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDR);

	while (true) {

		//GQCS를 넘어가고 Handle 값이 사라짐? ---> local 하게 해서 해결
		IoEventResult = GetQueuedCompletionStatus(iocpHandle, &IoByte, &Iokey, &overlapped, INFINITE);
		if (IoEventResult == false) {
			if (overlapped == nullptr && WSAGetLastError() != 64) {
				std::cout << WSAGetLastError() << "\n";
				return 0;
			}
		}

		OverlappedEx* overEX = reinterpret_cast<OverlappedEx*>(overlapped);

		//나중에 hash 혹은 다른 자료구조를 이용해 추상화 하기
		switch (overEX->ioEvent) {
		case IO_RECV: {
			overEX->session->ProcessIO();
			break;
		}
		case IO_CONNECT: {

			static int ConnectErrorCount = 0;
			static int lastError = 0;

			if (connect(overEX->session->GetSocket(),
				reinterpret_cast<SOCKADDR*>(&serverAddr),
				sizeof(serverAddr)) == SOCKET_ERROR) {
				std::cout << "connect Error: " << WSAGetLastError() << "\n";
				lastError = WSAGetLastError();
				++ConnectErrorCount;
			}
			else {
				overEX->session->DoRecv();
				overEX->session->SetClientID(DUMMY_RECY);
			}
			break;
		}
		case IO_SEND: {
			//다시 Recv
			overEX->session->DoRecv();
			overEX->session->SetClientID(DUMMY_RECY);
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

	_endthreadex(0);
	return 0;
}