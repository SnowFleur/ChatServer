#include"Lock.h"

Lock::Lock() {   InitializeCriticalSection(&cs); }
Lock::~Lock() { DeleteCriticalSection(&cs); }

void Lock::AcquiredLock() { EnterCriticalSection(&cs); }
void Lock::ReleaseLock() { LeaveCriticalSection(&cs); }