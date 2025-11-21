// UdpSocketCS.cpp : implementation file
//
#include "stdafx.h"
#include "UdpSocketCS.h"

// 추가: 에러 코드 해석용 헬퍼
static CString _GetSocketErrorText(int err)
{
	switch (err) {
	case 0: return "NO_ERROR";
	case WSAEINTR: return "WSAEINTR(Interrupted)";
	case WSAEBADF: return "WSAEBADF(Bad file handle)";
	case WSAEACCES: return "WSAEACCES(Permission denied)";
	case WSAEFAULT: return "WSAEFAULT(Bad address)";
	case WSAEINVAL: return "WSAEINVAL(Invalid argument)";
	case WSAEMFILE: return "WSAEMFILE(Too many open files)";
	case WSAEWOULDBLOCK: return "WSAEWOULDBLOCK(Resource temporarily unavailable)";
	case WSAEINPROGRESS: return "WSAEINPROGRESS(Operation now in progress)";
	case WSAEALREADY: return "WSAEALREADY(Operation already in progress)";
	case WSAENOTSOCK: return "WSAENOTSOCK(Not a socket)";
	case WSAEDESTADDRREQ: return "WSAEDESTADDRREQ(Destination address required)";
	case WSAEMSGSIZE: return "WSAEMSGSIZE(Message too long)";
	case WSAEPROTOTYPE: return "WSAEPROTOTYPE(Protocol wrong type)";
	case WSAENOPROTOOPT: return "WSAENOPROTOOPT(Bad protocol option)";
	case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT(Protocol not supported)";
	case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT(Socket type not supported)";
	case WSAEOPNOTSUPP: return "WSAEOPNOTSUPP(Operation not supported)";
	case WSAEPFNOSUPPORT: return "WSAEPFNOSUPPORT(Protocol family not supported)";
	case WSAEAFNOSUPPORT: return "WSAEAFNOSUPPORT(Address family not supported)";
	case WSAEADDRINUSE: return "WSAEADDRINUSE(Address already in use)";
	case WSAEADDRNOTAVAIL: return "WSAEADDRNOTAVAIL(Address not available)";
	case WSAENETDOWN: return "WSAENETDOWN(Network is down)";
	case WSAENETUNREACH: return "WSAENETUNREACH(Network unreachable)";
	case WSAENETRESET: return "WSAENETRESET(Network dropped connection)";
	case WSAECONNABORTED: return "WSAECONNABORTED(Connection aborted)";
	case WSAECONNRESET: return "WSAECONNRESET(Connection reset)";
	case WSAENOBUFS: return "WSAENOBUFS(No buffer space)";
	case WSAEISCONN: return "WSAEISCONN(Socket is connected)";
	case WSAENOTCONN: return "WSAENOTCONN(Socket not connected)";
	case WSAETIMEDOUT: return "WSAETIMEDOUT(Timed out)";
	case WSAECONNREFUSED: return "WSAECONNREFUSED(Connection refused)";
	case WSAEHOSTDOWN: return "WSAEHOSTDOWN(Host down)";
	case WSAEHOSTUNREACH: return "WSAEHOSTUNREACH(No route to host)";
	case WSAEPROCLIM: return "WSAEPROCLIM(Too many processes)";
	case WSASYSNOTREADY: return "WSASYSNOTREADY(System not ready)";
	case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED(Version not supported)";
	case WSANOTINITIALISED: return "WSANOTINITIALISED(WSA not initialised)";
	default: {
		CString s; s.Format("Unknown(%d)", err); return s; }
	}
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUFFER_SIZE		4096

CUdpSocketCS::CUdpSocketCS()
{
	m_hWnd = NULL;
	m_strIP = "0.0.0.0";
	m_nPort = 0;
}

CUdpSocketCS::~CUdpSocketCS()
{
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CUdpSocketCS, CAsyncSocket)
	//{{AFX_MSG_MAP(CUdpSocketCS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CUdpSocketCS member functions

void CUdpSocketCS::OnReceive(int nErrorCode) 
{
	if (!nErrorCode) {
		char cRecv[BUFFER_SIZE] = {0};
		int nReturn = ReceiveFrom(cRecv, BUFFER_SIZE, m_strIP, m_nPort);
		// CMI3000 에서는 활성화 시켜서 수신을 체크해라
		if (nReturn == SOCKET_ERROR) {
			int wsaErr = WSAGetLastError();
			CString msg; msg.Format("Handler UDP 데이터 수신 에러 (WSAErr=%d, %s) 포트=%u IP=%s", wsaErr, _GetSocketErrorText(wsaErr), m_nPort, m_strIP);
			AfxMessageBox(msg, MB_ICONWARNING);
		}
		else {
			CString strRecv = (CString)cRecv;
			if (m_hWnd)	SendMessage(m_hWnd, UM_UDP_RECEIVE, (WPARAM)&m_strIP, (LPARAM)&strRecv);
		}
	}
	else {
		int wsaErr = WSAGetLastError();
		CString msg; msg.Format("UDP Network System Fail. nErrorCode=%d WSAErr=%d %s", nErrorCode, wsaErr, _GetSocketErrorText(wsaErr));
		AfxMessageBox(msg, MB_ICONWARNING);
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
// Public Functions

BOOL CUdpSocketCS::Open_Socket(CString strIP, UINT nPort, HWND hWnd)
{
	m_strIP = strIP;
	///////////////////////// changed for CMI 3000 2000 ====> /////////////////
	m_nPort = nPort-1;	// CMI 3000에서는 nPort -1로 바꿔서 8000으로 맞춰준다
	///////////////////////// <==== changed for CMI 3000 2000 /////////////////
	m_nPortHandler = nPort-1;
	m_hWnd = hWnd;
	if (Create(m_nPort, SOCK_DGRAM)) return TRUE;
	else return FALSE;
}

void CUdpSocketCS::Close_Socket()
{
	Close();
}

void CUdpSocketCS::Write_String(CString strSend)
{
	int nReturn = SendTo(strSend, strSend.GetLength(), m_nPortHandler, m_strIP);
	if (nReturn == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			int wsaErr = WSAGetLastError();
			CString msg; msg.Format("Handler 데이터 전송 실패 (WSAErr=%d, %s) 포트=%u IP=%s", wsaErr, _GetSocketErrorText(wsaErr), m_nPortHandler, m_strIP);
			AfxMessageBox(msg, MB_ICONWARNING);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
