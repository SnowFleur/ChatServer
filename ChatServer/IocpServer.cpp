#include<iostream>
#include"IocpServer.h"
#include"OverlappedEx.h"
#include"Session.h"
#include"User.h"

void CIocpServer::ReleaseWorkerThreadHandle(){
  delete[] m_workerThreads;
}

bool CIocpServer::ServerInitialize(DWORD createWorkThreadCount, const int port,bool ignore) {

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
  m_listenSocket = WSASocket(AF_INET, SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
  if (m_listenSocket == INVALID_SOCKET) {
    std::cout << "Error: Can Not Init Listen Socket\n";
    WSACleanup();
    return false;
  }

  //Regit listenSocket
  if (RegitSocketInIOCP(m_listenSocket,m_iocpHandle) == false) {
    std::cout << "Error Register In IOCP\n";
  }
  
  if(ignore==false){

  SOCKADDR_IN serverAddr;
  ZeroMemory(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_port = port;
  serverAddr.sin_family = PF_INET;
  serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
  

  // Socket Bind
  if (bind(m_listenSocket, reinterpret_cast<SOCKADDR *>(&serverAddr),
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

  //listen Accpet ����
  BOOL on = true;
  setsockopt(m_listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT,
             reinterpret_cast<char *>(&on), sizeof(on));

  //�񵿱� Accept�ɱ� 
  AsyncAccept();
  }
  
  return true;
}

bool CIocpServer::StartWorkerThread() {
  for (int i = 0; i < m_workthreadCount; ++i) {
    DWORD NumberOfWork = 10000 + i;
    m_workerThreads[i]=(HANDLE)_beginthreadex(NULL, 0, &CIocpServer::WinCreateWorkerThread, this, 0, (unsigned*)&NumberOfWork);
  }

  // if(_beginthreadex(NULL, 0, &CIocpServer::WinCreateNetworkThread, this, 0, NULL)==-1)
  //   std::cout<<"Error: Not Create Thread\n";

  //Server Start!!
  InitSubServer();
  RunSubServer();

  return true;
}

void CIocpServer::WaitForWorkerThreads(){
  WaitForMultipleObjects(m_workthreadCount,m_workerThreads,TRUE,INFINITE);
}

unsigned int WINAPI CIocpServer::WinCreateWorkerThread(LPVOID lpParam) {
  CIocpServer* iocpServer=reinterpret_cast<CIocpServer*>(lpParam);
  iocpServer->DoThread();
  return 0;
}

unsigned int WINAPI CIocpServer::WinCreateNetworkThread(LPVOID lpParam) {
  CIocpServer* iocpServer=reinterpret_cast<CIocpServer*>(lpParam);
  iocpServer->SendThread();
  return 0;
}

void CIocpServer::AsyncAccept(){

    // ���߿� �޸� Ǯ ����� �� �ֵ��� ����
    DWORD   dwBytes = 0, dwFlags = 0;

    CSession* session=CreateSubSession();

    OverlappedAcceptEx* overEx=new OverlappedAcceptEx(session);

    if (AcceptEx(m_listenSocket,session->GetSocket(), &m_acceptClientInfor, 0,
       sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes,
      (LPOVERLAPPED)overEx) == FALSE) {

       if (WSAGetLastError() != WSA_IO_PENDING) {
           std::cout << "Error: AcceptEx "<<WSAGetLastError()<<"\n";
       }
    }

}

bool CIocpServer::RegitSocketInIOCP(SOCKET regitHandle,HANDLE iocpHandle) {

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
  LPOVERLAPPED  overlapped=NULL;
  bool          IoEventResult=true;
#ifdef _WIN64
  ULONGLONG Iokey;
#else
  ULONG Iokey;
#endif //  _WIN64
  while (true) {

   //GQCS�� �Ѿ�� Handle ���� �����? ---> local �ϰ� �ؼ� �ذ�
   IoEventResult = GetQueuedCompletionStatus(iocpHandle, &IoByte, &Iokey,&overlapped,INFINITE);

    if (IoEventResult == false) {
      if (overlapped == false && WSAGetLastError() != 64) {
        std::cout << WSAGetLastError() << "\n";
        return 0;
      }
    }

    OverlappedEx* overEX=reinterpret_cast<OverlappedEx*>(overlapped);

  //���߿� hash Ȥ�� �ٸ� �ڷᱸ���� �̿��� �߻�ȭ �ϱ�
  switch (overEX->ioEvent) {
    case IO_ACCEPT: {
      std::cout << "Client Accept\n";
      ProcessAccept(overEX,iocpHandle);
      break;
    }
    case IO_RECV: {
     // std::cout<<"Recv\n";
      ProcessRecv(overEX,IoByte);
      break;
    }
    case IO_SEND: {
      ProcessSend(overEX,IoByte);
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

  std::cout<<"Terminateed Work Thread\n";
   _endthreadex( 0 );
  return 0;
}

void CIocpServer::PushSendPacketQUeue(void* packet){
  LockGuard lockguard(m_SendPakcetQueueLock);
  m_SendPacketQueue.push(packet);
}

DWORD CIocpServer::SendThread(){
  //[21.04.1] ���ʿ��� �����Ҵ��� �ʹ� ����

  // SendData* sendData=NULL;


  // while (true) {
  //   //Lock Guard���ٴ� �� ���� release�ϱ� ���� �׳� Lock ���
  //   m_SendPakcetQueueLock.AcquiredLock();
  //   if (m_SendPacketQueue.empty()) {
  //     m_SendPakcetQueueLock.ReleaseLock();
  //     Sleep(1);
  //     continue;
  //   }

  //   if(sendData!=NULL){
  //       delete sendData;
  //       sendData=NULL;
  //   }

  //   sendData = m_SendPacketQueue.front();
  //   m_SendPakcetQueueLock.ReleaseLock();

  //   //Send ó��
  //   sendData->first->DoSend(sendData->second);
  // }

  //  //thread loop

	return 0;
}


void CIocpServer::ProcessAccept(OverlappedEx *overEX,HANDLE iocpHandle) {

  // socket IOCP�� ���
  RegitSocketInIOCP(overEX->session->GetSocket(),iocpHandle);

  // Asny Accpet�� ���� ��Ŷ ���� �� Server�� ��� (���� ���)
  ProcessAsyncAccpet(*(overEX->session));

  // Call Recv
  overEX->session->DoRecv();

  // call Async Accpet
  AsyncAccept();
}

void CIocpServer::ProcessRecv(OverlappedEx *overEX,DWORD ioByte) {

  int rest = ioByte;
  char* ptr= overEX->session->GetRecvReadBuffer();
  PacketSize packet_size = 0;

  if (overEX->session->GetPrevSize() > 0) {
    packet_size = overEX->session->GetPrevSize();
  }

  while (0 < rest) {

    if (0 == packet_size){
      packet_size = ptr[0];
    }

    //�޾ƾ� �ϴ� ��Ŷ ũ��
    int required = packet_size - overEX->session->GetPrevSize();

    //[21.03.25] ����κ��� ��¥ ��Ʈ��ũ Ÿ ���� ���� üũ ����
    if (required <= rest) {
      //���� ���ۿ� �������� ���� �����ؿ´�
      memcpy_s(overEX->session->GetUserBuffer()+overEX->session->GetPrevSize(),BUFFER_SIZE,ptr,required);
      
      //BUFFSZE_SIZE ũ�⸸ŭ ������ Index �̵�
      overEX->session->SetRecvBufferReadIndex(BUFFER_SIZE);

      //���� Ŭ�������� �۵� 
      overEX->session->ProcessIO();

      rest -= required;
      ptr += required;
      overEX->session->SetPrevSize(0);
      packet_size=0;
    }

    else {
      memcpy_s(overEX->session->GetUserBuffer() + packet_size, BUFFER_SIZE, ptr,rest);
      overEX->session->SetPrevSize(overEX->session->GetPrevSize()+rest);

      //���� ũ�⸸ŭ ������ Index �̵�
      overEX->session->SetRecvBufferReadIndex(rest);
      rest = 0;
    }
  }
  
}

void CIocpServer::ProcessSend(OverlappedEx* overEX,DWORD ioByte){

    overEX->session->PostSend(NULL,ioByte);

  //���� �����Ͱ� �ִ��� üũ
  // //���δ� ������ �ƴϴ�.
  // if(false){
    
  //   // refī���� �� ����
  //   // �� ī���Ͱ� ��� �̻� �Ѿ�� �� ������ �ڸ���.
  //   overEX->session->InCrementSendRef();

  // }

  // //���� �����Ͱ� ����
  // else {
  //   // refī���� ����
  //   overEX->session->DeCrementSendRef();

  //   // SendQueue Pop
  //   LockGuard LockGuard(m_SendPakcetQueueLock);
  //   delete m_SendQueue.front()->second;
  //   m_SendQueue.pop();
  // }

}