#pragma once
#include <vector>
#include <map>
#include"DataDefine.h"
#include"BaseServer.h"

#include"Lock.h"

// 이름이 너무 비직관적

enum USER_IO_TYPE{
    USER_ACCEPT=1,
    USER_SEND_MESSAGE,
    USER_CHANGE_ROOM
};

// 이 클래스가 채팅의 모든걸 관리한다.
class CRoom;
class CSession;
class CUser;

class CChatServer : public CBaseServer {

    typedef std::vector<CSession*>::iterator        UsersIter;
    typedef std::vector<CSession*>::const_iterator  ConstUsersIter;
    typedef std::vector<CRoom*>::iterator           RoomsIter;
    typedef std::vector<CRoom*>::const_iterator     ConstRoomsIter;

private:
    std::vector<CSession*>  m_users;
    std::vector<CRoom*>     m_rooms;
    volatile ClientID       m_clientIndex;

    Lock                    m_roomsLock;
    Lock                    m_usersLock;
    Lock                    m_clientIndexLock;

    CChatServer(const CChatServer &);
    CChatServer &operator=(const CChatServer &);

    void Send_LoginOK(const CSession* user);
    void Send_Message(const CSession* user);
public:
    CChatServer(){}
    ~CChatServer();


    void Init() override;
    void Run() override;
    void ProcessSocketIO(void* packet,const  PacketType packIoType,const CSession& session)override;
    void ProcessAsyncAccpet(const CSession &session);
};
