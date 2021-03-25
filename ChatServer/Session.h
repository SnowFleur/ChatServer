#pragma once
#include"RingBuffer.h"
#include"WinSocketHeader.h"
#include"OverlappedEx.h"

/*
�������� Network ��� ProcessIO �Լ��� �����ε� �Ͽ� ����Ѵ�.
*/

class CBaseServer;

class CSession {
private:
    SOCKET				  m_socket;
    CRingBuffer<char>     m_recvBuffer;
    CRingBuffer<char>     m_sendBuffer;
    char                  m_userBuffer[BUFFER_SIZE]; //�� ���ۿ��� �����ؿ� ����(�ϴ� Recv��)
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

    //��� ������ ���� Index Return
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