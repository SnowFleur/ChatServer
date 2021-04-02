#pragma once
#include<stack>
#include"Lock.h"

class CSession;

const int MAX_SESSION = 900; //���� Ŭ���̾�Ʈ�� ����
 

/*
[21.03.23] Object Pool ���� �� �� �ֵ��� �����ϱ�
[21.03.25] �̻��
*/
template<class Object>
class CObjectManager{
private:
    std::stack<Object*>     m_objectPool;
    size_t                  m_objectPoolSize;
    Lock                    m_objectPoolLock;

    CObjectManager(const Object&);
    CObjectManager& operator=(const Object&);
public:
    CObjectManager(size_t poolSize) :
     m_objectPool(),
     m_objectPoolSize(poolSize) {

    }

    ~CObjectManager() {
        while(m_objectPool.empty()==false){

            Object* temp=NULL;
            temp= m_sessionPool.top();
            
            if(temp!=NULL){
                delete temp;
                temp=NULL;
            }
            m_sessionPool.pop();            
        }
    }

    Object* PopObjectInPool();
    void    PushObjectInPool(Object* session);

};