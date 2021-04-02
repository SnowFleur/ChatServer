#include"StressServer.h"


class CIocpServer;


/*
생각보다 할 수 있는게 많이 없음 상위 클래스의 대부분을 virtual 해야할듯
=0 하지말고 추상X

- WorkThread 개수 정할 수 있게 수정하기
*/

int main(){

    CIocpServer* serverHandle=new StressTest();

    //Init
    serverHandle->ServerInitialize(4,9001,true);

    //Start Server
    serverHandle->StartWorkerThread();


    //Rand -> Sleep(1000)

    //Client 


}