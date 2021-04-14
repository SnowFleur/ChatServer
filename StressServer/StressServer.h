#pragma once 

#include<vector>
#include<cstdlib>
#include<iomanip>
#include<ctime>

#include"..\ChatServer\IocpServer.h"
#include"..\ChatServer\Lock.h"

class CSession;

const int COUNT_OF_DUMMY = MAX_ROOM_CLIENT - 5;

class StressTest : public CIocpServer {
    typedef std::vector<CSession*>::iterator        dummyIter;
private:
    
    std::vector<CSession*>      m_dummys;
    Lock                        m_dummysLock;

    void InitSubServer()override;
    void RunSubServer()override;
    void ProcessAsyncAccpet(CSession &session)override{}
    void ProcessSocketIO(void *packet, const CSession &session)override;
    DWORD DoThread()override;
    
    CSession* CreateSubSession()override;

	void Send_Message(CSession* user, void* packetBuffer = nullptr);
	void Changed_Room(CSession* user, void* packetBuffer = nullptr);

public:
  StressTest():CIocpServer(this){}
  ~StressTest() = default;
  StressTest(const StressTest&)             = delete;
  StressTest& operator=(const StressTest&)  = delete;
  StressTest(StressTest&&)                  = delete;
  StressTest&& operator=(StressTest&&)      = delete;
};

