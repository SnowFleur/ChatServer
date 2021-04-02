#pragma once
#include<queue>
#include"WinSocketHeader.h"
#include"ObjectPool.h"
#include"Protocol.h"
#include"Lock.h"

class CWorkerThread;
class CBaseServer;
class OverlappedEx;
class CSession;
class PacketHeader;

/*
Server 큐는 그냥 넘겨주는거 하고 따로 후처리 하지말고
Sessino 큐에서 후처리 하자
*/

typedef std::pair<CSession*,PacketHeader*> SendData;

// 1 4 4 4 4 
class CIocpServer{
private:
    char                          m_acceptClientInfor[50];
    HANDLE                        m_iocpHandle;
    SOCKET                        m_listenSocket;
    HANDLE                        m_workerThreads[MAX_WORKER_THREAD];
    CIocpServer*                  m_subServerComponent;
    DWORD                         m_workthreadCount;
 
    CIocpServer(const CIocpServer&);
    CIocpServer& operator=(const CIocpServer&);

    void ReleaseWorkerThreadHandle();
    void WaitForWorkerThreads();

    //Thread Function 
    virtual DWORD DoThread();
    static unsigned int WINAPI WinCreateWorkerThread(LPVOID lpParam);

    virtual DWORD SendThread();
    static unsigned int WINAPI WinCreateNetworkThread(LPVOID lpParam);
    

    //Main Class used Functions
    void  ProcessAccept(OverlappedEx* overEX,HANDLE iocpHandle);
    void  ProcessRecv(OverlappedEx* overEX,DWORD ioByte);
    void  ProcessSend(OverlappedEx* overEX,DWORD ioByte);
    void  AsyncAccept();

    std::queue<void*>           m_SendPacketQueue;
    Lock                        m_SendPakcetQueueLock;
public:
    CIocpServer(CIocpServer* sub):
      m_iocpHandle(INVALID_HANDLE_VALUE),
      m_listenSocket(INVALID_SOCKET),
      m_subServerComponent(NULL){
      m_subServerComponent=sub;
    }

    ~CIocpServer(){}

    //Sub Class Used Functions
    virtual void InitSubServer()=0;
    virtual void RunSubServer()=0;
    virtual void ProcessAsyncAccpet(CSession& session)=0;
    virtual void ProcessSocketIO(void* packet,const CSession& session)=0;
    
    virtual CSession* CreateSubSession()=0; 

    //Main Class used Functions
    bool  ServerInitialize(DWORD createWorkThreadCount = MAX_WORKER_THREAD,const int port = SERVER_PORT,bool ignore=false);
    bool  StartWorkerThread();
    bool  RegitSocketInIOCP(SOCKET regitHandle,HANDLE iocpHandle);
    void  PushSendPacketQUeue(void* packet);
    //void  PushSendQueue(SendData* data);

    inline HANDLE GetIOCPHandle()const{return m_iocpHandle;}
    inline SOCKET GetListenSocket()const{return m_listenSocket;}

};