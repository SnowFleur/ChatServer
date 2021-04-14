#pragma once 

#include "..\ChatServer\Session.h"


enum WORKS{
  DUMMY_RECY=1,DUMMY_MESSAGE,DUUMY_CHANG_ROOM,DUMMY_ACCEPT,
};

class CDummyClinet : public CSession {
private:
  ClientID    m_myId;
public:
  
  CDummyClinet(CIocpServer* server):
  CSession(server),
  m_myId(DEFAULT_CLINET_ID){}
  
  ~CDummyClinet() = default;

  void ProcessIO() override;

  ClientID GetClientID()const override{return m_myId;}
  void     SetClientID(const ClientID id )override{m_myId=id;}
};

