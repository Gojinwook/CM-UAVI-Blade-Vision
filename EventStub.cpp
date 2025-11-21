// EventStub.cpp : implementation file
//
#include "stdafx.h"
#include "EventStub.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char* THIS_FILE = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventStub

CEventStub::CEventStub(CWnd *pReceiverWnd, UINT uFireMsgID, BOOL bMsgType)
{
	m_pReceiverWnd = pReceiverWnd; 
	m_uFireMsgID = uFireMsgID;

	m_bMsgType = bMsgType;
}

CEventStub::~CEventStub()
{
}

void CEventStub::FireEvent(DWORD wParam, DWORD lParam)
{
	if(m_pReceiverWnd && m_uFireMsgID)
	{
		if(m_pReceiverWnd->GetSafeHwnd())
		{
			if(m_bMsgType)
			{
				m_pReceiverWnd->SendMessage(m_uFireMsgID, (WPARAM) wParam, (LPARAM) lParam);
			}
			else
			{
				m_pReceiverWnd->PostMessage(m_uFireMsgID, (WPARAM) wParam, (LPARAM) lParam);
			}
		}
	}
#ifdef _DEBUG
	else
	{
//		AfxMessageBox("m_pReceiverWnd & m_uFireMsgID are not specified!!!");
	}
#endif
}

void CEventStub::PostEvent(DWORD wParam, DWORD lParam)
{
	if(m_pReceiverWnd && m_uFireMsgID)
	{
		if(m_pReceiverWnd->GetSafeHwnd())
		{
			m_pReceiverWnd->PostMessage(m_uFireMsgID, (WPARAM) wParam, (LPARAM) lParam);
		}
	}
#ifdef _DEBUG
	else
	{
//		AfxMessageBox("m_pReceiverWnd & m_uFireMsgID are not specified!!!");
	}
#endif
}

BOOL CEventStub::operator == (CWnd *pWnd)
{
//	return (m_pReceiverWnd == pWnd);
	return (m_pReceiverWnd->m_hWnd == pWnd->m_hWnd);
}

