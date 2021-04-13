#pragma once
#include <vector>
#include <map>
#include"IocpServer.h"
#include"ObjectPool.h"

// 이 클래스가 채팅의 모든걸 관리한다.
class CRoom;
class CSession;

class CChatServer : public CIocpServer {

	typedef void (CChatServer::* MFP)(const CSession*, void*);

	//typedef Iterator
	using ConstRoomsIter	= std::vector<CRoom*>::const_iterator;
	using RoomsIter			= std::vector<CRoom*>::iterator;
	using ConstUsersIter	= std::vector<CSession*>::const_iterator;
	using UsersIter			= std::vector<CSession*>::iterator;

	//typedef Smart Pointer
	using UptrRoom			= std::unique_ptr<CRoom>;

	//typedef Objectpool
	//using UptrSessionPool	= std::unique_ptr<CObjectPool<CSession>>;


	//Function Number
	using FN				= char;

	using PairProcessPacket	= std::pair<FN, MFP>;

private:
	std::vector<CSession*>  m_users;
	Lock                    m_usersLock;

	UptrRoom				m_roomHandle;
	
	std::atomic<ClientID>	m_amClientIndex;

	std::map<FN, MFP>       m_processPacket;

	//UptrSessionPool			m_sessionPool;


	void Send_Message(const CSession* user, void* packetBuffer = nullptr);
	void Send_ChangedRoom(const CSession* user, void* packetBuffer = nullptr);
	void Send_WhisperMessage(const CSession* user, void* packetBuffer = nullptr);
	void Send_LoginOK(const CSession* user, void* packetBuffer = nullptr);

public:
	CChatServer(const CChatServer&)				= delete;
	CChatServer& operator=(const CChatServer&)	= delete;
	CChatServer(CChatServer&&)					= delete;
	CChatServer&& operator=(CChatServer&&)		= delete;


	void InitSubServer() override;

	void RunSubServer() override;

	void ProcessAsyncAccpet(CSession& session)override;

	void ProcessSocketIO(void* packet, const CSession& session)override;

	CSession* CreateSubSession()override;

	CChatServer();
	~CChatServer();
};
