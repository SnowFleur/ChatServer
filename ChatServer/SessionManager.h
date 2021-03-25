#pragma once
#include<vector>
#include"Lock.h"

class CSession;

const int MAX_SESSION = 900; //���� Ŭ���̾�Ʈ�� ����
 

/*
[21.03.23] Object Pool ���� �� �� �ֵ��� �����ϱ�
[21.03.25] �̻��
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