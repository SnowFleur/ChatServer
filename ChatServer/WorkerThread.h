#pragma once
#include"WinSocketHeader.h"

class OverlappedEx;

class CWorkerThread{
private:
    HANDLE m_threadHandle;
    HANDLE m_iocpHandle;

    CWorkerThread(const CWorkerThread&);
    CWorkerThread& operator=(const CWorkerThread&);
public:

    CWorkerThread(HANDLE threadHandle,HANDLE iocpHandle):m_threadHandle(INVALID_HANDLE_VALUE),m_iocpHandle(INVALID_HANDLE_VALUE){
        m_threadHandle=threadHandle;
        m_iocpHandle=iocpHandle;
    }
    ~CWorkerThread(){
        if(m_threadHandle!=INVALID_HANDLE_VALUE)
            CloseHandle(m_threadHandle);
    }

    inline HANDLE GetTheradHandle()const{return m_threadHandle;}
    
    DWORD DoThread();
    void  ProcessAccept(OverlappedEx* overEX);
    void  ProcessRecv(OverlappedEx* overEX,DWORD ioByte);
};
