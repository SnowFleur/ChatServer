#pragma once
#include"WinSocketHeader.h"

class CWorkerThread;
class CBaseServer;

class CIocpManager{
private:
    HANDLE            m_iocpHandle;
    SOCKET            m_listenSocket;
    CWorkerThread*    m_workerThreads[MAX_WORKER_THREAD];
    CBaseServer*      m_serverHandle;
    char              m_acceptClientInfor[100];

    CIocpManager(const CIocpManager&);
    CIocpManager& operator=(const CIocpManager&);
    CIocpManager():m_iocpHandle(INVALID_HANDLE_VALUE),m_listenSocket(INVALID_SOCKET),m_serverHandle(NULL){}

    void ReleaseWorkerThreadHandle();
    void WaitForWorkerThreads();
public:
    static CIocpManager* m_instance;
    
    static CIocpManager *GetInstance() {
      if (m_instance == NULL) {
        m_instance = new CIocpManager();
      }
      return m_instance;
    };

    static void ReleaseInstance(){
        if(m_instance!=NULL){
          m_instance->ReleaseWorkerThreadHandle();

          if(m_instance->m_serverHandle!=NULL)
          delete m_instance->m_serverHandle;

          delete m_instance;
          m_instance=NULL;
        }
    }

    ~CIocpManager(){}
    
    bool Initialize(CBaseServer* server);
    bool StartWorkerThread();
    bool RegitSocketInIOCP(SOCKET regitHandle);
    void AsyncAccept();
    void RegitServerHandle(CBaseServer* server);

    inline HANDLE GetIOCPHandle()const{return m_iocpHandle;}
    inline SOCKET GetListenSocket()const{return m_listenSocket;}
    inline CBaseServer* GetServerHandle()const{return m_serverHandle;}

    static unsigned WINAPI WinCreateThread(LPVOID lpParam);
};