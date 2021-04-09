#pragma once

#include <vector>
#include"WinSocketHeader.h"
#include"Lock.h"
#include"Protocol.h"

class CSession;

class CRoom {
    typedef std::vector<CSession*>::iterator RoomIter;
    typedef std::vector<CSession*> Rooms;
private:
    std::vector<Rooms>      m_rooms;
    RoomNumber              m_currUserCountInRoom;
    RoomNumber              m_maxUserCountInRoom;
    Lock                    m_roomLock;

   
public:
  CRoom(WORD roomNumber);

  CRoom(const CRoom&) = delete;
  CRoom& operator=(const CRoom&) = delete;
  CRoom(CRoom&&) = delete;
  CRoom operator=(CRoom&&) = delete;
  
  ~CRoom() = default;

  inline RoomNumber GetCurrUserCountInRoom()const{return m_currUserCountInRoom;} 
  inline RoomNumber GetMaxUserCountInRoom()const{return m_maxUserCountInRoom;}

  //Process Funcs
  bool InsertUserInRoom(CSession* user, const RoomNumber roomNumber=0);
  bool DeletedUserInRoom(CSession* user);
  bool FindUserInRoom(const CSession* user);
  void CopyAllUserInRoom(const CSession* user,ClientID* roomClientids,size_t& countOfClinet);
  void CopyAllUserInRoom(const RoomNumber roomNumber,ClientID* roomClientids,size_t& countOfClinet);
  void ChangedRoom(const CSession* user, RoomNumber roomNumber);

  //Send Funcs
  void SendAllMessageInRoom(const CSession* user, void* packet,bool ignoreMe=false);
  void SendAllMessageInRoom(const RoomNumber roomNumber,void* packet);
  void SendNewUserInRoom(const CSession* user);
  void SendOutUserInRoom(const CSession* user);
  void SendRoomInfor(const CSession* user);
  void SendRoomInfor(const RoomNumber roomNumber);
};