#pragma once
#define SERVER_ADDR "127.0.0.1"
#include "DataDefine.h"


const int           SERVER_PORT         = 9000;
const int           BUFFER_SIZE         = 1024;
const int           MESSAGE_SIZE        = 50;
const int           MAX_CLIENT          = 10000;
const int           MAX_ROOM_CLIENT     = 140;
const RoomNumber    CHAT_ROOM_COUNT     = 6;      //Ã¤ÆÃ¹æ °¹¼ö

namespace PACKET_TYPE {

  const int SC_LOGIN_OK           = 1;
  const int SC_SEND_MESSAGE       = 2;
  const int SC_NEW_USER           = 3;
  const int SC_CHANGED_ROOM       = 4;
  const int SC_OUT_USER           = 5;

  const int CS_SEND_MESSAGE       = 1;
  const int CS_CHANGED_ROOM       = 2;
  const int CS_WHISPER_MESSAGE    = 3;

} // namespace PACKET_TYPE


#pragma pack(push, 1)

struct PacketHeader {
private:
  PacketSize size;
  PacketType type;
public:
  PacketHeader(PacketSize s, PacketType t) : size(s), type(t) {}
};

// Server ----> Client
struct sc_loginOk_packet : public PacketHeader{
  ClientID myNumber;

  sc_loginOk_packet():PacketHeader(sizeof(sc_loginOk_packet),PACKET_TYPE::SC_LOGIN_OK){}
};

struct sc_message_packet: public PacketHeader {
  ClientID id;
  char message[MESSAGE_SIZE];

  sc_message_packet():PacketHeader(sizeof(sc_message_packet),PACKET_TYPE::SC_SEND_MESSAGE){}
};

struct sc_newUser_packet:public PacketHeader{
  ClientID new_userNumber;

  sc_newUser_packet():PacketHeader(sizeof(sc_newUser_packet),PACKET_TYPE::SC_NEW_USER){}
};

struct sc_changedRoom_packet:public PacketHeader{
  ClientID    roomClientids[MAX_ROOM_CLIENT];
  size_t      countOfclient;

  sc_changedRoom_packet():PacketHeader(sizeof(sc_changedRoom_packet),PACKET_TYPE::SC_CHANGED_ROOM),
  roomClientids(),countOfclient(){}
};

struct sc_outUser_packet:public PacketHeader{
  ClientID out_userNumber;

  sc_outUser_packet():PacketHeader(sizeof(sc_outUser_packet),PACKET_TYPE::SC_OUT_USER){}
};

// Client----> Server
struct cs_message_packet:public PacketHeader{
  char message[MESSAGE_SIZE];

  cs_message_packet():PacketHeader(sizeof(cs_message_packet),PACKET_TYPE::CS_SEND_MESSAGE){}
};

struct cs_changedRoom_packet:public PacketHeader{
  RoomNumber changed_room_number;

  cs_changedRoom_packet():PacketHeader(sizeof(cs_changedRoom_packet),PACKET_TYPE::CS_CHANGED_ROOM){}
};

struct cs_whisperMessage_packet:public PacketHeader{
  ClientID userNumber;
  char message[MESSAGE_SIZE];

  cs_whisperMessage_packet():PacketHeader(sizeof(cs_whisperMessage_packet),PACKET_TYPE::CS_WHISPER_MESSAGE){}
};

#pragma pack(pop)
