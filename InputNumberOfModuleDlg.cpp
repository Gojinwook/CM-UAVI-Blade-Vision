// InputNumberOfModuleDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InputNumberOfModuleDlg.h"
#include "afxdialogex.h"


// CInputNumberOfModuleDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInputNumberOfModuleDlg, CDialog)

CInputNumberOfModuleDlg::CInputNumberOfModuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputNumberOfModuleDlg::IDD, pParent)
{

}

CInputNumberOfModuleDlg::~CInputNumberOfModuleDlg()
{
}

void CInputNumberOfModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputNumberOfModuleDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CInputNumberOfModuleDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CInputNumberOfModuleDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CInputNumberOfModuleDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CInputNumberOfModuleDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CInputNumberOfModuleDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CInputNumberOfModuleDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CInputNumberOfModuleDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CInputNumberOfModuleDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CInputNumberOfModuleDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CInputNumberOfModuleDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON_ACCEPT, &CInputNumberOfModuleDlg::OnBnClickedButtonAccept)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_, &CInputNumberOfModuleDlg::OnBnClickedButtonCancel)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CInputNumberOfModuleDlg 메시지 처리기입니다.


void CInputNumberOfModuleDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	m_strNumber+="1";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);
	m_strNumber+="2";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButton3()
{
	UpdateData(TRUE);
	m_strNumber+="3";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButton4()
{
	UpdateData(TRUE);
	m_strNumber+="4";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButton5()
{
	UpdateData(TRUE);
	m_strNumber+="5";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButton6()
{
	UpdateData(TRUE);
	m_strNumber+="6";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButton7()
{
	UpdateData(TRUE);
	m_strNumber+="7";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButton8()
{
	UpdateData(TRUE);
	m_strNumber+="8";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButton9()
{
	UpdateData(TRUE);
	m_strNumber+="9";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButton10()
{
	UpdateData(TRUE);
	m_strNumber+="0";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}


void CInputNumberOfModuleDlg::OnBnClickedButtonAccept()
{
	UpdateData(TRUE);
	CDialog::OnOK();
}


void CInputNumberOfModuleDlg::OnBnClickedButtonCancel()
{
	UpdateData(TRUE);
	m_strNumber="";
	SetDlgItemText(IDC_STATIC,m_strNumber);
	UpdateColorString();
	UpdateData(FALSE);
	Invalidate(false);
}
CFont* CInputNumberOfModuleDlg::GetFont(LONG lfWidth, LONG lfHeight, LONG lfWeight)
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

HBRUSH CInputNumberOfModuleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	CRect rect; 
	UINT nID = pWnd->GetDlgCtrlID();

	if(nCtlColor == CTLCOLOR_STATIC)

	{     
		switch(nID)
		{
		case IDC_STATIC:
			{
				pDC->SetTextColor(RGB(255, 0, 255));
				pDC->SetBkColor(RGB(240, 240, 240));
				break;
			}
		}

	}
	return hbr;
}
void CInputNumberOfModuleDlg::UpdateColorString()
{
	CFont* pFont;
        LONG lfWidth = 30, lfHeight = 60, lfWeight = FW_SEMIBOLD;
        pFont = GetFont(lfWidth, lfHeight, lfWeight);
		GetDlgItem(IDC_STATIC)->SetFont(pFont);
        pFont->Detach();
        delete pFont;   
}