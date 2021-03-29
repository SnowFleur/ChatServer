#pragma once
#include <vector>
#include <map>
#include"DataDefine.h"
#include"BaseServer.h"
#include"Lock.h"

// 이 클래스가 채팅의 모든걸 관리한다.
class CRoom;
class CSession;
class CUser;

class CChatServer : public CBaseServer {

    typedef std::vector<CSession*>::iterator        UsersIter;
    typedef std::vector<CSession*>::const_iterator  ConstUsersIter;
    typedef std::vector<CRoom*>::iterator           RoomsIter;
    typedef std::vector<CRoom*>::const_iterator     ConstRoomsIter;

    typedef void (CChatServer::*MFP)(const CSession*, void*);
    typedef  char               FN;  //Function Number
    typedef std::pair<FN,MFP>   PairProcessPacket;

private:
    std::vector<CSession*>  m_users;
    CRoom*                  m_roomHandle;
    //std::vector<CRoom*>     m_rooms;
    volatile ClientID       m_clientIndex;

    Lock                    m_usersLock;
    Lock                    m_clientIndexLock;
    
    std::map<FN,MFP>        m_processPacket;

    CChatServer(const CChatServer &);
    CChatServer &operator=(const CChatServer &);

    void Send_Message(const CSession* user,void* packetBuffer=NULL);
    void Send_ChangedRoom(const CSession* user,void* packetBuffer=NULL);
    void Send_WhisperMessage(const CSession* user,void* packetBuffer=NULL);

    void Send_LoginOK(const CSession* user,void* packetBuffer=NULL);

public:
    CChatServer():m_roomHandle(NULL){}

    ~CChatServer() {
      if (m_roomHandle != NULL)
        delete m_roomHandle;
    }

    void Init() override;
    void Run() override;
    void ProcessSocketIO(void* packet,const CSession& session)override;
    void ProcessAsyncAccpet(CSession &session);
};
