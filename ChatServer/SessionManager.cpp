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

// std::call_once�� C++11..
// ���߿� freeList�� ����
void CSessionManager::InitSession(){

  static bool once = false;

  if (once) {
    std::cout << "Error: Twice call Functions\n";
  }

  //���⿡���� IOCP�� ������ �ִ� �޸𸮸� �޾ƾ��� new CchatServer �ϴ°� �ƴ�
  for (int i = 0; i < MAX_SESSION; ++i) {
  //  m_sessions.push_back(new CUser(new CChatServer()));
  }
  std::cout<<"Sucess Session\n";
}