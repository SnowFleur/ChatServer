#pragma once
#include<iostream>
#include"Protocol.h"
#include"Lock.h"

const int ERROR_RINGBUFFER_FULL     = -1;

//최대 5개까지 받을 수 있는 크기
const int RING_BUFFER_SIZE          = BUFFER_SIZE*5;

//1 4 4 8 ? ?
template<class T>
class CRingBuffer {
private:
    T*          m_data;
    int         m_head;         //Write Index
    int         m_tail;         //Read  Index
    size_t      m_ringSpace;

    //Head, Tail Index ++ -- Thread Safe Lock
    Lock        m_tailLock;   //read
    Lock        m_headLock;   //write

    //다음 Index로 
    int NextIndexing(int index) {
        index = (index + 1) % m_ringSpace;
        return index;
    }

    //head의 다음 값이 tail과 같다면 풀
    bool IsFull(){
        return m_tail==NextIndexing(m_head)? true : false;
    }

public:
  CRingBuffer(size_t size) : m_ringSpace(size), m_head(0), m_tail(0) {m_data = new T[m_ringSpace];}
  CRingBuffer() : m_ringSpace(RING_BUFFER_SIZE), m_head(0), m_tail(0) {m_data = new T[m_ringSpace];}
  

  char *GetWriteIndex() const { return m_data + m_head; }
  char *GetReadIndex() const { return m_data + m_tail; }
  size_t GetRingBufferSize() const { return m_ringSpace; }

  void SetWriteIndex(const int index){
    LockGuard lockGuard(m_headLock);
     m_head = NextIndexing(index);
  }

  void SetReadIndex(const int index){
    LockGuard lockGuard(m_tailLock);
     m_tail = NextIndexing(index);
  }

  void Enqueue(const T data) {
    m_data[m_head] = data;
    m_head = NextIndexing(m_head);
  }

  T Dequeue() {
    T temp = m_data[m_tail];
    m_tail = NextIndexing(m_tail);
    return temp;
  }
};