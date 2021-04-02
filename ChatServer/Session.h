#pragma once
#include<queue>
#include"RingBuffer.h"
#include"WinSocketHeader.h"
/*
- �������� Network ��� ProcessIO �Լ��� �����ε� �Ͽ� ����Ѵ�.
- ��Ŷ Ÿ�Կ� �������� �ʵ��� void*�� ����Ѵ�.
*/
class CIocpServer;
class OverlappedEx;

//1(3) 4 4 ? ? ? 4 ? 4 ? ?
class CSession {
    typedef void*   Packet;
private:
    char                  m_userBuffer[BUFFER_SIZE]; //�� ���ۿ��� �����ؿ� ����(�ϴ� Recv��)
    SOCKET				  m_socket;
    DWORD                 m_prevSize;
    
    CRingBuffer<char>     m_recvBuffer;
    CRingBuffer<char>     m_sendBuffer;

    Lock                  m_sendRefLock;
    DWORD                 m_sendRefCount;

    Lock                  m_readRefLock;
    DWORD                 m_recvRefCount;

    Lock                  m_sendQueueLock;
    std::queue<void*>     m_sendQueue;
    volatile bool         m_sendComplte;

    CSession(const CSession&);
    CSession& operator=(const CSession&);
protected:
    CIocpServer*          m_serverHandle;
public:
    CSession(CIocpServer* server):
    m_socket(INVALID_SOCKET),
    m_recvBuffer(),
    m_sendBuffer(),
    m_prevSize(0),
    m_serverHandle(NULL),
    m_sendRefLock(),
    m_sendRefCount(0),
    m_readRefLock(),
    m_recvRefCount(0),
    m_sendComplte(true){
        m_serverHandle=server;
        m_socket=WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        ZeroMemory(m_userBuffer,sizeof(BUFFER_SIZE));
    }

    virtual ~CSession(){}

    inline SOCKET   GetSocket()const{return m_socket;}
    inline void     SetSocket(const SOCKET socket){m_socket=socket;}

    void    ProcessAccept();
    void    ProcessRecv();
    bool    DoRecv();
    void    PostSend(void *packet,DWORD ioByte);
    bool    DoSend(void* packet);
    void    PushSendQueue(void* packet);


    //��� ������ ���� Index Return (Buffer ���� �Լ���)
    char*   GetRecvReadBuffer() { return m_recvBuffer.GetReadIndex(); }
    char*   GetRecvWriteBuffer() { return m_recvBuffer.GetWriteIndex(); }
    char*   GetSendReadBuffer() { return m_sendBuffer.GetReadIndex(); }
    char*   GetSendWriteBuffer() { return m_sendBuffer.GetWriteIndex(); }

    void    SetRecvBufferWriteIndex(const int index){m_recvBuffer.SetWriteIndex(index);}
    void    SetRecvBufferReadIndex(const int index){m_recvBuffer.SetReadIndex(index);}
    inline DWORD   GetPrevSize()const{return m_prevSize;}
    inline char*   GetUserBuffer(){return m_userBuffer;}
    inline void    SetPrevSize(const int prevSize){m_prevSize=prevSize;}

    inline void InCrementSendRef(){LockGuard lockguard(m_sendRefLock); ++m_sendRefCount;}
    inline void DeCrementSendRef(){LockGuard lockguard(m_sendRefLock);--m_sendRefCount;}
    
    inline void InCrementRecvRef(){LockGuard lockguard(m_readRefLock); ++m_recvRefCount;}
    inline void DeCrementRecvRef(){LockGuard lockguard(m_readRefLock); --m_recvRefCount;}

    virtual void   ProcessIO()=0;
    virtual void   SetClientID(const ClientID)=0;
    virtual ClientID GetClientID()const=0;
};