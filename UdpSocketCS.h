// UdpSocketCS.h : header file
//
#pragma once

#define UM_UDP_RECEIVE	WM_USER + 9030

class CUdpSocketCS : public CAsyncSocket
{
public:
	CUdpSocketCS();
	virtual ~CUdpSocketCS();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUdpSocketCS)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CUdpSocketCS)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

private:
	HWND	m_hWnd;
	CString	m_strIP;		// IP Address
	UINT	m_nPort;		// Port
	UINT	m_nPortHandler;		// Port

public:
	BOOL Open_Socket(CString strIP, UINT nPort, HWND hWnd = NULL);
	void Close_Socket();
	void Write_String(CString strSend);
};

/////////////////////////////////////////////////////////////////////////////
