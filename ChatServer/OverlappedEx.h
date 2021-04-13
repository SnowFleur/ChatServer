#pragma once
#include"WinSocketHeader.h"

class CSession;

class OverlappedEx {
public:
	OVERLAPPED		overlapped;
	WSABUF			wsabuf;
	IO_EVENT		ioEvent;
	CSession*		session;
	char			m_buffer[BUFFER_SIZE];

	OverlappedEx(CSession* s, IO_EVENT io) : session(NULL), ioEvent(io) {
		ZeroMemory(&overlapped, sizeof(overlapped));
		session = s;
	}
};

class OverlappedAcceptEx : public OverlappedEx {
public:
	OverlappedAcceptEx(CSession* s) : OverlappedEx(s, IO_ACCEPT) {
		wsabuf.buf = NULL;
		wsabuf.len = 0;
	}
};

class OverlappedSendEx : public OverlappedEx {
public:
	OverlappedSendEx(CSession* s) : OverlappedEx(s, IO_SEND) {}
};

class OverlappedRecvEx : public OverlappedEx {
public:
	OverlappedRecvEx(CSession* s) : OverlappedEx(s, IO_RECV) {}
};

class OverlappedConnetEx : public OverlappedEx {
public:
	OverlappedConnetEx(CSession* s) : OverlappedEx(s, IO_CONNECT) {}
};