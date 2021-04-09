#pragma once
#include<memory>
#include"ObjectPool.h"
//#include"Protocol.h"

/*
Object Pool 에서 데이터를 가져오고 생성과 삭제를 책임지는 Pakcet 팩토리
*/
class CPacketPoolFactory {
	struct PacketHeader;
	
	const int POOL_SIZE		= 2000;

	using SharePakcetPtr	= std::shared_ptr<PacketHeader>;
private:

	CObjectPool<PacketHeader>   m_packetPool;
	

#pragma region Packet Structs
#pragma pack(push, 1)
	struct PacketHeader {
	public:
		PacketSize size;
		PacketType type;
		PacketHeader():
			size{0},
			type{0} 
		{}
	};

	// Server ----> Client
	struct sc_loginOk_packet : public PacketHeader {
		ClientID myNumber;

		/*sc_loginOk_packet() :
			PacketHeader(sizeof(sc_loginOk_packet),
				PACKET_TYPE::SC_LOGIN_OK),
			myNumber{ DEFAULT_CLINET_ID }{}*/
	};


	struct sc_message_packet : public PacketHeader {

		ClientID id;
		char message[MESSAGE_SIZE];

		//sc_message_packet() :
		//	PacketHeader(sizeof(sc_message_packet),
		//		PACKET_TYPE::SC_SEND_MESSAGE),
		//	id{ DEFAULT_CLINET_ID },
		//	message{}{}
	};

	struct sc_newUser_packet :public PacketHeader {

		ClientID new_userNumber;

	/*	sc_newUser_packet() :
			PacketHeader(sizeof(sc_newUser_packet),
				PACKET_TYPE::SC_NEW_USER),
			new_userNumber{ DEFAULT_CLINET_ID }{}*/
	};

	struct sc_changedRoom_packet :public PacketHeader {

		ClientID    roomClientids[MAX_ROOM_CLIENT];
		size_t      countOfclient;

		/*sc_changedRoom_packet() :
			PacketHeader(sizeof(sc_changedRoom_packet),
				PACKET_TYPE::SC_CHANGED_ROOM),
			roomClientids(),
			countOfclient() {}*/
	};

	struct sc_outUser_packet :public PacketHeader {

		ClientID out_userNumber;

		/*sc_outUser_packet() :
			PacketHeader(sizeof(sc_outUser_packet),
				PACKET_TYPE::SC_OUT_USER),
			out_userNumber{ DEFAULT_CLINET_ID }{}*/
	};

	// Client----> Server
	struct cs_message_packet :public PacketHeader {
		char message[MESSAGE_SIZE];

	/*	cs_message_packet() :
			PacketHeader(sizeof(cs_message_packet),
				PACKET_TYPE::CS_SEND_MESSAGE),
			message{}{}*/
	};

	struct cs_changedRoom_packet :public PacketHeader {
		RoomNumber changed_room_number;

	/*	cs_changedRoom_packet() :
			PacketHeader(sizeof(cs_changedRoom_packet),
				PACKET_TYPE::CS_CHANGED_ROOM),
			changed_room_number{ DEFAULT_CLINET_ID }{}*/
	};

	struct cs_whisperMessage_packet :public PacketHeader {

		ClientID userNumber;
		char message[MESSAGE_SIZE];

		/*cs_whisperMessage_packet() :
			PacketHeader(sizeof(cs_whisperMessage_packet),
				PACKET_TYPE::CS_WHISPER_MESSAGE),
			userNumber{ DEFAULT_CLINET_ID },
			message{}{}*/
	};
#pragma pack(pop)
#pragma endregion
public:

	CPacketPoolFactory() :
		m_packetPool(POOL_SIZE)
	{}

	//타입에 대한 세이프가 없음
	SharePakcetPtr CraetePacket(const int packetType) {

		auto packet = m_packetPool.GetMemory();

		switch (packetType) {
		case PACKET_TYPE::SC_LOGIN_OK: {
			packet->size = sizeof(sc_loginOk_packet);
			packet->type = PACKET_TYPE::SC_LOGIN_OK;

			break;
		}
		case PACKET_TYPE::SC_SEND_MESSAGE: {
			break;
		}
		case PACKET_TYPE::SC_NEW_USER: {
			break;
		}
		case PACKET_TYPE::SC_CHANGED_ROOM: {
			break;
		}
		case PACKET_TYPE::SC_OUT_USER: {
			break;
		}
		default:
			break;
		}
		return packet;
	}
};





