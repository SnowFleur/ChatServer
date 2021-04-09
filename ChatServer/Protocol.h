#pragma once
#include"IOCP_DataDefine.h"

using ClientID		= short;
using RoomNumber	= short;

constexpr int MESSAGE_SIZE		= 50;
constexpr int MAX_CLIENT		=3000;
constexpr int MAX_ROOM_CLIENT	= 4000;

constexpr RoomNumber CHAT_ROOM_COUNT = 6;
constexpr ClientID DEFAULT_CLINET_ID = -999;

namespace PACKET_TYPE {

	constexpr int SC_LOGIN_OK			= 1;
	constexpr int SC_SEND_MESSAGE		= 2;
	constexpr int SC_NEW_USER			= 3;
	constexpr int SC_CHANGED_ROOM		= 4;
	constexpr int SC_OUT_USER			= 5;

	constexpr int CS_SEND_MESSAGE		= 1;
	constexpr int CS_CHANGED_ROOM		= 2;
	constexpr int CS_WHISPER_MESSAGE	= 3;

} // namespace PACKET_TYPE


#pragma pack(push, 1)
//잠재적 문제: unsigned int(4바이트) -> PacketSize(1바이트) Overflow

struct PacketHeader {
private:
	PacketSize size;
	PacketType type;
public:
	PacketHeader() {}
	PacketHeader(PacketSize s, PacketType t) : size(s), type(t) {}
};

// Server ----> Client
struct sc_loginOk_packet : public PacketHeader {
	ClientID myNumber;

	sc_loginOk_packet() :
		PacketHeader(sizeof(sc_loginOk_packet),
			PACKET_TYPE::SC_LOGIN_OK),
		myNumber{ DEFAULT_CLINET_ID }{}
};

struct sc_message_packet : public PacketHeader {

	ClientID id;
	char message[MESSAGE_SIZE];

	sc_message_packet() :
		PacketHeader(sizeof(sc_message_packet),
			PACKET_TYPE::SC_SEND_MESSAGE),
		id{ DEFAULT_CLINET_ID },
		message{}{}
};

struct sc_newUser_packet :public PacketHeader {

	ClientID new_userNumber;

	sc_newUser_packet() :
		PacketHeader(sizeof(sc_newUser_packet),
			PACKET_TYPE::SC_NEW_USER),
		new_userNumber{ DEFAULT_CLINET_ID }{}
};

struct sc_changedRoom_packet :public PacketHeader {

	ClientID    roomClientids[MAX_ROOM_CLIENT];
	size_t      countOfclient;

	sc_changedRoom_packet() :
		PacketHeader(sizeof(sc_changedRoom_packet),
			PACKET_TYPE::SC_CHANGED_ROOM),
		roomClientids(),
		countOfclient() {}
};

struct sc_outUser_packet :public PacketHeader {

	ClientID out_userNumber;

	sc_outUser_packet() :
		PacketHeader(sizeof(sc_outUser_packet),
			PACKET_TYPE::SC_OUT_USER),
		out_userNumber{ DEFAULT_CLINET_ID }{}
};

// Client----> Server
struct cs_message_packet :public PacketHeader {
	char message[MESSAGE_SIZE];

	cs_message_packet() :
		PacketHeader(sizeof(cs_message_packet),
			PACKET_TYPE::CS_SEND_MESSAGE),
		message{}{}
};

struct cs_changedRoom_packet :public PacketHeader {
	RoomNumber changed_room_number;

	cs_changedRoom_packet() :
		PacketHeader(sizeof(cs_changedRoom_packet),
			PACKET_TYPE::CS_CHANGED_ROOM),
		changed_room_number{ DEFAULT_CLINET_ID }{}
};

struct cs_whisperMessage_packet :public PacketHeader {

	ClientID userNumber;
	char message[MESSAGE_SIZE];

	cs_whisperMessage_packet() :
		PacketHeader(sizeof(cs_whisperMessage_packet),
			PACKET_TYPE::CS_WHISPER_MESSAGE),
		userNumber{ DEFAULT_CLINET_ID },
		message{}{}
};

#pragma pack(pop)
