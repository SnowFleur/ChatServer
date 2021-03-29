#pragma once

#include"DataDefine.h"
//서버가 가져야하는 기능들을 선언하는 인터페이스 서버

typedef unsigned char SESSION_IO_TYPE;

class CSession;

class CBaseServer{
private:
    CBaseServer(const CBaseServer&);
    CBaseServer& operator=(const CBaseServer&);
public:
    CBaseServer(){}
    virtual ~CBaseServer(){}

    virtual void Init()=0;
    virtual void Run()=0;
    virtual void ProcessSocketIO(void* packet,const CSession& session)=0;
    virtual void ProcessAsyncAccpet(CSession& session)=0;
};