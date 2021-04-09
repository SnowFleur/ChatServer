#pragma once
#include <deque>
#include <string>
#include <vector>

#include "..\ChatServer\Protocol.h"
#include "..\ChatServer\WinSocketHeader.h"
#include "ConsoleLogger.h"

#define MAX_SCREEN_LINE 10

#define CONSOLE_TITLE   TEXT("Chatting Clinet")

struct ChatInfor;

using RoomUsers = std::vector<ClientID>;

//Console Chat Const
constexpr int CHAT_LINE_X = 0;
constexpr int CHAT_LINE_Y = 19;
constexpr int MAX_ROW = 30;
constexpr int MAX_COL = 120;
constexpr int USER_LIST_X = 102;
constexpr int USER_LIST_Y = 15;

constexpr int NO_RECV_NUMBER_TO_SERVER = 99;

typedef std::deque<ChatInfor> Screen;

struct ChatInfor {
private:
public:
	ClientID  id;
	char* message;
	ChatInfor();
	ChatInfor(const ChatInfor&);
	~ChatInfor();
};

//  1 4 4 ? ?
class CScreenManager {
private:
	static CScreenManager* handle_;
	size_t current_line;
	Screen screen_;
	CConsoleLoggerEx chatConsole_;

	CScreenManager();
	void DrawToFrameWork();
public:

	CScreenManager& operator=(const CScreenManager&)	= delete;
	CScreenManager&& operator=(CScreenManager&&)		= delete;
	CScreenManager(const CScreenManager&)				= delete;
	CScreenManager(CScreenManager&&)					= delete;

	static CScreenManager* GetInstance();
	void DrawToMynameAndRoomNumber(const ClientID number, const RoomNumber roomNumber);
	void DrawToUserIndex(const RoomUsers&);
	void Draw();
	void InsertChat(const ClientID number, const char* message);
};
