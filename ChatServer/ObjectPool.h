#pragma once
#include<stack>
#include"Lock.h"
#include<memory>
/*
[21.03.23] Object Pool ���� �� �� �ֵ��� �����ϱ�
[21.03.25] �̻��
*/

#include<iostream>
#include<memory>
#include<queue>

template<class T>
class CObjectPool {
    using SharedPtr = std::shared_ptr<T>;
    using UniquePtr = std::unique_ptr<T>;
    using ObjectQueue = std::queue<UniquePtr>;
private:
    ObjectQueue objectQueue_;
public:
    CObjectPool(size_t initMemorySize);
    std::shared_ptr<T> GetMemory();
};

template<class T>
CObjectPool<T>::CObjectPool(size_t initMemorySize) {
    for (size_t i = 0; i < initMemorySize; ++i) {
        objectQueue_.emplace(std::make_unique<T>());
    }
}

template<class T>
std::shared_ptr<T> CObjectPool<T>::GetMemory() {

    if (objectQueue_.empty()) {
        objectQueue_.emplace(std::make_unique<T>());
    }

    UniquePtr frontobject = std::move(objectQueue_.front());
    objectQueue_.pop();

    SharedPtr obj(frontobject.release(), [this](T* t) {
        objectQueue_.emplace(t);
        });

    return obj;
}


//
//
//
//
//template<class MemoryBlock>
//class CObjectManager{
//private:
//    struct freeBlock {
//        freeBlock* next_;
//    };
//
//    freeBlock*      m_freeBlockPtr;
//    MemoryBlock     m_memoryBlockHandle;
//    size_t          m_blockSize;
//
//public:
//    CObjectManager(const CObjectManager&)               = delete;
//    CObjectManager& operator=(const CObjectManager&)    = delete;
//    CObjectManager(CObjectManager&&)                    = delete;
//    CObjectManager&& operator=(CObjectManager&&)        = delete;
//
//
//    // CObjectManager(size_t poolSize) :
//    // m_freeBlockPtr(NULL),
//    // m_memoryBlockHandle(poolSize),
//    // m_blockSize(0){}
//
//    // ~CObjectManager() {
//    //     while(m_objectPool.empty()==false){
//
//    //         Object* temp=NULL;
//    //         temp= m_objectPool.top();
//            
//    //         if(temp!=NULL){
//    //             delete temp;
//    //             temp=NULL;
//    //         }
//    //         m_objectPool.pop();            
//    //     }
//    // }
//
//    static void* operator new(size_t size){
//
//
//    }
//    static void operator delete(void *ptr) {
//      if (ptr != NULL) {
//        freeBlock *freePtr = reinterpret_cast<freeBlock *>(ptr);
//
//        freePtr->next_ = m_freeBlockPtr;
//        m_freeBlockPtr = freePtr;
//      }
//    }
//
//};
//
//
//class CFixedMemoryBlock {
//private:
//    void*     m_memoryHandle;
//    size_t    m_fixedMemorySize;    //  �߶�� �ϴ� �޸� ������
//    size_t    m_maxMemorySize;      //  �� �޸� ������
//public:
//
//  CFixedMemoryBlock(size_t size):
//  m_memoryHandle(NULL),
//  m_fixedMemorySize(size){
//     m_memoryHandle = (char*)malloc(m_maxMemorySize);
//  }
//
//  void*  Allocate(size_t size){
//
//     //�޸� �ڵ��� 1����Ʈ�� �ڸ� �� �ִ� char* ������������ ĳ����
//    char* p = reinterpret_cast<char*>(m_memoryHandle);
//
//
//    size_t divdieMemory=m_maxMemorySize/m_fixedMemorySize;
//
//
//    //��ϰ��� ���Ḯ��Ʈ ����
//    for (; divdieMemory > 1; --divdieMemory, p += size) {
//        *reinterpret_cast<char**>(p) = p + size;
//    }
//    //�������� NULL
//    *reinterpret_cast<char**>(p) = NULL;
//    return m_memoryHandle;
//  }
//
//};