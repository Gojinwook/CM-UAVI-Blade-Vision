// Delegate.h: interface for the CDelegate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DELEGATE_H__D4C5C0D8_5403_440B_84B5_E70A4B0699CC__INCLUDED_)
#define AFX_DELEGATE_H__D4C5C0D8_5403_440B_84B5_E70A4B0699CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EventStub.h"
#include <Afxtempl.h>

class CDelegate  
{
public:
	CDelegate();
	virtual ~CDelegate();

	void	UseEvent(BOOL bUse) { m_bUseEvent = bUse; }

	void	FireEvent(DWORD wParam=0, DWORD lParam=0);
	void	PostEvent(DWORD wParam=0, DWORD lParam=0);

	void	operator += (CEventStub *pEventStub);
	void	operator -= (CWnd *pWnd);

	void	operator = (CDelegate& delegate);

protected:
	CList<CEventStub*, CEventStub*>	m_EventStubList;

	BOOL   m_bUseEvent;
};

#endif // !defined(AFX_DELEGATE_H__D4C5C0D8_5403_440B_84B5_E70A4B0699CC__INCLUDED_)
