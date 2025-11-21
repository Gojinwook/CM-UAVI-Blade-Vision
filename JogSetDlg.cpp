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
	, m_nDxHandlerPosX(0)
	, m_nDxHandlerPosY(0)
{
	SetPosition(VIEW1_DLG3_LEFT+100, VIEW1_DLG3_TOP+40+50+VIEW1_DLG3_HEIGHT+270-100, VIEW1_DLG3_WIDTH-520, VIEW1_DLG3_HEIGHT+300);
	
	m_iJogButtonStatus = 0;

	m_pJogButtonStatic1.m_nType = 1;
	m_pJogButtonStatic2.m_nType = 2;
	m_pJogButtonStatic3.m_nType = 3;
	m_pJogButtonStatic4.m_nType = 4;
	m_nDxHandlerPosX = 176.5;
	m_nDxHandlerPosY = 0.0;
	m_Edit_SetModulePitchX = 25.0;
	m_Edit_SetModulePitchY = 28.0;
	m_dEditSetStartPositionX = 176.5;
	m_dEditSetStartPositionY = 0.0;

	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		m_dEditSetInspPositionZ[iImageIdx] = 15.0;
	}
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

	DDX_Text(pDX, IDC_EDIT_CURRENT_POSITION_X, m_nDxHandlerPosX);
	DDX_Text(pDX, IDC_EDIT_CURRENT_POSITION_Y, m_nDxHandlerPosY);
	DDX_Text(pDX, IDC_EDIT_SET_MODULE_PITCH_X, m_Edit_SetModulePitchX);
	DDX_Text(pDX, IDC_EDIT_SET_MODULE_PITCH_Y, m_Edit_SetModulePitchY);
	DDX_Text(pDX, IDC_EDIT_SET_START_POSITION_X, m_dEditSetStartPositionX);
	DDX_Text(pDX, IDC_EDIT_SET_START_POSITION_Y, m_dEditSetStartPositionY);

	DDX_Text(pDX, IDC_EDIT_SET_INSP_POSITION_Z1, m_dEditSetInspPositionZ[0]);
	DDX_Text(pDX, IDC_EDIT_SET_INSP_POSITION_Z2, m_dEditSetInspPositionZ[1]);
	DDX_Text(pDX, IDC_EDIT_SET_INSP_POSITION_Z3, m_dEditSetInspPositionZ[2]);
	DDX_Text(pDX, IDC_EDIT_SET_INSP_POSITION_Z4, m_dEditSetInspPositionZ[3]);
	DDX_Text(pDX, IDC_EDIT_SET_INSP_POSITION_Z5, m_dEditSetInspPositionZ[4]);
	DDX_Text(pDX, IDC_EDIT_SET_INSP_POSITION_Z6, m_dEditSetInspPositionZ[5]);
	DDX_Text(pDX, IDC_EDIT_SET_INSP_POSITION_Z7, m_dEditSetInspPositionZ[6]);

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
	ON_BN_CLICKED(IDC_BUTTON_SET_START_POSITION, &CJogSetDlg::OnBnClickedButtonSetStartPosition)
	ON_BN_CLICKED(IDC_BUTTON_SET_XY_PITCH, &CJogSetDlg::OnBnClickedButtonSetXyPitch)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_NEXT_MODULE, &CJogSetDlg::OnBnClickedButtonMoveNextModule)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_PREV_MODULE, &CJogSetDlg::OnBnClickedButtonMovePrevModule)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_UPPER_MODULE, &CJogSetDlg::OnBnClickedButtonMoveUpperModule)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_DOWNER__MODULE, &CJogSetDlg::OnBnClickedButtonMoveDowner)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_HANDLER_POSITION_MAP, &CJogSetDlg::OnBnClickedButtonMakeHandlerPositionMap)
	ON_BN_CLICKED(IDC_BUTTON_MOVETO_POSITION, &CJogSetDlg::OnBnClickedButtonMovetoPosition)
	ON_STN_CLICKED(IDC_STATIC_STAGE_NO, &CJogSetDlg::OnStnClickedStaticStageNo)
	ON_BN_CLICKED(IDC_BUTTON_BARCODE_SETTING, &CJogSetDlg::OnBnClickedButtonBarcodeSetting)
	ON_BN_CLICKED(IDC_BUTTON_SET_INSP_Z_POSITION, &CJogSetDlg::OnBnClickedButtonSetInspZPosition)
	ON_BN_CLICKED(IDC_BUTTON_SET_MATCHING_IMAGE_NUMBER, &CJogSetDlg::OnBnClickedButtonSetMatchingImageNumber)
	ON_BN_CLICKED(IDC_BUTTON_GET_INSP_Z_POSITION, &CJogSetDlg::OnBnClickedButtonGetInspZPosition)
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

void CJogSetDlg::OnBnClickedButtonMovetoPosition()
{
	UpdateData(TRUE);
	THEAPP.m_pHandlerService->Set_AMoveRequest(m_nDxHandlerPosX, m_nDxHandlerPosY, m_iStageNo_teaching);
}



void CJogSetDlg::OnBnClickedButtonSetStartPosition()
{
	CString strXposition,strYposition;
	
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_X, strXposition);
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_Y, strYposition);

	SetDlgItemText(IDC_EDIT_SET_START_POSITION_X, strXposition);
	SetDlgItemText(IDC_EDIT_SET_START_POSITION_Y, strYposition);
}


void CJogSetDlg::OnBnClickedButtonSetXyPitch()
{
	UpdateData(TRUE);
}


void CJogSetDlg::OnBnClickedButtonMoveNextModule()
{
	UpdateData(TRUE);

	
	CString strXposition,strYposition;
	double dXposition,dYposition;
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_X,strXposition);
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_Y,strYposition);

	dXposition = atof(strXposition);
	dYposition = atof(strYposition);


#ifdef INLINE_MODE
	THEAPP.m_pHandlerService->Set_AMoveRequest(dXposition+m_Edit_SetModulePitchX, dYposition, m_iStageNo_teaching);
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
	{
		Sleep(1);
	}
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	THEAPP.m_pHandlerService->Set_PositionRequest();
#endif
}


void CJogSetDlg::OnBnClickedButtonMovePrevModule()
{
	UpdateData(TRUE);
	
	CString strXposition,strYposition;
	double dXposition,dYposition;
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_X,strXposition);
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_Y,strYposition);

	dXposition = atof(strXposition);
	dYposition = atof(strYposition);
#ifdef INLINE_MODE
	THEAPP.m_pHandlerService->Set_AMoveRequest(dXposition-m_Edit_SetModulePitchX, dYposition, m_iStageNo_teaching);
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
	{
		Sleep(1);
	}
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	THEAPP.m_pHandlerService->Set_PositionRequest();
#endif
}


void CJogSetDlg::OnBnClickedButtonMoveUpperModule()
{
	UpdateData(TRUE);

	
	CString strXposition,strYposition;
	double dXposition,dYposition;
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_X, strXposition);
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_Y, strYposition);

	dXposition = atof(strXposition);
	dYposition = atof(strYposition);
#ifdef INLINE_MODE
	THEAPP.m_pHandlerService->Set_AMoveRequest(dXposition, dYposition+m_Edit_SetModulePitchY, m_iStageNo_teaching);
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
	{
		Sleep(1);
	}
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	THEAPP.m_pHandlerService->Set_PositionRequest();
#endif
}


void CJogSetDlg::OnBnClickedButtonMoveDowner()
{
	UpdateData(TRUE);

	
	CString strXposition,strYposition;
	double dXposition,dYposition;
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_X,strXposition);
	GetDlgItemText(IDC_EDIT_CURRENT_POSITION_Y,strYposition);

	dXposition = atof(strXposition);
	dYposition = atof(strYposition);
#ifdef INLINE_MODE
	THEAPP.m_pHandlerService->Set_AMoveRequest(dXposition, dYposition-m_Edit_SetModulePitchY, m_iStageNo_teaching);
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
	{
		Sleep(1);
	}
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	THEAPP.m_pHandlerService->Set_PositionRequest();
#endif
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


void CJogSetDlg::OnBnClickedButtonMakeHandlerPositionMap()
{
	int nRes = 0;
	nRes = AfxMessageBox("포지션 맵이 변경되오니 주의하십시오.\n포지션 맵을 변경하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	UpdateData(TRUE);
	
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;
	CString FolderName;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		FolderName=  THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		FolderName=  THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	////////////////////////////////// Added for CMI3000 2000 ====> ///////////////////////////////////////////////////
	////// 테이블 번호에 따라 포지션맵을 만든다 //////////////////////////////////////
	CString FileName;
	switch (m_iStageNo_teaching) {
	case 1:	{FileName = FolderName+"\\HW\\MotionMovingPosition1.ini"; break;}
	case 2: {FileName = FolderName+"\\HW\\MotionMovingPosition2.ini"; break;}
	default: {MessageBox("The StageNo. is not valid. Please retry it with valid StageNo."); break;}
	}

	////////////////////////////////// <==== Added for CMI3000 2000 ///////////////////////////////////////////////////

	CIniFileCS INI(FileName);
	CString strSection = "Start Position";

	CString strXposition,strYposition;
	GetDlgItemText(IDC_EDIT_SET_START_POSITION_X, strXposition);
	GetDlgItemText(IDC_EDIT_SET_START_POSITION_Y, strYposition);
	double nStartPosX = atof(strXposition);
	double nStartPosY = atof(strYposition);

	INI.Set_Double(strSection, "X", nStartPosX);
	INI.Set_Double(strSection, "Y", nStartPosY);

	strSection = "XY Pitch";
	INI.Set_Double(strSection,"X", m_Edit_SetModulePitchX);
	INI.Set_Double(strSection,"Y", m_Edit_SetModulePitchY);

	strSection = "Moving Position";

	int ArrayX = THEAPP.m_pModelDataManager->m_iTrayArrayX;
	if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_Y)
		ArrayX = THEAPP.m_pModelDataManager->m_iTrayArrayY;

	CString strModuleNumber;

	for (int i = 0; i < ArrayX; i++)
	{
		strModuleNumber.Format("X-%d", i);
		INI.Set_Double(strSection, strModuleNumber, nStartPosX - (i * m_Edit_SetModulePitchX));

		strModuleNumber.Format("Y-%d", i);
		INI.Set_Double(strSection, strModuleNumber, nStartPosY);
	}
	
	AfxBeginThread(MotionPositionTest_Thread,this);
}



void CJogSetDlg::OnStnClickedStaticStageNo()
{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;
	CString FolderName;

	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		FolderName =  THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		FolderName =  THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CString sStageNo = "";

	CString FileName;
#ifdef INLINE_MODE
	switch (m_iStageNo_teaching) {
	case 1:	{FileName = FolderName+"\\HW\\MotionMovingPosition2.ini"; m_iStageNo_teaching = 2; break;}
	case 2: {FileName = FolderName+"\\HW\\MotionMovingPosition1.ini"; m_iStageNo_teaching = 1; break;}
	default: {FileName = FolderName+"\\HW\\MotionMovingPosition1.ini"; m_iStageNo_teaching = 1; break;}
	}
#endif
	CIniFileCS INI(FileName);

	CString strSection = "Start Position";

	m_nDxHandlerPosX = INI.Get_Double(strSection, "X", 0.0);
	m_nDxHandlerPosY = INI.Get_Double(strSection, "Y", 0.0);

	strSection = "XY Pitch";
	m_Edit_SetModulePitchX = INI.Get_Double(strSection,"X", 25);
	m_Edit_SetModulePitchY = INI.Get_Double(strSection,"Y", 28);

	sStageNo.Format("Stage %d", m_iStageNo_teaching); SetDlgItemText (IDC_STATIC_STAGE_NO, sStageNo);
	
	UpdateData(FALSE);
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
	FileName = FolderName+"TeachingBarcode";

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

void CJogSetDlg::OnBnClickedButtonGetInspZPosition()
{
	// Z축 설정값 불러오기
	if (THEAPP.m_pModelDataManager->m_sModelName != ".")
	{
		CString FolderName;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
			FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;
		else
			FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + THEAPP.m_pModelDataManager->m_sModelName;

		CString FileName;
		switch (m_iStageNo_teaching) {
		case 1: { FileName = FolderName + "\\HW\\MotionMovingPosition1.ini"; break; }
		case 2: { FileName = FolderName + "\\HW\\MotionMovingPosition2.ini"; break; }
		}

		CIniFileCS INI(FileName);

		CString strZposition[MAX_IMAGE_TAB];

		double dInspPosZ;
		CString strSection, strKey, strInspPosZ;
		strSection = "Insp Z Position";
		for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
		{
			strKey.Format("Image%d", iImageIdx + 1);
			dInspPosZ = INI.Get_Double(strSection, strKey, 30.0);
			strZposition[iImageIdx].Format("%.2f", dInspPosZ);

		}

		SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z1, strZposition[0]);
		SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z2, strZposition[1]);
		SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z3, strZposition[2]);
		SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z4, strZposition[3]);
		SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z5, strZposition[4]);
		SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z6, strZposition[5]);
		SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z7, strZposition[6]);
	}
}


void CJogSetDlg::OnBnClickedButtonSetInspZPosition()
{
	int nRes = 0;
	nRes = AfxMessageBox("Z축 티칭 값이 변경되오니 주의하십시오.\nZ축 티칭 값을 변경하시겠습니까?", MB_YESNO | MB_SYSTEMMODAL);

	if ((nRes != IDYES))
		return;

	UpdateData(TRUE);

	if(THEAPP.m_pModelDataManager->m_sModelName == ".") return;
	CString FolderName;
	if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
		FolderName =  THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;
	else
		FolderName =  THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName;

	CString FileName;

	FileName = FolderName+"\\HW\\MotionMovingPosition1.ini";

	CIniFileCS INI(FileName);

	CString strZposition[MAX_IMAGE_TAB];
	GetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z1, strZposition[0]);
	GetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z2, strZposition[1]);
	GetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z3, strZposition[2]);
	GetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z4, strZposition[3]);
	GetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z5, strZposition[4]);
	GetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z6, strZposition[5]);
	GetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z7, strZposition[6]);

	double dInspPosZ;
	CString strSection, strKey;
	strSection = "Insp Z Position";
	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		strKey.Format("Image%d", iImageIdx + 1);
		dInspPosZ = atof(strZposition[iImageIdx]);
		INI.Set_Double(strSection, strKey, dInspPosZ);
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
