// LogDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LogDlg.h"
#include "afxdialogex.h"


// CLogDlg 대화 상자입니다.
CLogDlg* CLogDlg::m_pInstance = NULL;

CLogDlg* CLogDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CLogDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_LOG_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}
void CLogDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CLogDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	
	CRect DisplayRect;
	this->GetClientRect( &DisplayRect );
	DisplayRect.NormalizeRect();
	this->ClientToScreen( &DisplayRect );	//picture control size
	ScreenToClient( &DisplayRect);

	m_ListLog.MoveWindow(10,10,DisplayRect.Width()-20,DisplayRect.Height()-20);
	ShowWindow(SW_SHOW);
}
IMPLEMENT_DYNAMIC(CLogDlg, CDialog)

CLogDlg::CLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+70, VIEW1_DLG3_TOP+40+VIEW1_DLG3_HEIGHT+510, VIEW1_DLG3_WIDTH-530, VIEW1_DLG3_HEIGHT+20);

	m_sCurrentLotID = "";
	m_sCurrentTotalModuleMix = "";
	m_sCurrentTotalBarcodeShift = "";
	m_sCurrentTotalMatchingError = "";
	m_sCurrentTotalFAISpecialNG = "";

	m_sBeforeLotID = "";
	m_sBeforeTotalModuleMix = "";
	m_sBeforeTotalBarcodeShift = "";
	m_sBeforeTotalMatchingError = "";
	m_sBeforeTotalFAISpecialNG = "";

	m_sBefore2LotID = "";
	m_sBefore2TotalModuleMix = "";
	m_sBefore2TotalBarcodeShift = "";
	m_sBefore2TotalMatchingError = "";
	m_sBefore2TotalFAISpecialNG = "";
}

CLogDlg::~CLogDlg()
{
}

void CLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListLog);
}


BEGIN_MESSAGE_MAP(CLogDlg, CDialog)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// CLogDlg 메시지 처리기입니다.


void CLogDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	 MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	 lpwndpos->flags |= SWP_DRAWFRAME; 
	 CDialog::OnWindowPosChanging(lpwndpos); 
}


BOOL CLogDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch (pMsg->message) {
	case WM_NCLBUTTONDOWN :
		SetActiveWindow();
		return TRUE;
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) return TRUE;
		break;
	case WM_SYSKEYDOWN:
		if (pMsg->wParam == VK_F4) return TRUE;
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CLogDlg::ClearView()
{
	m_ListLog.SetBackgroundColor(FALSE, RGB(255, 255, 255));
	m_ListLog.SetSel(0,-1);
	m_ListLog.Clear();
	m_ListLog.SetWindowText("");

	m_ListLog.Invalidate();
	m_ListLog.UpdateWindow();
}

void CLogDlg::WriteLog(CString sLogText, int iColor, int iCharHeight, BOOL bChangeBGColor, BOOL bClearView)
{
	if (bClearView)
	{
		m_ListLog.SetSel(0,-1);
		m_ListLog.Clear();
		m_ListLog.SetWindowText("");
	}

	long len;

	len = m_ListLog.GetTextLength();

	m_ListLog.SetSel(len,len);

	CHARFORMAT cf;

	cf.cbSize    = sizeof(CHARFORMAT);
	cf.dwMask    = CFM_COLOR|CFM_SIZE;
	cf.dwEffects  = (unsigned long)~( CFE_AUTOCOLOR | CFE_UNDERLINE | CFE_BOLD);

	if (iColor == LOG_COLOR_BLACK)
		cf.crTextColor = RGB(0, 0, 0);
	else if (iColor == LOG_COLOR_RED)
		cf.crTextColor = RGB(255, 0, 0);
	else if (iColor == LOG_COLOR_YELLOW)
		cf.crTextColor = RGB(255, 255, 0);
	else if (iColor == LOG_COLOR_GREEN)
		cf.crTextColor = RGB(0, 255, 0);
	else if (iColor == LOG_COLOR_BLUE)
		cf.crTextColor = RGB(0, 0, 255);
	else if (iColor == LOG_COLOR_PURPLE)
		cf.crTextColor = RGB(128, 0, 128);
	else if (iColor == LOG_COLOR_ORANGE)
		cf.crTextColor = RGB(255, 165, 0);
	else if (iColor == LOG_COLOR_LIME)
		cf.crTextColor = RGB(173, 255, 47);
	else if (iColor == LOG_COLOR_CYAN)
		cf.crTextColor = RGB(0, 255, 255);
	else if (iColor == LOG_COLOR_NAVY)
		cf.crTextColor = RGB(0, 0, 128);
	else if (iColor == LOG_COLOR_MAGENTA)
		cf.crTextColor = RGB(255, 0, 255);
	else
		cf.crTextColor = RGB(0, 0, 0); // 기본값: 검정

	cf.yHeight = iCharHeight;
	m_ListLog.SetSelectionCharFormat(cf);

	if (bChangeBGColor)
		m_ListLog.SetBackgroundColor(FALSE, RGB(255, 255, 0));

	CString strInsert;

	strInsert = sLogText;

	strInsert += "\r\n";

	m_ListLog.ReplaceSel((LPCTSTR)strInsert);

	m_ListLog.Invalidate();
}
