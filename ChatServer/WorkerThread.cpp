#include<iostream>
#include"WorkerThread.h"
#include"User.h"
#include"OverlappedEx.h"
#include"IocpManager.h"
#include"BaseServer.h"

DWORD CWorkerThread::DoThread(){
  DWORD IoByte;
  LPOVERLAPPED  overlapped=NULL;
  bool          IoEventResult=true;
#ifdef _WIN64
  ULONGLONG Iokey;
#else
  ULONG Iokey;
#endif //  _WIN64
  while (true) {

    BOOL result = GetQueuedCompletionStatus(m_iocpHandle, &IoByte, &Iokey,&overlapped,GQCS_WAIT_TIME);

    	if( overlapped == NULL )
				continue;

    OverlappedEx* overEX=reinterpret_cast<OverlappedEx*>(overlapped);

  //나중에 hash 혹은 다른 자료구조를 이용해 추상화 하기
  switch (overEX->ioEvent) {
    case IO_ACCEPT: {
		  std::cout<<"Accept New Client\n";
      ProcessAccept(overEX);
      break;
    }
    case IO_RECV: {
      std::cout<<"Recv\n";
      ProcessRecv(overEX,IoByte);
      break;
    }
    case IO_SEND: {
      std::cout<<"Send\n";
      break;
    }
    default: {
      std::cout << "Error: Not Define Io Event\n";
      IoEventResult = false;
      break;
    }
  } // End Switch

    if (IoEventResult == true) {
      delete overlapped;
      overlapped = NULL;
    }

  } //end While

   _endthreadex( 0 );
  return 0;
}

void CWorkerThread::ProcessAccept(OverlappedEx* overEX){
  //socket IOCP에 등록
  CIocpManager::GetInstance()->RegitSocketInIOCP(overEX->session->GetSocket());

  //Asny Accpet에 따른 패킷 전송 및 Server에 등록 (수정 요망)
  CIocpManager::GetInstance()->GetServerHandle()->ProcessAsyncAccpet(*(overEX->session));

  //Call Recv
  overEX->session->DoRecv();

  //call Async Accpet
  CIocpManager::GetInstance()->AsyncAccept();
}

void CWorkerThread::ProcessRecv(OverlappedEx *overEX,DWORD ioByte) {

  int rest = ioByte;
  char* ptr= overEX->session->GetRecvReadBuffer();
  PacketSize packet_size = 0;

  if (overEX->session->GetPrevSize() > 0) {
    packet_size = overEX->session->GetPrevSize();
  }

  while (0 < rest) {

    if (0 == packet_size){
      packet_size = ptr[0];
    }

    //받아야 하는 패킷 크기
    int required = packet_size - overEX->session->GetPrevSize();

    //[21.03.25] 여기부분은 진짜 네트워크 타 봐야 버그 체크 가능
    if (required <= rest) {
      //유저 버퍼에 링버퍼의 값을 복사해온다
      memcpy_s(overEX->session->GetUserBuffer()+overEX->session->GetPrevSize(),BUFFER_SIZE,ptr,required);
      
      //BUFFSZE_SIZE 크기만큼 링버퍼 Index 이동
      overEX->session->SetRecvBufferReadIndex(BUFFER_SIZE);

      //서브 클래스별로 작동 
      overEX->session->ProcessIO();

      rest -= required;
      ptr += required;
      overEX->session->SetPrevSize(0);
    }

    else {
      memcpy_s(overEX->session->GetUserBuffer() + packet_size, BUFFER_SIZE, ptr,rest);
      overEX->session->SetPrevSize(overEX->session->GetPrevSize()+rest);

      //받은 크기만큼 링버퍼 Index 이동
      overEX->session->SetRecvBufferReadIndex(rest);
      rest = 0;
    }
  }
}
