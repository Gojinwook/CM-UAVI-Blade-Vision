// LightAverageValueDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "LightAverageValueDlg.h"
#include "afxdialogex.h"


// CLightAverageValueDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightAverageValueDlg, CDialog)

CLightAverageValueDlg::CLightAverageValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightAverageValueDlg::IDD, pParent)
{

	m_iEditLight1 = 0;
	m_iEditLight2 = 0;
	m_iEditLight3 = 0;
	m_iEditLight4 = 0;
	m_iEditLight5 = 0;
	m_iEditLightTotal = 0;
}

CLightAverageValueDlg::~CLightAverageValueDlg()
{
}

void CLightAverageValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE1, m_iEditLight1);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE2, m_iEditLight2);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE3, m_iEditLight3);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE4, m_iEditLight4);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE5, m_iEditLight5);
	DDX_Text(pDX, IDC_EDIT_AVERAGE_VALUE_TOTAL, m_iEditLightTotal);

	DDX_Control(pDX, IDOK, m_bnOK);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);
}


BEGIN_MESSAGE_MAP(CLightAverageValueDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLightAverageValueDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLightAverageValueDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLightAverageValueDlg 메시지 처리기입니다.


void CLightAverageValueDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_1] = m_iEditLight1;
	THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_2] = m_iEditLight2;
	THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_3] = m_iEditLight3;
	THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_4] = m_iEditLight4;
	THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_5] = m_iEditLight5;
	THEAPP.m_pModelDataManager->m_iLightAverageValueTotal[THEAPP.m_pTabControlDlg->m_iCurrentTab-1] = m_iEditLightTotal;

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	THEAPP.m_pModelDataManager->SaveLightAverageValue(strModelFolder+"\\HW\\LightAverageValue.ini");

	CDialog::OnOK();
}


void CLightAverageValueDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}


BOOL CLightAverageValueDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bnOK.Init_Ctrl(_T("Arial"), 10, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnCancel.Init_Ctrl(_T("Arial"), 10, FALSE, BLACK, ROYAL_BLUE, 0, 0);

	m_iEditLight1 = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_1];
	m_iEditLight2 = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_2];
	m_iEditLight3 = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_3];
	m_iEditLight4 = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_4];
	m_iEditLight5 = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_5];
	m_iEditLightTotal = THEAPP.m_pModelDataManager->m_iLightAverageValueTotal[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
