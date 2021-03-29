#include <algorithm>

#include "Room.h"
#include"Session.h"
#include"User.h"

bool CRoom::InsertUserInRoom(CSession *user,const RoomNumber roomNumber) {

  // Lock Guard
  LockGuard lockguard(m_roomLock);
  if (m_currUserCountInRoom + 1 < m_maxUserCountInRoom) {

    m_rooms[roomNumber].push_back(user);

    //User�� �� ������ �뿡���� Lock�� �ɰ� m_roomLock�� ���
    dynamic_cast<CUser*>(user)->SetCurrentRoomNumber(roomNumber);

    ++m_currUserCountInRoom;
    return true;
  }

  return false;
}

bool CRoom::DeletedUserInRoom(CSession *user) {

  RoomNumber number= dynamic_cast<CUser*>(user)->GetCurrentRoomNumber();

  // Lock Guard
  LockGuard lockguard(m_roomLock);
  //RoomIter iter = std::find(m_userList.begin(), m_userList.end(), user);
  RoomIter iter = std::find(m_rooms[number].begin(),m_rooms[number].end(),user);

  if (iter != m_rooms[number].end()) {
     m_rooms[number].erase(iter);
    --m_currUserCountInRoom;
    return true;
  }

  return false;
}

bool CRoom::FindUserInRoom(const CSession *user) {

  RoomNumber number = dynamic_cast<const CUser *>(user)->GetCurrentRoomNumber();

  LockGuard lockguard(m_roomLock);
  RoomIter iter = std::find(m_rooms[number].begin(),m_rooms[number].end(),user);

  if (iter != m_rooms[number].end()) {
     return true;
   }
   return false;
}

void CRoom::CopyAllUserInRoom(const CSession* user,ClientID* roomClientids,size_t& countOfClinet){

  RoomNumber number = dynamic_cast<const CUser *>(user)->GetCurrentRoomNumber();

  LockGuard lockGuard(m_roomLock);
  //�̰Ŵ� �Լ��� Ÿ���ϱ� ������ �ѹ��� Read X
  countOfClinet = m_rooms[number].size();

  for (int i = 0; i < m_rooms[number].size(); ++i) {
    roomClientids[i] =  m_rooms[number][i]->GetClientID();
  }

}

void CRoom::SendAllMessageInRoom(const CSession* user, void* packet){

  //���̳��� ���� �ٸ� ��� ã��
  RoomNumber number = dynamic_cast<const CUser *>(user)->GetCurrentRoomNumber();

  //������ ���ִ� �濡 ��� ����
 for (RoomIter iter = m_rooms[number].begin(); iter != m_rooms[number].end(); ++iter) {

    if((*iter)->GetClientID()==user->GetClientID())continue;

    (*iter)->DoSend(packet);
  }

}