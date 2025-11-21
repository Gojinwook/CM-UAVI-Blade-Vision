// InspectResultDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectResultDlg.h"
#include "afxdialogex.h"


// CInspectResultDlg 대화 상자입니다.
CInspectResultDlg* CInspectResultDlg::m_pInstance = NULL;

CInspectResultDlg* CInspectResultDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectResultDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_RESULT_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}
void CInspectResultDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectResultDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}


IMPLEMENT_DYNAMIC(CInspectResultDlg, CDialog)

	CInspectResultDlg::CInspectResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectResultDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+520, VIEW1_DLG3_TOP+VIEW1_DLG3_HEIGHT+50, VIEW1_DLG3_WIDTH-530, VIEW1_DLG3_HEIGHT+70);
	m_iResetTime = 0;
}

CInspectResultDlg::~CInspectResultDlg()
{

}

void CInspectResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_RESET_TIME_HR, m_iResetTime);
}


BEGIN_MESSAGE_MAP(CInspectResultDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_WINDOWPOSCHANGING()
	ON_EN_CHANGE(IDC_EDIT_LOT_ID, &CInspectResultDlg::OnEnChangeEditLotId)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SET_RESET_TIME, &CInspectResultDlg::OnBnClickedButtonSetResetTime)
END_MESSAGE_MAP()


// CInspectResultDlg 메시지 처리기입니다.


BOOL CInspectResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	InitStaticText();

	CString sStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS TXT_Status(sStatusFileName);
	CString sSection = "Status";
	SetDlgItemText(IDC_EDIT_LOT_ID, TXT_Status.Get_String(sSection, "LotID", "Default"));
	SetDlgItemText(IDC_EDIT_MODEL_NAME, TXT_Status.Get_String(sSection, "ModelID", "Default"));

	THEAPP.m_pInspectSummary->m_iDxTrayNo = TXT_Status.Get_Integer(sSection, "TrayNo", 1);
	THEAPP.m_pInspectSummary->m_iDxLineNo = TXT_Status.Get_Integer(sSection, "LineNo", 1);
	THEAPP.m_pInspectSummary->UpdateData(FALSE);


	SetDlgItemText(IDC_EDIT_MODEL_NAME, "MODEL_NAME");
	SetDlgItemText(IDC_EDIT_COUNTTRAY, "0");

	SetDlgItemText(IDC_EDIT_NUMBER_OF_MODULE, "0");
	SetDlgItemText(IDC_EDIT_OKCOUNT, "0");
	SetDlgItemText(IDC_EDIT_NGCOUNT, "0");
	SetDlgItemText(IDC_EDIT_BARCODE_ERROR_LOT, "0");
	SetDlgItemText(IDC_EDIT_YIELD, "0%");
	SetDlgItemText(IDC_EDIT_DEFECT_RATIO_LOT, "0/0/0 (%)");

	SetDlgItemText(IDC_EDIT_NUMBER_OF_MODULE_DAY, "0");
	SetDlgItemText(IDC_EDIT_OKCOUNT_DAY, "0");
	SetDlgItemText(IDC_EDIT_NGCOUNT_DAY, "0");
	SetDlgItemText(IDC_EDIT_YIELD_DAY, "0%");
	SetDlgItemText(IDC_EDIT_DEFECT_RATIO_DAY, "0/0/0 (%)");

	SetTimer(1,500,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CInspectResultDlg::InitStaticText()
{
	CFont* pFont;
	LONG lfWidth = 6, lfHeight = 16, lfWeight = FW_SEMIBOLD;

	pFont = GetFont(lfWidth, lfHeight, lfWeight);

	GetDlgItem(IDC_STATIC_LOTID)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_ALLCOUNT_MODULE)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_MODELID)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_YIELD)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_COUNT_OK_MODULE)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_COUNT_NG_MODULE)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_TRAYCOUNT)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_DEFECT_RATIO_LOT)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_BARCODE_ERROR_LOT)->SetFont(pFont);

	GetDlgItem(IDC_STATIC_ALLCOUNT_MODULE_DAY)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_YIELD_DAY)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_COUNT_OK_MODULE_DAY)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_COUNT_NG_MODULE_DAY)->SetFont(pFont);
	GetDlgItem(IDC_STATIC_DEFECT_RATIO_DAY)->SetFont(pFont);

	pFont->Detach();
	delete pFont;     
}

CFont* CInspectResultDlg::GetFont(LONG lfWidth, LONG lfHeight, LONG lfWeight)
{
	CFont* pFont;
	LOGFONT logFont;

	pFont = new CFont();
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfWidth = lfWidth;
	logFont.lfHeight = lfHeight;
	logFont.lfWeight = lfWeight;
	_tcscpy( logFont.lfFaceName, _T("사용자글자") );
	pFont->CreateFontIndirectA(&logFont);
	return pFont;        
}

HBRUSH CInspectResultDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	CRect rect; 
	UINT nID = pWnd->GetDlgCtrlID();

	if(nCtlColor == CTLCOLOR_STATIC)

	{     
		switch(nID)
		{
		case IDC_STATIC_LOTID:
			{
				pDC->SetTextColor(RGB(255, 0, 0));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_MODELID:
			{
				pDC->SetTextColor(RGB(255, 0, 0));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_ALLCOUNT_MODULE:
			{
				pDC->SetTextColor(RGB(255, 0, 0));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_YIELD:
			{
				pDC->SetTextColor(RGB(255, 0, 0));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_COUNT_OK_MODULE:
			{
				pDC->SetTextColor(RGB(255, 0, 0));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_COUNT_NG_MODULE:
			{
				pDC->SetTextColor(RGB(255, 0, 0));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_TRAYCOUNT:
			{
				pDC->SetTextColor(RGB(255, 0, 0));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_DEFECT_RATIO_LOT:
			{
				pDC->SetTextColor(RGB(255, 0, 0));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_BARCODE_ERROR_LOT:
			{
				pDC->SetTextColor(RGB(255, 255, 255));
				pDC->SetBkColor(RGB(1, 1, 1));
				break;
			}
		case IDC_STATIC_ALLCOUNT_MODULE_DAY:
			{
				pDC->SetTextColor(RGB(100, 0, 100));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_YIELD_DAY:
			{
				pDC->SetTextColor(RGB(100, 0, 100));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_COUNT_OK_MODULE_DAY:
			{
				pDC->SetTextColor(RGB(100, 0, 100));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_COUNT_NG_MODULE_DAY:
			{
				pDC->SetTextColor(RGB(100, 0, 100));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		case IDC_STATIC_DEFECT_RATIO_DAY:
			{
				pDC->SetTextColor(RGB(100, 0, 100));
				pDC->SetBkColor(RGB(0, 255, 0));
				break;
			}
		}

	}
	return hbr;
}

void CInspectResultDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	lpwndpos->flags |= SWP_NOMOVE; 
	CDialog::OnWindowPosChanging(lpwndpos); 
}


void CInspectResultDlg::OnEnChangeEditLotId()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	// 하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	GetDlgItemText(IDC_EDIT_LOT_ID, CurrentLotID);
}


BOOL CInspectResultDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;

	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONDBLCLK)
	{
		CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);
		if (pWnd && pWnd->GetDlgCtrlID() == IDC_EDIT_LOT_ID)
		{
			pWnd->SetFocus();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL bTimeReset = FALSE;
void CInspectResultDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case 1:
		{
			UpdateData(TRUE);
			CTime time = CTime::GetTickCount();
			if(time.GetHour() == m_iResetTime)
			{
				if(time.GetMinute() == 1)
				{
					if(time.GetSecond() > 1 && time.GetSecond() < 5)
					{
						if (bTimeReset == FALSE) 
						{
							CString sPerformFile;
							sPerformFile.Format("%s\\DaySummary_%04d%02d%02d.txt", THEAPP.m_FileBase.m_strResultFolderPrev, time.GetYear(), time.GetMonth(), time.GetDay());

							CIniFileCS DPINI(sPerformFile);

							CString sSectionDay = "Today";

							CString sOldDay; sOldDay.Format("%d%02d%02d", time.GetYear(), time.GetMonth(), time.GetDay());
							DPINI.Set_Integer(sOldDay, "총량", DPINI.Get_Integer(sSectionDay, "총량", 0));
							DPINI.Set_Integer(sOldDay, "양품", DPINI.Get_Integer(sSectionDay, "양품", 0));
							DPINI.Set_Integer(sOldDay, "불량", DPINI.Get_Integer(sSectionDay, "불량", 0));
							DPINI.Set_Double(sOldDay, "수율", DPINI.Get_Double(sSectionDay, "수율", 0.0));
							DPINI.Set_Integer(sOldDay, "기준시간", DPINI.Get_Integer(sSectionDay, "기준시간", 0));

							DPINI.Set_Integer(sSectionDay, "총량", 0);
							DPINI.Set_Integer(sSectionDay, "양품", 0);
							DPINI.Set_Integer(sSectionDay, "불량", 0);
							DPINI.Set_Double(sSectionDay, "수율", 0.0);

							bTimeReset = TRUE;
						}
					}
				}
				else {
					if (bTimeReset == TRUE) 
						bTimeReset = FALSE;
				}
			}
			break;
		}
	}
	CDialog::OnTimer(nIDEvent);
}


void CInspectResultDlg::OnBnClickedButtonSetResetTime()
{
	UpdateData(TRUE);
	if (m_iResetTime < 0) m_iResetTime = 0;
	if (m_iResetTime > 23) m_iResetTime = 23;
	UpdateData(FALSE); // 변수 -> 컨트롤

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString sPerformFile;
	sPerformFile.Format("%s\\DaySummary_%04d%02d%02d.txt", THEAPP.m_FileBase.m_strResultFolderPrev, time.wYear, time.wMonth, time.wDay);
	
	CIniFileCS DPINI(sPerformFile);

	CString sSectionDay = "Today";

	DPINI.Set_Integer(sSectionDay, "기준시간", m_iResetTime);
}



SYSTEMTIME CInspectResultDlg::FindingYesterDay(SYSTEMTIME Todaytime)
{
	SYSTEMTIME YesterDayTime;

	if(Todaytime.wMonth == 1 && Todaytime.wDay == 1)		// 1 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear - 1;
		YesterDayTime.wMonth	= 12;
		YesterDayTime.wDay		= 31;
	}
	else if(Todaytime.wMonth == 2 && Todaytime.wDay == 1)	// 2 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 1;
		YesterDayTime.wDay		= 31;
	}
	else if(Todaytime.wMonth == 3 && Todaytime.wDay == 1)	// 3 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 2;
		YesterDayTime.wDay		= 28;
	}
	else if(Todaytime.wMonth == 4 && Todaytime.wDay == 1)	// 4 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 3;
		YesterDayTime.wDay		= 31;
	}
	else if(Todaytime.wMonth == 5 && Todaytime.wDay == 1)	// 5 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 4;
		YesterDayTime.wDay		= 30;
	}
	else if(Todaytime.wMonth == 6 && Todaytime.wDay == 1)	// 6 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 5;
		YesterDayTime.wDay		= 31;
	}
	else if(Todaytime.wMonth == 7 && Todaytime.wDay == 1)	// 7 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 6;
		YesterDayTime.wDay		= 30;
	}
	else if(Todaytime.wMonth == 8 && Todaytime.wDay == 1)	// 8 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 7;
		YesterDayTime.wDay		= 31;
	}
	else if(Todaytime.wMonth == 9 && Todaytime.wDay == 1)	// 9 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 8;
		YesterDayTime.wDay		= 31;
	}
	else if(Todaytime.wMonth == 10 && Todaytime.wDay == 1)	// 10 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 9;
		YesterDayTime.wDay		= 30;
	}
	else if(Todaytime.wMonth == 11 && Todaytime.wDay == 1)	// 11 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 10;
		YesterDayTime.wDay		= 31;
	}
	else if(Todaytime.wMonth == 12 && Todaytime.wDay == 1)	// 12 월 1일 일 경우
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= 11;
		YesterDayTime.wDay		= 30;
	}
	else
	{
		YesterDayTime.wYear		= Todaytime.wYear;
		YesterDayTime.wMonth	= Todaytime.wMonth;
		YesterDayTime.wDay		= Todaytime.wDay-1;
	}

	return YesterDayTime;

}