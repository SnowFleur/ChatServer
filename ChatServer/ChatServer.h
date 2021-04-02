#pragma once
#include <vector>
#include <map>
#include"DataDefine.h"
#include"IocpServer.h"

// �� Ŭ������ ä���� ���� �����Ѵ�.
class CRoom;
class CSession;

class CChatServer : public CIocpServer {

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
    void InitSubServer() override;
    void RunSubServer() override;
    void ProcessAsyncAccpet(CSession &session)override;
    void ProcessSocketIO(void* packet,const CSession& session)override;
    CSession* CreateSubSession()override; 

    CChatServer():
    CIocpServer(this),
    m_roomHandle(NULL){}

    ~CChatServer() {
      if (m_roomHandle != NULL)
        delete m_roomHandle;
    }

};
