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

  //���߿� hash Ȥ�� �ٸ� �ڷᱸ���� �̿��� �߻�ȭ �ϱ�
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
  //socket IOCP�� ���
  CIocpManager::GetInstance()->RegitSocketInIOCP(overEX->session->GetSocket());

  //Asny Accpet�� ���� ��Ŷ ���� �� Server�� ��� (���� ���)
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

    //�޾ƾ� �ϴ� ��Ŷ ũ��
    int required = packet_size - overEX->session->GetPrevSize();

    //[21.03.25] ����κ��� ��¥ ��Ʈ��ũ Ÿ ���� ���� üũ ����
    if (required <= rest) {
      //���� ���ۿ� �������� ���� �����ؿ´�
      memcpy_s(overEX->session->GetUserBuffer()+overEX->session->GetPrevSize(),BUFFER_SIZE,ptr,required);
      
      //BUFFSZE_SIZE ũ�⸸ŭ ������ Index �̵�
      overEX->session->SetRecvBufferReadIndex(BUFFER_SIZE);

      //���� Ŭ�������� �۵� 
      overEX->session->ProcessIO();

      rest -= required;
      ptr += required;
      overEX->session->SetPrevSize(0);
    }

    else {
      memcpy_s(overEX->session->GetUserBuffer() + packet_size, BUFFER_SIZE, ptr,rest);
      overEX->session->SetPrevSize(overEX->session->GetPrevSize()+rest);

      //���� ũ�⸸ŭ ������ Index �̵�
      overEX->session->SetRecvBufferReadIndex(rest);
      rest = 0;
    }
  }
}
