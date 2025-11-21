// DlgInspectFobTest.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "DlgInspectFobTest.h"
#include "afxdialogex.h"


// CDlgInspectFobTest 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInspectFobTest, CDialogEx)

CDlgInspectFobTest::CDlgInspectFobTest(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgInspectFobTest::IDD, pParent)
{
	m_iDxGPro = 100;
	m_iDxNPro = 0;
	m_iDxEPro = 0;

}

CDlgInspectFobTest::~CDlgInspectFobTest()
{
}

void CDlgInspectFobTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GPRO, m_iDxGPro);
	DDX_Text(pDX, IDC_EDIT_NPRO, m_iDxNPro);
	DDX_Text(pDX, IDC_EDIT_EPRO, m_iDxEPro);
}


BEGIN_MESSAGE_MAP(CDlgInspectFobTest, CDialogEx)
	ON_BN_CLICKED(IDC_BN_SET_GOOD, &CDlgInspectFobTest::OnBnClickedBnSetGood)
	ON_BN_CLICKED(IDC_BN_SET_NG, &CDlgInspectFobTest::OnBnClickedBnSetNg)
	ON_BN_CLICKED(IDC_BN_SET_EMPTY, &CDlgInspectFobTest::OnBnClickedBnSetEmpty)
	ON_BN_CLICKED(IDC_BN_SET_ALLGOOD, &CDlgInspectFobTest::OnBnClickedBnSetAllgood)
	ON_BN_CLICKED(IDC_BN_SET_ALLNG, &CDlgInspectFobTest::OnBnClickedBnSetAllng)
	ON_BN_CLICKED(IDC_BN_SET_ALLEMPTY, &CDlgInspectFobTest::OnBnClickedBnSetAllempty)
	ON_BN_CLICKED(IDOK, &CDlgInspectFobTest::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CDlgInspectFobTest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_iDxGPro = THEAPP.m_pInspectService->m_iFobTestG;
	m_iDxNPro = THEAPP.m_pInspectService->m_iFobTestN;
	m_iDxEPro = THEAPP.m_pInspectService->m_iFobTestE;
	UpdateData(FALSE);

	return TRUE;
}

// CDlgInspectFobTest 메시지 처리기입니다.


void CDlgInspectFobTest::OnBnClickedBnSetGood()
{
	UpdateData(TRUE);

	if (m_iDxGPro < 0) m_iDxGPro = 0;
	if (m_iDxNPro < 0) m_iDxNPro = 0;
	if (m_iDxEPro < 0) m_iDxEPro = 0;

	if (m_iDxGPro >= 100) {m_iDxGPro = 100; m_iDxNPro = 0; m_iDxEPro = 0;}
	else {m_iDxEPro = 0; m_iDxNPro = 100 - m_iDxGPro;}
	//if (m_iDxNPro > 100-m_iDxGPro) {m_iDxNPro = 100-m_iDxGPro; 100-m_iDxGPro-m_iDxNPro;}
	//if (m_iDxNPro < 100-m_iDxGPro-m_iDxEPro) m_iDxNPro = 100-m_iDxGPro-m_iDxEPro;
	//m_iDxEPro = 100-m_iDxGPro-m_iDxNPro;
	
	UpdateData(FALSE);
}


void CDlgInspectFobTest::OnBnClickedBnSetNg()
{
	UpdateData(TRUE);

	if (m_iDxGPro < 0) m_iDxGPro = 0;
	if (m_iDxNPro < 0) m_iDxNPro = 0;
	if (m_iDxEPro < 0) m_iDxEPro = 0;

	if (m_iDxNPro >= 100) {m_iDxNPro = 100; m_iDxGPro = 0; m_iDxEPro = 0;}
	else {m_iDxEPro = 0; m_iDxGPro = 100 - m_iDxNPro;}

	//if (m_iDxEPro > 100-m_iDxNPro) {m_iDxEPro = 100-m_iDxNPro; m_iDxGPro = 100-m_iDxNPro-m_iDxEPro;}
	//if (m_iDxEPro < 100-m_iDxGPro-m_iDxNPro) m_iDxEPro = 100-m_iDxGPro-m_iDxNPro;
	//m_iDxGPro = 100-m_iDxNPro-m_iDxEPro;

	UpdateData(FALSE);
}


void CDlgInspectFobTest::OnBnClickedBnSetEmpty()
{
	UpdateData(TRUE);

	if (m_iDxGPro < 0) m_iDxGPro = 0;
	if (m_iDxNPro < 0) m_iDxNPro = 0;
	if (m_iDxEPro < 0) m_iDxEPro = 0;

	if (m_iDxGPro > 100) m_iDxGPro = 100;
	if (m_iDxNPro > 100) m_iDxNPro = 100;
	if (m_iDxEPro >= 100) {m_iDxEPro = 100; m_iDxNPro = 0; m_iDxGPro = 0;}
	else {m_iDxEPro = 0; m_iDxGPro = 100-m_iDxNPro;}
	//if (m_iDxGPro > 100-m_iDxEPro) {m_iDxGPro = 100-m_iDxEPro; m_iDxNPro = 100-m_iDxEPro-m_iDxGPro;}
	//if (m_iDxGPro < 100-m_iDxNPro-m_iDxEPro) m_iDxGPro = 100-m_iDxNPro-m_iDxEPro;
	m_iDxNPro = 100-m_iDxGPro-m_iDxEPro;
	
	UpdateData(FALSE);
}


void CDlgInspectFobTest::OnBnClickedBnSetAllgood()
{
	m_iDxGPro = 100;
	m_iDxNPro = 0;
	m_iDxEPro = 0;
	
	UpdateData(FALSE);
}


void CDlgInspectFobTest::OnBnClickedBnSetAllng()
{
	m_iDxGPro = 0;
	m_iDxNPro = 100;
	m_iDxEPro = 0;
	
	UpdateData(FALSE);
}


void CDlgInspectFobTest::OnBnClickedBnSetAllempty()
{
	m_iDxGPro = 0;
	m_iDxNPro = 0;
	m_iDxEPro = 100;
	
	UpdateData(FALSE);
}


void CDlgInspectFobTest::OnBnClickedOk()
{
	OnBnClickedBnSetEmpty();
	THEAPP.m_pInspectService->m_iFobTestG = m_iDxGPro;
	THEAPP.m_pInspectService->m_iFobTestN = m_iDxNPro;
	THEAPP.m_pInspectService->m_iFobTestE = m_iDxEPro;
	CDialogEx::OnOK();
}

