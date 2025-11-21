// ROIIDChangeDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "uScan.h"
#include "ROIIDChangeDlg.h"
#include "afxdialogex.h"


// CROIIDChangeDlg 대화 상자

IMPLEMENT_DYNAMIC(CROIIDChangeDlg, CDialog)

CROIIDChangeDlg::CROIIDChangeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ROI_ID_CHANGE, pParent)
{
	m_iEditId = 1;
	m_iROIType = INSPECTION_TYPE_LOCAL_ALIGN;
}

CROIIDChangeDlg::~CROIIDChangeDlg()
{

}

void CROIIDChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROI_CHANGE_ID, m_iEditId);
	DDV_MinMaxInt(pDX, m_iEditId, 1, 1000);
}


BEGIN_MESSAGE_MAP(CROIIDChangeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CROIIDChangeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CROIIDChangeDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CROIIDChangeDlg 메시지 처리기


void CROIIDChangeDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	BOOL bIDUsed = FALSE;

	if (m_iROIType == INSPECTION_TYPE_LOCAL_ALIGN)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckLocalAlignID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckFAIOuterCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckFAIInnerCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckFAIDatumCircleID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckFAIDatumEllipseID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_FAI_WELDING_SPOT)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckFAIWeldingSpotID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_FAI_WELDING_POCKET)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckFAIWeldingPocketID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}
	else if (m_iROIType == INSPECTION_TYPE_FAI_BRACKET)
	{
		bIDUsed = THEAPP.m_pModelDataManager->CheckFAIBracketID(THEAPP.m_pTabControlDlg->m_iCurrentTab, m_iEditId);
	}

	if (bIDUsed)
	{
		AfxMessageBox("변경할 ID는 이미 사용중입니다. 다른 ID를 지정하여 주십시요.", MB_SYSTEMMODAL);
		return;
	}


	CDialog::OnOK();
}


void CROIIDChangeDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}


BOOL CROIIDChangeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CROIIDChangeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}
