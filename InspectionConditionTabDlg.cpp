// InspectionConditionTabDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectionConditionTabDlg.h"
#include "afxdialogex.h"


// CInspectionConditionTabDlg 대화 상자입니다.

CInspectionConditionTabDlg* CInspectionConditionTabDlg::m_pInstance = NULL;

CInspectionConditionTabDlg* CInspectionConditionTabDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectionConditionTabDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECTION_CONDITION_TAB, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CInspectionConditionTabDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectionConditionTabDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CInspectionConditionTabDlg, CDialog)

CInspectionConditionTabDlg::CInspectionConditionTabDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectionConditionTabDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+100, VIEW1_DLG3_TOP+40+50+VIEW1_DLG3_HEIGHT+270+170+40, VIEW1_DLG3_WIDTH-110, VIEW1_DLG3_HEIGHT-10);

	m_pConditionTopBarrelDlg = NULL;
	m_pConditionBottomBarrelDlg = NULL;
	m_pConditionInnerBarrelDlg = NULL;
	m_pConditionLensDlg = NULL;
}

CInspectionConditionTabDlg::~CInspectionConditionTabDlg()
{
	SAFE_DELETE(m_pConditionTopBarrelDlg);
	SAFE_DELETE(m_pConditionBottomBarrelDlg);
	SAFE_DELETE(m_pConditionInnerBarrelDlg);
	SAFE_DELETE(m_pConditionLensDlg);
}

void CInspectionConditionTabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONDITION_TAB, m_InspectionConditionTabCtrl);
}


BEGIN_MESSAGE_MAP(CInspectionConditionTabDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CONDITION_TAB, &CInspectionConditionTabDlg::OnSelchangeInspectionConditionTab)
END_MESSAGE_MAP()


// CInspectionConditionTabDlg 메시지 처리기입니다.

BOOL CInspectionConditionTabDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_InspectionConditionTabCtrl.DeleteAllItems();
	m_InspectionConditionTabCtrl.InsertItem(0, " Top Barrel");
	m_InspectionConditionTabCtrl.InsertItem(1, " Bottom Barrel");
	m_InspectionConditionTabCtrl.InsertItem(2, " Inner Barrel");
	m_InspectionConditionTabCtrl.InsertItem(3, "     Lens    ");

	CRect DlgRect;

	m_pConditionTopBarrelDlg = new CConditionTopBarrelDlg;
	m_pConditionTopBarrelDlg->Create(IDD_CONDITION_TOP_BARREL, &m_InspectionConditionTabCtrl);
	m_pConditionTopBarrelDlg->GetWindowRect(&DlgRect);
	m_pConditionTopBarrelDlg->MoveWindow(5, 25, DlgRect.Width(), DlgRect.Height());
	m_pConditionTopBarrelDlg->ShowWindow(SW_SHOW);

	m_pConditionBottomBarrelDlg = new CConditionBottomBarrelDlg;
	m_pConditionBottomBarrelDlg->Create(IDD_CONDITION_BOTTOM_BARREL, &m_InspectionConditionTabCtrl);
	m_pConditionBottomBarrelDlg->GetWindowRect(&DlgRect);
	m_pConditionBottomBarrelDlg->MoveWindow(5, 25, DlgRect.Width(), DlgRect.Height());
	m_pConditionBottomBarrelDlg->ShowWindow(SW_HIDE);

	m_pConditionInnerBarrelDlg = new CConditionInnerBarrelDlg;
	m_pConditionInnerBarrelDlg->Create(IDD_CONDITION_INNER_BARREL, &m_InspectionConditionTabCtrl);
	m_pConditionInnerBarrelDlg->GetWindowRect(&DlgRect);
	m_pConditionInnerBarrelDlg->MoveWindow(5, 25, DlgRect.Width(), DlgRect.Height());
	m_pConditionInnerBarrelDlg->ShowWindow(SW_HIDE);

	m_pConditionLensDlg = new CConditionLensDlg;
	m_pConditionLensDlg->Create(IDD_CONDITION_LENS, &m_InspectionConditionTabCtrl);
	m_pConditionLensDlg->GetWindowRect(&DlgRect);
	m_pConditionLensDlg->MoveWindow(5, 25, DlgRect.Width(), DlgRect.Height());
	m_pConditionLensDlg->ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CInspectionConditionTabDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

void CInspectionConditionTabDlg::OnSelchangeInspectionConditionTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iSelected = m_InspectionConditionTabCtrl.GetCurSel();

	switch (iSelected)
	{
	case 0:
		m_pConditionTopBarrelDlg->ShowWindow(SW_SHOW);
		m_pConditionBottomBarrelDlg->ShowWindow(SW_HIDE);
		m_pConditionInnerBarrelDlg->ShowWindow(SW_HIDE);
		m_pConditionLensDlg->ShowWindow(SW_HIDE);
		break;

	case 1:
		m_pConditionTopBarrelDlg->ShowWindow(SW_HIDE);
		m_pConditionBottomBarrelDlg->ShowWindow(SW_SHOW);
		m_pConditionInnerBarrelDlg->ShowWindow(SW_HIDE);
		m_pConditionLensDlg->ShowWindow(SW_HIDE);
		break;

	case 2:
		m_pConditionTopBarrelDlg->ShowWindow(SW_HIDE);
		m_pConditionBottomBarrelDlg->ShowWindow(SW_HIDE);
		m_pConditionInnerBarrelDlg->ShowWindow(SW_SHOW);
		m_pConditionLensDlg->ShowWindow(SW_HIDE);
		break;

	case 3:
		m_pConditionTopBarrelDlg->ShowWindow(SW_HIDE);
		m_pConditionBottomBarrelDlg->ShowWindow(SW_HIDE);
		m_pConditionInnerBarrelDlg->ShowWindow(SW_HIDE);
		m_pConditionLensDlg->ShowWindow(SW_SHOW);
		break;

	default:
		break;
	}

	*pResult = 0;
}