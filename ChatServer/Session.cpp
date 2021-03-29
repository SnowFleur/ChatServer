#include<iostream>
#include "Session.h"
#include"OverlappedEx.h"
#include"IocpManager.h"
#include"SessionManager.h"
/*
���߿� �޸�Ǯ �ֱ�
*/
bool CSession::DoRecv() {
  DWORD   dwBytes = 0, dwFlags = 0;
	int		  recvReturn = 0;

  OverlappedRecvEx* overEX=new OverlappedRecvEx(this);

  overEX->wsabuf.buf = m_recvBuffer.GetWriteIndex();
  overEX->wsabuf.len = BUFFER_SIZE;

  m_recvBuffer.SetWriteIndex(BUFFER_SIZE);

  recvReturn = WSARecv(m_socket, &overEX->wsabuf, 1, &dwBytes, &dwFlags,(LPWSAOVERLAPPED)overEX, 0);
  if (recvReturn == SOCKET_ERROR) {
    if (WSAGetLastError() != WSA_IO_PENDING) {
      std::cout << "Error: Recv  " << WSAGetLastError() << "\n";
    }
  }
  return true;
}

bool CSession::DoSend(void* packet) {
  DWORD   dwBytes = 0, dwFlags = 0;
	int		  sendReturn = 0;

  char *p = reinterpret_cast<char *>(packet);

  OverlappedSendEx* overEX = new OverlappedSendEx(this);

  //�����ۿ��� ��� ������ ���� ������
  overEX->wsabuf.buf = m_sendBuffer.GetWriteIndex();
  //��Ŷ ������
  overEX->wsabuf.len = p[0];
  // ������ ��Ŷ������ ��ŭ Index �̵�
  m_sendBuffer.SetWriteIndex(p[0]);

  memcpy_s(overEX->wsabuf.buf,overEX->wsabuf.len, packet, p[0]);

 sendReturn=WSASend(m_socket, &overEX->wsabuf, 1, &dwBytes, dwFlags,(LPWSAOVERLAPPED)overEX,NULL);
 if (sendReturn == SOCKET_ERROR) {
   if (WSAGetLastError() != WSA_IO_PENDING) {
     std::cout << "Error: Send  " << WSAGetLastError() << "\n";
   }
 }

 return true;
}

