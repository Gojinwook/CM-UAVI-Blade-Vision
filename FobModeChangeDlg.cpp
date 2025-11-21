// FobModeChange.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "FobModeChangeDlg.h"
#include "afxdialogex.h"


// CFobModeChangeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFobModeChangeDlg, CDialogEx)

CFobModeChangeDlg::CFobModeChangeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFobModeChangeDlg::IDD, pParent)
{
	m_nRadioFobMode  = FOB_MODE_OPERATOR;
}

CFobModeChangeDlg::~CFobModeChangeDlg()
{
	
}

void CFobModeChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_FOB_MODE_OPERATOR, m_nRadioFobMode);
}


BEGIN_MESSAGE_MAP(CFobModeChangeDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_FOB_MODE_OPERATOR, &CFobModeChangeDlg::OnBnClickedRadioFobModeOperator)
	ON_BN_CLICKED(IDC_RADIO_FOB_MODE_ADMIN, &CFobModeChangeDlg::OnBnClickedRadioFobModeAdmin)
	ON_BN_CLICKED(IDOK, &CFobModeChangeDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CFobModeChangeDlg::OnBnClickedRadioFobModeOperator()
{
	m_nRadioFobMode  = FOB_MODE_OPERATOR;
	GetDlgItem(IDC_FOBMODE_STATIC)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_FOBMODE)->EnableWindow(TRUE);
	GetDlgItem(IDC_FOBMODE_STATIC_ADMIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FOBMODE_ADMIN)->EnableWindow(FALSE);

}


void CFobModeChangeDlg::OnBnClickedRadioFobModeAdmin()
{
	m_nRadioFobMode  = FOB_MODE_ADMIN;
	GetDlgItem(IDC_FOBMODE_STATIC)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FOBMODE)->EnableWindow(FALSE);
	GetDlgItem(IDC_FOBMODE_STATIC_ADMIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_FOBMODE_ADMIN)->EnableWindow(TRUE);
}


void CFobModeChangeDlg::OnBnClickedOk()
{
	
	if( m_nRadioFobMode == FOB_MODE_ADMIN )
	{
		CString strRegisterFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
		CIniFileCS INI(strRegisterFolder + "\\RegisterInfo.ini");

		//************************* Text 에서 받아온 값 **************************
		CString strTextInputPassWord = "";
		GetDlgItem(IDC_EDIT_FOBMODE_ADMIN)->GetWindowText(strTextInputPassWord);

		strTextInputPassWord.TrimLeft();
		strTextInputPassWord.TrimRight();
		//*************************************************************************

		//************************* Ini 파일에서 받아온 값 **************************
		CString strRegisterPassWord;
		strRegisterPassWord = _T("");

		strRegisterPassWord = INI.Get_String("FOB ADMIN Password", "Pwd", "1111");
		//*************************************************************************

		int nResPassWord = 0;
		nResPassWord = strTextInputPassWord.Compare(strRegisterPassWord);

		if(nResPassWord != 0)
		{
			AfxMessageBox("패스워드 불일치!!", MB_SYSTEMMODAL);
			return;
		}

		if(IDNO == AfxMessageBox("FOB 사용하시겠습니까?", MB_YESNO))
		{
			return;
		}
	}
	else
	{
		CString str;
		GetDlgItem(IDC_EDIT_FOBMODE)->GetWindowTextA(str);
		if( str == "" )
		{
			AfxMessageBox("Operator ID를 입력하세요.");
			return;
		}

		if(IDNO == AfxMessageBox("FOB 사용하시겠습니까?", MB_YESNO))
		{
			return;
		}

		THEAPP.m_strFOBModeOperatorID = str;
	}

	THEAPP.m_nFOBModeUser = m_nRadioFobMode;

	CString strLog;
	if( m_nRadioFobMode == FOB_MODE_ADMIN )
	{
		strLog.Format("[FOB Mode(Admin)]Use FOB Mode");
	}
	else
	{
		strLog.Format("[FOB Mode(Operator)]Use FOB Mode (Operator ID) : %s",THEAPP.m_strFOBModeOperatorID);
	}
	THEAPP.SaveLog(strLog);

	CDialogEx::OnOK();
}