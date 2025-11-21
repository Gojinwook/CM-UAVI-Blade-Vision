// Delegate.cpp: implementation of the CDelegate class.
// 
// Designed by Park.Ik.Hyung 2005.04.22
//		You can use this Delegate class as a C# Code,
//		and it usually works with CEventStub class...
//		see, example code in the TestDelegate project.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Delegate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char* THIS_FILE=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDelegate::CDelegate()
{
	m_bUseEvent = TRUE;
}

CDelegate::~CDelegate()
{
	CEventStub *pEventStub;

	POSITION pos = m_EventStubList.GetHeadPosition();

	while(pos)
	{
		pEventStub = m_EventStubList.GetNext(pos);

		delete pEventStub;
	}

	m_EventStubList.RemoveAll();
}

void CDelegate::FireEvent(DWORD wParam, DWORD lParam)
{

	if(!m_bUseEvent) return;

	POSITION pos = m_EventStubList.GetHeadPosition();
	CEventStub *pEventStub;

	while(pos)
	{
		pEventStub = m_EventStubList.GetNext(pos);
		pEventStub->FireEvent(wParam, lParam);
	}
}

void CDelegate::PostEvent(DWORD wParam, DWORD lParam)
{

	if(!m_bUseEvent) return;

	POSITION pos = m_EventStubList.GetHeadPosition();
	CEventStub *pEventStub;

	while(pos)
	{
		pEventStub = m_EventStubList.GetNext(pos);
		pEventStub->PostEvent(wParam, lParam);
	}
}

void CDelegate::operator += (CEventStub *pEventStub)
{
	POSITION pos = m_EventStubList.GetHeadPosition();
	CEventStub *pEventStubList;

	while(pos)		// yjs 2006.09.29
	{
		pEventStubList = (CEventStub *)m_EventStubList.GetNext(pos);

		if((pEventStubList->m_pReceiverWnd == pEventStub->m_pReceiverWnd) && (pEventStubList->m_uFireMsgID == pEventStub->m_uFireMsgID))
		{
			return;
		}
	}
	
	m_EventStubList.AddTail(pEventStub);
}

void CDelegate::operator -= (CWnd *pWnd)
{
	POSITION pos = m_EventStubList.GetHeadPosition();
	CEventStub *pEventStub;

	while(pos)
	{
		pEventStub = (CEventStub *)m_EventStubList.GetAt(pos);

		if(pEventStub->m_pReceiverWnd == pWnd)
		{
			delete pEventStub;

			m_EventStubList.RemoveAt(pos);
			return;
		}

		m_EventStubList.GetNext(pos);
	}
}

void CDelegate::operator = (CDelegate& delegate)
{
	POSITION pos = delegate.m_EventStubList.GetHeadPosition();
	CEventStub *pEventStub;

	while(pos)
	{
		pEventStub = delegate.m_EventStubList.GetNext(pos);

		CEventStub *pNewEventStub;			// 20080723 eunsung
		pNewEventStub = new CEventStub( pEventStub->m_pReceiverWnd, pEventStub->m_uFireMsgID, pEventStub->m_bMsgType);

		m_EventStubList.AddTail(pNewEventStub);//pEventStub);		// 20080723 eunsung
	}
}

