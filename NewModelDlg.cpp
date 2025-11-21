// NewModelDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "NewModelDlg.h"
#include "IniFileCS.h"
#include "afxdialogex.h"


// CNewModelDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNewModelDlg, CDialog)

CNewModelDlg::CNewModelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewModelDlg::IDD, pParent)
{

	m_Edit_strModelName = _T("");
	m_Edit_dModulePitchX = 28.0;
	m_Edit_dModulePitchY = 28.0;
	m_Edit_iTrayArrayX = 6;
	m_Edit_iTrayArrayY = 6;
	m_iRadioBarrelType = BT_TOP_BOTTOM;
}

CNewModelDlg::~CNewModelDlg()
{
}

void CNewModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MODEL_NAME, m_Edit_strModelName);
	DDX_Text(pDX, IDC_EDIT_MODULE_PITCH_X, m_Edit_dModulePitchX);
	DDX_Text(pDX, IDC_EDIT_MODULE_PITCH_Y, m_Edit_dModulePitchY);
	DDX_Text(pDX, IDC_EDIT_TRAY_ARRAY_X, m_Edit_iTrayArrayX);
	DDX_Text(pDX, IDC_EDIT_TRAY_ARRAY_Y, m_Edit_iTrayArrayY);
	DDX_Control(pDX, IDC_EDIT_MODEL_NAME, m_edtModelName);
	DDX_Check(pDX, IDC_CHK_K51, m_Edit_bBlackCoating);
	DDX_Radio(pDX, IDC_RADIO_BARREL_TYPE_TOP, m_iRadioBarrelType);
}


BEGIN_MESSAGE_MAP(CNewModelDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CNewModelDlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CNewModelDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CNewModelDlg 메시지 처리기입니다.


void CNewModelDlg::OnBnClickedButtonOk()
{
	UpdateData(TRUE);


	if(CheckInfomation())
	{
		CString strModelFolder;
		if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
			strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+m_Edit_strModelName;
		else
			strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+m_Edit_strModelName;
		
		CString strTeachImageFolder = strModelFolder+"\\SW\\TeachImage\\";
		CString strMatchingModelFolder = strModelFolder+"\\SW\\MatchingModel\\";

		THEAPP.m_FileBase.CreateFolder(strModelFolder);			// 모델 폴더 만들기
		THEAPP.m_FileBase.CreateFolder(strTeachImageFolder);	// 티칭 이미지 저장폴더 만들기
		THEAPP.m_FileBase.CreateFolder(strMatchingModelFolder);	// 매칭 모델 저장폴더 만들기

		/////////////////////////////////////////////////////////////Modelbase ini file
		CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
		CString strSection = "Model Base";

		INI.Set_String(strSection, "MODEL NAME", m_Edit_strModelName);
		INI.Set_Integer(strSection, "VERSION", THEAPP.GetVersion());
		INI.Set_Integer(strSection, "TrayArrayX", m_Edit_iTrayArrayX);
		INI.Set_Integer(strSection, "TrayArrayY", m_Edit_iTrayArrayY);
		INI.Set_Integer(strSection, "TrayLotModuleAmt", m_Edit_iTrayArrayX*m_Edit_iTrayArrayY);
		INI.Set_Double(strSection, "ModulePitchX", m_Edit_dModulePitchX);
		INI.Set_Double(strSection, "ModulePitchY", m_Edit_dModulePitchY);
		INI.Set_Integer(strSection, "TrayHESizeX", 600);
		INI.Set_Integer(strSection, "TrayHESizeY", 712);

		BOOL bOneBarrel;
		if (m_iRadioBarrelType==BT_TOP)
		{
			bOneBarrel = TRUE;
		}
		else
		{
			bOneBarrel = FALSE;
		}

		INI.Set_Bool(strSection,"OneBarrel", bOneBarrel);
		INI.Set_Bool(strSection,"Black Coating", m_Edit_bBlackCoating);


		THEAPP.m_pModelDataManager->m_sModelName = m_Edit_strModelName;
		THEAPP.m_pModelDataManager->m_iTrayArrayX = m_Edit_iTrayArrayX;
		THEAPP.m_pModelDataManager->m_iTrayArrayY = m_Edit_iTrayArrayY;
		THEAPP.m_pModelDataManager->m_iTrayModuleMax = m_Edit_iTrayArrayY * m_Edit_iTrayArrayY;
		THEAPP.m_pModelDataManager->m_dModulePitchX = m_Edit_dModulePitchX;
		THEAPP.m_pModelDataManager->m_dModulePitchY = m_Edit_dModulePitchY;
		THEAPP.m_pModelDataManager->m_bOneBarrel = bOneBarrel;
		THEAPP.m_pModelDataManager->m_bBlackCoating = m_Edit_bBlackCoating;

		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_MODEL_NAME, m_Edit_strModelName);

		CString sLinfoPath = strModelFolder+"\\HW\\InspectLightInfo.ini";
		THEAPP.m_pModelDataManager->LoadLightInfo (sLinfoPath);
		THEAPP.m_pModelDataManager->InitLightInfo();

		sLinfoPath = strModelFolder+"\\HW\\LightAverageValue.ini";
		THEAPP.m_pModelDataManager->LoadLightAverageValue(sLinfoPath);
		THEAPP.m_pModelDataManager->InitLightAverageValue();
	}
	else
	{
		return;
	}

	CDialog::OnOK();
}


void CNewModelDlg::OnBnClickedButtonCancel()
{


	CDialog::OnCancel();
}

BOOL CNewModelDlg::CheckInfomation()
{
	if(m_Edit_strModelName=="")
	{
		MessageBox("Model Name 입력 해주세요");
		return FALSE;
	}
	
	if(m_Edit_iTrayArrayX==0)
	{
		MessageBox("TrayArrayX 입력 해주세요");
		return FALSE;
	}
	if(m_Edit_iTrayArrayY==0)
	{
		MessageBox("TrayArrayY 입력 해주세요");
		return FALSE;
	}
	if(m_Edit_dModulePitchX==0)
	{
		MessageBox("ModulePitchX 입력 해주세요");
		return FALSE;
	}
	if(m_Edit_dModulePitchY==0)
	{
		MessageBox("ModulePitchY 입력 해주세요");
		return FALSE;
	}
	return TRUE;
}

BOOL CNewModelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_Edit_strModelName = _T("");
	m_Edit_dModulePitchX = 28.0;
	m_Edit_dModulePitchY = 28.0;
	m_Edit_iTrayArrayX = 6;
	m_Edit_iTrayArrayY = 6;
	m_iRadioBarrelType = BT_TOP_BOTTOM;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CNewModelDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
