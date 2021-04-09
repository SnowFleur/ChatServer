#include <algorithm>
#include "Room.h"
#include"Session.h"
#include"User.h"

CRoom::CRoom(WORD roomNumber) :m_rooms(roomNumber), m_currUserCountInRoom(0),
m_maxUserCountInRoom(MAX_ROOM_CLIENT), m_roomLock() {

    for (int i = 0;i < roomNumber;++i) {
        m_rooms[i].reserve(MAX_ROOM_CLIENT);
    }

}


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
  // �̰Ŵ� �Լ��� Ÿ���ϱ� ������ �ѹ��� Read X
  countOfClinet = m_rooms[number].size();

  for (size_t i = 0; i < m_rooms[number].size(); ++i) {
    roomClientids[i] =  m_rooms[number][i]->GetClientID();
  }

}

void CRoom::CopyAllUserInRoom(const RoomNumber roomNumber,ClientID* roomClientids,size_t& countOfClinet){
  LockGuard lockGuard(m_roomLock);

  countOfClinet = m_rooms[roomNumber].size();
  for (size_t i = 0; i < m_rooms[roomNumber].size(); ++i) {
    roomClientids[i] =  m_rooms[roomNumber][i]->GetClientID();
  }

}

void CRoom::SendAllMessageInRoom(const CSession* user, void* packet,bool ignoreMe){

  LockGuard lockGuard(m_roomLock);

  //���̳��� ���� �ٸ� ��� ã��
  RoomNumber number = dynamic_cast<const CUser *>(user)->GetCurrentRoomNumber();

  //������ ���ִ� �濡 ��� ����
 for (RoomIter iter = m_rooms[number].begin(); iter != m_rooms[number].end(); ++iter) {

   if(ignoreMe)
	  if((*iter)->GetClientID()==user->GetClientID())continue;

    (*iter)->PushSendQueue(packet);
  }

}

void CRoom::SendAllMessageInRoom(const RoomNumber roomNumber,void* packet){

  LockGuard lockGuard(m_roomLock);

  //Ư�� �濡 �ִ� ��� �������� ����
 for (RoomIter iter = m_rooms[roomNumber].begin(); iter != m_rooms[roomNumber].end(); ++iter) {
    (*iter)->PushSendQueue(packet);
  }
}


void CRoom::ChangedRoom(const CSession* user,RoomNumber newRoomNumber){

  RoomNumber oldRoomNumber=dynamic_cast<const CUser *>(user)->GetCurrentRoomNumber();

  //�ش� ������ old->user�� �ٲٰ�
  DeletedUserInRoom(const_cast<CSession*>(user));
  InsertUserInRoom(const_cast<CSession *>(user),newRoomNumber);

  //���� �����鿡�� �ٲ� �� ���� ����
  SendRoomInfor(oldRoomNumber);
  //���ο� �����鿡�� �ٲ� �� ���� ����
  SendRoomInfor(newRoomNumber);
}

void CRoom::SendNewUserInRoom(const CSession* user){

  // sc_newUser_packet newUserPacket;
  // newUserPacket.new_userNumber = user->GetClientID();
  // SendAllMessageInRoom(user,&newUserPacket);


  sc_newUser_packet* newUserPacket=new sc_newUser_packet();
  newUserPacket->new_userNumber = user->GetClientID();
  SendAllMessageInRoom(user,newUserPacket);

  SendRoomInfor(user);
}

void CRoom::SendOutUserInRoom(const CSession* user){

  // sc_outUser_packet outUserpacket;
  // outUserpacket.out_userNumber = user->GetClientID();
  // SendAllMessageInRoom(user, &outUserpacket);

  sc_outUser_packet* outUserpacket= new sc_outUser_packet();
  outUserpacket->out_userNumber = user->GetClientID();
  SendAllMessageInRoom(user, outUserpacket);

  SendRoomInfor(user);
}

void CRoom::SendRoomInfor(const CSession* user){
  // sc_changedRoom_packet changedRoomPacket;
  // CopyAllUserInRoom(user, changedRoomPacket.roomClientids,changedRoomPacket.countOfclient);
  // SendAllMessageInRoom(user,&changedRoomPacket);

  sc_changedRoom_packet* changedRoomPacket=new sc_changedRoom_packet();
  CopyAllUserInRoom(user, changedRoomPacket->roomClientids,changedRoomPacket->countOfclient);
  SendAllMessageInRoom(user,changedRoomPacket);
}

void CRoom::SendRoomInfor(const RoomNumber roomNumber){
//  sc_changedRoom_packet changedRoomPacket;
//   CopyAllUserInRoom(roomNumber, changedRoomPacket.roomClientids,changedRoomPacket.countOfclient);
//   SendAllMessageInRoom(roomNumber,&changedRoomPacket);

  sc_changedRoom_packet* changedRoomPacket=new sc_changedRoom_packet();
  CopyAllUserInRoom(roomNumber, changedRoomPacket->roomClientids,changedRoomPacket->countOfclient);
  SendAllMessageInRoom(roomNumber,changedRoomPacket);
}
