// AdminPasswordDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AdminPasswordDlg.h"
#include "afxdialogex.h"


// CAdminPasswordDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAdminPasswordDlg, CDialog)

CAdminPasswordDlg::CAdminPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdminPasswordDlg::IDD, pParent)
{

	m_sUserID = _T("Admin");
}

CAdminPasswordDlg::~CAdminPasswordDlg()
{
}

void CAdminPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ADMINOPERATOR, m_sUserID);
}


BEGIN_MESSAGE_MAP(CAdminPasswordDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAdminPasswordDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAdminPasswordDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAdminPasswordDlg 메시지 처리기입니다.


void CAdminPasswordDlg::OnBnClickedOk()
{
	CString strRegisterFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
	CIniFileCS INI(strRegisterFolder + "\\RegisterInfo.ini");

	//************************* Text 에서 받아온 값 **************************
	CString strTextInputPassWord = "";
	GetDlgItem(IDC_EDIT_ADMINPASSWORD)->GetWindowText(strTextInputPassWord);

	strTextInputPassWord.TrimLeft();
	strTextInputPassWord.TrimRight();
	//*************************************************************************

	//************************* Ini 파일에서 받아온 값 **************************
	CString strRegisterPassWord;
	strRegisterPassWord = _T("");

	strRegisterPassWord = INI.Get_String("PassWord", "Pwd", "1111");
	//*************************************************************************

	int nResPassWord = 0;
	nResPassWord = strTextInputPassWord.Compare(strRegisterPassWord);

	if(nResPassWord != 0)
	{
		AfxMessageBox("패스워드 불일치!!", MB_SYSTEMMODAL);
		return;
	}

	INI.Set_String("Login Mode", "Mode", "Admin");
	INI.Set_String("Operator", "Operator", "Admin");

	CDialog::OnOK();
}


BOOL CAdminPasswordDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
//	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
//		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


void CAdminPasswordDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}
