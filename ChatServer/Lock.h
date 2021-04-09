#pragma once
#include "WinSocketHeader.h"


constexpr int MAX_SPIN_COUNT = 4000;

struct Lock {
private:
	CRITICAL_SECTION cs;
public:

	Lock();
	~Lock();

	Lock(const Lock&)				= delete;
	Lock& operator=(const Lock&)	= delete;
	Lock&& operator=(Lock&&)		= delete;
	Lock(Lock&&)					= delete;

	void AcquiredLock();
	void ReleaseLock();
};

/*
RAII ���� ��� LockGuard Default ������, ���� ������, ���� ���Կ��� ����
*/
struct LockGuard {
private:
	Lock& lockGuard;
public:
	LockGuard(const LockGuard&)				= delete;
	LockGuard& operator=(const LockGuard&)	= delete;
	LockGuard&& operator=(LockGuard&&)		= delete;
	LockGuard(LockGuard&&)					= delete;

	explicit LockGuard(Lock& lock) : lockGuard(lock) { lockGuard.AcquiredLock(); }
	~LockGuard() { lockGuard.ReleaseLock(); }
};
