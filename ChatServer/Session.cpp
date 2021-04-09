#include <iostream>

#include "Session.h"
#include "IocpServer.h"
#include "OverlappedEx.h"


bool CSession::DoRecv() {
	DWORD dwBytes = 0, dwFlags = 0;
	int recvReturn = 0;

	OverlappedRecvEx* overEX = new OverlappedRecvEx(this);


#ifdef _USED_RING_BUFFER
	overEX->wsabuf.buf = m_recvBuffer.GetWriteIndex();
	overEX->wsabuf.len = BUFFER_SIZE;
#else
	overEX->wsabuf.buf = overEX->m_buffer;
	overEX->wsabuf.len = BUFFER_SIZE;
#endif

	m_recvBuffer.SetWriteIndex(BUFFER_SIZE);


	InCrementRecvRef();

	recvReturn = WSARecv(m_socket, &overEX->wsabuf, 1, &dwBytes, &dwFlags, (LPWSAOVERLAPPED)overEX, 0);
	if (recvReturn == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "Error: Recv  " << WSAGetLastError() << "\n";

		}
	}
	return true;
}

void CSession::PostSend(void* packet, DWORD ioByte) {

	//남은 데이터가 있는지 체크
	//전부다 보낸게 아니다.
	// if(보낸 패킷량 <ioByte)
	if (false) {

		// ref카운터 또 증가
		// 이 카운터가 어느 이상 넘어가면 그 세션을 자른다.
		InCrementSendRef();


		//패킷이 남아있으니 다시 Send
		DoSend(packet);
	}

	else {
		// ref카운터 감소
		DeCrementSendRef();

		// SendQueue Pop
		delete packet;

		// std::cout << "Before Delete\n";
		// delete m_sendQueue.front();
		// std::cout << "After Delete\n";

		 // Queue가 남아있다면 다음꺼 전송
		void* ptr = nullptr;
		if (m_sendQueue.try_pop(ptr)) {
			DoSend(ptr);
		}
		//아니라면 sendComplet true
		else {
			m_amSendComplte.store(true);
		}
	}
}

bool CSession::DoSend(void* packet) {
	DWORD dwBytes = 0, dwFlags = 0;
	int sendReturn = 0;

	char* p = reinterpret_cast<char*>(packet);

	//이상한 패킷이 있나 체크
	if (p[0] <= 0) {
		std::cout << "Error packet Size Session: " << GetClientID() << "\n";

		PostSend(NULL, 0);
		return true;
	}

	OverlappedSendEx* overEX = new OverlappedSendEx(this);

#ifdef _USED_RING_BUFFER
	//링버퍼에서 사용 가능한 버퍼 가져옴
	overEX->wsabuf.buf = m_sendBuffer.GetWriteIndex();
	//패킷 사이즈
	overEX->wsabuf.len = p[0];
	// 보내는 패킷사이즈 만큼 Index 이동
	m_sendBuffer.SetWriteIndex(p[0]);
#else
	overEX->wsabuf.buf = overEX->m_buffer;
	overEX->wsabuf.len = p[0];
#endif

	memcpy_s(overEX->wsabuf.buf, overEX->wsabuf.len, packet, p[0]);

	InCrementSendRef();

	sendReturn = WSASend(m_socket, &overEX->wsabuf, 1, &dwBytes, dwFlags,
		(LPWSAOVERLAPPED)overEX, NULL);
	if (sendReturn == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			std::cout << "Error: Send  " << WSAGetLastError() << "\n";
		}
	}

	return true;
}

void CSession::PushSendQueue(void* packet) {
	m_sendQueue.push(packet);

	void* ptr = nullptr;
	if (m_sendQueue.try_pop(ptr) && m_amSendComplte) {
		m_amSendComplte.store(false);
		DoSend(ptr);
	}
}