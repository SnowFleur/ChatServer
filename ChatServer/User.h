#pragma once
#include"Session.h"
class CBaseServer;

class CUser : public CSession {
private:
	ClientID    m_myId;

	//Room Class에서만 Write 그 외에는 X
	std::atomic<RoomNumber>	m_atRoomNumber;
public:
	CUser(CIocpServer* server) :
		CSession(server),
		m_atRoomNumber{} {}

	~CUser() = default;

  void ProcessIO() override;
  void ProcessPacket(void *packet);

  ClientID GetClientID()const override{return m_myId;}
  void     SetClientID(const ClientID id )override{m_myId=id;}

  inline  const std::atomic<RoomNumber>& GetCurrentRoomNumber()const{return m_atRoomNumber;}
  inline void SetCurrentRoomNumber(const  std::atomic<RoomNumber>number) { m_atRoomNumber.store(number); }

};