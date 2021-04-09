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

	//���� �����Ͱ� �ִ��� üũ
	//���δ� ������ �ƴϴ�.
	// if(���� ��Ŷ�� <ioByte)
	if (false) {

		// refī���� �� ����
		// �� ī���Ͱ� ��� �̻� �Ѿ�� �� ������ �ڸ���.
		InCrementSendRef();


		//��Ŷ�� ���������� �ٽ� Send
		DoSend(packet);
	}

	else {
		// refī���� ����
		DeCrementSendRef();

		// SendQueue Pop
		delete packet;

		// std::cout << "Before Delete\n";
		// delete m_sendQueue.front();
		// std::cout << "After Delete\n";

		 // Queue�� �����ִٸ� ������ ����
		void* ptr = nullptr;
		if (m_sendQueue.try_pop(ptr)) {
			DoSend(ptr);
		}
		//�ƴ϶�� sendComplet true
		else {
			m_amSendComplte.store(true);
		}
	}
}

bool CSession::DoSend(void* packet) {
	DWORD dwBytes = 0, dwFlags = 0;
	int sendReturn = 0;

	char* p = reinterpret_cast<char*>(packet);

	//�̻��� ��Ŷ�� �ֳ� üũ
	if (p[0] <= 0) {
		std::cout << "Error packet Size Session: " << GetClientID() << "\n";

		PostSend(NULL, 0);
		return true;
	}

	OverlappedSendEx* overEX = new OverlappedSendEx(this);

#ifdef _USED_RING_BUFFER
	//�����ۿ��� ��� ������ ���� ������
	overEX->wsabuf.buf = m_sendBuffer.GetWriteIndex();
	//��Ŷ ������
	overEX->wsabuf.len = p[0];
	// ������ ��Ŷ������ ��ŭ Index �̵�
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