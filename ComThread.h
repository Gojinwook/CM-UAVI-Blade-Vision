// ComThread.h: interface for the CComThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMTHREAD_H__89F660A2_4792_48D2_A1C5_B7C993969B36__INCLUDED_)
#define AFX_COMTHREAD_H__89F660A2_4792_48D2_A1C5_B7C993969B36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//---------------------------- 상수 정의 --------------------------//
#define	BUFF_SIZE			16384

#define	WM_COMM_READ	(WM_USER+1)		
#define	WM_COMM_READ2	(WM_USER+2)		
#define	WM_COMM_READ3	(WM_USER+3)		
#define	WM_COMM_READ4	(WM_USER+4)		
#define WM_COMM_START	(WM_USER+5)
#define WM_COMM_RESET	(WM_USER+6)

#define	ASCII_LF		0x0a
#define	ASCII_CR		0x0d
#define	ASCII_XON		0x11
#define	ASCII_XOFF		0x13

#include "afxmt.h"

// Queue 클래스 정의 //
class	CQueue
{
public:
	BYTE	buff[BUFF_SIZE];
	BYTE	buff2[BUFF_SIZE];
	BYTE	buff3[BUFF_SIZE];
	BYTE	buff4[BUFF_SIZE];

	int		m_iHead, m_iTail;
	int		m_iHead2, m_iTail2;
	int		m_iHead3, m_iTail3;
	int		m_iHead4, m_iTail4;

	CQueue();

	void	Clear();
	int		GetSize();
	BOOL	PutByte(BYTE b);		// 1 byte 넣기
	BOOL	GetByte(BYTE *pb);		// 1 byte 꺼내기

	void	Clear2();
	int		GetSize2();
	BOOL	PutByte2(BYTE b);		// 1 byte 넣기
	BOOL	GetByte2(BYTE *pb);		// 1 byte 꺼내기

	void	Clear3();
	int		GetSize3();
	BOOL	PutByte3(BYTE b);		// 1 byte 넣기
	BOOL	GetByte3(BYTE *pb);		// 1 byte 꺼내기

	void	Clear4();
	int		GetSize4();
	BOOL	PutByte4(BYTE b);		// 1 byte 넣기
	BOOL	GetByte4(BYTE *pb);		// 1 byte 꺼내기

	CCriticalSection	m_sec;

};


//	통신 클래스	CCommThread 

// 포트 열기 : OpenPort("COM1", CBR_9600);
// 포트에서 읽기 :
//   포트를 연 후에 포트에 자료가 도착하면 WM_COMM_READ 메시지가 메인 
//   윈도우에 전달된다. ON_MESSAGE 매크로를 이용, 함수를 연결하고
//   m_ReadData String에 저장된 데이터를 이용 읽기
// 포트에 쓰기 : WriteComm(buff, 30)과 같이 버퍼와 그 크기를 건네면 된다.

class	CCommThread
{
public:
	//--------- 환경 변수 -----------------------------------------//
	//COM1
	HANDLE		m_hComm;				// 통신 포트 파일 핸들
	CString		m_sPortName;			// 포트 이름 (COM1 ..)
	BOOL		m_bConnected;			// 포트가 열렸는지 유무를 나타냄.
	OVERLAPPED	m_osRead, m_osWrite;	// 포트 파일 Overlapped structure
	HANDLE		m_hThreadWatchComm;		// Watch함수 Thread 핸들.
	WORD		m_wPortID;				// WM_COMM_READ와 함께 보내는 인수.

	//COM2
	HANDLE		m_hComm2;				// 통신 포트 파일 핸들
	CString		m_sPortName2;			// 포트 이름 (COM1 ..)
	BOOL		m_bConnected2;			// 포트가 열렸는지 유무를 나타냄.
	OVERLAPPED	m_osRead2, m_osWrite2;	// 포트 파일 Overlapped structure
	HANDLE		m_hThreadWatchComm2;	// Watch함수 Thread 핸들.
	WORD		m_wPortID2;				// WM_COMM_READ와 함께 보내는 인수.

	//COM3
	HANDLE		m_hComm3;				// 통신 포트 파일 핸들
	CString		m_sPortName3;			// 포트 이름 (COM1 ..)
	BOOL		m_bConnected3;			// 포트가 열렸는지 유무를 나타냄.
	OVERLAPPED	m_osRead3, m_osWrite3;	// 포트 파일 Overlapped structure
	HANDLE		m_hThreadWatchComm3;	// Watch함수 Thread 핸들.
	WORD		m_wPortID3;				// WM_COMM_READ와 함께 보내는 인수.

	//COM4
	HANDLE		m_hComm4;				// 통신 포트 파일 핸들
	CString		m_sPortName4;			// 포트 이름 (COM1 ..)
	BOOL		m_bConnected4;			// 포트가 열렸는지 유무를 나타냄.
	OVERLAPPED	m_osRead4, m_osWrite4;	// 포트 파일 Overlapped structure
	HANDLE		m_hThreadWatchComm4;	// Watch함수 Thread 핸들.
	WORD		m_wPortID4;				// WM_COMM_READ와 함께 보내는 인수.

	//--------- 통신 버퍼 -----------------------------------------//
	CQueue	m_QueueRead;
	CQueue	m_QueueRead2;
	CQueue	m_QueueRead3;
	CQueue	m_QueueRead4;

	//--------- 외부 사용 함수 ------------------------------------//
	BOOL	OpenPort(DWORD wPortID, DWORD dwBaud);
	void	ClosePort();
	DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);

	BOOL	OpenPort2(DWORD wPortID, DWORD dwBaud);
	void	ClosePort2();
	DWORD	WriteComm2(BYTE *pBuff, DWORD nToWrite);

	BOOL	OpenPort3(DWORD wPortID, DWORD dwBaud);
	void	ClosePort3();
	DWORD	WriteComm3(BYTE *pBuff, DWORD nToWrite);

	BOOL	OpenPort4(DWORD wPortID, DWORD dwBaud);
	void	ClosePort4();
	DWORD	WriteComm4(BYTE *pBuff, DWORD nToWrite);

	//--------- 내부 사용 함수 ------------------------------------//
	DWORD	ReadComm(BYTE *pBuff, DWORD nToRead);
	DWORD	ReadComm2(BYTE *pBuff, DWORD nToRead);
	DWORD	ReadComm3(BYTE *pBuff, DWORD nToRead);
	DWORD	ReadComm4(BYTE *pBuff, DWORD nToRead);

	CEvent  event;
};

// Thread로 사용할 함수 
DWORD	ThreadWatchComm(CCommThread* pComm);
DWORD	ThreadWatchComm2(CCommThread* pComm);
DWORD	ThreadWatchComm3(CCommThread* pComm);
DWORD	ThreadWatchComm4(CCommThread* pComm);

#endif // !defined(AFX_COMTHREAD_H__89F660A2_4792_48D2_A1C5_B7C993969B36__INCLUDED_)
