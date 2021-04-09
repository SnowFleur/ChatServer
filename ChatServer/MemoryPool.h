#pragma once
#include<stdlib.h>

template<class MemoryBlock>
class CObjectManager {
private:
    struct freeBlock {
        freeBlock* next_;
    };

    freeBlock* m_freeBlockPtr;
    MemoryBlock     m_memoryBlockHandle;
    size_t          m_blockSize;

public:
    CObjectManager(const CObjectManager&) = delete;
    CObjectManager& operator=(const CObjectManager&) = delete;
    CObjectManager(CObjectManager&&) = delete;
    CObjectManager&& operator=(CObjectManager&&) = delete;


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

    static void* operator new(size_t size) {


    }
    static void operator delete(void* ptr) {
        if (ptr != nullptr) {
            freeBlock* freePtr = reinterpret_cast<freeBlock*>(ptr);

            freePtr->next_ = m_freeBlockPtr;
            m_freeBlockPtr = freePtr;
        }
    }

};


class CFixedMemoryBlock {
private:
    void*     m_memoryHandle;
    size_t    m_fixedMemorySize;    //  �߶�� �ϴ� �޸� ������
    size_t    m_maxMemorySize;      //  �� �޸� ������
public:

    CFixedMemoryBlock(size_t size) :
        m_memoryHandle(nullptr),
        m_fixedMemorySize(size) {
        m_memoryHandle = (char*)malloc(m_maxMemorySize);
    }

    void* Allocate(size_t size) {

        //�޸� �ڵ��� 1����Ʈ�� �ڸ� �� �ִ� char* ������������ ĳ����
        char* p = reinterpret_cast<char*>(m_memoryHandle);


        size_t divdieMemory = m_maxMemorySize / m_fixedMemorySize;


        //��ϰ��� ���Ḯ��Ʈ ����
        for (; divdieMemory > 1; --divdieMemory, p += size) {
            *reinterpret_cast<char**>(p) = p + size;
        }
        //�������� NULL
        *reinterpret_cast<char**>(p) = nullptr;
        return m_memoryHandle;
    }

};