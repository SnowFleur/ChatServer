#pragma once
#include "WinSocketHeader.h"

struct Lock {
private:
  CRITICAL_SECTION cs;
  Lock(const Lock &);
  Lock &operator=(const Lock &);
public:
  Lock();
  ~Lock();
  void AcquiredLock();
  void ReleaseLock();
};

/*
RAII 패턴 기반 LockGuard Default 생성자, 복사 생성자, 복사 대입연산 삭제
*/
struct LockGuard {
private:
  Lock &lockGuard;

  LockGuard(const LockGuard &);
  LockGuard &operator=(const LockGuard &);

public:
  LockGuard(Lock &lock) : lockGuard(lock) { lockGuard.AcquiredLock(); }
  ~LockGuard() { lockGuard.ReleaseLock(); }
};
