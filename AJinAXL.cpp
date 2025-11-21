// AJinAXL.cpp : 구현 파일
//
#include "stdafx.h"
#include "AJinAXL.h"

#include "AXL.h"
#include "AXD.h"
#pragma comment (lib, "AXL.lib")

CAJinAXL g_objAJinAXL;

CCriticalSection TriggerCS;

CAJinAXL::CAJinAXL(void)
{
	m_DY0.nValue = 0;
	m_DY1.nValue = 0;
	m_DY2.nValue = 0;
	m_DY3.nValue = 0;

 	LARGE_INTEGER freq;
 	QueryPerformanceFrequency(&freq);
 	m_nFreq = freq.QuadPart;
}

CAJinAXL::~CAJinAXL(void)
{
}

BOOL CAJinAXL::Initialize()
{
#ifdef AJIN_BOARD_USE
	DWORD dwReturn = AxlOpenNoReset(7);
	if (dwReturn != AXT_RT_SUCCESS) 
		return FALSE;

	long lDIOCount;
	dwReturn = AxdInfoGetModuleCount(&lDIOCount);
	if (dwReturn != AXT_RT_SUCCESS) 
		return FALSE;
	if (lDIOCount < 1) 
		return FALSE;	// Board 1 or 2장
#endif

	Init_Trigger(TRUE);		// 시작시 All On

	return TRUE;
}

void CAJinAXL::Terminate()
{
	Init_Trigger(FALSE);	// 종료시 All Off

#ifdef AJIN_BOARD_USE
	if (AxlIsOpened()) 
		AxlClose();
#endif
}

void CAJinAXL::Init_Trigger(BOOL bOn)
{
	DWORD dwValue = (bOn ? 0xFFFF : 0x0000);
	m_DY0.nValue = m_DY1.nValue = m_DY2.nValue = m_DY3.nValue = dwValue;

#ifdef AJIN_BOARD_USE
	for (int i = 0; i < 2; i++) 
		for (int j = 0; j < 2; j++) 
			AxdoWriteOutportWord(i, j, dwValue);
#endif
}

///////////////////////////////////////////////////////////////////////////////

void CAJinAXL::Set_Trigger(int nPort, BOOL* bLight, int nS, int iPageOffset, BOOL bSharedPage)
{
	TriggerCS.Lock();

	int nModule = nPort / 2;	// 0 or 1
	int nOffset = nPort % 2;	// 0 or 1

	DWORD dwValue = 0xFFFE;	// Camera Trigger Off

	for (int i = 0; i < 11; i++)
	{
		if (bLight[i])
			dwValue &= ~(1 << (i + iPageOffset));
	}

	switch (nPort) 
	{
	case 0: m_DY0.nValue = dwValue; break;
	case 1: 
		if (bSharedPage)
		{
			m_DY1.nValue = 0xFFFE;
			m_DY0.nValue = dwValue;
		}
		else
		{
			m_DY1.nValue = dwValue;
		}
		break;
	case 2: m_DY2.nValue = dwValue; break;
	case 3: m_DY3.nValue = dwValue; break;
	}

#ifdef AJIN_BOARD_USE

	if (bSharedPage && nPort == 1)
	{
		AxdoWriteOutportWord(0, 0, dwValue);
		AxdoWriteOutportWord(nModule, nOffset, 0xFFFE);
	}
	else
	{
		AxdoWriteOutportWord(nModule, nOffset, dwValue);
	}
#endif

	Delay(nS);	// Delay

	dwValue = 0xFFFF;		// All On
	switch (nPort) 
	{
	case 0: 
		m_DY0.nValue = dwValue; 
		break;
	case 1: 
		if (bSharedPage)
			m_DY0.nValue = dwValue; 
		m_DY1.nValue = dwValue;
		break;
	case 2: m_DY2.nValue = dwValue; break;
	case 3: m_DY3.nValue = dwValue; break;
	}
#ifdef AJIN_BOARD_USE
	if (bSharedPage && nPort == 1)
		AxdoWriteOutportWord(0, 0, dwValue);
	AxdoWriteOutportWord(nModule, nOffset, dwValue);
#endif

	TriggerCS.Unlock();
}

void CAJinAXL::Special_Set_Trigger(int nPort, BOOL* bLight, int nS)
{
	TriggerCS.Lock();

	int nModule = nPort / 2;	// 0 or 1
	int nOffset = nPort % 2;	// 0 or 1

	DWORD dwValue = (nOffset == 1 ? 0xFFFEFFFF : 0xFFFFFFFE);	// Camer Trigger Off
	for (int i = 0; i < 10; i++) if (bLight[i]) dwValue &= ~(1 << (i + 1));
	for (int i = 0; i < 10; i++) if (bLight[i+10]) dwValue &= ~(1 << (i + 17));

	switch (nModule)
	{
	case 0: m_DY0.nValue = dwValue; break;
	case 1: m_DY1.nValue = dwValue; break;
	}

#ifdef AJIN_BOARD_USE
	AxdoWriteOutportDword(nModule, 0, dwValue);
#endif

	Delay(nS);	// Delay

	dwValue = 0xFFFFFFFF;		// All On
	switch (nModule)
	{
	case 0: m_DY0.nValue = dwValue; break;
	case 1: m_DY1.nValue = dwValue; break;
	}

#ifdef AJIN_BOARD_USE
	AxdoWriteOutportDword(nModule, 0, dwValue);
#endif

	TriggerCS.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
// Bit Test

BOOL CAJinAXL::Get_Output(int nPort, int nIdx)
{
	switch (nPort) {
	case 0: return ((m_DY0.nValue >> nIdx) & 1);	// Y000 - Y015
	case 1: return ((m_DY1.nValue >> nIdx) & 1);	// Y100 - Y115
	case 2: return ((m_DY2.nValue >> nIdx) & 1);	// Y200 - Y215
	case 3: return ((m_DY3.nValue >> nIdx) & 1);	// Y300 - Y315
	}
	return FALSE;
}

void CAJinAXL::Set_Output(int nPort, int nIdx, BOOL bOn)
{
	int nModule = nPort / 2;	// 0 or 1
	int nOffset = nPort % 2;	// 0 or 1

	DWORD dwValue = 0, dwBit = (1 << nIdx);
	switch (nPort) {
	case 0: m_DY0.nValue = dwValue = (bOn ? (m_DY0.nValue | dwBit) : (m_DY0.nValue & ~dwBit)); break;	// Y000 - Y015
	case 1: m_DY1.nValue = dwValue = (bOn ? (m_DY1.nValue | dwBit) : (m_DY1.nValue & ~dwBit)); break;	// Y100 - Y115
	case 2: m_DY2.nValue = dwValue = (bOn ? (m_DY2.nValue | dwBit) : (m_DY2.nValue & ~dwBit)); break;	// Y200 - Y215
	case 3: m_DY3.nValue = dwValue = (bOn ? (m_DY3.nValue | dwBit) : (m_DY3.nValue & ~dwBit)); break;	// Y300 - Y315
	}
#ifdef AJIN_BOARD_USE
	AxdoWriteOutportWord(nModule, nOffset, dwValue);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// uSleep
// 
void CAJinAXL::DoEvents()
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CAJinAXL::uSleep(int msec)
{
	LARGE_INTEGER tStart, tNow;
	LONGLONG lTerm = 0;
	QueryPerformanceCounter(&tStart);

	while (TRUE) {
		QueryPerformanceCounter(&tNow);
		lTerm = (tNow.QuadPart - tStart.QuadPart) * 1000 / m_nFreq;
		if (lTerm > msec) break;
		DoEvents();
	}
}

void CAJinAXL::Delay(int msec)
{
	LARGE_INTEGER tStart, tNow;
	LONGLONG lTerm = 0;
	QueryPerformanceCounter(&tStart);

	while (TRUE) {
		QueryPerformanceCounter(&tNow);
		lTerm = (tNow.QuadPart - tStart.QuadPart) * 1000 / m_nFreq;
		if (lTerm > msec) 
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
