#include"ObjectPool.h"
#include"Session.h"

#include<iostream>



template<class Object>
void CObjectManager<Object>::PushObjectInPool(Object* object){
  LockGuard lockguard(m_objectPoolLock);
  m_objectPool.push(object);
}


template<class Object>
Object* CObjectManager<Object>::PopObjectInPool(){
  LockGuard lockguard(m_objectPoolLock);
  Object *temp = NULL;

  if (m_objectPool.empty() == false) {

    temp = m_objectPool.top();

    if (temp != NULL) {
      m_objectPool.pop();
      return temp;
    }
  }
  return NULL;
}