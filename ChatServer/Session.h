#pragma once

#include<queue>
#include<concurrent_queue.h>
#include"RingBuffer.h"
#include"WinSocketHeader.h"
/*
- 전반적인 Network 담당 ProcessIO 함수를 오버로드 하여 사용한다.
- 패킷 타입에 의존하지 않도록 void*를 사용한다.
*/
class CIocpServer;
class OverlappedEx;

template<class T>
using LockFreeQueue = Concurrency::concurrent_queue<T>;

//1(3) 4 4 ? ? ? 4 ? 4 ? ?
class CSession {
	typedef void* Packet;
private:
	char                  m_userBuffer[BUFFER_SIZE]; //링 버퍼에서 복사해올 버퍼(일단 Recv만)
	SOCKET				  m_socket;
	DWORD                 m_prevSize;

	CRingBuffer<char>     m_recvBuffer;
	CRingBuffer<char>     m_sendBuffer;

	Lock                  m_sendRefLock;
	DWORD                 m_sendRefCount;

	Lock                  m_readRefLock;
	DWORD                 m_recvRefCount;

	LockFreeQueue<void*>  m_sendQueue;
	std::atomic<bool>     m_amSendComplte;
protected:
	CIocpServer* m_serverHandle;
public:
	Lock                  m_sessionLock;

	CSession& operator=(const CSession&) = delete;
	CSession&& operator=(CSession&&) = delete;
	CSession(const CSession&) = delete;
	CSession(CSession&&) = delete;

	CSession(CIocpServer* server) :
		m_socket(INVALID_SOCKET),
		m_recvBuffer(),
		m_sendBuffer(),
		m_prevSize(0),
		m_serverHandle(NULL),
		m_sendRefLock(),
		m_sendRefCount(0),
		m_readRefLock(),
		m_recvRefCount(0),
		m_amSendComplte(true) {
		m_serverHandle = server;
		m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		ZeroMemory(m_userBuffer, sizeof(BUFFER_SIZE));
	}

	virtual ~CSession() {}

	inline SOCKET   GetSocket()const { return m_socket; }
	inline void     SetSocket(const SOCKET socket) { m_socket = socket; }

	bool    DoRecv();
	void    PostSend(void* packet, DWORD ioByte);
	bool    DoSend(void* packet);
	void    PushSendQueue(void* packet);


	//사용 가능한 버퍼 Index Return (Buffer 관련 함수들)
	char* GetRecvReadBuffer() { return m_recvBuffer.GetReadIndex(); }
	char* GetRecvWriteBuffer() { return m_recvBuffer.GetWriteIndex(); }
	char* GetSendReadBuffer() { return m_sendBuffer.GetReadIndex(); }
	char* GetSendWriteBuffer() { return m_sendBuffer.GetWriteIndex(); }

	void    SetRecvBufferWriteIndex(const int index) { m_recvBuffer.SetWriteIndex(index); }
	void    SetRecvBufferReadIndex(const int index) { m_recvBuffer.SetReadIndex(index); }

	inline DWORD	GetPrevSize()const { return m_prevSize; }
	inline char*	GetUserBuffer() { return m_userBuffer; }
	inline void		SetPrevSize(const int prevSize) { m_prevSize = prevSize; }

	inline void InCrementSendRef() { LockGuard lockguard(m_sendRefLock); ++m_sendRefCount; }
	inline void DeCrementSendRef() { LockGuard lockguard(m_sendRefLock);--m_sendRefCount; }

	inline void InCrementRecvRef() { LockGuard lockguard(m_readRefLock); ++m_recvRefCount; }
	inline void DeCrementRecvRef() { LockGuard lockguard(m_readRefLock); --m_recvRefCount; }

	virtual void   ProcessIO() = 0;
	virtual void   SetClientID(const ClientID) = 0;
	virtual ClientID GetClientID()const = 0;
};