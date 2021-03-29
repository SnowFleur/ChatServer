#pragma once
#include"Session.h"
#include"DataDefine.h"
class CBaseServer;

class CUser : public CSession {
private:
  ClientID    m_myId;
  RoomNumber  m_currentRoomNumber;  //Room Class에서만 Write 그 외에는 X
public:
  CUser(CBaseServer* server):CSession(server){}
  ~CUser() {}

  void ProcessIO() override;
  void ProcessPacket(void *packet);

  ClientID GetClientID()const override{return m_myId;}
  void     SetClientID(const ClientID id )override{m_myId=id;}

  inline RoomNumber GetCurrentRoomNumber()const{return m_currentRoomNumber;}
  inline void SetCurrentRoomNumber(const RoomNumber number){m_currentRoomNumber=number;}

};