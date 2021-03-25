#pragma once
#include"RingBuffer.h"
#include"WinSocketHeader.h"
#include"OverlappedEx.h"

/*
전반적인 Network 담당 ProcessIO 함수를 오버로드 하여 사용한다.
*/

class CBaseServer;

class CSession {
private:
    SOCKET				  m_socket;
    CRingBuffer<char>     m_recvBuffer;
    CRingBuffer<char>     m_sendBuffer;
    char                  m_userBuffer[BUFFER_SIZE]; //링 버퍼에서 복사해올 버퍼(일단 Recv만)
    DWORD                 m_prevSize;
    CSession(const CSession&);
    CSession& operator=(const CSession&);
protected:
    CBaseServer*          m_serverHandle;
public:
    CSession(CBaseServer* server):m_socket(INVALID_SOCKET)
    ,m_recvBuffer(),m_sendBuffer(),m_prevSize(0),m_serverHandle(NULL){
        
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
    bool    DoSend(void* packet);

    //사용 가능한 버퍼 Index Return
    char*   GetRecvReadBuffer() { return m_recvBuffer.GetReadIndex(); }
    char*   GetRecvWriteBuffer() { return m_recvBuffer.GetWriteIndex(); }
    char*   GetSendReadBuffer() { return m_sendBuffer.GetReadIndex(); }
    char*   GetSendWriteBuffer() { return m_sendBuffer.GetWriteIndex(); }

    void    SetRecvBufferWriteIndex(const int index){m_recvBuffer.SetWriteIndex(index);}
    void    SetRecvBufferReadIndex(const int index){m_recvBuffer.SetReadIndex(index);}

    inline DWORD   GetPrevSize()const{return m_prevSize;}
    inline char*   GetUserBuffer(){return m_userBuffer;}

    inline void    SetPrevSize(const int prevSize){m_prevSize=prevSize;}

    virtual void   ProcessIO()=0;

};