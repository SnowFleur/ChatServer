#pragma once
#include<stack>
#include"Lock.h"
#include<memory>
/*
[21.03.23] Object Pool 역할 할 수 있도록 구현하기
[21.03.25] 미사용
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


enum class OBJ_TYPE {
	PACKET = 1, OVERLAPPED = 2, SESSION = 3
};


//팩토리 패턴
// 
//class CObjectPoolFactory {
//    const int PACKET_POOL_SIZE      = 1000;
//    const int SESSION_POOL_SIZE     = 4000;
//    const int OVERLAPPED_PPOL_SIZE  = 2000;
//
//    using SharePakcetPtr        = std::shared_ptr<PacketHeader>;
//    using ShareOverlappedPtr    = std::shared_ptr<OverlappedEx>;
//    using ShareSessinoPtr       = std::shared_ptr<CSession>;
//
//private:
//    CObjectPool<PacketHeader>   m_packetPool;
//    CObjectPool<CSession>       m_sessionPool;
//    CObjectPool<OverlappedEx>   m_overlappedPool;
//public:
//	CObjectPoolFactory() :m_packetPool(PACKET_POOL_SIZE),
//		m_sessionPool(SESSION_POOL_SIZE),
//		m_overlappedPool(OVERLAPPED_PPOL_SIZE)
//	{
//
//	}
//
//    SharePakcetPtr CraetePacket() {
//        return m_packetPool.GetMemory();
//    }
//
//};


