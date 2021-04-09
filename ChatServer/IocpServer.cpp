#include<iostream>
#include"IocpServer.h"
#include"OverlappedEx.h"
#include"Session.h"
#include"User.h"

void CIocpServer::ReleaseWorkerThreadHandle() {
	for (DWORD i = 0;i < m_workthreadCount;++i)
		CloseHandle(m_workerThreads);
}

bool CIocpServer::ServerInitialize(DWORD createWorkThreadCount, const int port, bool ignore) {

	m_workthreadCount = createWorkThreadCount;
	WSADATA WSAData;

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cout << "Error: Can Not Load winsock.dll\n";
		return false;
	}

	// Create IOCP HANDLE
	m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (m_iocpHandle == NULL) {
		std::cout << "Error: Can Not Init IOCP HANDLE\n";
		return false;
	}

	//Create listen Socket
	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET) {
		std::cout << "Error: Can Not Init Listen Socket\n";
		WSACleanup();
		return false;
	}

	//Regit listenSocket
	if (RegitSocketInIOCP(m_listenSocket, m_iocpHandle) == false) {
		std::cout << "Error Register In IOCP\n";
	}

	if (ignore == false) {

		SOCKADDR_IN serverAddr;
		ZeroMemory(&serverAddr, sizeof(serverAddr));
		serverAddr.sin_port = port;
		serverAddr.sin_family = PF_INET;
		serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


		// Socket Bind
		if (bind(m_listenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr),
			sizeof(serverAddr)) == SOCKET_ERROR) {
			std::cout << "Error: Bind " << WSAGetLastError() << "\n";
			closesocket(m_listenSocket);
			WSACleanup();
			return false;
		}

		// Socket Listen
		if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
			std::cout << "Error: Listne " << WSAGetLastError() << "\n";
			closesocket(m_listenSocket);
			WSACleanup();
			return false;
		}

		//listen Accpet 막기
		BOOL on = true;
		setsockopt(m_listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT,
			reinterpret_cast<char*>(&on), sizeof(on));

		//비동기 Accept걸기 
		AsyncAccept();
	}

	return true;
}

bool CIocpServer::StartWorkerThread() {
	for (DWORD i = 0; i < m_workthreadCount; ++i) {
		DWORD NumberOfWork = 10000 + i;
		m_workerThreads[i] = (HANDLE)_beginthreadex(NULL, 0, &CIocpServer::WinCreateWorkerThread, this, 0, (unsigned*)&NumberOfWork);
	}

	// if(_beginthreadex(NULL, 0, &CIocpServer::WinCreateNetworkThread, this, 0, NULL)==-1)
	//   std::cout<<"Error: Not Create Thread\n";

	//Server Start!!
	InitSubServer();
	RunSubServer();

	return true;
}

void CIocpServer::WaitForWorkerThreads() {
	WaitForMultipleObjects(m_workthreadCount, m_workerThreads, TRUE, INFINITE);
}

unsigned int WINAPI CIocpServer::WinCreateWorkerThread(LPVOID lpParam) {
	CIocpServer* iocpServer = reinterpret_cast<CIocpServer*>(lpParam);
	iocpServer->DoThread();
	return 0;
}

void CIocpServer::AsyncAccept() {

	// 나중에 메모리 풀 사용할 수 있도록 변경
	DWORD   dwBytes = 0, dwFlags = 0;

	CSession* session = CreateSubSession();

	OverlappedAcceptEx* overEx = new OverlappedAcceptEx(session);

	if (AcceptEx(m_listenSocket, session->GetSocket(), &m_acceptClientInfor, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes,
		(LPOVERLAPPED)overEx) == FALSE) {

		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "Error: AcceptEx " << WSAGetLastError() << "\n";
		}
	}

}

bool CIocpServer::RegitSocketInIOCP(SOCKET regitHandle, HANDLE iocpHandle) {

	HANDLE resultHandle = CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(regitHandle), iocpHandle, NULL, NULL);

	if (resultHandle != iocpHandle)
		return false;

	return true;
}

DWORD CIocpServer::DoThread() {
	// ?
	HANDLE iocpHandle = (HANDLE)m_iocpHandle;
	DWORD IoByte;
	LPOVERLAPPED  overlapped = nullptr;
	bool          IoEventResult = true;
#ifdef _WIN64
	ULONGLONG Iokey;
#else
	ULONG Iokey;
#endif //  _WIN64
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
		case IO_ACCEPT: {
			std::cout << "Client Accept\n";
			ProcessAccept(overEX, iocpHandle);
			break;
		}
		case IO_RECV: {
			// std::cout<<"Recv\n";
			ProcessRecv(overEX, IoByte);
			break;
		}
		case IO_SEND: {
			//nullptr 은 나중에 수정
			overEX->session->PostSend(nullptr, IoByte);
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

	std::cout << "Terminateed Work Thread\n";
	_endthreadex(0);
	return 0;
}

void CIocpServer::ProcessAccept(OverlappedEx* overEX, HANDLE iocpHandle) {

	// socket IOCP에 등록
	RegitSocketInIOCP(overEX->session->GetSocket(), iocpHandle);

	// Asny Accpet에 따른 패킷 전송 및 Server에 등록 (수정 요망)
	ProcessAsyncAccpet(*(overEX->session));

	// Call Recv
	overEX->session->DoRecv();

	// call Async Accpet
	AsyncAccept();
}

void CIocpServer::ProcessRecv(OverlappedEx* overEX, DWORD ioByte) {

	int rest = ioByte;
	char* ptr = overEX->session->GetRecvReadBuffer();
	PacketSize packet_size = 0;

	if (overEX->session->GetPrevSize() > 0) {
		packet_size = overEX->session->GetPrevSize();
	}

	while (0 < rest) {

		if (0 == packet_size) {
			packet_size = ptr[0];
		}

		//받아야 하는 패킷 크기
		int required = packet_size - overEX->session->GetPrevSize();

		//[21.03.25] 여기부분은 진짜 네트워크 타 봐야 버그 체크 가능
		if (required <= rest) {
			//유저 버퍼에 링버퍼의 값을 복사해온다
			memcpy_s(overEX->session->GetUserBuffer() + overEX->session->GetPrevSize(), BUFFER_SIZE, ptr, required);

			//BUFFSZE_SIZE 크기만큼 링버퍼 Index 이동
			overEX->session->SetRecvBufferReadIndex(BUFFER_SIZE);

			//서브 클래스별로 작동 
			overEX->session->ProcessIO();

			rest -= required;
			ptr += required;
			overEX->session->SetPrevSize(0);
			packet_size = 0;
		}

		else {
			memcpy_s(overEX->session->GetUserBuffer() + packet_size, BUFFER_SIZE, ptr, rest);
			overEX->session->SetPrevSize(overEX->session->GetPrevSize() + rest);

			//받은 크기만큼 링버퍼 Index 이동
			overEX->session->SetRecvBufferReadIndex(rest);
			rest = 0;
		}
	}

}
