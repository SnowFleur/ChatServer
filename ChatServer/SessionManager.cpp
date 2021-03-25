#include"SessionManager.h"
#include"Session.h"
#include"User.h"
#include"ChatServer.h"

#include<iostream>

CSessionManager* CSessionManager::m_instance=NULL;

void CSessionManager::InsertSessgion(CSession* session){
    m_sessionLock.AcquiredLock();
    m_sessions.push_back(session);
    m_sessionLock.ReleaseLock();
}

// std::call_once가 C++11..
// 나중에 freeList로 연계
void CSessionManager::InitSession(){

  static bool once = false;

  if (once) {
    std::cout << "Error: Twice call Functions\n";
  }

  //여기에서는 IOCP가 가지고 있는 메모리를 받아야함 new CchatServer 하는게 아님
  for (int i = 0; i < MAX_SESSION; ++i) {
  //  m_sessions.push_back(new CUser(new CChatServer()));
  }
  std::cout<<"Sucess Session\n";
}