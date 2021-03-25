#pragma once

#include"DataDefine.h"
//������ �������ϴ� ��ɵ��� �����ϴ� �������̽� ����

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
    virtual void ProcessSocketIO(void* packet,const PacketType packIoType,const CSession& session)=0;
    virtual void ProcessAsyncAccpet(const CSession& session)=0;
};