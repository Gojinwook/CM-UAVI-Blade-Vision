// ComThread.cpp: implementation of the CComThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uScan.h"
#include "ComThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char* THIS_FILE=__FILE__;
#define new DEBUG_NEW
#endif

// 메세지를 받을 윈도우 핸들, 부모 윈도우에서 HWND hCommWnd= this->m_hWnd로
// 설정해 준다.
HWND hCommWnd;
HWND hCommWnd2;
HWND hCommWnd3;
HWND hCommWnd4;

// CQueue 구현부========================================= 

// Queue의 생성자
CQueue::CQueue()
{
	Clear();
	Clear2();
	Clear3();
	Clear4();
}

// Queue를 초기화
void CQueue::Clear()
{
	m_sec.Lock();
	m_iHead = m_iTail = 0;
	memset(buff, 0, BUFF_SIZE);
	m_sec.Unlock();

}
void CQueue::Clear2()
{
	m_sec.Lock();
	m_iHead2 = m_iTail2 = 0;
	memset(buff2, 0, BUFF_SIZE);
	m_sec.Unlock();

}
void CQueue::Clear3()
{
	m_sec.Lock();
	m_iHead3 = m_iTail3 = 0;
	memset(buff3, 0, BUFF_SIZE);
	m_sec.Unlock();
}
void CQueue::Clear4()
{
	m_sec.Lock();
	m_iHead4 = m_iTail4 = 0;
	memset(buff4, 0, BUFF_SIZE);
	m_sec.Unlock();
}

// Queue에 들어 있는 자료 개수.
int CQueue::GetSize()
{
	return (m_iHead - m_iTail + BUFF_SIZE) % BUFF_SIZE;
}
int CQueue::GetSize2()
{
	return (m_iHead2 - m_iTail2 + BUFF_SIZE) % BUFF_SIZE;
}
int CQueue::GetSize3()
{
	return (m_iHead3 - m_iTail3 + BUFF_SIZE) % BUFF_SIZE;
}
int CQueue::GetSize4()
{
	return (m_iHead4 - m_iTail4 + BUFF_SIZE) % BUFF_SIZE;
}

// Queue에 1 byte 넣음.
BOOL CQueue::PutByte(BYTE b)
{
	m_sec.Lock();

	if (GetSize() == (BUFF_SIZE-1)) {
		m_sec.Unlock();
		return FALSE;
	}

	buff[m_iHead++] = b;
	m_iHead %= BUFF_SIZE;

	m_sec.Unlock();

	return TRUE;
}
BOOL CQueue::PutByte2(BYTE b)
{
	m_sec.Lock();

	if (GetSize2() == (BUFF_SIZE-1)) {
		m_sec.Unlock();
		return FALSE;
	}
	
	buff2[m_iHead2++] = b;
	m_iHead2 %= BUFF_SIZE;

	m_sec.Unlock();

	return TRUE;
}
BOOL CQueue::PutByte3(BYTE b)
{
	m_sec.Lock();

	if (GetSize3() == (BUFF_SIZE-1)) {
		m_sec.Unlock();
		return FALSE;
	}
	
	buff3[m_iHead3++] = b;
	m_iHead3 %= BUFF_SIZE;

	m_sec.Unlock();

	return TRUE;
}
BOOL CQueue::PutByte4(BYTE b)
{
	m_sec.Lock();

	if (GetSize4() == (BUFF_SIZE-1)) {
		m_sec.Unlock();
		return FALSE;
	}
	
	buff4[m_iHead4++] = b;
	m_iHead4 %= BUFF_SIZE;

	m_sec.Unlock();

	return TRUE;
}

// Queue에서 1 byte 꺼냄.
BOOL CQueue::GetByte(BYTE* pb)
{
	m_sec.Lock();

	if (GetSize() == 0) {
		m_sec.Unlock();
		return FALSE;
	}

	*pb = buff[m_iTail++];
	m_iTail %= BUFF_SIZE;

	m_sec.Unlock();

	return TRUE;
}
BOOL CQueue::GetByte2(BYTE* pb)
{
	m_sec.Lock();

	if (GetSize2() == 0) {
		m_sec.Unlock();
		return FALSE;
	}

	*pb = buff2[m_iTail2++];
	m_iTail2 %= BUFF_SIZE;

	m_sec.Unlock();

	return TRUE;
}
BOOL CQueue::GetByte3(BYTE* pb)
{
	m_sec.Lock();

	if (GetSize3() == 0) {
		m_sec.Unlock();
		return FALSE;
	}

	*pb = buff3[m_iTail3++];
	m_iTail3 %= BUFF_SIZE;

	m_sec.Unlock();

	return TRUE;
}
BOOL CQueue::GetByte4(BYTE* pb)
{
	m_sec.Lock();

	if (GetSize4() == 0) {
		m_sec.Unlock();
		return FALSE;
	}

	*pb = buff4[m_iTail4++];
	m_iTail4 %= BUFF_SIZE;

	m_sec.Unlock();

	return TRUE;
}


// 포트 sPortName을 dwBaud 속도로 연다.
// ThreadWatchComm 함수에서 포트에 무언가 읽혔을 때 MainWnd에 알리기
// 위해 WM_COMM_READ메시지를 보낼때 같이 보낼 wPortID값을 전달 받는다.
BOOL CCommThread::OpenPort(DWORD wPortID, DWORD dwBaud)
{
	// Local 변수.
	COMMTIMEOUTS	timeouts;
	DCB				dcb;
	DWORD			dwThreadID;

	// 변수 초기화
	m_bConnected = FALSE;
	m_wPortID	= wPortID; // COM1-> 0, COM2->1,,,,,

	CString sPortName;
	sPortName.Format("\\\\.\\COM%d", wPortID );

	// overlapped structure 변수 초기화.
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	if (! (m_osRead.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL)))	
		return FALSE;

	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	if (! (m_osWrite.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL)))
		return FALSE;
	
	// 포트 열기
	m_sPortName = sPortName;


	m_hComm = CreateFile( m_sPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL| FILE_FLAG_OVERLAPPED, NULL);


	if (m_hComm == (HANDLE) -1) return FALSE;

	// 포트 상태 설정.

	// EV_RXCHAR event 설정
	SetCommMask( m_hComm, EV_RXCHAR);	

	// InQueue, OutQueue 크기 설정.
	SetupComm( m_hComm, BUFF_SIZE, BUFF_SIZE);	

	// 포트 비우기.
	PurgeComm( m_hComm,					
		PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);


	// timeout 설정.
	timeouts.ReadIntervalTimeout = 100;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;

	timeouts.WriteTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 1;
	SetCommTimeouts( m_hComm, &timeouts);

	// dcb 설정
	dcb.DCBlength = sizeof(DCB);
	GetCommState( m_hComm, &dcb);	// 예전 값을 읽음.
	dcb.BaudRate = dwBaud;
		
	dcb.ByteSize = 8;				// number of bits/byte, 4-8
	dcb.Parity = 0;					// 0-4=no,odd,even,mark,space 
	dcb.StopBits = 0;				// 0,1,2 = 1, 1.5, 2

		
	dcb.fInX = dcb.fOutX = 0;		// Xon, Xoff 사용.
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;

	
	if (! SetCommState( m_hComm, &dcb))	return FALSE;

		
	// 포트 감시 쓰레드 생성.
	m_bConnected = TRUE;

	m_hThreadWatchComm = NULL;
	
	m_hThreadWatchComm = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadWatchComm, this, 0, &dwThreadID);

	if (! m_hThreadWatchComm)
	{
		ClosePort();
		return FALSE;
	}
	
	return TRUE;
}

//COM2
BOOL CCommThread::OpenPort2(DWORD wPortID, DWORD dwBaud)
{
	COMMTIMEOUTS	timeouts;
	DCB				dcb;

	// 변수 초기화
	m_bConnected2 = FALSE;
	m_wPortID2	= wPortID; // COM1-> 0, COM2->1,,,,,

	CString sPortName;
	if(wPortID == 0)
		sPortName = "COM1";
	else if(wPortID == 1)
		sPortName = "COM2";
	else if(wPortID == 2)
		sPortName = "COM3";
	else if(wPortID == 3)
		sPortName = "COM4";
	else if(wPortID == 4)
		sPortName = "COM5";
	else if(wPortID == 5)
		sPortName = "COM6";
	else if(wPortID == 6)
		sPortName = "COM7";
	else if(wPortID == 7)
		sPortName = "COM8";

	// overlapped structure 변수 초기화.
	m_osRead2.Offset = 0;
	m_osRead2.OffsetHigh = 0;
	if (! (m_osRead2.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL)))	
		return FALSE;

	m_osWrite2.Offset = 0;
	m_osWrite2.OffsetHigh = 0;
	if (! (m_osWrite2.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL)))
		return FALSE;
	
	// 포트 열기
	m_sPortName2 = sPortName;
	m_hComm2 = CreateFile( m_sPortName2, 
		GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL| FILE_FLAG_OVERLAPPED, 
		NULL);

	if (m_hComm2 == (HANDLE) -1) return FALSE;

	// 포트 상태 설정.

	// EV_RXCHAR event 설정
	SetCommMask( m_hComm2, EV_RXCHAR);	

	// InQueue, OutQueue 크기 설정.
	SetupComm( m_hComm2, BUFF_SIZE, BUFF_SIZE);	

	// 포트 비우기.
	PurgeComm( m_hComm2,					
		PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout 설정.
	timeouts.ReadIntervalTimeout = 100;
	timeouts.ReadTotalTimeoutMultiplier = 2;
	timeouts.ReadTotalTimeoutConstant = 1;
	
	timeouts.WriteTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 1;
	SetCommTimeouts( m_hComm, &timeouts);
	
	// dcb 설정
	dcb.DCBlength = sizeof(DCB);
	GetCommState( m_hComm, &dcb);	// 예전 값을 읽음.
	dcb.BaudRate = dwBaud;
	
	dcb.ByteSize = 8;				// number of bits/byte, 4-8
	dcb.Parity = 0;					// 0-4=no,odd,even,mark,space 
	dcb.StopBits = 0;				// 0,1,2 = 1, 1.5, 2
	
	dcb.fInX = dcb.fOutX = 0;		// Xon, Xoff 사용.
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;

	if (! SetCommState( m_hComm2, &dcb))	return FALSE;

	// 포트 감시 쓰레드 생성.
	m_bConnected2 = TRUE;

	/*
	m_hThreadWatchComm2 = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadWatchComm2, this, 0, &dwThreadID);
	if (! m_hThreadWatchComm2)
	{
		ClosePort2();
		return FALSE;
	}
	*/

	return TRUE;
}
//COM3
BOOL CCommThread::OpenPort3(DWORD wPortID, DWORD dwBaud)
{
	COMMTIMEOUTS	timeouts;
	DCB				dcb;

	// 변수 초기화
	m_bConnected3 = FALSE;
	m_wPortID3	= wPortID; // COM1-> 0, COM2->1,,,,,

	CString sPortName;
	if(wPortID == 0)
		sPortName = "COM1";
	else if(wPortID == 1)
		sPortName = "COM2";
	else if(wPortID == 2)
		sPortName = "COM3";
	else if(wPortID == 3)
		sPortName = "COM4";
	else if(wPortID == 4)
		sPortName = "COM5";
	else if(wPortID == 5)
		sPortName = "COM6";
	else if(wPortID == 6)
		sPortName = "COM7";
	else if(wPortID == 7)
		sPortName = "COM8";

	// overlapped structure 변수 초기화.
	m_osRead3.Offset = 0;
	m_osRead3.OffsetHigh = 0;
	if (! (m_osRead3.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL)))	
		return FALSE;

	m_osWrite3.Offset = 0;
	m_osWrite3.OffsetHigh = 0;
	if (! (m_osWrite3.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL)))
		return FALSE;
	
	PurgeComm( m_hComm3,
		PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// 포트 열기
	m_sPortName3 = sPortName;
	m_hComm3 = CreateFile( m_sPortName3, 
		GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL| FILE_FLAG_OVERLAPPED, 
		NULL);

	if (m_hComm3 == (HANDLE) -1) return FALSE;

	// 포트 상태 설정.

	// EV_RXCHAR event 설정
	SetCommMask( m_hComm3, EV_RXCHAR);	

	// InQueue, OutQueue 크기 설정.
	SetupComm( m_hComm3, BUFF_SIZE, BUFF_SIZE);	

	// 포트 비우기.
	PurgeComm( m_hComm3,
		PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout 설정.
	timeouts.ReadIntervalTimeout = 100;
	timeouts.ReadTotalTimeoutMultiplier = 2;
	timeouts.ReadTotalTimeoutConstant = 1;
	
	timeouts.WriteTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 1;
	SetCommTimeouts( m_hComm, &timeouts);
	
	// dcb 설정
	dcb.DCBlength = sizeof(DCB);
	GetCommState( m_hComm, &dcb);	// 예전 값을 읽음.
	dcb.BaudRate = dwBaud;
	
	dcb.ByteSize = 8;				// number of bits/byte, 4-8
	dcb.Parity = 1;					// 0-4=no,odd,even,mark,space 
	dcb.StopBits = 0;				// 0,1,2 = 1, 1.5, 2
	
	dcb.fInX = dcb.fOutX = 0;		// Xon, Xoff 사용.
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;

	if (! SetCommState( m_hComm3, &dcb))	return FALSE;

	// 포트 감시 쓰레드 생성.
	m_bConnected3 = TRUE;

	/*
	m_hThreadWatchComm3 = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadWatchComm3, this, 0, &dwThreadID);
	if (! m_hThreadWatchComm3)
	{
		ClosePort3();
		return FALSE;
	}
	*/

	return TRUE;
}
//COM4
BOOL CCommThread::OpenPort4(DWORD wPortID, DWORD dwBaud)
{
	COMMTIMEOUTS	timeouts;
	DCB				dcb;

	// 변수 초기화
	m_bConnected4 = FALSE;
	m_wPortID4	= wPortID; // COM1-> 0, COM2->1,,,,,

	CString sPortName;
	if(wPortID == 0)
		sPortName = "COM1";
	else if(wPortID == 1)
		sPortName = "COM2";
	else if(wPortID == 2)
		sPortName = "COM3";
	else if(wPortID == 3)
		sPortName = "COM4";
	else if(wPortID == 4)
		sPortName = "COM5";
	else if(wPortID == 5)
		sPortName = "COM6";
	else if(wPortID == 6)
		sPortName = "COM7";
	else if(wPortID == 7)
		sPortName = "COM8";

	// overlapped structure 변수 초기화.
	m_osRead4.Offset = 0;
	m_osRead4.OffsetHigh = 0;
	if (! (m_osRead4.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL)))	
		return FALSE;
	
	m_osWrite4.Offset = 0;
	m_osWrite4.OffsetHigh = 0;
	if (! (m_osWrite4.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL)))
		return FALSE;
	
	PurgeComm( m_hComm4,
		PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// 포트 열기
	m_sPortName4 = sPortName;
	m_hComm4 = CreateFile( m_sPortName4, 
		GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL| FILE_FLAG_OVERLAPPED, 
		NULL);
		
	if (m_hComm4 == (HANDLE) -1) return FALSE;

	// 포트 상태 설정.

	// EV_RXCHAR event 설정
	SetCommMask( m_hComm4, EV_RXCHAR);	

	// InQueue, OutQueue 크기 설정.
	SetupComm( m_hComm4, BUFF_SIZE, BUFF_SIZE);	

	// 포트 비우기.
	PurgeComm( m_hComm4,
		PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout 설정.
	timeouts.ReadIntervalTimeout = 100;
	timeouts.ReadTotalTimeoutMultiplier = 2;
	timeouts.ReadTotalTimeoutConstant = 1;
	
	timeouts.WriteTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 1;
	SetCommTimeouts( m_hComm, &timeouts);
	
	// dcb 설정
	dcb.DCBlength = sizeof(DCB);
	GetCommState( m_hComm, &dcb);	// 예전 값을 읽음.
	dcb.BaudRate = dwBaud;
	
	dcb.ByteSize = 8;				// number of bits/byte, 4-8
	dcb.Parity = 1;					// 0-4=no,odd,even,mark,space 
	dcb.StopBits = 0;				// 0,1,2 = 1, 1.5, 2
	
	dcb.fInX = dcb.fOutX = 0;		// Xon, Xoff 사용.
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;

	if (! SetCommState( m_hComm4, &dcb))	return FALSE;

	// 포트 감시 쓰레드 생성.
	m_bConnected4 = TRUE;

	/*
	m_hThreadWatchComm4 = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadWatchComm4, this, 0, &dwThreadID);
	if (! m_hThreadWatchComm4)
	{
		ClosePort4();
		return FALSE;
	}
	*/

	return TRUE;
}
	
// 포트를 닫는다.
void CCommThread::ClosePort()
{
	if( m_bConnected ) {
		m_bConnected = FALSE;

		SetCommMask( m_hComm, 0);
		PurgeComm( m_hComm,					
			PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

		if(m_hThreadWatchComm)
		{
			WaitForSingleObject(m_hThreadWatchComm, 10000);
			
			CloseHandle(m_hThreadWatchComm);	m_hThreadWatchComm = NULL;
		}

		CloseHandle(m_osRead.hEvent);
		CloseHandle(m_osWrite.hEvent);

		CloseHandle(m_hComm);				m_hComm = NULL;
	}
}

void CCommThread::ClosePort2()
{
	if( m_bConnected2 ) {
		m_bConnected2 = FALSE;

		SetCommMask( m_hComm2, 0);
		PurgeComm( m_hComm2,					
			PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

//		WaitForSingleObject(m_hThreadWatchComm2, 10000);
//		CloseHandle(m_hThreadWatchComm2);	m_hThreadWatchComm2 = NULL;

		CloseHandle( m_hComm2);				m_hComm2 = NULL;
	}
}

void CCommThread::ClosePort3()
{
	if( m_bConnected3 ) {
		m_bConnected3 = FALSE;

		SetCommMask( m_hComm3, 0);
		PurgeComm( m_hComm3,					
			PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

//		WaitForSingleObject(m_hThreadWatchComm3, 10000);
//		CloseHandle(m_hThreadWatchComm3);	m_hThreadWatchComm3 = NULL;

		CloseHandle( m_hComm3);				m_hComm3 = NULL;
	}
}

void CCommThread::ClosePort4()
{
	if( m_bConnected4 ) {
		m_bConnected4 = FALSE;

		SetCommMask( m_hComm4, 0);
		PurgeComm( m_hComm4,					
			PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

//		WaitForSingleObject(m_hThreadWatchComm4, 10000);
//		CloseHandle(m_hThreadWatchComm4);	m_hThreadWatchComm4 = NULL;
		CloseHandle( m_hComm4);				m_hComm4 = NULL;
	}
}

// 포트에 pBuff의 내용을 nToWrite만큼 쓴다.
// 실제로 쓰여진 Byte수를 리턴한다.
DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	if (! WriteFile(m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// 읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의
			// 특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
			//timeouts에 정해준 시간만큼 기다려준다.
			while (! GetOverlappedResult( m_hComm, &m_osWrite, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError( m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError( m_hComm, &dwErrorFlags, &comstat);
		}
	}

	return dwWritten;
}
//COM2
DWORD CCommThread::WriteComm2(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	if (! WriteFile( m_hComm2, pBuff, nToWrite, &dwWritten, &m_osWrite2))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// 읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의
			// 특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
			//timeouts에 정해준 시간만큼 기다려준다.
			while (! GetOverlappedResult( m_hComm2, &m_osWrite2, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError( m_hComm2, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError( m_hComm2, &dwErrorFlags, &comstat);
		}
	}

	return dwWritten;
}
//COM3
DWORD CCommThread::WriteComm3(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	if(! WriteFile( m_hComm3, pBuff, nToWrite, &dwWritten, &m_osWrite3))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			while (! GetOverlappedResult( m_hComm3, &m_osWrite3, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError( m_hComm3, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError( m_hComm3, &dwErrorFlags, &comstat);
		}
	}

	return dwWritten;
}
//COM4
DWORD CCommThread::WriteComm4(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	if(! WriteFile( m_hComm4, pBuff, nToWrite, &dwWritten, &m_osWrite4))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			while (! GetOverlappedResult( m_hComm4, &m_osWrite4, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError( m_hComm4, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError( m_hComm4, &dwErrorFlags, &comstat);
		}
	}

	return dwWritten;
}

// 포트로부터 pBuff에 nToWrite만큼 읽는다.
// 실제로 읽혀진 Byte수를 리턴한다.
DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//----------------- system queue에 도착한 byte수만 미리 읽는다.
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);
	dwRead = comstat.cbInQue;
	
	if (dwRead > 0)	
	{
		if (! ReadFile( m_hComm, pBuff, nToRead, &dwRead, &m_osRead))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				//--------- timeouts에 정해준 시간만큼 기다려준다.
				while (! GetOverlappedResult( m_hComm, &m_osRead, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{	
						ClearCommError( m_hComm, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_hComm, &dwErrorFlags, &comstat);
			}
		}
	}

	return dwRead;
}
//COM2
DWORD CCommThread::ReadComm2(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead, dwError, dwErrorFlags;
	COMSTAT	comstat;

	ClearCommError( m_hComm2, &dwErrorFlags, &comstat);
	dwRead = comstat.cbInQue;
	
	if (dwRead > 0)
	{
		if (! ReadFile( m_hComm2, pBuff, nToRead, &dwRead, &m_osRead2))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				while (! GetOverlappedResult( m_hComm2, &m_osRead2, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_hComm2, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_hComm2, &dwErrorFlags, &comstat);
			}
		}
	}

	return dwRead;
}
//COM3
DWORD CCommThread::ReadComm3(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead, dwError, dwErrorFlags;
	COMSTAT	comstat;


	ClearCommError( m_hComm3, &dwErrorFlags, &comstat);
	dwRead = comstat.cbInQue;
	
	if (dwRead > 0)
	{
		if (! ReadFile( m_hComm3, pBuff, nToRead, &dwRead, &m_osRead3))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				while (! GetOverlappedResult( m_hComm3, &m_osRead3, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_hComm3, &dwErrorFlags, &comstat);
						break;
					}
			
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_hComm3, &dwErrorFlags, &comstat);
			}
		}
	}


	return dwRead;
}

//COM4
DWORD CCommThread::ReadComm4(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead, dwError, dwErrorFlags;
	COMSTAT	comstat;


	ClearCommError( m_hComm4, &dwErrorFlags, &comstat);
	dwRead = comstat.cbInQue;
	
	if (dwRead > 0)
	{
		if (! ReadFile( m_hComm4, pBuff, nToRead, &dwRead, &m_osRead4))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				while (! GetOverlappedResult( m_hComm4, &m_osRead4, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_hComm4, &dwErrorFlags, &comstat);
						break;
					}
			
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_hComm4, &dwErrorFlags, &comstat);
			}
		}
	}


	return dwRead;
}

// 포트를 감시하고, 읽힌 내용이 있으면 
// m_ReadData에 저장한 뒤에 MainWnd에 메시지를 보내어 Buffer의 내용을
// 읽어가라고 신고한다.

DWORD	ThreadWatchComm(CCommThread* pComm)
{
	DWORD		dwEvent;
	OVERLAPPED	os;
	BOOL		bOk = TRUE;
	BYTE		buff[BUFF_SIZE];	 // InQueue의 데이터를 읽어오기 위한 Input Buffer
	DWORD		dwRead;	 // 읽은 바이트수.
	CString		SendData1;
	
	// Event, OS 설정.
	memset( &os, 0, sizeof(OVERLAPPED));
	if (! (os.hEvent = CreateEventA( NULL, TRUE, FALSE, NULL)))
		bOk = FALSE;
	if (! SetCommMask( pComm->m_hComm, EV_RXCHAR))
		bOk = FALSE;
	if (! bOk)
	{
		AfxMessageBox("Error while creating ThreadWatchComm, " + pComm->m_sPortName);
		return FALSE;
	}
	//SendData1.Format("%1c%s%1c",0x05,"00RSS0107%DW0010",0x04);
	//pComm->WriteComm((unsigned char*)(LPCTSTR) SendData1, (DWORD)SendData1.GetLength());
	// 포트를 감시하는 루프.
	
	Sleep(1);

	while (pComm->m_bConnected)
	{
		dwEvent = 0;
	   
		// 포트에 읽을 거리가 올때까지 기다린다.
		WaitCommEvent( pComm->m_hComm, &dwEvent, NULL);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			// 포트에서 읽을 수 있는 만큼 읽는다.
			bool bSendMessage = false;

			dwRead = pComm->ReadComm( buff, BUFF_SIZE);			
			Sleep(10);
			
			if (BUFF_SIZE - pComm->m_QueueRead.GetSize() > (int)dwRead)
			{
				for ( WORD i = 0; i < dwRead; i++)
 					pComm->m_QueueRead.PutByte(buff[i]);
			}
			else
				AfxMessageBox("m_QueueRead FULL!");
		  
			if(dwRead == 0)
				bSendMessage= false;
			else
				bSendMessage = true;

			//	읽어 가도록 메시지를 보낸다.
			if(bSendMessage)
 				::SendMessage(hCommWnd,WM_COMM_READ, pComm->m_wPortID, 0);
			
		}
	}	
	
	// 포트가 ClosePort에 의해 닫히면 m_bConnected 가 FALSE가 되어 종료.

	CloseHandle(os.hEvent);
//	pComm->m_hThreadWatchComm = NULL;

	return TRUE;
}


//COM2
DWORD	ThreadWatchComm2(CCommThread* pComm)
{
	DWORD		dwEvent;
	OVERLAPPED	os;
	BOOL		bOk = TRUE;
	BYTE		buff[BUFF_SIZE];	 // 읽기 버퍼
	DWORD		dwRead;	 // 읽은 바이트수.

	memset( &os, 0, sizeof(OVERLAPPED));
	if (! (os.hEvent = CreateEventA( NULL, TRUE, FALSE, NULL)))
		bOk = FALSE;
	if (! SetCommMask( pComm->m_hComm2, EV_RXCHAR))
		bOk = FALSE;
	if (! bOk)
	{
		AfxMessageBox("Error while creating ThreadWatchComm, " + pComm->m_sPortName2);
		return FALSE;
	}

	Sleep(1);

	while (pComm->m_bConnected2)
	{
		dwEvent = 0;

		WaitCommEvent( pComm->m_hComm2, &dwEvent, NULL);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			do	
			{
				dwRead = pComm->ReadComm2( buff, BUFF_SIZE);
				
				Sleep(10);
				
				if(BUFF_SIZE - pComm->m_QueueRead2.GetSize2() > (int)dwRead)
				{
					for ( WORD i = 0; i < dwRead; i++)
						pComm->m_QueueRead2.PutByte2(buff[i]);
				}
				else
					AfxMessageBox("COM2 - m_QueueRead FULL!");
			  
			} while (dwRead);

			::SendMessage(hCommWnd2,WM_COMM_READ2, pComm->m_wPortID2, 0);
		}
	}	
	
	CloseHandle( os.hEvent);
	pComm->m_hThreadWatchComm2 = NULL;

	return TRUE;
}
//COM3
DWORD	ThreadWatchComm3(CCommThread* pComm)
{
	DWORD		dwEvent;
	OVERLAPPED	os;
	BOOL		bOk = TRUE;
	BYTE		buff[BUFF_SIZE];	 // 읽기 버퍼
	DWORD		dwRead;	 // 읽은 바이트수.

	memset( &os, 0, sizeof(OVERLAPPED));
	if (! (os.hEvent = CreateEventA( NULL, TRUE, FALSE, NULL)))
		bOk = FALSE;
	if (! SetCommMask( pComm->m_hComm3, EV_RXCHAR))
		bOk = FALSE;
	if (! bOk)
	{
		AfxMessageBox("Error while creating ThreadWatchComm, " + pComm->m_sPortName3);
		return FALSE;
	}

	Sleep(1);

	while (pComm->m_bConnected3)
	{
		dwEvent = 0;

		WaitCommEvent( pComm->m_hComm3, &dwEvent, 0);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			do
			{
				dwRead = pComm->ReadComm3( buff, BUFF_SIZE);
				
				Sleep(10);

				if(BUFF_SIZE - pComm->m_QueueRead3.GetSize3() > (int)dwRead)
				{
					for ( WORD i = 0; i < dwRead; i++)
						pComm->m_QueueRead3.PutByte3(buff[i]);
				}
				else
					AfxMessageBox("COM3 - m_QueueRead FULL!");
			  
			} while (dwRead);

			::SendMessage(hCommWnd3,WM_COMM_READ3, pComm->m_wPortID3, 0);
		}
	}	

	CloseHandle( os.hEvent);
	pComm->m_hThreadWatchComm3 = NULL;

	return TRUE;
}

//COM4
DWORD	ThreadWatchComm4(CCommThread* pComm)
{
	DWORD		dwEvent;
	OVERLAPPED	os;
	BOOL		bOk = TRUE;
	BYTE		buff[BUFF_SIZE];	 // 읽기 버퍼
	DWORD		dwRead;	 // 읽은 바이트수.

	memset( &os, 0, sizeof(OVERLAPPED));
	if (! (os.hEvent = CreateEventA( NULL, TRUE, FALSE, NULL)))
		bOk = FALSE;
	if (! SetCommMask( pComm->m_hComm4, EV_RXCHAR))
		bOk = FALSE;
	if (! bOk)
	{
		AfxMessageBox("Error while creating ThreadWatchComm, " + pComm->m_sPortName4);
		return FALSE;
	}

	Sleep(1);

	while (pComm->m_bConnected4)
	{
		dwEvent = 0;

		WaitCommEvent( pComm->m_hComm4, &dwEvent, 0);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			do
			{
				dwRead = pComm->ReadComm4( buff, BUFF_SIZE);

				Sleep(10);
				
				if(BUFF_SIZE - pComm->m_QueueRead4.GetSize4() > (int)dwRead)
				{
					for ( WORD i = 0; i < dwRead; i++)
						pComm->m_QueueRead4.PutByte4(buff[i]);
				}
				else
					AfxMessageBox("COM4 - m_QueueRead FULL!");
			  
			} while (dwRead);

			::SendMessage(hCommWnd4,WM_COMM_READ4, pComm->m_wPortID4, 0);
		}
	}	

	CloseHandle( os.hEvent);
	pComm->m_hThreadWatchComm4 = NULL;

	return TRUE;
}