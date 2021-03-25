#include<iostream>
#include"IocpManager.h"
#include"WorkerThread.h"
#include"OverlappedEx.h"
#include"Session.h"
#include"User.h"
#include"BaseServer.h"

CIocpManager* CIocpManager::m_instance=NULL;

void CIocpManager::ReleaseWorkerThreadHandle(){
  delete[] m_workerThreads;
}

bool CIocpManager::Initialize(CBaseServer* server){

  m_serverHandle = server;
  if(m_serverHandle==NULL){
    std::cout << "Error: serverHandle Is null\n";
  return false;
  }

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
  if (RegitSocketInIOCP(m_listenSocket) == false) {
    std::cout << "Error Register In IOCP\n";
  }

  SOCKADDR_IN serverAddr;
  ZeroMemory(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_port = SERVER_PORT;
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

  //listen Accpet 막기
  BOOL on = true;
  setsockopt(m_listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT,
             reinterpret_cast<char *>(&on), sizeof(on));

  //비동기 Accept걸기 
  AsyncAccept();
  
  return true;
}

bool CIocpManager::StartWorkerThread() {
  for (int i = 0; i < MAX_WORKER_THREAD; ++i) {
    DWORD NumberOfWork = 10000 + i;

    // C/C++ 라이브러리를 사용한다면 BeginThreadEx를 사용해야한다.
    HANDLE handle=(HANDLE)_beginthreadex(NULL, 0, WinCreateThread, (LPVOID)i, 0, (unsigned*)&NumberOfWork);
    m_workerThreads[i] = new CWorkerThread(handle, m_iocpHandle);
  }

  //Server Start!!
  m_serverHandle->Init();
  m_serverHandle->Run();

  return true;
}

void CIocpManager::WaitForWorkerThreads(){
   for(int i=0;i<MAX_WORKER_THREAD;++i){
    WaitForSingleObject(m_workerThreads[i]->GetTheradHandle(),INFINITE);
    CloseHandle(m_workerThreads[i]->GetTheradHandle());
  }
}

unsigned WINAPI CIocpManager::WinCreateThread(LPVOID lpParam) {
  int threadId = (reinterpret_cast<int>(lpParam));
  return CIocpManager::GetInstance()->m_workerThreads[threadId]->DoThread();
}

void CIocpManager::AsyncAccept(){

    // 나중에 메모리 풀 사용할 수 있도록 변경
    DWORD   dwBytes = 0, dwFlags = 0;
    CSession* session=new CUser(m_serverHandle);
    OverlappedAcceptEx* overEx=new OverlappedAcceptEx(session);

    if (AcceptEx(m_listenSocket,session->GetSocket(), &m_acceptClientInfor, 0,
        sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes,
       (LPOVERLAPPED)overEx) == FALSE) {

        if (WSAGetLastError() != WSA_IO_PENDING) {
            std::cout << "Error: AcceptEx "<<WSAGetLastError()<<"\n";
        }
    }
}

bool CIocpManager::RegitSocketInIOCP(SOCKET regitHandle) {
  // IOCP에 Socket(HANDLE)등록

  //Thread Safe??
  HANDLE resultHandle = CreateIoCompletionPort(
      reinterpret_cast<HANDLE>(regitHandle), m_iocpHandle, NULL, NULL);

  if (resultHandle != m_iocpHandle)
    return false;

  return true;
}


void CIocpManager::RegitServerHandle(CBaseServer* server){
    //IOCP에 Server 등록
    m_serverHandle=server;
}
