#include "StdAfx.h"
#include <stdint.h>
#include <winsock2.h>

#define NET_EVNET_NUM			1
#define NET_EVENT_WAIT_TIME		800
#define SOCKET_CLOSE			-2

// 유지: 기존과 같은 이미지 영역 찾아 Crop
#define MAX_BUF_SIZE			(1600 * 1600 * 4) + 1000
#define PACKET_STRING_SIZE		60

#define CONNECT_EVENT 0
#define RECV_EVENT 1
#define SERVER_CLOSE_EVENT 2
#define RESTART_EVENT 3 //jwj 190819 add
#define TCP_EVENT_NUM 4