#include "Session.h"
#include "IocpServer.h"
#include "OverlappedEx.h"
#include <iostream>
/*
���߿� �޸�Ǯ �ֱ�
*/
bool CSession::DoRecv() {
  DWORD dwBytes = 0, dwFlags = 0;
  int recvReturn = 0;

  OverlappedRecvEx *overEX = new OverlappedRecvEx(this);

  overEX->wsabuf.buf = m_recvBuffer.GetWriteIndex();
  overEX->wsabuf.len = BUFFER_SIZE;

  m_recvBuffer.SetWriteIndex(BUFFER_SIZE);

  InCrementRecvRef();

  recvReturn = WSARecv(m_socket, &overEX->wsabuf, 1, &dwBytes, &dwFlags,
                       (LPWSAOVERLAPPED)overEX, 0);
  if (recvReturn == SOCKET_ERROR) {
    if (WSAGetLastError() != WSA_IO_PENDING) {
      std::cout << "Error: Recv  " << WSAGetLastError() << "\n";
    }
  }
  return true;
}

void CSession::PostSend(void *packet, DWORD ioByte) {

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
    LockGuard LockGuard(m_sendQueueLock);
    
	//�ϴ� �н�
	//  delete m_sendQueue.front();
    m_sendQueue.pop();

    // Queue�� �����ִٸ� ������ ����
    if (m_sendQueue.empty() == false) {
      DoSend(m_sendQueue.front());
    }
    //�ƴ϶�� sendComplet true
    else {
      m_sendComplte = true;
    }
  }
}

/*
���⼭ ť�� �ִ� �����͸� �����ͼ� WSASend�� �����Ű��
*/

bool CSession::DoSend(void *packet) {
  DWORD dwBytes = 0, dwFlags = 0;
  int sendReturn = 0;

  char *p = reinterpret_cast<char *>(packet);

  if (p[0] <= 0) {
    std::cout << "Error"
              << "\n";
    return true;
  }

  OverlappedSendEx *overEX = new OverlappedSendEx(this);

  //�����ۿ��� ��� ������ ���� ������
  overEX->wsabuf.buf = m_sendBuffer.GetWriteIndex();
  //��Ŷ ������
  overEX->wsabuf.len = p[0];
  // ������ ��Ŷ������ ��ŭ Index �̵�
  m_sendBuffer.SetWriteIndex(p[0]);

  
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

void CSession::PushSendQueue(void* packet){
  LockGuard lockGurad(m_sendQueueLock);
  m_sendQueue.push(packet);

  //�� �� �̻��� ��Ŷ�� ť�� �׿��ְ� ���� ��Ŷ�� ������ �����ϸ� Send
  if(m_sendQueue.empty()==false && m_sendComplte){
    m_sendComplte=false;
    DoSend(m_sendQueue.front());
  }

}