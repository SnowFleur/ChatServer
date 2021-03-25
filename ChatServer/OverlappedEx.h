#pragma once
#include"WinSocketHeader.h"

class CSession;

struct OverlappedEx {
  OVERLAPPED    overlapped;
  WSABUF        wsabuf;
  IO_EVENT      ioEvent;
  CSession*     session;

  OverlappedEx(CSession* s,IO_EVENT io) : session(NULL), ioEvent(io){
    ZeroMemory(&overlapped, sizeof(overlapped));
    session=s;
  }
};

struct OverlappedAcceptEx : public OverlappedEx {
  OverlappedAcceptEx(CSession* s) : OverlappedEx(s,IO_ACCEPT){
    wsabuf.buf = NULL;
    wsabuf.len = 0;
  }
};

struct OverlappedSendEx : public OverlappedEx {
  OverlappedSendEx(CSession* s) : OverlappedEx(s,IO_SEND) {}
};

struct OverlappedRecvEx : public OverlappedEx {
  OverlappedRecvEx(CSession* s) : OverlappedEx(s,IO_RECV) {}
};