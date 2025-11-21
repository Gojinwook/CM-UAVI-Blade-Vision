// ResultViewSelectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "ResultViewSelectDlg.h"
#include "afxdialogex.h"


// CResultViewSelectDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CResultViewSelectDlg, CDialog)

CResultViewSelectDlg::CResultViewSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResultViewSelectDlg::IDD, pParent)
{
	m_iSelectedResultView = 0;

}

CResultViewSelectDlg::~CResultViewSelectDlg()
{
}

void CResultViewSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResultViewSelectDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_BSURFACE, &CResultViewSelectDlg::OnBnClickedButtonResultBsurface)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_BEDGE, &CResultViewSelectDlg::OnBnClickedButtonResultBedge)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_LENS, &CResultViewSelectDlg::OnBnClickedButtonResultLens)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_LENS2, &CResultViewSelectDlg::OnBnClickedButtonResultLens2)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_EXTRA1, &CResultViewSelectDlg::OnBnClickedButtonResultExtra1)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_EXTRA2, &CResultViewSelectDlg::OnBnClickedButtonResultExtra2)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_EXTRA3, &CResultViewSelectDlg::OnBnClickedButtonResultExtra3)
END_MESSAGE_MAP()


// CResultViewSelectDlg 메시지 처리기입니다.

BOOL CResultViewSelectDlg::PreTranslateMessage(MSG* pMsg) 
{
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

BOOL CResultViewSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CString strImageType;
	CRect rectExtra, rectSelectView;

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (THEAPP.m_iMachineInspType == MACHINE_BRACKET)
	{
		strImageType = g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_EXTRA1];
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA)->SetWindowTextA(strImageType);
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA)->ShowWindow(SW_SHOW);

		strImageType = g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_EXTRA2];
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA2)->SetWindowTextA(strImageType);
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA2)->ShowWindow(SW_SHOW);

		strImageType = g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_EXTRA3];
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA3)->SetWindowTextA(strImageType);
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA3)->ShowWindow(SW_SHOW);
	}
	else if (THEAPP.m_iMachineInspType == MACHINE_SIDEFILL ||
		THEAPP.m_iMachineInspType == MACHINE_FIDUCIAL )
	{
		strImageType = g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_EXTRA1];
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA)->SetWindowTextA(strImageType);
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA)->ShowWindow(SW_SHOW);

		strImageType = g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_EXTRA2];
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA2)->SetWindowTextA(strImageType);
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA2)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_BUTTON_RESULT_EXTRA3)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BUTTON_RESULT_EXTRA3)->GetWindowRect(&rectExtra);
		ScreenToClient(&rectExtra);

		GetWindowRect(&rectSelectView);
		ScreenToClient(&rectSelectView);
		MoveWindow(0, 0, rectSelectView.Width() - rectExtra.Width() - 10, rectSelectView.Height());
	}
	else if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
	{
		strImageType = g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_SURFACE];
		GetDlgItem(IDC_BUTTON_RESULT_BSURFACE)->SetWindowTextA(strImageType);
		strImageType = g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_EDGE];
		GetDlgItem(IDC_BUTTON_RESULT_BEDGE)->SetWindowTextA(strImageType);
		strImageType = g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_LENS1];
		GetDlgItem(IDC_BUTTON_RESULT_LENS)->SetWindowTextA(strImageType);
		strImageType = g_sImageName[THEAPP.m_iMachineInspType][TRIGGER_LENS2];
		GetDlgItem(IDC_BUTTON_RESULT_LENS2)->SetWindowTextA(strImageType);

		GetDlgItem(IDC_BUTTON_RESULT_EXTRA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA3)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BUTTON_RESULT_EXTRA)->GetWindowRect(&rectExtra);
		ScreenToClient(&rectExtra);

		GetWindowRect(&rectSelectView);
		ScreenToClient(&rectSelectView);
		MoveWindow(0, 0, rectSelectView.Width() - 2 * rectExtra.Width() - 20, rectSelectView.Height());
	}
	else
	{
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RESULT_EXTRA3)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BUTTON_RESULT_EXTRA)->GetWindowRect(&rectExtra);
		ScreenToClient(&rectExtra);

		GetWindowRect(&rectSelectView);
		ScreenToClient(&rectSelectView);
		MoveWindow(0, 0, rectSelectView.Width() - 2 * rectExtra.Width() - 20 , rectSelectView.Height());
	}

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CResultViewSelectDlg::OnBnClickedButtonResultBsurface()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	if(THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge == FALSE)
		m_iSelectedResultView = 0;
	else
		m_iSelectedResultView =	1;


	CDialog::OnOK();
}


void CResultViewSelectDlg::OnBnClickedButtonResultBedge()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iSelectedResultView =	1;

	CDialog::OnOK();
}


void CResultViewSelectDlg::OnBnClickedButtonResultLens()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iSelectedResultView =	2;

	CDialog::OnOK();
}


void CResultViewSelectDlg::OnBnClickedButtonResultLens2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge == FALSE)
		m_iSelectedResultView = 3;
	else
		m_iSelectedResultView = 2;
	CDialog::OnOK();
}


void CResultViewSelectDlg::OnBnClickedButtonResultExtra1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iSelectedResultView =	4;

	CDialog::OnOK();
}


void CResultViewSelectDlg::OnBnClickedButtonResultExtra2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iSelectedResultView = 5;

	CDialog::OnOK();
}


void CResultViewSelectDlg::OnBnClickedButtonResultExtra3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iSelectedResultView = 6;

	CDialog::OnOK();
}
