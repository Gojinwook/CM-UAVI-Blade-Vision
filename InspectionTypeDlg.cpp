// InspectionTypeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectionTypeDlg.h"
#include "afxdialogex.h"


// CInspectionTypeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInspectionTypeDlg, CDialog)

CInspectionTypeDlg::CInspectionTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectionTypeDlg::IDD, pParent)
{
	m_iRadioInspectionType = 1;
}

CInspectionTypeDlg::~CInspectionTypeDlg()
{
}

void CInspectionTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_INSPECTION_TYPE_0, m_iRadioInspectionType);

}


BEGIN_MESSAGE_MAP(CInspectionTypeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CInspectionTypeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CInspectionTypeDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CInspectionTypeDlg 메시지 처리기입니다.

BOOL CInspectionTypeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


void CInspectionTypeDlg::OnBnClickedOk()
{
	UpdateData();

	CDialog::OnOK();
}


void CInspectionTypeDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}


BOOL CInspectionTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
