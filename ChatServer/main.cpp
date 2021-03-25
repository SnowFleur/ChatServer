#include <iostream>
#include"IocpManager.h"
#include"SessionManager.h"
#include"BaseServer.h"

#include"User.h"
#include"ChatServer.h"

/*
규칙

- 멤버 변수는 항상 m_ 붙이기
- 생성자 소멸자등 간단한 멤버함수(Get/set)는 inline 및 헤더파일에 작성
- 하나만 존재야 하는 객체는 복사 생성자, 복사 대입연산자 방지
- 함수는 항상 대문자로 시작
- 자료형은 탭 한번 변수 및 함수는 최대한 줄 맞추기
*/


/*
설계 방향
- 재사용 가능한 IOCP Server -> 채팅 서버 뿐 아니라 다른 서버에도 사용가능하도록 추상화 및 모듈화
- 상속과 다형성을 사용 But 최대한 지양 ->결합도, 확장성 등등 문제가 있을 수 있음
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