#pragma once
#include"Session.h"
#include"DataDefine.h"
class CBaseServer;

class CUser : public CSession {
private:
  ClientID  m_myId;
public:
  CUser(CBaseServer* server):CSession(server){}
  ~CUser() {}

  void ProcessIO() override;
  void ProcessPacket(void *packet);

  inline void SetMyID(const ClientID id){m_myId=id;}
  inline ClientID GetMyID()const{return m_myId;}

};