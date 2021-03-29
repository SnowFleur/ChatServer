#include <iostream>
#include"IocpManager.h"
#include"SessionManager.h"
#include"BaseServer.h"

#include"User.h"
#include"ChatServer.h"

/*
설계 방향
- 재사용 가능한 IOCP Server -> 채팅 서버 뿐 아니라 다른 서버에도 사용가능하도록 추상화 및 모듈화
- 상속과 다형성을 사용 But 최대한 지양 ->결합도, 확장성 등등 문제가 있을 수 있음
- 싱글턴 지양 및 다른 디자인 패턴 적용 띵크
*/

int main() {

  //IOCP 초기화 및 사용할 서버 등록
  if (CIocpManager::GetInstance()->Initialize(new CChatServer) == false) {
    std::cout << "Error: IOCP Initializtion false\n";
  }

  //worker Thread 생성 및 등록한 서버 시작
  if (CIocpManager::GetInstance()->StartWorkerThread() == false) {
    std::cout << "Error: StartWorkerThread false\n";
  }
}