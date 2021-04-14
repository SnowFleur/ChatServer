#include"StressClient.h"
#include"..\ChatServer\IocpServer.h"


void CDummyClinet::ProcessIO() {
    m_serverHandle->ProcessSocketIO(NULL, *this);
}

