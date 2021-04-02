#include "Session.h"
#include "IocpServer.h"
#include "OverlappedEx.h"
#include <iostream>
/*
나중에 메모리풀 넣기
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
    LockGuard LockGuard(m_sendQueueLock);
    
	//일단 패스
	//  delete m_sendQueue.front();
    m_sendQueue.pop();

    // Queue가 남아있다면 다음꺼 전송
    if (m_sendQueue.empty() == false) {
      DoSend(m_sendQueue.front());
    }
    //아니라면 sendComplet true
    else {
      m_sendComplte = true;
    }
  }
}

/*
여기서 큐에 있는 데이터를 가져와서 WSASend를 실행시키고
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

  //링버퍼에서 사용 가능한 버퍼 가져옴
  overEX->wsabuf.buf = m_sendBuffer.GetWriteIndex();
  //패킷 사이즈
  overEX->wsabuf.len = p[0];
  // 보내는 패킷사이즈 만큼 Index 이동
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

  //한 개 이상의 패킷이 큐에 쌓여있고 다음 패킷의 전송이 가능하면 Send
  if(m_sendQueue.empty()==false && m_sendComplte){
    m_sendComplte=false;
    DoSend(m_sendQueue.front());
  }

}