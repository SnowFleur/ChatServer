#include<algorithm>

#include"Room.h"
#include"User.h"


void CRoom::InsertUserInRoom(CUser* user){
    m_roomLock.AcquiredLock();
    m_userList.push_back(user);
    m_roomLock.ReleaseLock();
}

void CRoom::DeletedUserInRoom(CUser *user) {

  RoomIter iter = std::find(m_userList.begin(), m_userList.end(), user);

  m_roomLock.AcquiredLock();
  if (iter != m_userList.end()) {
    m_userList.erase(iter);
    m_roomLock.ReleaseLock();
  } 
  
  else {
    m_roomLock.ReleaseLock();
  }
  
}