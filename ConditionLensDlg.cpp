// ConditionLensDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "ConditionLensDlg.h"
#include "afxdialogex.h"


// CConditionLensDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CConditionLensDlg, CDialog)

CConditionLensDlg::CConditionLensDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConditionLensDlg::IDD, pParent)
{

}

CConditionLensDlg::~CConditionLensDlg()
{
}

void CConditionLensDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConditionLensDlg, CDialog)
END_MESSAGE_MAP()


// CConditionLensDlg 메시지 처리기입니다.

BOOL CConditionLensDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Grid.AttachGrid(this, IDC_GRID_INSPECTION_CONDITION);

	m_Grid.QuickSetText(0, 0, "Lens 오염");	// 24.05.09 - v2646 - 불량명 변경 - LeeGW
	m_Grid.QuickSetText(0, 1, "Lens 스크래치");
	m_Grid.QuickSetText(0, 2, "Lens 이물");	// 24.05.09 - v2646 - 불량명 변경 - LeeGW
	m_Grid.QuickSetText(0, 3, "Lens WhiteDot");	// 24.05.09 - v2646 - 불량명 변경 - LeeGW

	m_Grid.QuickSetText(2, 0, "0.0080");
	m_Grid.QuickSetText(3, 0, "0.0100");
	m_Grid.QuickSetText(4, 0, "0.0150");
	m_Grid.QuickSetText(5, 0, "100");
	m_Grid.QuickSetText(6, 0, "2");
	m_Grid.QuickSetText(7, 0, "1");

	m_Grid.QuickSetText(2, 1, "0.0080");
	m_Grid.QuickSetText(3, 1, "0.0100");
	m_Grid.QuickSetText(4, 1, "0.0150");
	m_Grid.QuickSetText(5, 1, "100");
	m_Grid.QuickSetText(6, 1, "2");
	m_Grid.QuickSetText(7, 1, "1");

	m_Grid.QuickSetText(2, 2, "0.0010");
	m_Grid.QuickSetText(3, 2, "0.0020");
	m_Grid.QuickSetText(4, 2, "0.0025");
	m_Grid.QuickSetText(5, 2, "100");
	m_Grid.QuickSetText(6, 2, "100");
	m_Grid.QuickSetText(7, 2, "1");

	m_Grid.QuickSetText(2, 3, "0.0010");
	m_Grid.QuickSetText(3, 3, "0.0020");
	m_Grid.QuickSetText(4, 3, "0.0025");
	m_Grid.QuickSetText(5, 3, "100");
	m_Grid.QuickSetText(6, 3, "100");
	m_Grid.QuickSetText(7, 3, "1");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}