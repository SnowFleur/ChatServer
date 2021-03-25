#pragma once
#include<vector>
#include"Lock.h"

class CSession;

const int MAX_SESSION = 900; //받을 클라이언트와 동일
 

/*
[21.03.23] Object Pool 역할 할 수 있도록 구현하기
[21.03.25] 미사용
*/
class CSessionManager{
private:
    std::vector<CSession*>  m_sessions;
    Lock                    m_sessionLock;
    CSessionManager(const CSessionManager&);
    CSessionManager& operator=(const CSessionManager&);
    CSessionManager():m_sessions(MAX_SESSION){}
    
public:
    ~CSessionManager(){}

    static CSessionManager* m_instance;

    static CSessionManager* GetInstance(){
        if(m_instance==NULL){
            m_instance= new CSessionManager();
        }
        return m_instance;
    }
    
    void InsertSessgion(CSession* session);

   void InitSession();

};