// Operator_Register.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "Operator_RegisterDlg.h"
#include "afxdialogex.h"


// COperator_Register 대화 상자입니다.

IMPLEMENT_DYNAMIC(COperator_RegisterDlg, CDialog)

COperator_RegisterDlg::COperator_RegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COperator_RegisterDlg::IDD, pParent)
{
	m_strLogin_Mode = "";
	m_strOperator = "";
	m_strPwd = "";
}

COperator_RegisterDlg::~COperator_RegisterDlg()
{
}

void COperator_RegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COperator_RegisterDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COperator_RegisterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COperator_RegisterDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// COperator_Register 메시지 처리기입니다.


void COperator_RegisterDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nResOper = 0;
	int nResAdmin = 0;

	CString strPassWord;

	CString strRegisterFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
	CIniFileCS INI(strRegisterFolder+"\\RegisterInfo.ini");

	strPassWord = INI.Get_String("PassWord", "Pwd", "1111");

	GetDlgItem(IDC_EDIT_LOGINMODE)->GetWindowText(m_strLogin_Mode);
	m_strLogin_Mode.TrimLeft();
	m_strLogin_Mode.TrimRight();

	if (strPassWord != m_strLogin_Mode)
	{
		AfxMessageBox("현재 패스워드 불일치!!", MB_SYSTEMMODAL);
		return;
	}

	GetDlgItem(IDC_EDIT_OPERATOR)->GetWindowText(m_strOperator);
	m_strOperator.TrimLeft();
	m_strOperator.TrimRight();

	GetDlgItem(IDC_EDIT_PWD)->GetWindowText(m_strPwd);
	m_strPwd.TrimLeft();
	m_strPwd.TrimRight();

	if (m_strOperator!=m_strPwd)
	{
		AfxMessageBox("변경 패스워드 재확인 입력 불일치!!", MB_SYSTEMMODAL);
		return;
	}

	INI.Set_String("PassWord", "Pwd", m_strOperator);

	CDialog::OnOK();
}


void COperator_RegisterDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
