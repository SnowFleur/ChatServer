#include"StressServer.h"


class CIocpServer;


/*
�������� �� �� �ִ°� ���� ���� ���� Ŭ������ ��κ��� virtual �ؾ��ҵ�
=0 �������� �߻�X

- WorkThread ���� ���� �� �ְ� �����ϱ�
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