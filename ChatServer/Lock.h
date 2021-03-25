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
RAII ���� ��� LockGuard Default ������, ���� ������, ���� ���Կ��� ����
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
