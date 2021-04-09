#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef _WIN32
#include <winsock2.h>
#include <process.h> 
#include <windows.h>
#include <mswsock.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#endif

enum SOCKET_TYPE { TCP_TYPE = 1, UDP_TYPE };

enum IO_EVENT { IO_SEND = 1, IO_RECV, IO_ACCEPT,IO_CONNECT};

const int MAX_WORKER_THREAD         = 16;
const unsigned int GQCS_WAIT_TIME            = INFINITE;
//const int MAX_USER                  = 100000;
