#if !defined(AFX_EVENTSTUB_H__7C9A6F3A_7466_4CE7_86A4_E64A3F2EFBD0__INCLUDED_)
#define AFX_EVENTSTUB_H__7C9A6F3A_7466_4CE7_86A4_E64A3F2EFBD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventStub.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventStub window

class CEventStub
{
// Construction
public:						//bMsgType - 1: SendMassage, 0: PostMessage
	CEventStub(CWnd *pReceiverWnd, UINT uFireMsgID, BOOL bMsgType=1);
	virtual ~CEventStub();

	void	FireEvent(DWORD wParam, DWORD lParam);
	void	PostEvent(DWORD wParam, DWORD lParam);

	BOOL	operator == (CWnd *pWnd);

// Attributes
	CWnd*	m_pReceiverWnd;
	UINT	m_uFireMsgID;
	BOOL	m_bMsgType;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTSTUB_H__7C9A6F3A_7466_4CE7_86A4_E64A3F2EFBD0__INCLUDED_)
