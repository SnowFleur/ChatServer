#include"StressServer.h"


class CIocpServer;


int main(){

    CIocpServer* serverHandle=new StressTest();

    //Init(thread, port, option)
    serverHandle->ServerInitialize(4,9001,true);

    //Start Server
    serverHandle->StartWorkerThread();

}