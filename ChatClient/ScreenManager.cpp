#include "ScreenManager.h"
#include "..\ChatServer\Protocol.h"
#include <iostream>
#include <locale.h>

CScreenManager *CScreenManager::handle_ = nullptr;

ChatInfor::ChatInfor() : id(NO_RECV_NUMBER_TO_SERVER), message(nullptr) {}

ChatInfor::~ChatInfor() {
  if (message != nullptr)
    delete[] message;
}

ChatInfor::ChatInfor(const ChatInfor &lhs) {
  this->id = NO_RECV_NUMBER_TO_SERVER;
  this->message = new char[MESSAGE_SIZE];
}

CScreenManager::CScreenManager() :current_line(0) {
  
  //테스트용 
  for (int i = 0; i < 100; ++i) {
    screen_.push_back(ChatInfor());
  }

  setlocale(LC_ALL, "ko-KR");
  chatConsole_.Create(CONSOLE_TITLE);
  DrawToFrameWork();
}

CScreenManager *CScreenManager::GetInstance() {
  if (handle_ == nullptr) {
    handle_ = new CScreenManager();
  }
  return handle_;
}

// MAX Width 120, MAX Height 30
void CScreenManager::DrawToFrameWork() {
  // Draw Upper col
  chatConsole_.gotoxy(100, 0);
  chatConsole_.cprintf(CConsoleLoggerEx::COLOR_WHITE |CConsoleLoggerEx::COLOR_BACKGROUND_BLUE,"********************");

  // Draw row
  for (int i = 0; i < MAX_ROW; ++i) {
    chatConsole_.gotoxy(100, i);
    chatConsole_.cprintf(CConsoleLoggerEx::COLOR_WHITE |CConsoleLoggerEx::COLOR_BACKGROUND_BLUE,"*");
  }

  // Draw Group
  for (int i = 0; i < 5; ++i) {
    chatConsole_.gotoxy(102, (i + 1) * 2);
    chatConsole_.cprintf(CConsoleLoggerEx::COLOR_WHITE |CConsoleLoggerEx::COLOR_BACKGROUND_RED,"Chat Room: %d ", i + 1);
  }

  // Draw Lower col
  //chatConsole_.gotoxy(100, 25);
  chatConsole_.gotoxy(100, 12);
  chatConsole_.cprintf(CConsoleLoggerEx::COLOR_WHITE |CConsoleLoggerEx::COLOR_BACKGROUND_BLUE,"********************");

  //Draw User List
  chatConsole_.gotoxy(USER_LIST_X, USER_LIST_Y-1);
  chatConsole_.cprintf(CConsoleLoggerEx::COLOR_WHITE |CConsoleLoggerEx::COLOR_BACKGROUND_GREEN,"User List");


  chatConsole_.gotoxy(0, CHAT_LINE_Y-2);
  for(int i=0;i<5;++i)
  chatConsole_.cprintf(CConsoleLoggerEx::COLOR_WHITE |CConsoleLoggerEx::COLOR_BACKGROUND_BLUE,"********************");


  chatConsole_.gotoxy(0, 0);
}

void CScreenManager::DrawToMynameAndRoomNumber(const ClientID number,const RoomNumber roomNumber) {
  chatConsole_.cls(CConsoleLoggerEx::COLOR_BACKGROUND_BLACK);
  chatConsole_.gotoxy(CHAT_LINE_X, CHAT_LINE_Y-1);
  chatConsole_.cprintf(CConsoleLoggerEx::COLOR_WHITE, "My name: Guest%d, Current Room Number:%d\n",
                       number,roomNumber);
}

void CScreenManager::Draw() {
  // Console Clear
  // chatConsole_.cls(CConsoleLoggerEx::COLOR_BACKGROUND_BLACK);

  if (current_line>MAX_SCREEN_LINE) {
    ChatInfor temp = screen_.front();
    screen_.pop_front();
    screen_.push_back(temp);
    --current_line;
  }

  DrawToFrameWork();
  chatConsole_.gotoxy(CHAT_LINE_X, CHAT_LINE_Y);
  for (size_t i = 0; i < current_line; ++i) {
    chatConsole_.cprintf(CConsoleLoggerEx::COLOR_WHITE, "[Guest%d]%s",
                         screen_[i].id, screen_[i].message);
  }

  chatConsole_.gotoxy(0, 0);
}

void CScreenManager::DrawToUserIndex(const RoomUsers& userList){
    
  for (int i = 0; i < 10; ++i) {
    if(userList[i]==NO_RECV_NUMBER_TO_SERVER)continue;
    chatConsole_.gotoxy(USER_LIST_X,USER_LIST_Y+(i+1)*2);
        chatConsole_.cprintf(CConsoleLoggerEx::COLOR_WHITE |CConsoleLoggerEx::COLOR_BACKGROUND_GREEN,"[Guest%d]",userList[i]);
  }
}

void CScreenManager::InsertChat(const ClientID number, const char *message) {
  
  if(current_line>MAX_SCREEN_LINE)return;

  screen_[current_line].id = number;
  char *currChar = screen_[current_line++].message;
  memcpy_s(currChar, MESSAGE_SIZE, message, MESSAGE_SIZE);

}
