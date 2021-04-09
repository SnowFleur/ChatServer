#include"ChatServer.h"
#include"Session.h"
#include"Room.h"
#include"Protocol.h"
#include"WinSocketHeader.h"
#include"User.h"

CChatServer::CChatServer() :
	CIocpServer(this),
	m_roomHandle() {}

CChatServer::~CChatServer() {}

void CChatServer::InitSubServer() {
	m_amClientIndex.store(0);

	//생성할 방 개수
	m_roomHandle = std::make_unique<CRoom>(CHAT_ROOM_COUNT);

	// 패킷 프로세스 등록
	m_processPacket.emplace(PairProcessPacket(PACKET_TYPE::CS_SEND_MESSAGE, &CChatServer::Send_Message));
	m_processPacket.emplace(PairProcessPacket(PACKET_TYPE::CS_CHANGED_ROOM, &CChatServer::Send_ChangedRoom));
	m_processPacket.emplace(PairProcessPacket(PACKET_TYPE::CS_WHISPER_MESSAGE, &CChatServer::Send_WhisperMessage));

}

void CChatServer::RunSubServer() {
	std::cout << "Run Server\n";
	while (true) {}
}

void CChatServer::ProcessAsyncAccpet(CSession& user) {
	// Lock Guard
	{
		LockGuard lockGuard(m_usersLock);
		//해당 유저를 users에 등록
		m_users.push_back(const_cast<CSession*>(&user));
		user.SetClientID(m_amClientIndex++);
	}
	//해당 유저를 rooms에 등록 Default 0번
	m_roomHandle->InsertUserInRoom(&user);

	//Send Login Ok
	Send_LoginOK(&user);

	//새로운 유저의 접속 알림 및 리스트 전송
	m_roomHandle->SendNewUserInRoom(&user);

}

CSession* CChatServer::CreateSubSession() {
	return new CUser(this);
}

void CChatServer::ProcessSocketIO(void* packet, const CSession& session) {

	PacketType type = reinterpret_cast<PacketType*>(packet)[1];

	MFP fp = m_processPacket[type];
	(this->*fp)(&session, packet);

	//call Recv
	const_cast<CSession&>(session).DoRecv();
}

void CChatServer::Send_LoginOK(const CSession* user, void* packetBuffer) {
	sc_loginOk_packet* packet = new sc_loginOk_packet();
	packet->myNumber = user->GetClientID();
	const_cast<CSession*>(user)->PushSendQueue(packet);


	// sc_loginOk_packet packet;
	// packet.myNumber=user->GetClientID();
	// const_cast<CSession *>(user)->PushSendQueue(&packet);

}

void CChatServer::Send_Message(const CSession* user, void* packetBuffer) {

	cs_message_packet* message_packet =
		reinterpret_cast<cs_message_packet*>(packetBuffer);

#ifdef _DEBUG
	for (int i = 0; message_packet->message[i] != '\n'; ++i) {
		std::cout << message_packet->message[i];
	}
	std::cout << "\n";
#endif

	  // sc_message_packet packet;
	  // // message Copy
	  // memcpy_s(packet.message, MESSAGE_SIZE, message_packet->message, MESSAGE_SIZE);
	  // // Insert User number
	  // packet.id = user->GetClientID();
	  //m_roomHandle->SendAllMessageInRoom(user, &packet,true);


	sc_message_packet* packet = new sc_message_packet();
	// message Copy
	memcpy_s(packet->message, MESSAGE_SIZE, message_packet->message, MESSAGE_SIZE);
	// Insert User number
	packet->id = user->GetClientID();

	// 자기가 들어가 있는 방에 Broad Cast
	m_roomHandle->SendAllMessageInRoom(user, packet, true);
}

void CChatServer::Send_ChangedRoom(const CSession* user, void* packetBuffer) {

	cs_changedRoom_packet* changedRoom_packet =
		reinterpret_cast<cs_changedRoom_packet*>(packetBuffer);

	m_roomHandle->ChangedRoom(user, changedRoom_packet->changed_room_number);
}

void CChatServer::Send_WhisperMessage(const CSession* user, void* packetBuffer) {


	cs_whisperMessage_packet* whisperMessage =
		reinterpret_cast<cs_whisperMessage_packet*>(packetBuffer);

	LockGuard lockguard(m_usersLock);

	UsersIter findIter = m_users.end();

	for (UsersIter iter = m_users.begin(); iter != m_users.end(); ++iter) {

		if ((*iter)->GetClientID() == whisperMessage->userNumber) {
			findIter = iter;
			break;
		}
	}

	if (findIter != m_users.end()) {

		// sc_message_packet packet;
		// packet.id=user->GetClientID();
		// memcpy_s(packet.message,BUFFER_SIZE,whisperMessage->message,BUFFER_SIZE);
		// (*findIter)->OnSend(&packet);


		sc_message_packet* packet = new sc_message_packet();
		packet->id = user->GetClientID();
		memcpy_s(packet->message, sizeof(packet->message),
			whisperMessage->message, sizeof(whisperMessage->message));
		(*findIter)->PushSendQueue(packet);
	}

}
