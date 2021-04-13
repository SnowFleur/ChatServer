#pragma once
#include<vector>
// Network Packet Type
typedef char                    PacketSize;
typedef char                    PacketType;

typedef short                   ClientID;
typedef unsigned short          RoomNumber;


//클라이언트 때문에 일단 살려두기
typedef std::vector<ClientID>   RoomUsers;
