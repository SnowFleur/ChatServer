#pragma once
#include<stack>
#include"Lock.h"

/*
[21.03.23] Object Pool 역할 할 수 있도록 구현하기
[21.03.25] 미사용
*/
template<class MemoryBlock>
class CObjectManager{
private:
    struct freeBlock {
        freeBlock* next_;
    };

    freeBlock*      m_freeBlockPtr;
    MemoryBlock     m_memoryBlockHandle;
    size_t          m_blockSize;

    CObjectManager(const CObjectManager&);
    CObjectManager& operator=(const CObjectManager&);
public:
    // CObjectManager(size_t poolSize) :
    // m_freeBlockPtr(NULL),
    // m_memoryBlockHandle(poolSize),
    // m_blockSize(0){}

    // ~CObjectManager() {
    //     while(m_objectPool.empty()==false){

    //         Object* temp=NULL;
    //         temp= m_objectPool.top();
            
    //         if(temp!=NULL){
    //             delete temp;
    //             temp=NULL;
    //         }
    //         m_objectPool.pop();            
    //     }
    // }

    static void* operator new(size_t size){


    }
    static void operator delete(void *ptr) {
      if (ptr != NULL) {
        freeBlock *freePtr = reinterpret_cast<freeBlock *>(ptr);

        freePtr->next_ = m_freeBlockPtr;
        m_freeBlockPtr = freePtr;
      }
    }

};


class CFixedMemoryBlock {
private:
    void*     m_memoryHandle;
    size_t    m_fixedMemorySize;    //  잘라야 하는 메모리 사이즈
    size_t    m_maxMemorySize;      //  총 메모리 사이즈
public:

  CFixedMemoryBlock(size_t size):
  m_memoryHandle(NULL),
  m_fixedMemorySize(size){
     m_memoryHandle = (char*)malloc(m_maxMemorySize);
  }

  void*  Allocate(size_t size){

     //메모리 핸들을 1바이트씩 자를 수 있는 char* 포인터형으로 캐스팅
    char* p = reinterpret_cast<char*>(m_memoryHandle);


    size_t divdieMemory=m_maxMemorySize/m_fixedMemorySize;


    //블록간의 연결리스트 생성
    for (; divdieMemory > 1; --divdieMemory, p += size) {
        *reinterpret_cast<char**>(p) = p + size;
    }
    //마지막은 NULL
    *reinterpret_cast<char**>(p) = NULL;
    return m_memoryHandle;
  }

};