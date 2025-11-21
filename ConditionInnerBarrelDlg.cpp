// ConditionInnerBarrelDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "ConditionInnerBarrelDlg.h"
#include "afxdialogex.h"


// CConditionInnerBarrelDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CConditionInnerBarrelDlg, CDialog)

CConditionInnerBarrelDlg::CConditionInnerBarrelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConditionInnerBarrelDlg::IDD, pParent)
{

}

CConditionInnerBarrelDlg::~CConditionInnerBarrelDlg()
{
}

void CConditionInnerBarrelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConditionInnerBarrelDlg, CDialog)
END_MESSAGE_MAP()


// CConditionInnerBarrelDlg 메시지 처리기입니다.

BOOL CConditionInnerBarrelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Grid.AttachGrid(this, IDC_GRID_INSPECTION_CONDITION);

	m_Grid.QuickSetText(0, 0, "이물");
	m_Grid.QuickSetText(0, 1, "스크래치");
	m_Grid.QuickSetText(0, 2, "얼룩");
	m_Grid.QuickSetText(0, 3, "덴트");
	m_Grid.QuickSetText(0, 4, "칩핑");	
	m_Grid.QuickSetText(0, 5, "에폭시");	
	m_Grid.QuickSetText(0, 6, "외곽오염");	
	m_Grid.QuickSetText(0, 7, "Peel Off");	
	m_Grid.QuickSetText(0, 8, "WhiteDot");	

	//////////////////////// changed for CMI3000 2000 ====>//////////////////////////////// 
	m_Grid.QuickSetText(2, 0, "0.0100");
	m_Grid.QuickSetText(3, 0, "0.0200");
	m_Grid.QuickSetText(4, 0, "0.0500");
	m_Grid.QuickSetText(5, 0, "100");
	m_Grid.QuickSetText(6, 0, "2");
	m_Grid.QuickSetText(7, 0, "1");

	m_Grid.QuickSetText(2, 1, "0.0100");
	m_Grid.QuickSetText(3, 1, "0.0200");
	m_Grid.QuickSetText(4, 1, "0.0500");
	m_Grid.QuickSetText(5, 1, "100");
	m_Grid.QuickSetText(6, 1, "2");
	m_Grid.QuickSetText(7, 1, "1");

	m_Grid.QuickSetText(2, 2, "0.0080");
	m_Grid.QuickSetText(3, 2, "0.0100");
	m_Grid.QuickSetText(4, 2, "0.0150");
	m_Grid.QuickSetText(5, 2, "100");
	m_Grid.QuickSetText(6, 2, "2");
	m_Grid.QuickSetText(7, 2, "1");

	m_Grid.QuickSetText(2, 3, "0.0080");
	m_Grid.QuickSetText(3, 3, "0.0100");
	m_Grid.QuickSetText(4, 3, "0.0150");
	m_Grid.QuickSetText(5, 3, "100");
	m_Grid.QuickSetText(6, 3, "2");
	m_Grid.QuickSetText(7, 3, "1");

	m_Grid.QuickSetText(2, 4, "0.0100");
	m_Grid.QuickSetText(3, 4, "0.0200");
	m_Grid.QuickSetText(4, 4, "0.0500");
	m_Grid.QuickSetText(5, 4, "100");
	m_Grid.QuickSetText(6, 4, "2");
	m_Grid.QuickSetText(7, 4, "1");

	m_Grid.QuickSetText(2, 5, "0.0100");
	m_Grid.QuickSetText(3, 5, "0.0200");
	m_Grid.QuickSetText(4, 5, "0.0500");
	m_Grid.QuickSetText(5, 5, "100");
	m_Grid.QuickSetText(6, 5, "2");
	m_Grid.QuickSetText(7, 5, "1");

	m_Grid.QuickSetText(2, 6, "0.0100");
	m_Grid.QuickSetText(3, 6, "0.0200");
	m_Grid.QuickSetText(4, 6, "0.0500");
	m_Grid.QuickSetText(5, 6, "100");
	m_Grid.QuickSetText(6, 6, "2");
	m_Grid.QuickSetText(7, 6, "1");

	m_Grid.QuickSetText(2, 7, "0.0100");
	m_Grid.QuickSetText(3, 7, "0.0200");
	m_Grid.QuickSetText(4, 7, "0.0500");
	m_Grid.QuickSetText(5, 7, "100");
	m_Grid.QuickSetText(6, 7, "2");
	m_Grid.QuickSetText(7, 7, "1");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}