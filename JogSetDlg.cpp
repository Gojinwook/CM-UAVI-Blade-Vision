// JogSetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "JogSetDlg.h"
#include "afxdialogex.h"
#include "IniFileCS.h"
#include "FAISettingDlg.h"

// CJogSetDlg 대화 상자입니다.
CJogSetDlg* CJogSetDlg::m_pInstance = NULL;

CJogSetDlg* CJogSetDlg::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CJogSetDlg();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_JOG_SET_DLG, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}

void CJogSetDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CJogSetDlg::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CJogSetDlg, CDialog)

CJogSetDlg::CJogSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJogSetDlg::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+100, VIEW1_DLG3_TOP+40+50+VIEW1_DLG3_HEIGHT+270-100, VIEW1_DLG3_WIDTH-520, VIEW1_DLG3_HEIGHT+300);
	
	m_iJogButtonStatus = 0;

	m_pJogButtonStatic1.m_nType = 1;
	m_pJogButtonStatic2.m_nType = 2;
	m_pJogButtonStatic3.m_nType = 3;
	m_pJogButtonStatic4.m_nType = 4;
	
	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		m_bCheckMatchingImage[iImageIdx] = FALSE;
	}
}

CJogSetDlg::~CJogSetDlg()
{
}

void CJogSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_1, m_bCheckMatchingImage[0]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_2, m_bCheckMatchingImage[1]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_3, m_bCheckMatchingImage[2]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_4, m_bCheckMatchingImage[3]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_5, m_bCheckMatchingImage[4]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_6, m_bCheckMatchingImage[5]);
	DDX_Check(pDX, IDC_CHECK_MATCHING_IMAGE_7, m_bCheckMatchingImage[6]);
}


BEGIN_MESSAGE_MAP(CJogSetDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_BARCODE_SETTING, &CJogSetDlg::OnBnClickedButtonBarcodeSetting)
	ON_BN_CLICKED(IDC_BUTTON_SET_MATCHING_IMAGE_NUMBER, &CJogSetDlg::OnBnClickedButtonSetMatchingImageNumber)
	ON_BN_CLICKED(IDC_BUTTON_SET_FAI_OPTION, &CJogSetDlg::OnBnClickedButtonSetFaiOption)
END_MESSAGE_MAP()


// CJogSetDlg 메시지 처리기입니다.


void CJogSetDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}


BOOL CJogSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	if (THEAPP.m_iMachineInspType == MACHINE_BRACKET)
	{
		GetDlgItem(IDC_CHECK_MATCHING_IMAGE_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_MATCHING_IMAGE_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_MATCHING_IMAGE_7)->EnableWindow(TRUE);
	}
	else if (THEAPP.m_iMachineInspType == MACHINE_SIDEFILL ||
		THEAPP.m_iMachineInspType == MACHINE_FIDUCIAL )
	{
		GetDlgItem(IDC_CHECK_MATCHING_IMAGE_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_MATCHING_IMAGE_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_MATCHING_IMAGE_7)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_MATCHING_IMAGE_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_MATCHING_IMAGE_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_MATCHING_IMAGE_7)->EnableWindow(FALSE);
	}

	m_iStageNo_teaching = 1;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

UINT MotionPositionTest_Thread(LPVOID lp)
{
	CJogSetDlg* pJogSetDlg = (CJogSetDlg*)lp;

	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return 0;

	CString FolderName;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		FolderName =  THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		FolderName =  THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CString FileName;
#ifdef INLINE_MODE
	switch (pJogSetDlg->m_iStageNo_teaching) {
	case 1:	{FileName = FolderName+"\\HW\\MotionMovingPosition1.ini"; break;}
	case 2: {FileName = FolderName+"\\HW\\MotionMovingPosition2.ini"; break;}
	}
#endif
	CIniFileCS INI(FileName);

	CString strSection = "Start Position";
	double nStartPosX = INI.Get_Double(strSection,"X",0.0);
	double nStartPosY = INI.Get_Double(strSection,"Y",0.0);

	if(nStartPosX == 0) {return 0;}

	strSection = "Moving Position";
	CString strModuleNumber;

	int ArrayX = THEAPP.m_pModelDataManager->m_iTrayArrayX;
	if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_Y)
		ArrayX = THEAPP.m_pModelDataManager->m_iTrayArrayY;

	for(int i=0; i< ArrayX; i++)
	{
		strModuleNumber.Format("X-%d",i);
		double AMoveX = INI.Get_Double(strSection,strModuleNumber,0.0);
		strModuleNumber.Format("Y-%d",i);
		double AMoveY = INI.Get_Double(strSection,strModuleNumber,0.0);
#ifdef INLINE_MODE
		THEAPP.m_pHandlerService->Set_AMoveRequest(AMoveX, AMoveY, pJogSetDlg->m_iStageNo_teaching);
			
		THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
		while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
		{
			if(THEAPP.m_pHandlerService->m_nInspectPCStatus >= 4) {return 0;}
			Sleep(1);
		}
		THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
		THEAPP.m_pHandlerService->Set_PositionRequest();
		Sleep(200);

		THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, TRIGGER_EDGE);
		Sleep(200);
#endif
	}

	nStartPosX = INI.Get_Double(strSection, "X-0",0.0);	nStartPosY = INI.Get_Double(strSection, "Y-0",0.0);
	THEAPP.m_pHandlerService->Set_AMoveRequest (nStartPosX, nStartPosY, pJogSetDlg->m_iStageNo_teaching);
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	// 무브 컴플리트 기다렸다가 좌표 표시
	while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) {Sleep(1);}
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	THEAPP.m_pHandlerService->Set_PositionRequest();

	return 0;
}

#include "BarcodeDlg.h"

void CJogSetDlg::OnBnClickedButtonBarcodeSetting()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;

	CBarcodeDlg dlg;

	dlg.SetLightValue(THEAPP.m_pModelDataManager->m_iBarcodeLightBright);

	CString FolderName;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		FolderName =  THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName+"\\SW\\TeachImage\\";
	else
		FolderName =  THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName+"\\SW\\TeachImage\\";

	THEAPP.m_FileBase.CreatePath(FolderName,TRUE); //Ver2629

	CString FileName;
	FileName = FolderName+"TeachingBarcode_Cam1";

	Hobject HBarcodeTeachImage;
	HBarcodeTeachImage.Reset();

	try
	{
		read_image(&HBarcodeTeachImage, FileName);
		dlg.SetImage(&HBarcodeTeachImage);
	}
	catch(HException &except)
	{
		;
	}

	if (dlg.DoModal() == IDOK)
	{
		;
	}
}

void CJogSetDlg::LoadViewParam()
{
	for (int i=0; i<MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		m_bCheckMatchingImage[i] = THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i];
	}

	UpdateData(FALSE);
}

void CJogSetDlg::OnBnClickedButtonSetMatchingImageNumber()
{
	UpdateData(TRUE);

	if(THEAPP.m_pModelDataManager->m_sModelName == ".") 
		return;

	CString strModelFolder;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CIniFileCS INI(strModelFolder+"\\SW\\ModelBase.ini");
	CString strSection, sTemp;

	strSection = "Local Align";

	for (int i=0; i<MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i] = m_bCheckMatchingImage[i];
		sTemp.Format("m_bUseLocalAlignMatching_%d", i+1);
		INI.Set_Bool(strSection, sTemp, m_bCheckMatchingImage[i]);
	}
}



void CJogSetDlg::OnBnClickedButtonSetFaiOption()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFAISettingDlg dlg;
	dlg.DoModal();
}
