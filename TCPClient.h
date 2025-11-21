#pragma once
/**
 * @file TCPClient.h
 * @brief MFC-based TCP client (CTCPClient).
 *
 * Brief:
 * ADJ 를 위한 TCP Client.
 * TCPServer 와는 전혀 다른 목표.
 * (TCPServer : Handler 와 통신)
 */
#pragma comment( lib, "ws2_32" )

#include <WinSock2.h>
#include "SafeQueue.h"
#include "TCPDefine.h"

static CRITICAL_SECTION CS_TCP_LOG;

class CTCPClient : public CWnd
{
	DECLARE_DYNAMIC(CTCPClient)

public:
	CTCPClient( void );
	virtual ~CTCPClient( void );

	//Initializing about socket
	void SockInit( void );
	BOOL TCPClientInit(CString strServerIP, int nPort, CString strClientIP);
	void SetServerParam(CString strServerIP, int nPort);
	BOOL GetSockInit(void) { return m_bSockInit; };
	void SetSockInit(BOOL bInit) { m_bSockInit = bInit; };

	// TCP communication
	BOOL TCPConnect(void);
	void TCPonReceive(void);
	void TCPonClose(void);
	void TCPSend(BYTE *pbyteMessage, int nMessageByte);
	// network event
	int TCPNetConnectEventWait(void);  // receive wait for network event( connect )
	int TCPNetRcvCloseEventWait(void); // receive wait for network event( close & receive )
	void TCPReset(void);

	// about total byte number from server ( logical protocol )
	void SetTotalByteTokenPos(int nTokenPos) { m_nTotalByteTokenPos = nTokenPos; }
	int GetTotalByteTokenPos(void) { return m_nTotalByteTokenPos; }

	//logical communication buffer
	CSafeQueue<BYTE*> m_qRecvBuffer;		//ASSERT : memory release after pop...

	//Control External Receive Event
	HANDLE* GetTCPReceiveHanlde ( void ){ return m_HGetReceiveData; }
	void   ResetTCPReceiveHandle ( int nEventNo ) { ResetEvent( m_HGetReceiveData[ nEventNo ] ); }

	//ReceiveThread
	CWinThread* m_threadRecv;
	BOOL        m_bRecvThreadRun;
	///////////////////  utility method  ////////////////////////////
	void SocketErrorMessage(CString strError);
	void CreateDirForLog(void);
	void SetLogPath(CString strPath) { m_strCurrentLogPath = strPath; }
	CString GetLogPath(void) { return m_strCurrentLogPath; }
	void TCPSaveLog(CString strLog);
	int GetTCPPort(void) { return m_nTCPPort; }
	BOOL GetTcpConnect(void) { return m_bConnect; }
	/////////////////////////////////////////////////////////////////

protected:
	DECLARE_MESSAGE_MAP()

private:
	SOCKET					m_TCPClient;						 //Socket ID
	WSANETWORKEVENTS		m_networkEvents;					 //Network Events(receive, connect close )
	WSAEVENT                m_WSAEventArr[ NET_EVNET_NUM ];		 //[ WSA_MAXIMUM_WAIT_EVENTS ]; //each client has 1 event
	CString					m_strServerIP;						 //Server IP
	int						m_nTCPPort;							 //Port Number
	CString					m_strClientIP;						 //Client IP 20190903 add
	int                     m_nTotalByteTokenPos;			     //total byte number from server ( logical protocol )
	CString                 m_strCurrentLogPath;			     //Log Path
	HANDLE					m_HGetReceiveData[ TCP_EVENT_NUM ];  //External Network Event Handle
	BOOL				    m_bConnect;
	BOOL					m_bSockInit;
};