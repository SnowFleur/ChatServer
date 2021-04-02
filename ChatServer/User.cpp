#include<iostream>
#include "User.h"
#include"ChatServer.h"


void CUser::ProcessIO(){
    ProcessPacket(GetUserBuffer());
}

void CUser::ProcessPacket(void* packet){
    m_serverHandle->ProcessSocketIO(packet, *this);
}