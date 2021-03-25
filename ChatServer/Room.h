#pragma once

#include <vector>
#include"WinSocketHeader.h"
#include"Lock.h"
class CUser;


class CRoom {
    typedef std::vector<CUser*>::iterator RoomIter;
private:
    std::vector<CUser*> m_userList;
    DWORD               m_currUserCountInRoom;
    DWORD               m_maxUserCountInRoom;
    Lock                m_roomLock;
public:
  inline DWORD GetCurrUserCountInRoom()const{return m_currUserCountInRoom;} 
  inline DWORD GetMaxUserCountInRoom()const{return m_maxUserCountInRoom;}

  void InsertUserInRoom(CUser* user);
  void DeletedUserInRoom(CUser* user);
};