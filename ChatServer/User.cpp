#include<iostream>
#include "User.h"
#include"ChatServer.h"


void CUser::ProcessIO() {
#ifdef _USED_RING_BUFFER
	ProcessPacket(GetUserBuffer());
#else
	char buffer[BUFFER_SIZE];
	memcpy_s(buffer, BUFFER_SIZE, GetUserBuffer(), BUFFER_SIZE);
	ProcessPacket(buffer);
#endif
}

void CUser::ProcessPacket(void* packet){
    m_serverHandle->ProcessSocketIO(packet, *this);
}