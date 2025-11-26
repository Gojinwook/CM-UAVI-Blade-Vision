// uScan.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "uScan.h"

#include "MainFrm.h"
#include "uScanDoc.h"
#include "uScanView.h"
#include "AboutDlg.h"

#include <Psapi.h>
#include <winioctl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CuScanApp

BEGIN_MESSAGE_MAP(CuScanApp, CWinApp)
//{{AFX_MSG_MAP(CuScanApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
ON_COMMAND(ID_HELP, OnHelp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
// Standard print setup command
ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CuScanApp construction

CuScanApp::CuScanApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	ResetEachImgAvgGV(); // APD

	m_pGFunction = NULL;
	m_pInspectService = NULL;
	m_pAutoCalService = NULL;

	m_iModeSwitch = MODE_INSPECT_VIEW_ADMIN;
	m_iAppVersion = 0;

	m_sLoginMode = "";
	m_sOperator = "";

	m_dLotStartTickCount = -1;

	InitializeCriticalSection(&m_csSendADJ);
	InitializeCriticalSection(&m_csADJResultLog);
	InitializeCriticalSection(&m_csADJDaySummaryLog);

	for (int i = 0; i < DEEP_MODEL_NUM; i++)
		for (int j = 0; j < MAX_MODULE_NUM; j++)
			m_nADJResult[i][j] = RCV_WAIT;

	m_bShowReviewWindow = FALSE;

	m_iTrayLineNumber = 5;
	m_iModuleNumberOneLine = 5;
	m_iMaxModuleOneTray = m_iTrayLineNumber * m_iModuleNumberOneLine + 1;
	m_iMaxModuleOneLot = m_iTrayLineNumber * m_iModuleNumberOneLine * MAX_TRAY_LOT;

	// Multiple Defect start
	InitializeCriticalSection(&m_csMULTIPLEDEFECTLOG);

	// 3차원 배열을 범위 기반 for 루프로 순회
	for (auto &magazine : m_strMultipleDefectBarcode) // 1차원 (MAX_VIRTUAL_MAGAZINE_NO)
		for (auto &tray : magazine)					  // 2차원 (MAX_TRAY_LOT)
			for (auto &module : tray)				  // 3차원 (MAX_MODULE_ONE_TRAY)
				module = BARCODE_STATUS_DEFAULT;	  // 각 CString 객체에 값을 대입

	// Multiple Defect end

	// FOB Mode Change add Start
	m_nFOBModeUser = FOB_MODE_OPERATOR;
	m_strFOBModeOperatorID = "";
	// FOB Mode Change add End
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CuScanApp object

CuScanApp theApp;

void MyHalconExceptionHandler(const HException &except)
{
	throw except;
}

/////////////////////////////////////////////////////////////////////////////
// CuScanApp initialization

BOOL CuScanApp::InitInstance()
{
	if (!AfxSocketInit())
		return FALSE; // UDP Socket 초기화

	srand((unsigned)time(NULL));

	// if( CheckProcessExist() > 1) {
	//	AfxMessageBox("CMI System Restart.", MB_SYSTEMMODAL);
	// }

	// LeeGW START
	InitializeCriticalSection(&CS_DSF_LOG);
	InitializeCriticalSection(&CS_LOT_RESULT_LOG);
	InitializeCriticalSection(&CS_LOT_SUMMARY_LOG);
	// LeeGW END

	//	set_check("all");

	set_check("give_error");
	set_system("store_empty_region", "true"); // Ignore no region output
	set_system("clock_mode", "elapsed_time");
	set_system("max_connection", 200);
	set_system("clip_region", "false");

	// how to react in case of invalid input object
	set_system("no_object_result", "true");
	//	set_system("no_object_result", "exception");

	// how to react in case of invalid input region
	set_system("empty_region_result", "true");
	//	set_system("empty_region_result", "exception");

	//	set_system("do_low_error", "true");

	reset_obj_db(30000, 30000, 0);

	//////////////////////////////////////////////////////////////////////////
	// Use Parallel Halcon

	set_system("parallelize_operators", "false"); // default : true
	set_system("reentrant", "true");			  // default : true

	set_system("global_mem_cache", "exclusive");
	set_system("temporary_mem_cache", "true");

	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	Hlong iImgChacheSize = memInfo.ullTotalPhys * (0.6); // System Memory 60% 사용

	set_system("image_cache_capacity", iImgChacheSize);

	//////////////////////////////////////////////////////////////////////////

	m_bInitComplete = FALSE;
	AfxEnableControlContainer();
	AfxInitRichEdit(); // 리치에디트 초기화..

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls(); // Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic(); // Call this when linking to MFC statically
#endif

	AfxOleInit();

	TCHAR szCurDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH - 1, szCurDir);
	m_szWorkingDir = szCurDir;

	// Ver2629 add
	m_bUseEvms = FALSE;
	m_szWorkingEvmsDir = "";
	CString strCheckEvmsFoler;
	strCheckEvmsFoler = m_szWorkingDir + "\\..\\..\\..\\..\\ENV";
	int nEvmsExist = GetFileAttributes(strCheckEvmsFoler);
	if (nEvmsExist != -1) // EVMS파일 존재
	{
		CString strTempPreference = strCheckEvmsFoler + "\\Data\\Prefecrence.ini";

		CIniFileCS INI(strTempPreference);
		CString strSection = "EVMS_Option";

		m_bUseEvms = INI.Get_Bool(strSection, "EVMS_USE_CHANGE_DIRECTORY", FALSE);

		if (m_bUseEvms == TRUE)
		{
			m_szWorkingDir = strCheckEvmsFoler;
			m_szWorkingEvmsDir = szCurDir;
		}
	}
	// TO DO :
	// LOG : relative path -> absolute path
	// Model : add option (current folder or ENV folder)

	// Ver2629 end

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	// NOTE : MRU 는 최근사용한 파일 목록 리스트를 뜻하는 듯? -251030, jhkim
	LoadStdProfileSettings(); // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	// WriteProfileString(_T("Settings"), _T("LastUser"), _T("admin"));

	SetVersion();

	CString szDataFolder = m_szWorkingDir + "\\Data";
	CString szLogFolder = szDataFolder + "\\LOG";

	SYSTEMTIME logTime;
	GetLocalTime(&logTime);

	CString szLogFileName;
	szLogFileName.Format("%s\\%04d%02d%02d_%02d%02d%02d_Log.txt",
						 szLogFolder, logTime.wYear, logTime.wMonth, logTime.wDay, logTime.wHour, logTime.wMinute, logTime.wSecond);

	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	if ((hFindFile = FindFirstFile(szLogFolder, &FindFileData)) == INVALID_HANDLE_VALUE)
		CreateDirectory(szLogFolder, NULL);

	FindClose(hFindFile);

	// Save Thread - LeeGW
	m_pSaveManager = CSaveManager::GetInstance(); // SaveLog를 위해 여기 있음

	SaveLog("★★★★★★★★★★★★★★★★★★ Start CMI System Application ★★★★★★★★★★★★★★★★★★");
	SaveLog("★★★★★★★★★★★★★★★★★★★ 프로그램이 시작되었습니다 ★★★★★★★★★★★★★★★★★★");

	ReadPreferenceINI();
	ReadFaiReviewInfo();

	HException::InstallHHandler(&MyHalconExceptionHandler);

	CSingleDocTemplate *pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CuScanDoc),
		RUNTIME_CLASS(CMainFrame), // main SDI frame window
		RUNTIME_CLASS(CuScanView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	CheckProcessExist();

	// For Release

	m_bInitComplete = TRUE;
	m_bRun = TRUE;

	// Handler Initial
	m_pHandlerService = CHandlerService::GetInstance();
	m_pHandlerService->Initialize_Handler();
	SaveLog("Create Handler Class Complete");

	m_MIInterface.Initialize();

	m_pCalDataService = new CCalDataService;
	CString sCalDataFile1 = m_szWorkingDir + "\\Data\\cal_data_1.cal";
	m_pCalDataService->Load(sCalDataFile1);
	m_pGFunction = CGFunction::GetInstance();
	SaveLog("CalData Load Complete");

	//////////////////////////////////////////////////////////////////////////
	m_pCameraManager = CCameraManager::GetInstance();

#ifdef VISION_TRIGGER
	m_pTriggerManager = CTriggerManager::GetInstance();
#endif

	///////// $$$ DCF로부터 정보를 얻어와서 MIL에 관련된 하드웨어 제어 프로그램에 뭔가를 써주는 것 같다

#ifdef INLINE_MODE
	m_pCameraManager->InitGrabInterface();
	SaveLog("Create CameraManager Class Complete");

#ifdef VISION_TRIGGER
	m_pTriggerManager->Initialize();
	m_pTriggerManager->SetTriggerSleepTime(2);
#endif

	SaveLog("Create TriggerManager Class Complete");
#endif

	//////////////////////////////////////////////////////////////////////////Dlg display
	m_pInspectSummary = CInspectSummary::GetInstance();
	m_pInspectSummary->Show();
	m_pInspectAdminViewDlg = CInspectAdminViewDlg::GetInstance();
	m_pInspectAdminViewHideDlg = CInspectAdminViewHideDlg::GetInstance();
	m_pInspectAdminViewHideDlg->Show();
	m_pInspectAdminViewHideDlg->ShowWindow(SW_SHOWNA);
	m_pInspectAdminViewDlg->Show();
	m_pInspectViewBarrelSurfaceDlg = CInspectViewBarrelSurfaceDlg::GetInstance();
	m_pInspectViewBarrelEdgeDlg = CInspectViewBarrelEdgeDlg::GetInstance();
	m_pInspectViewLensInnerDlg = CInspectViewLensInnerDlg::GetInstance();
	m_pInspectViewLensOuterDlg = CInspectViewLensOuterDlg::GetInstance();

	if (m_iMachineInspType == MACHINE_SIDEFILL ||
		m_iMachineInspType == MACHINE_FIDUCIAL ||
		m_iMachineInspType == MACHINE_BRACKET)
	{
		m_pInspectViewExtra1Dlg = CInspectViewExtra1Dlg::GetInstance();
		m_pInspectViewExtra2Dlg = CInspectViewExtra2Dlg::GetInstance();
	}

	m_pInspectViewOverayImageDlg = CInspectViewOverayImageDlg::GetInstance();

	m_pInspectResultDlg = CInspectResultDlg::GetInstance();
	m_pInspectResultDlg->Show();
	m_pDefectListDlg = CDefectListDlg::GetInstance();
	m_pDefectListDlg->Show();
	m_pLogDlg = CLogDlg::GetInstance();
	m_pLogDlg->Show();

	// TODO : Need to optimize - 251104, jhkim
	m_pTabControlDlg = CTabControlDlg::GetInstance();
	m_pTabControlDlg->Show();
	m_pTabControlDlg->ShowWindow(SW_HIDE);

	////////////////////////////////////////////////////////////////////////////////////
	SaveLog("Create Dialog Instance Complete");

	m_pModelDataManager = CModelDataManager::GetInstance(); /// Model Data Manager class 생성
	m_pAlgorithm = Algorithm::GetInstance();
	SaveLog("Create ModelData & Algorithm Class Complete");

	m_pInspectService = CInspectService::GetInstance();
	m_pAutoCalService = CAutoCalService::GetInstance();

#ifdef INLINE_MODE
	// 0524
	m_pCameraManager->m_bUseGrabErrFIltering = Struct_PreferenceStruct.m_bUseGrabErrFIltering;
	m_pCameraManager->m_iGrabErrDarkAreaGVLimit = Struct_PreferenceStruct.m_iGrabErrDarkAreaGVLimit;
	m_pCameraManager->m_iGrabErrBrightAreaGVLimit = Struct_PreferenceStruct.m_iGrabErrBrightAreaGVLimit;
#endif

	//////////// $$$ 이미지 저장 포맷 결정 //////////////////////////////////
	m_pInspectAdminViewHideDlg->m_sImageFormat = ".bmp";
	if (Struct_PreferenceStruct.m_bSaveBMP)
		m_pInspectAdminViewHideDlg->m_sImageFormat = ".bmp";
	else
		m_pInspectAdminViewHideDlg->m_sImageFormat = ".jpg";

	///////// 마지막 모델 불러오기
	CString strStatusFileName = GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS ModelINI(strStatusFileName);
	CString strSection = "Status";
	m_pModelDataManager->m_sModelName = ModelINI.Get_String(strSection, "ModelID", "default");

	// TODO : Need to optimize - 251104, jhkim
	m_pModelDataManager->LoadModel();

	// ********************** Login, Operator *********************
	CString strRegisterFolder = GetWorkingDirectory() + "\\Data\\";
	CIniFileCS RegisterINI(strRegisterFolder + "\\RegisterInfo.ini");

	strSection = "Login Mode";
	m_sLoginMode = RegisterINI.Get_String(strSection, "Mode", "Default");

	strSection = "Operator";
	m_sOperator = RegisterINI.Get_String(strSection, "Operator", "Default");
	// *************************************************************

	/////////////////////////////// 모델 정보부터 로딩 - 모델의 갯수를 알아야 트레이 안에 모듈 갯수가 나옴
	m_pTrayAdminViewDlg = CTrayAdminViewDlg::GetInstance();
	m_pTrayAdminViewDlg->Show();
	m_pTrayOperatorViewPrevDlg = CTrayOperatorViewPrevDlg::GetInstance();
	m_pTrayOperatorViewPrevDlg->Show();
	m_pTrayOperatorViewPrevDlg->ShowWindow(SW_HIDE);
	m_pTrayOperatorViewNowDlg = CTrayOperatorViewNowDlg::GetInstance();
	m_pTrayOperatorViewNowDlg->Show();
	m_pTrayOperatorViewNowDlg->ShowWindow(SW_HIDE);

	m_pTrayBeamProjectViewDlg = CTrayBeamProjectViewDlg::GetInstance();
	if (Struct_PreferenceStruct.m_bCheckBeamProject == TRUE)
	{
		m_pTrayBeamProjectViewDlg->Show();				/// beamproject 설치 후
		m_pTrayBeamProjectViewDlg->ShowWindow(SW_HIDE); /// beamproject 설치 후
	}

	init_valiable();
	CuScanView *pView = (CuScanView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
	pView->OnBnClickedButtonModeChange();

	////// 최근 사용한 저장 폴더 확인 및 생성 //////////////////////////
	m_pInspectSummary->CheckPrevSaveFolder();

	////// 디스크 용량 표시 //////////////////////////
	m_pInspectSummary->ShowDiskCapacity();

	////// 바코드 NoRead 저장 폴더 /////
	CString FolderName = Struct_PreferenceStruct.m_strBasic_FolderPath + "\\CMI_Results";
	m_FileBase.CreateFolder(FolderName);
	CString FolderSub;
	FolderSub.Format("\\Barcode_NoRead");
	m_FileBase.CreateFolder(FolderName + FolderSub);
	//////////////////////////////////////////////////////////////////////////

	////// LAS Data 저장 폴더 /////
	const char *strLASBaseFolder = _T("D:\\EVMS");
	m_szLASDataFolder = strLASBaseFolder;
	m_szLASImageFolder = strLASBaseFolder;
	m_szLASTdmFolder = strLASBaseFolder;
	m_FileBase.CreateFolder(m_szLASDataFolder);

	const char *strTmpFolder = _T("\\TEMP");
	m_szLASTempFolder = strLASBaseFolder + CString(strTmpFolder);
	m_FileBase.CreateFolder(m_szLASTempFolder);

	FolderSub.Format("\\TP");
	m_szLASDataFolder += FolderSub;
	m_szLASImageFolder += FolderSub;
	m_FileBase.CreateFolder(m_szLASDataFolder);

	FolderSub.Format("\\Log");
	m_szLASDataFolder += FolderSub;
	FolderSub.Format("\\Logs");
	m_szLASImageFolder += FolderSub;
	m_FileBase.CreateFolder(m_szLASDataFolder);
	m_FileBase.CreateFolder(m_szLASImageFolder);

	FolderSub.Format("\\OP");
	m_szLASTdmFolder += FolderSub;
	m_FileBase.CreateFolder(m_szLASTdmFolder);

	FolderSub.Format("\\TDM");
	m_szLASTdmFolder += FolderSub;
	m_FileBase.CreateFolder(m_szLASTdmFolder);

	// MEMO : (Auto Param.) SECTION, EVENT Folder Set -250912, jhkim
	FolderSub.Format("\\SECTION");
	m_szLASSectionFolder = m_szLASTdmFolder + FolderSub;
	m_FileBase.CreateFolder(m_szLASSectionFolder);

	FolderSub.Format("\\Event");
	m_szLASEventFolder = m_szLASTdmFolder + FolderSub;
	;
	m_FileBase.CreateFolder(m_szLASEventFolder);

	//////////////////////////////////////////////////////////////////////////

	////// PC ID /////////
	DWORD nSize = MAX_COMPUTERNAME_LENGTH + 1;
	char szBuffer[MAX_COMPUTERNAME_LENGTH + 1];
	::GetComputerName(szBuffer, &nSize);
	m_szPCID.Format("%s", szBuffer);
	//////////////////////////////////////////////////////////////////////////

	// 초기 활성화 컨트롤들
	m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
	m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_STATUS_CHECK)->EnableWindow(TRUE);

#ifdef INLINE_MODE
	m_pInspectResultDlg->GetDlgItem(IDC_EDIT_LOT_ID)->EnableWindow(FALSE);
#endif

	THEAPP.m_pHandlerService->Set_StatusUpdate(VS_READY);
	THEAPP.DoubleLogOut("Init Done. %d", THEAPP.m_pHandlerService->m_nInspectPCStatus);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CuScanApp message handlers

int CuScanApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	SaveLog("★★★★★★★★★★★★★★★★★★ End CMI System Application ★★★★★★★★★★★★★★★★★★");
	SaveLog("★★★★★★★★★★★★★★★★★★ 프로그램이 종료되었습니다 ★★★★★★★★★★★★★★★★★★");

	CString strStatusFileName = GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS INI(strStatusFileName);
	CString strSection = "Status";
	INI.Set_String(strSection, "LotID", m_pInspectResultDlg->CurrentLotID);
	INI.Set_String(strSection, "ModelID", m_pModelDataManager->m_sModelName);

	if (m_bRun)
	{
		// Clean up dynamically allocated arrays
		if (m_pTrayAdminViewDlg->m_TrayImagePB.OkNg) delete[] m_pTrayAdminViewDlg->m_TrayImagePB.OkNg;
		if (m_pTrayAdminViewDlg->mp_dBlackCoatingOuterDiameter) delete[] m_pTrayAdminViewDlg->mp_dBlackCoatingOuterDiameter;
		if (m_pTrayAdminViewDlg->mp_dBlackCoatingPosX) delete[] m_pTrayAdminViewDlg->mp_dBlackCoatingPosX;
		if (m_pTrayAdminViewDlg->mp_dBlackCoatingPosY) delete[] m_pTrayAdminViewDlg->mp_dBlackCoatingPosY;
		if (m_pTrayAdminViewDlg->mp_dO1O2Distance) delete[] m_pTrayAdminViewDlg->mp_dO1O2Distance;
		if (m_pTrayAdminViewDlg->mp_dDiameterMin) delete[] m_pTrayAdminViewDlg->mp_dDiameterMin;
		if (m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion) delete[] m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion;
		if (m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg) delete[] m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg;
		if (m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion) delete[] m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion;
		if (m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg) delete[] m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg;
		if (m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion) delete[] m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion;
		if (m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg) delete[] m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg;
		if (m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion) delete[] m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion;
		if (m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG) delete[] m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG;
		if (m_pInspectAdminViewHideDlg->m_bLensDefectOKNG) delete[] m_pInspectAdminViewHideDlg->m_bLensDefectOKNG;
		if (m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG) delete[] m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG;
		if (m_pInspectAdminViewHideDlg->LensOkNg) delete[] m_pInspectAdminViewHideDlg->LensOkNg;
		if (m_pInspectService->m_pInspectAlgorithm) delete[] m_pInspectService->m_pInspectAlgorithm;
		
		delete m_pCalDataService;
#ifdef INLINE_MODE
		m_pCameraManager->DeleteInstance();
#endif
#ifdef VISION_TRIGGER
		m_pTriggerManager->DeleteInstance();
#endif
		if (m_pHandlerService) 
		{
#ifdef INLINE_MODE
			m_pHandlerService->Set_StatusUpdate(HS_STOP);
			Sleep(200);
			m_pHandlerService->Terminate_Handler();
#endif
			m_pHandlerService->DeleteInstance();
		}

		m_MIInterface.UnInitialize();

		m_pInspectSummary->DeleteInstance();
		m_pInspectAdminViewDlg->DeleteInstance();
		m_pInspectViewBarrelSurfaceDlg->DeleteInstance();
		m_pInspectViewBarrelEdgeDlg->DeleteInstance();
		m_pInspectViewLensInnerDlg->DeleteInstance();
		m_pInspectViewLensOuterDlg->DeleteInstance();

		if (m_iMachineInspType == MACHINE_SIDEFILL ||
			m_iMachineInspType == MACHINE_FIDUCIAL ||
			m_iMachineInspType == MACHINE_BRACKET)
		{
			m_pInspectViewExtra1Dlg->DeleteInstance();
			m_pInspectViewExtra2Dlg->DeleteInstance();
		}

		m_pTrayAdminViewDlg->DeleteInstance();
		m_pTrayOperatorViewPrevDlg->DeleteInstance();
		m_pTrayOperatorViewNowDlg->DeleteInstance();
		m_pInspectResultDlg->DeleteInstance();
		m_pDefectListDlg->DeleteInstance();
		m_pLogDlg->DeleteInstance();
		m_pTrayOperatorViewPrevDlg->DeleteInstance();
		m_pTrayOperatorViewNowDlg->DeleteInstance();
		m_pTabControlDlg->DeleteInstance();

		m_pModelDataManager->DeleteInstance();
		m_pAlgorithm->DeleteInstance();
		m_pInspectService->DeleteInstance();
		m_pAutoCalService->DeleteInstance();

		m_pGFunction->DeleteInstance();

		m_pInspectViewOverayImageDlg->DeleteInstance();
		m_pInspectAdminViewHideDlg->DeleteInstance();
		m_pTrayBeamProjectViewDlg->DeleteInstance();
		m_pSaveManager->DeleteInstance();
		
		DeleteCriticalSection(&CS_DSF_LOG); //  LeeGW

		m_bRun = FALSE;
	}

	return CWinApp::ExitInstance();
}
void CuScanApp::init_valiable()
{
	m_pInspectService->m_bInspectDone = TRUE; // inspection이 끝난 상태로 초기 세팅 한다. 안하면 시작을 안한다. inspection이 끝나지 않았기 때문에
	m_pInspectSummary->CheckLastTrayFolder();
	//	m_pInspectSummary->CheckResultSaveFolder();
	//	m_pInspectSummary->CheckPrevSaveFolder();

	m_pTrayAdminViewDlg->m_TrayImagePB.OkNg = new char[m_pModelDataManager->m_iTrayModuleMax];

	m_pTrayAdminViewDlg->mp_dBlackCoatingOuterDiameter = new double[m_pModelDataManager->m_iTrayModuleMax];
	m_pTrayAdminViewDlg->mp_dBlackCoatingPosX = new double[m_pModelDataManager->m_iTrayModuleMax];
	m_pTrayAdminViewDlg->mp_dBlackCoatingPosY = new double[m_pModelDataManager->m_iTrayModuleMax];
	m_pTrayAdminViewDlg->mp_dO1O2Distance = new double[m_pModelDataManager->m_iTrayModuleMax];
	m_pTrayAdminViewDlg->mp_dDiameterMin = new double[m_pModelDataManager->m_iTrayModuleMax];

	m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg = new char[m_pModelDataManager->m_iTrayModuleMax];

	m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg = new char[m_pModelDataManager->m_iTrayModuleMax];
	m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg = new char[m_pModelDataManager->m_iTrayModuleMax];

	m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion = new BOOL[m_pModelDataManager->m_iTrayModuleMax];
	m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion = new BOOL[m_pModelDataManager->m_iTrayModuleMax];
	m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion = new BOOL[m_pModelDataManager->m_iTrayModuleMax];
	m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion = new BOOL[m_pModelDataManager->m_iTrayModuleMax];

	m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG = new BOOL[m_pModelDataManager->m_iTrayModuleMax];
	m_pInspectAdminViewHideDlg->m_bLensDefectOKNG = new BOOL[m_pModelDataManager->m_iTrayModuleMax];
	m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG = new BOOL[m_pModelDataManager->m_iTrayModuleMax];

	m_pInspectService->m_pInspectAlgorithm = new Algorithm[m_pModelDataManager->m_iTrayModuleMax];

	m_pInspectAdminViewHideDlg->LensOkNg = new char[m_pModelDataManager->m_iTrayModuleMax]; // mes Lens Defect Info

	for (int i = 0; i < m_pModelDataManager->m_iTrayModuleMax; i++)
	{
		m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[i] = 0;
		m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg[i] = 0;
		m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg[i] = 0;
		m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[i] = 0;

		m_pTrayAdminViewDlg->mp_dBlackCoatingOuterDiameter[i] = 0;
		m_pTrayAdminViewDlg->mp_dBlackCoatingPosX[i] = 0;
		m_pTrayAdminViewDlg->mp_dBlackCoatingPosY[i] = 0;
		m_pTrayAdminViewDlg->mp_dO1O2Distance[i] = 0;
		m_pTrayAdminViewDlg->mp_dDiameterMin[i] = 0;

		for (int iImgIdx = 0; iImgIdx < MAX_IMAGE_TAB; iImgIdx++)
		{
			m_pInspectAdminViewDlg->pModuleGrabImage[i][iImgIdx].Reset();
			gen_empty_obj(&m_pInspectAdminViewDlg->pModuleGrabImage[i][iImgIdx]);
		}

		m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[i].Reset();
		gen_empty_obj(&m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[i]);

		m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG[i] = FALSE;
		m_pInspectAdminViewHideDlg->m_bLensDefectOKNG[i] = FALSE;
		m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG[i] = FALSE;

		m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion[i] = 0;
		m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion[i] = 0;
		m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion[i] = 0;
		m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion[i] = 0;

		m_pInspectAdminViewHideDlg->LensOkNg[i] = 0;
	}

	// FAI 실 사용 체크
	m_bUseFAI[26] = TRUE;
	m_bUseFAI[27] = TRUE;
	m_bUseFAI[28] = TRUE;
	m_bUseFAI[51] = TRUE;
	m_bUseFAI[52] = TRUE;
	m_bUseFAI[53] = TRUE;
	m_bUseFAI[14] = TRUE;
	m_bUseFAI[15] = TRUE;
	m_bUseFAI[16] = TRUE;
	m_bUseFAI[123] = TRUE;
	m_bUseFAI[124] = TRUE;
	m_bUseFAI[125] = TRUE;
	m_bUseFAI[126] = TRUE;
	m_bUseFAI[127] = TRUE;
	m_bUseFAI[128] = TRUE;
	m_bUseFAI[129] = TRUE;
	m_bUseFAI[130] = TRUE;

	LoadResultLotINI();
	// LoadResultYesterDayINI();
	LoadResultDayINI();
}

void CuScanApp::ReadPreferenceINI()
{
	CString strModelFolder = GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI(strModelFolder + "\\Prefecrence.ini");
	CString strSection = "Result Display";
	Struct_PreferenceStruct.m_bCheckBeamProject = INI.Get_Bool(strSection, "Beam Project", FALSE);
	strSection = "SaveImage File Extension";
	Struct_PreferenceStruct.m_bSaveBMP = INI.Get_Bool(strSection, "BMP", FALSE);
	Struct_PreferenceStruct.m_bSaveJPG = INI.Get_Bool(strSection, "JPG", TRUE);
	strSection = "Folder Path";
	Struct_PreferenceStruct.m_strBasic_FolderPath = INI.Get_String(strSection, "Save Result Folder Path", "D:\\");
	Struct_PreferenceStruct.m_strDebug_FolderPath = INI.Get_String(strSection, "Debug Folder Path", "C:\\");

	strSection = "EQUIPMENT NO";
	Struct_PreferenceStruct.m_strEquipNo = INI.Get_String(strSection, "EquipNo", "CMI4000");

	strSection = "BARCODE CAMERA";
	Struct_PreferenceStruct.m_iBarcodeCamSerial = INI.Get_Integer(strSection, "SerialNumber", 16119698);

	strSection = "Disk Cleaner";
	Struct_PreferenceStruct.m_iDiskMax = INI.Get_Integer(strSection, "m_iDiskMax", 85);
	Struct_PreferenceStruct.m_iDiskLean = INI.Get_Integer(strSection, "m_iDiskLean", 60);
	Struct_PreferenceStruct.m_iDiskTerm = INI.Get_Integer(strSection, "m_iDiskTerm", 40);
	Struct_PreferenceStruct.m_iDiskTimeH = INI.Get_Integer(strSection, "m_iDiskTimeH", 6);
	Struct_PreferenceStruct.m_iDiskTimeM = INI.Get_Integer(strSection, "m_iDiskTimeM", 45);
	Struct_PreferenceStruct.m_bDiskTermUse = INI.Get_Bool(strSection, "m_bDiskTermUse", FALSE);

	strSection = "Color";
	Struct_PreferenceStruct.m_iTrayOK_R = INI.Get_Integer(strSection, "TrayOK_R", 0);
	Struct_PreferenceStruct.m_iTrayOK_G = INI.Get_Integer(strSection, "TrayOK_G", 255);
	Struct_PreferenceStruct.m_iTrayOK_B = INI.Get_Integer(strSection, "TrayOK_B", 0);
	Struct_PreferenceStruct.m_iTrayOKFont_R = INI.Get_Integer(strSection, "TrayOKFont_R", 255);
	Struct_PreferenceStruct.m_iTrayOKFont_G = INI.Get_Integer(strSection, "TrayOKFont_G", 255);
	Struct_PreferenceStruct.m_iTrayOKFont_B = INI.Get_Integer(strSection, "TrayOKFont_B", 0);

	Struct_PreferenceStruct.m_iTrayNG_R = INI.Get_Integer(strSection, "TrayNG_R", 255);
	Struct_PreferenceStruct.m_iTrayNG_G = INI.Get_Integer(strSection, "TrayNG_G", 0);
	Struct_PreferenceStruct.m_iTrayNG_B = INI.Get_Integer(strSection, "TrayNG_B", 0);
	Struct_PreferenceStruct.m_iTrayNGB_R = INI.Get_Integer(strSection, "TrayNGB_R", 255);
	Struct_PreferenceStruct.m_iTrayNGB_G = INI.Get_Integer(strSection, "TrayNGB_G", 0);
	Struct_PreferenceStruct.m_iTrayNGB_B = INI.Get_Integer(strSection, "TrayNGB_B", 0);
	Struct_PreferenceStruct.m_iTrayNGL_R = INI.Get_Integer(strSection, "TrayNGL_R", 255);
	Struct_PreferenceStruct.m_iTrayNGL_G = INI.Get_Integer(strSection, "TrayNGL_G", 0);
	Struct_PreferenceStruct.m_iTrayNGL_B = INI.Get_Integer(strSection, "TrayNGL_B", 0);
	Struct_PreferenceStruct.m_iTrayNGFont_R = INI.Get_Integer(strSection, "TrayNGFont_R", 255);
	Struct_PreferenceStruct.m_iTrayNGFont_G = INI.Get_Integer(strSection, "TrayNGFont_G", 255);
	Struct_PreferenceStruct.m_iTrayNGFont_B = INI.Get_Integer(strSection, "TrayNGFont_B", 0);

	Struct_PreferenceStruct.m_iResultFont_R = INI.Get_Integer(strSection, "ResultFont_R", 255);
	Struct_PreferenceStruct.m_iResultFont_G = INI.Get_Integer(strSection, "ResultFont_G", 0);
	Struct_PreferenceStruct.m_iResultFont_B = INI.Get_Integer(strSection, "ResultFont_B", 128);

	strSection = "Defect_Priority";
	Struct_PreferenceStruct.iDefectPriority[0] = INI.Get_Integer(strSection, "NG_PRIORITY_1", 4);
	Struct_PreferenceStruct.iDefectPriority[1] = INI.Get_Integer(strSection, "NG_PRIORITY_2", 1);
	Struct_PreferenceStruct.iDefectPriority[2] = INI.Get_Integer(strSection, "NG_PRIORITY_3", 3);
	Struct_PreferenceStruct.iDefectPriority[3] = INI.Get_Integer(strSection, "NG_PRIORITY_4", 0);
	Struct_PreferenceStruct.iDefectPriority[4] = INI.Get_Integer(strSection, "NG_PRIORITY_5", 2);
	Struct_PreferenceStruct.iDefectPriority[5] = INI.Get_Integer(strSection, "NG_PRIORITY_6", 5);
	Struct_PreferenceStruct.iDefectPriority[6] = INI.Get_Integer(strSection, "NG_PRIORITY_7", 6);
	Struct_PreferenceStruct.iDefectPriority[7] = INI.Get_Integer(strSection, "NG_PRIORITY_8", 7);

	strSection = "Defect_Color";
	CString strTemp;

	strTemp = INI.Get_String(strSection, "COLOR_DEFECT_1", "255");
	Struct_PreferenceStruct.lNGColor[0] = atol(strTemp);
	strTemp = INI.Get_String(strSection, "COLOR_DEFECT_2", "255");
	Struct_PreferenceStruct.lNGColor[1] = atol(strTemp);
	strTemp = INI.Get_String(strSection, "COLOR_DEFECT_3", "255");
	Struct_PreferenceStruct.lNGColor[2] = atol(strTemp);
	strTemp = INI.Get_String(strSection, "COLOR_DEFECT_4", "255");
	Struct_PreferenceStruct.lNGColor[3] = atol(strTemp);
	strTemp = INI.Get_String(strSection, "COLOR_DEFECT_5", "255");
	Struct_PreferenceStruct.lNGColor[4] = atol(strTemp);
	strTemp = INI.Get_String(strSection, "COLOR_DEFECT_6", "255");
	Struct_PreferenceStruct.lNGColor[5] = atol(strTemp);
	strTemp = INI.Get_String(strSection, "COLOR_DEFECT_7", "255");
	Struct_PreferenceStruct.lNGColor[6] = atol(strTemp);
	strTemp = INI.Get_String(strSection, "COLOR_DEFECT_8", "255");
	Struct_PreferenceStruct.lNGColor[7] = atol(strTemp);

	strSection = "Option_Etc";
	Struct_PreferenceStruct.m_iGrabDelayTime = INI.Get_Integer(strSection, "GRAB_DELAY_TIME", 20);
	Struct_PreferenceStruct.m_iGrabDoneWaitTime = INI.Get_Integer(strSection, "GRAB_DONE_WAIT_TIME", 500);
	Struct_PreferenceStruct.m_iDefectDispDist = INI.Get_Integer(strSection, "Defect_Display_Dist", 5);
	Struct_PreferenceStruct.m_bDxLogDetail = INI.Get_Bool(strSection, "LOG_DETAIL", FALSE);
	Struct_PreferenceStruct.m_bSaveDefectFeatureLog = INI.Get_Bool(strSection, "SAVE_DSF_LOG", FALSE); // 불량 Feature 정보 추가 - LeeGW
	Struct_PreferenceStruct.m_bSaveDetectParamLog = INI.Get_Bool(strSection, "SAVE_PARAM_LOG", FALSE); // 검출 Param 로그 추가 - LeeGW

	Struct_PreferenceStruct.m_iSaveLasDataZipWaitTime = INI.Get_Integer(strSection, "SAVE_LAS_DATA_ZIP_WAIT_TIME", 0);
	Struct_PreferenceStruct.m_bSaveLasDataZip = INI.Get_Bool(strSection, "SAVE_LAS_DATA_ZIP", TRUE);
	Struct_PreferenceStruct.m_bSaveLasLog = INI.Get_Bool(strSection, "SAVE_LAS_LOG", TRUE);
	Struct_PreferenceStruct.m_bSaveLasImage = INI.Get_Bool(strSection, "SAVE_LAS_IMAGE", FALSE);
	Struct_PreferenceStruct.m_bSaveLasServerImage = INI.Get_Bool(strSection, "SAVE_LAS_SERVER_IMAGE", TRUE);

	Struct_PreferenceStruct.m_bSaveRawImage = INI.Get_Bool(strSection, "SAVE_RAW_IMAGE", TRUE);
	Struct_PreferenceStruct.m_bSaveNGImage = INI.Get_Bool(strSection, "SAVE_NG_IMAGE", FALSE);
	Struct_PreferenceStruct.m_bSaveReviewImage = INI.Get_Bool(strSection, "SAVE_REVIEW_IMAGE", FALSE);	// Review 이미지 저장 추가 - LeeGW
	Struct_PreferenceStruct.m_bSaveFAIImage = INI.Get_Bool(strSection, "SAVE_FAI_IMAGE", FALSE);		// Review 이미지 저장 추가 - LeeGW
	Struct_PreferenceStruct.m_bSaveResultImage = INI.Get_Bool(strSection, "SAVE_RESULT_IMAGE", TRUE);	// Result 이미지 저장 추가, Default TRUE - LeeGW
	Struct_PreferenceStruct.m_bSaveResultMerge = INI.Get_Bool(strSection, "RESULT_IMAGE_MERGE", FALSE); // 24.07.08 - v2652 - ResultImage 통합 저장 기능 추가 - LeeGW

	Struct_PreferenceStruct.m_iEmptyCircleRadius = INI.Get_Integer(strSection, "EM_CIRCLE_RADIUS", 300);
	Struct_PreferenceStruct.m_iEmptyMaxEdgePoint = INI.Get_Integer(strSection, "EM_MAX_EDGE_POINT", 7000);

	Struct_PreferenceStruct.m_iBarcodeNoReadImageType = INI.Get_Integer(strSection, "BARCODE_NO_READ_IMAGE_TYPE", 0);

	Struct_PreferenceStruct.m_sProductName = INI.Get_String(strSection, "PRODUCT NAME", "");

	// 0524
	Struct_PreferenceStruct.m_bUseGrabErrFIltering = INI.Get_Bool(strSection, "USE_GRAB_ERROR_FILTERING", FALSE);
	Struct_PreferenceStruct.m_iGrabErrDarkAreaGVLimit = INI.Get_Integer(strSection, "GRAB_ERROR_DARK_AREA_GV_LIMIT", 20);
	Struct_PreferenceStruct.m_iGrabErrBrightAreaGVLimit = INI.Get_Integer(strSection, "GRAB_ERROR_BRIGHT_AREA_GV_LIMIT", 255);
	Struct_PreferenceStruct.m_iGrabErrRetryNo = INI.Get_Integer(strSection, "GRAB_ERROR_RETRY_NUMBER", 3);
	Struct_PreferenceStruct.m_iTriggerSleepTime = INI.Get_Integer(strSection, "TRIGGER_SLEEP_TIME", 45);

	Struct_PreferenceStruct.m_iLightErrorMinLimit = INI.Get_Integer(strSection, "LIGHT_ERROR_MIN_LIMIT", 20);
	Struct_PreferenceStruct.m_iLightErrorMaxLimit = INI.Get_Integer(strSection, "LIGHT_ERROR_MAX_LIMIT", 255);

	Struct_PreferenceStruct.m_iDefectWarningDefectCount = INI.Get_Integer(strSection, "DEFECT_WARNING_DEFECT_COUNT", 5);
	Struct_PreferenceStruct.m_iDefectWarningDefectDistance = INI.Get_Integer(strSection, "DEFECT_WARNING_DEFECT_DISTANCE", 30);

	Struct_PreferenceStruct.m_iCamFOVType = INI.Get_Integer(strSection, "CAM_FOV_TYPE", 0);

	Struct_PreferenceStruct.m_iPickerTrayDir = INI.Get_Integer(strSection, "PICKER_TRAY_DIR", 1);

	Struct_PreferenceStruct.m_bSpecialNGSort = INI.Get_Bool(strSection, "SPECIAL_NG_SORT", FALSE); // 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

	Struct_PreferenceStruct.m_bSaveRawImageResize = INI.Get_Bool(strSection, "USE_RAWIMAGE_RESIZE", FALSE);			   // 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	Struct_PreferenceStruct.m_dSaveRawImageResizeRatio = INI.Get_Double(strSection, "USE_RAWIMAGE_RESIZE_RATIO", 1.0); // 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	Struct_PreferenceStruct.m_bSaveRawImageMerge = INI.Get_Bool(strSection, "USE_RAWIMAGE_MERGE", FALSE);			   // 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

	Struct_PreferenceStruct.m_bUseGrabErrFIltering2 = INI.Get_Bool(strSection, "USE_GRAB_ERROR_FILTERING2", FALSE); // 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW
	Struct_PreferenceStruct.m_iGrabErrSubGVLimit = INI.Get_Integer(strSection, "GRAB_ERROR_SUB_GV_MAX_LIMIT", 0);	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW

	Struct_PreferenceStruct.m_iMachineInspType = INI.Get_Integer(strSection, "MACHINE_INSPECTION_TYPE", 0);

	Struct_PreferenceStruct.m_bUseProcessRestart = INI.Get_Bool(strSection, "USE_PROCESS_RESTART", FALSE);					  // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	Struct_PreferenceStruct.m_dProcessRestartMemoryLimit = INI.Get_Double(strSection, "PROCESS_RESTART_MEMORY_LIMIT", 100.0); // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	Struct_PreferenceStruct.m_iProcessRestartTimeMin = INI.Get_Integer(strSection, "PROCESS_RESTART_TIME_MIN", 30);			  // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	Struct_PreferenceStruct.m_iProcessRestartTimeMax = INI.Get_Integer(strSection, "PROCESS_RESTART_TIME_MAX", 1440);		  // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	Struct_PreferenceStruct.m_iProcessRestartDelayTime = INI.Get_Integer(strSection, "PROCESS_RESTART_DELAY_TIME", 0);		  // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW

	Struct_PreferenceStruct.m_bUseMboMode = INI.Get_Bool(strSection, "USE_MBO_MODE", FALSE);
	Struct_PreferenceStruct.m_iMboModeCount = INI.Get_Integer(strSection, "MBO_MODE_COUNT", 1);

	Struct_PreferenceStruct.m_bUseHandlerRetry = INI.Get_Bool(strSection, "USE_HANDLER_RETRY", FALSE);			// 25.06.24 - LeeGW
	Struct_PreferenceStruct.m_iHandlerRetryCount = INI.Get_Integer(strSection, "HANDLER_RETRY_COUNT", 0);		// 25.06.24 - LeeGW
	Struct_PreferenceStruct.m_iHandlerRetryWaitTime = INI.Get_Integer(strSection, "HANDLER_RETRY_WAITTIME", 0); // 25.06.24 - LeeGW
	Struct_PreferenceStruct.m_iHandlerReplyWaitTime = INI.Get_Integer(strSection, "HANDLER_REPLY_WAITTIME", 0); // 25.06.24 - LeeGW

	strSection = "Option_Result";
	Struct_PreferenceStruct.m_iLensResultImageX = INI.Get_Integer(strSection, "LENS_RESULT_START_X", 700);
	Struct_PreferenceStruct.m_iLensResultImageY = INI.Get_Integer(strSection, "LENS_RESULT_START_Y", 700);
	Struct_PreferenceStruct.m_dLensResultImageZoom = INI.Get_Double(strSection, "LENS_RESULT_ZOOM_RATIO", 0.55);
	Struct_PreferenceStruct.m_iExtraResultImageX = INI.Get_Integer(strSection, "EXTRA_RESULT_START_X", 0);
	Struct_PreferenceStruct.m_iExtraResultImageY = INI.Get_Integer(strSection, "EXTRA_RESULT_START_Y", 0);
	Struct_PreferenceStruct.m_dExtraResultImageZoom = INI.Get_Double(strSection, "EXTRA_RESULT_ZOOM_RATIO", 1.0);

	strSection = "Option_ADJ";
	Struct_PreferenceStruct.m_bIsUseAIInsp = INI.Get_Bool(strSection, "USE_AI_INSP", FALSE);
	Struct_PreferenceStruct.m_bIsApplyAIResult = INI.Get_Bool(strSection, "APPLY_AI_RESULT", FALSE);
	Struct_PreferenceStruct.m_bIsApplyAISimulation = INI.Get_Bool(strSection, "APPLY_AI_SIMULATION", FALSE);
	Struct_PreferenceStruct.m_bUseAIResizeImageSave = INI.Get_Bool(strSection, "USE_AI_RESIZE_IMAGE_SAVE", FALSE);
	Struct_PreferenceStruct.m_iAICropImageWidth = INI.Get_Integer(strSection, "AI_CROP_IMAGE_WIDTH", 256);
	Struct_PreferenceStruct.m_iAICropImageHeight = INI.Get_Integer(strSection, "AI_CROP_IMAGE_HEIGHT", 256);
	Struct_PreferenceStruct.m_iAIResizeImageWidth = INI.Get_Integer(strSection, "AI_RESIZE_IMAGE_WIDTH", 256);
	Struct_PreferenceStruct.m_iAIResizeImageHeight = INI.Get_Integer(strSection, "AI_RESIZE_IMAGE_HEIGHT", 256);
	Struct_PreferenceStruct.m_strADJIPAddress = INI.Get_String(strSection, "ADJ_IP", "127.0.0.1");
	Struct_PreferenceStruct.m_iADJPortNo = INI.Get_Integer(strSection, "ADJ_PORT", 9001);
	Struct_PreferenceStruct.m_strCurrentADJ_IP = INI.Get_String(strSection, "Current_IP", "0.0.0.0");
	Struct_PreferenceStruct.m_iADJImageCropType = INI.Get_Integer(strSection, "ADJ_IMAGE_CROP_TYPE", 0);
	Struct_PreferenceStruct.m_strADJModelName = INI.Get_String(strSection, "ADJ_MODEL_NAME", "");
	Struct_PreferenceStruct.m_iADJDelayTime = INI.Get_Integer(strSection, "ADJ_DELAY_TIME", 10);
	Struct_PreferenceStruct.m_bIsUseADJRunCheck = INI.Get_Bool(strSection, "USE_ADJ_RUN_CHECK", FALSE);
	Struct_PreferenceStruct.m_strADJExePath = INI.Get_String(strSection, "ADJ_EXE_PATH", "");

	// Multiple Defect Start
	strSection = "Multiple_Defect";

	Struct_PreferenceStruct.m_bUseMultipleDefectAlarm = INI.Get_Bool(strSection, "MD_USE_ALARM", FALSE);
	Struct_PreferenceStruct.m_bUseMultipleDefectAlarmAreaSimilarity = INI.Get_Bool(strSection, "MD_USE_AREA_SIMILARITY", FALSE);
	Struct_PreferenceStruct.m_bUseMultipleDefectSendAlarm = INI.Get_Bool(strSection, "MD_USE_SEND_HANDLER_ALARM", FALSE);
	Struct_PreferenceStruct.m_iMultipleDefectAlarmSamePosTolerance = INI.Get_Integer(strSection, "MD_POS_TOLERANCE", 10);
	Struct_PreferenceStruct.m_iMultipleDefectAlarmAreaSimilarityTolerance = INI.Get_Integer(strSection, "MD_AREA_TOLERANCE", 30);
	Struct_PreferenceStruct.m_iMultipleDefectAlarmDefectRatio = INI.Get_Integer(strSection, "MD_DefectRatio", 5);
	Struct_PreferenceStruct.m_iMultipleDefectAlarmMinimumNum = INI.Get_Integer(strSection, "MD_MIN_INSPECTION_NUM", 100);
	// Multiple Defect Start

	// Ver2629 Start
	strSection = "EVMS_Option";

	Struct_PreferenceStruct.m_bChangeEvmsDirectory = INI.Get_Bool(strSection, "EVMS_USE_CHANGE_DIRECTORY", FALSE);
	Struct_PreferenceStruct.m_bUseAbsolutePathModel = INI.Get_Bool(strSection, "EVMS_USE_ABSOLUTE_MODEL_PATH", FALSE);
	Struct_PreferenceStruct.m_bUseAbsolutePathModel = FALSE;

	// Ver2629 End

	// RMS start
	strSection = "RMS_Option";
	Struct_PreferenceStruct.m_strRmsSaveFolderPath = INI.Get_String(strSection, "RMS_SAVE_FOLDER_PATH", "D:\\EVMS\\");
	// RMS end

	// Result Text Start
	strSection = "RESULT_TEXT";
	Struct_PreferenceStruct.m_iResultTextPosX = INI.Get_Integer(strSection, "RESULT_TEXT_POS_X", 0);
	Struct_PreferenceStruct.m_iResultTextPosY = INI.Get_Integer(strSection, "RESULT_TEXT_POS_Y", 0);
	Struct_PreferenceStruct.m_iResultTextSize = INI.Get_Integer(strSection, "RESULT_TEXT_SIZE", 0);
	// Result Text End

	m_iMachineInspType = Struct_PreferenceStruct.m_iMachineInspType;
	if (m_iMachineInspType < MACHINE_NORMAL || m_iMachineInspType > MACHINE_WELDING)
		m_iMachineInspType = MACHINE_NORMAL;

	// 검사유형 통합(ROI기준 알고리즘 설정) - LeeGW
	if (m_iMachineInspType == MACHINE_BRACKET)
		m_iMaxInspImageNo = MAX_IMAGE_TAB;
	else if ((m_iMachineInspType == MACHINE_SIDEFILL) || (m_iMachineInspType == MACHINE_FIDUCIAL))
		m_iMaxInspImageNo = EXTRA2_IMAGE_TAB;
	else if (m_iMachineInspType == MACHINE_WELDING)
		m_iMaxInspImageNo = WELDING_IMAGE_TAB;
	else
		m_iMaxInspImageNo = BARREL_LENS_IMAGE_TAB;
	// 검사유형 통합(ROI기준 알고리즘 설정) - LeeGW
}

void CuScanApp::ReadFaiReviewInfo()
{

	for (int i = 0; i < MAX_FAI_ITEM; i++)
		for (int ii = 0; ii < MAX_ONE_FAI_MEASURE_VALUE; ii++)
			Struct_FAI_ReviewInfo[i][ii].Reset();

	CString strDataFolder = GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI_FAI_ReviewInfo(strDataFolder + "ReviewInfo_FAI.ini");
	CString strSection = "FAI_REVIEW_INFO";
	CString strKey = _T("");

	char *sep = ",";
	CString sSampleInfo, sTemp;
	for (int i = 1; i <= MAX_REVIEW_SAMPLE; i++)
	{
		strKey.Format("%d", i);
		INI_FAI_ReviewInfo.Get_String(strSection, strKey, "Nan");

		sSampleInfo = INI_FAI_ReviewInfo.Get_String(strSection, strKey, "Nan");

		if (sSampleInfo == "Nan")
			break;

		char *psTemp = (LPTSTR)(LPCTSTR)sSampleInfo;
		int iFaiNo = 0, iFaiValNo = 0;

		sTemp = strtok(psTemp, sep);
		iFaiNo = atoi(sTemp);
		sTemp = strtok(NULL, sep);
		iFaiValNo = atoi(sTemp);
		Struct_FAI_ReviewInfo[iFaiNo][iFaiValNo].m_bSaveReview = TRUE;
		Struct_FAI_ReviewInfo[iFaiNo][iFaiValNo].m_sFaiName = strtok(NULL, sep);
		sTemp = strtok(NULL, sep);
		Struct_FAI_ReviewInfo[iFaiNo][iFaiValNo].m_iImageNo = atoi(sTemp);
		sTemp = strtok(NULL, sep);
		Struct_FAI_ReviewInfo[iFaiNo][iFaiValNo].m_iStartX = atoi(sTemp);
		sTemp = strtok(NULL, sep);
		Struct_FAI_ReviewInfo[iFaiNo][iFaiValNo].m_iStartY = atoi(sTemp);
		sTemp = strtok(NULL, sep);
		Struct_FAI_ReviewInfo[iFaiNo][iFaiValNo].m_fZoomRatio = atof(sTemp);
		sTemp = strtok(NULL, sep);
		Struct_FAI_ReviewInfo[iFaiNo][iFaiValNo].m_iMergeCnt = atoi(sTemp);
		Struct_FAI_ReviewInfo[iFaiNo][iFaiValNo].m_sColor = strtok(NULL, sep);
		;
	}
}

void CuScanApp::LoadResultLotINI()
{

	CString FolderName = Struct_PreferenceStruct.m_strBasic_FolderPath + "\\CMI_Results";

	CString FolderSub;

	SYSTEMTIME time;
	GetLocalTime(&time);
	FolderSub.Format("\\%d", time.wYear);
	FolderName = FolderName + FolderSub;
	FolderSub.Format("\\%d", time.wMonth);
	FolderName = FolderName + FolderSub;
	FolderSub.Format("\\%d", time.wDay);
	FolderName = FolderName + FolderSub;

	CString LotID;
	m_pInspectResultDlg->GetDlgItemText(IDC_EDIT_LOT_ID, LotID);
	FolderName = FolderName + "\\" + LotID;

	CString FileName;
	FileName.Format("%s\\LotSummary_%s_%04d%02d%02d.txt", FolderName, LotID, time.wYear, time.wMonth, time.wDay);

	CIniFileCS INI(FileName);

	CString strSection = "Results";

	int iOK = INI.Get_Integer(strSection, "양품", 0);
	int iNG = INI.Get_Integer(strSection, "불량", 0);
	int iOKNG = INI.Get_Double(strSection, "수율", 0.0);
	int iTotalBarcodeError = INI.Get_Integer(strSection, "바코드미인식", 0);

	m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_OKCOUNT, iOK);
	m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NGCOUNT, iNG);
	m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NUMBER_OF_MODULE, iOK + iNG);
	m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_BARCODE_ERROR_LOT, iTotalBarcodeError);

	double yield_Day;
	if ((iOK + iNG) > 0)
		yield_Day = ((double)iOK / (iOK + iNG)) * 100;
	else
		yield_Day = 0;

	CString strYield_Day;
	strYield_Day.Format("%.2lf", yield_Day);
	m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_YIELD, strYield_Day);
}
void CuScanApp::LoadResultDayINI()
{

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString sPerformFile;
	sPerformFile.Format("%s\\DaySummary_%04d%02d%02d.txt", m_FileBase.m_strResultFolderPrev, time.wYear, time.wMonth, time.wDay);

	CIniFileCS DPINI(sPerformFile);

	CString sPerfomSection = "Today";

	CString sYield;
	sYield.Format("%.2lf", DPINI.Get_Double(sPerfomSection, "수율", 0.0));

	m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_OKCOUNT_DAY, DPINI.Get_Integer(sPerfomSection, "양품", 0));
	m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NGCOUNT_DAY, DPINI.Get_Integer(sPerfomSection, "불량", 0));
	m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NUMBER_OF_MODULE_DAY, DPINI.Get_Integer(sPerfomSection, "총량", 0));
	m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_YIELD_DAY, sYield);
	m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_RESET_TIME_HR, DPINI.Get_Integer(sPerfomSection, "기준시간", 0));
}

void CuScanApp::GetLastDate(int *Year, int *Month, int *YesterDay)
{
	// 현재 날짜 구하기, #define GetCurrentTime() 동일
	// CTime::GetTickCount() <- 꼭 앞에 CTime로 호출해야 된다. 안하면 70년대 시간이 나옴;;;
	CTime time = CTime::GetTickCount();
	int year = time.GetYear();
	int month = time.GetMonth();
	int day = time.GetDay();
	int week = time.GetDayOfWeek();

	/* 이전 마지막 일자 구하기 */
	// CTime(년, 월, 일, 시, 분, 초);
	CTime firstDay = CTime(year, month, 1, 0, 0, 0);

	// 하루 24시간의 tick값 CTimeSpan(일, 시간, 분, 초);
	CTimeSpan span(1, 0, 0, 0);

	// 시작일에서 하루 빼기
	//    firstDay -= span;

	int beforeYear = firstDay.GetYear();
	int beforeMonth = firstDay.GetMonth();
	int beforeDay = firstDay.GetDay();
	int beforeWeek = firstDay.GetDayOfWeek();

	/* 현재달 마지막일 구하기 */
	if (day == 1)
	{
		CTime TlastDay = CTime(beforeYear, beforeMonth, 1, 0, 0, 0);
		TlastDay -= span;

		int lastYear = TlastDay.GetYear();
		int lastMonth = TlastDay.GetMonth();
		int lastDay = TlastDay.GetDay();
		int lastWeek = TlastDay.GetDayOfWeek();
		*Year = lastYear;
		*Month = lastMonth;
		*YesterDay = lastDay;
	}
	else
	{
		*Year = year;
		*Month = month;
		*YesterDay = day - 1;
	}
}

void CuScanApp::SetVersion()
{
	char AppFilePath[_MAX_PATH * 2];
	// 실행파일의 풀패스 경로를 구한다.
	strcpy(AppFilePath, AfxGetApp()->m_pszHelpFilePath);
	int slength = strlen(AppFilePath);
	strcpy(AppFilePath + (slength - 4), ".exe");

	//
	// 주어진 파일의 버전 정보를 읽는다. (이렇게 길어지리라 생각 못했다... )
	//
	DWORD Dummy = NULL;
	BOOL bResult = FALSE;
	HRESULT hResult = S_OK;
	DWORD dwLen = 0;
	DWORD dwError = 0;
	PVOID pVersionData = NULL;
	PVOID pVersionInfo = NULL;

	// 버전정보의 크기를 알아 내고 데이터 버퍼를 할당한다.
	// 파일의 버전정보의 크기가 파일마다 제각기 다르기 땜시...
	dwLen = ::GetFileVersionInfoSize(AppFilePath, &Dummy);
	if (dwLen == 0)
	{
		return;
	}
	// 버전 정보를 담을 버퍼를 할당한다. (수천 바이트가 필요하다)
	pVersionData = ::HeapAlloc(::GetProcessHeap(), 0, dwLen);
	ZeroMemory(pVersionData, dwLen);

	// 버전 정보 데이터를 읽는다.
	bResult = ::GetFileVersionInfo(AppFilePath, NULL, dwLen, pVersionData);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return;
	}

	// VS_FIXEDFILEINFO 로 부터 버전정보를 읽는다.
	// (숫자로 된 버전정보는 이 구조체 안에 정보가 담긴다)
	bResult = ::VerQueryValue(pVersionData, TEXT("\\"), &pVersionInfo, (PUINT)&dwLen);
	if (bResult == FALSE)
	{
		dwError = GetLastError();
		::HeapFree(::GetProcessHeap(), 0, pVersionData);
		return;
	}

	VS_FIXEDFILEINFO *pFileInfo = (VS_FIXEDFILEINFO *)pVersionInfo;
	// 숫자로된 버전 정보를 알아냈으니 필요한 대로 쓰문 되겠다.
	DWORD dwMajorVer = HIWORD(pFileInfo->dwFileVersionMS);
	DWORD dwMinorVer = LOWORD(pFileInfo->dwFileVersionMS);
	DWORD dwReleaseVer = HIWORD(pFileInfo->dwFileVersionLS);
	DWORD dwBuildVer = LOWORD(pFileInfo->dwFileVersionLS);

	CString strVer;
	strVer.Format("%d%d%d%d", dwMajorVer, dwMinorVer, dwReleaseVer, dwBuildVer);

	// SW Version Copy
	m_strVerInfo = strVer;

	m_iAppVersion = atoi((LPCTSTR)strVer);

	::HeapFree(::GetProcessHeap(), 0, pVersionData);
}

void CuScanApp::InitProgress(int range, CString sText)
{
}

// App command to run the dialog
void CuScanApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CuScanApp::OnHelp()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CuScanApp::SaveLog(CString strLog)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strFile;
	strFile.Format(GetWorkingDirectory() + "\\Data\\" + "LOG\\LogEvent_%04d%02d%02d_%02d.txt", time.wYear, time.wMonth, time.wDay, (time.wHour / LOG_EVENT_CUT_TIME) * LOG_EVENT_CUT_TIME); // 24.07.31 - v2656 - LogEvent Cut Time 추가 - LeeGW

	CString strSave;
	strSave.Format("[%04d-%02d-%02d %02d:%02d:%02d %03d] %s\r\n",
				   time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, strLog);

	m_pSaveManager->SaveLog(strSave, strFile); // LeeGW
}

// 로그 메시지를 두 군데(디버그 출력 창, LogEvent.txt)에 동시에 출력 - 250916, jhkim
void CuScanApp::DoubleLogOut(const char *format, ...)
{
	CString strOutMsg;
	va_list args;

	// va_start는 가변 인자 목록을 초기화합니다.
	// 두 번째 인자는  바로 앞에 위치한 마지막 고정 인자입니다.
	va_start(args, format);

	// 로그 메시지를 저장할 버퍼를 정의합니다.
	// 충분히 큰 크기로 설정하거나 동적으로 할당할 수 있습니다.
	std::vector<char> buffer(512);

	// vprintf와 유사한 vsnprintf를 사용하여 안전하게 문자열을 포맷팅합니다.
	// 이는 버퍼 오버플로우를 방지하는 데 유용합니다.
	int nWritten = vsnprintf(buffer.data(), buffer.size(), format, args);

	// vsnprintf가 -1을 반환하면 버퍼 크기가 충분하지 않았음을 의미합니다.
	if (nWritten < 0)
	{
		// 더 큰 버퍼로 다시 시도하거나, 오류를 출력합니다.
	}
	else
	{
		strOutMsg.Format("%s", buffer.data());
	}

	// va_end는 가변 인자 목록 사용을 종료하고 정리합니다.
	va_end(args);

	AfxTrace(strOutMsg + "\n");

	THEAPP.SaveLog(strOutMsg);
}


void CuScanApp::SaveDetectLog(CString strLog)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strFile;
	strFile.Format(GetWorkingDirectory() + "\\Data\\" + "LOG\\DetectLogEvent_%04d%02d%02d_%02d.txt", time.wYear, time.wMonth, time.wDay, (time.wHour / LOG_EVENT_CUT_TIME) * LOG_EVENT_CUT_TIME); // 24.07.31 - v2656 - LogEvent Cut Time 추가 - LeeGW

	CString strSave;
	strSave.Format("[%04d-%02d-%02d %02d:%02d:%02d %03d] %s\r\n",
				   time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, strLog);

	m_pSaveManager->SaveLog(strSave, strFile); // LeeGW
}

void CuScanApp::SaveLotSummaryLog(BOOL bSaveLas, CString sStartTime, CString sEndTime, CString sLotID, double dInspectTime,
								  int iTotalInsp, int iTotalOK, double dOKRatio, int iTotalNG, double dNGRatio,
								  int iTotalBarrelDefect, int iTotalLensDefect, int iTotalBarrelLensDefect, int iTotalBarrelDirt, int iTotalBarrelScratch, int iTotalBarrelStain,
								  int iTotalBarrelDent, int iTotalBarrelChipping, int iTotalBarrelEpoxy, int iTotalBarrelOutsideCT, int iTotalBarrelPeelOff, int iTotalBarrelWhiteDot,
								  int iTotalLensContamination, int iTotalLensScratch, int iTotalLensDirt, int iTotalLensWhiteDot, int iTotalEpoxyHole, int iTotalSpringNG,
								  int iTotalFAING, int iTotalBlackCoatingNG, int iTotalModuleMix, int iTotalBarcodeError, int iTotalBarcodeShift, int iTotalMatchingError, int iTotalLightError)
{

	EnterCriticalSection(&CS_LOT_SUMMARY_LOG);

	SYSTEMTIME time;
	GetLocalTime(&time);

	CFile file, file2;
	CString strFile, strFile2;
	CString strDelimeter, strSave;

	try
	{

		//////////
		strDelimeter = _T(",");

		if (bSaveLas)
		{
			strFile.Format("%s\\%s_%04d%02d%02d%02d_LotSummary.csv", m_szLASDataFolder, sLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
		}
		else
		{
			strFile.Format(m_FileBase.m_strResultFolderPrev + "\\DayLotSummary_%04d%02d%02d.csv", time.wYear, time.wMonth, time.wDay);
		}

		//////////
		if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			LeaveCriticalSection(&CS_LOT_SUMMARY_LOG);
			return;
		}

		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			strSave = _T("");
			for (long n = 0; n < LOT_SUMMARY_HEADER_END; n++)
			{
				strSave += g_sLotSummaryLogHeader[LOT_SUMMARY_HEADER_DATE + n];
				strSave += strDelimeter;
			}

			strSave += "\r\n";

			file.Write(strSave, strSave.GetLength());
		}

		//////////
		CString strTemp;
		strSave = _T("");

		// Data
		strTemp.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
		strSave += strTemp + strDelimeter;

		// Time
		strTemp.Format("\'%04d-%02d-%02d %02d:%02d:%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		strSave += strTemp + strDelimeter;

		// Start Time
		strSave += sStartTime + strDelimeter;

		// End Time
		strSave += sEndTime + strDelimeter;

		// Station
		strSave += GetPCID() + strDelimeter;

		// SW Version
		strTemp.Format(_T("%d"), m_iAppVersion);
		strSave += strTemp + strDelimeter;

		// Model Version
		strSave += _T("1000") + strDelimeter;

		// Model
		strSave += m_pModelDataManager->m_sModelName + strDelimeter;

		// Lot ID
		strSave += sLotID + strDelimeter;

		// Cycle Time
		strTemp.Format(_T("%.3f"), dInspectTime);
		strSave += strTemp + strDelimeter;

		// Total Insp Count
		strTemp.Format(_T("%d"), iTotalInsp);
		strSave += strTemp + strDelimeter;

		// Total OK Count
		strTemp.Format(_T("%d"), iTotalOK);
		strSave += strTemp + strDelimeter;

		// Total OK Ratio
		strTemp.Format(_T("%.3f"), dOKRatio);
		strSave += strTemp + strDelimeter;

		// Total NG Count
		strTemp.Format(_T("%d"), iTotalNG);
		strSave += strTemp + strDelimeter;

		// Total NG Ratio
		strTemp.Format(_T("%.3f"), dNGRatio);
		strSave += strTemp + strDelimeter;

		// Total Barrel Defect
		strTemp.Format(_T("%d"), iTotalBarrelDefect);
		strSave += strTemp + strDelimeter;

		// Total Lens Defect
		strTemp.Format(_T("%d"), iTotalLensDefect);
		strSave += strTemp + strDelimeter;

		// Total BarrelLens Defect
		strTemp.Format(_T("%d"), iTotalBarrelLensDefect);
		strSave += strTemp + strDelimeter;

		// Total Barrel Dirt Defect
		strTemp.Format(_T("%d"), iTotalBarrelDirt);
		strSave += strTemp + strDelimeter;

		// Total Barrel Scratch Defect
		strTemp.Format(_T("%d"), iTotalBarrelScratch);
		strSave += strTemp + strDelimeter;

		// Total Barrel Stain Defect
		strTemp.Format(_T("%d"), iTotalBarrelStain);
		strSave += strTemp + strDelimeter;

		// Total Barrel Chipping Defect
		strTemp.Format(_T("%d"), iTotalBarrelChipping);
		strSave += strTemp + strDelimeter;

		// Total Barrel Epoxy Defect
		strTemp.Format(_T("%d"), iTotalBarrelChipping);
		strSave += strTemp + strDelimeter;

		// Total Barrel OutsideCT Defect
		strTemp.Format(_T("%d"), iTotalBarrelOutsideCT);
		strSave += strTemp + strDelimeter;

		// Total Barrel PeelOff Defect
		strTemp.Format(_T("%d"), iTotalBarrelPeelOff);
		strSave += strTemp + strDelimeter;

		// Total Barrel WhiteDot Defect
		strTemp.Format(_T("%d"), iTotalBarrelWhiteDot);
		strSave += strTemp + strDelimeter;

		// Total Lens Contamination Defect
		strTemp.Format(_T("%d"), iTotalLensContamination);
		strSave += strTemp + strDelimeter;

		// Total Lens Scratch Defect
		strTemp.Format(_T("%d"), iTotalLensScratch);
		strSave += strTemp + strDelimeter;

		// Total Lens Dirt Defect
		strTemp.Format(_T("%d"), iTotalLensDirt);
		strSave += strTemp + strDelimeter;

		// Total Lens WhiteDot Defect
		strTemp.Format(_T("%d"), iTotalLensWhiteDot);
		strSave += strTemp + strDelimeter;

		// Total Lens WhiteDot Defect
		strTemp.Format(_T("%d"), iTotalLensWhiteDot);
		strSave += strTemp + strDelimeter;

		// Total Epoxy Hole Defect
		strTemp.Format(_T("%d"), iTotalEpoxyHole);
		strSave += strTemp + strDelimeter;

		// Total Spring NG Defect
		strTemp.Format(_T("%d"), iTotalSpringNG);
		strSave += strTemp + strDelimeter;

		// Total FAI NG Defect
		strTemp.Format(_T("%d"), iTotalFAING);
		strSave += strTemp + strDelimeter;

		// Total BlackCoating NG Defect
		strTemp.Format(_T("%d"), iTotalBlackCoatingNG);
		strSave += strTemp + strDelimeter;

		// Total ModuleMix Defect
		strTemp.Format(_T("%d"), iTotalModuleMix);
		strSave += strTemp + strDelimeter;

		// Total BarcodeError Defect
		strTemp.Format(_T("%d"), iTotalBarcodeError);
		strSave += strTemp + strDelimeter;

		// Total BarcodeShift Defect
		strTemp.Format(_T("%d"), iTotalBarcodeShift);
		strSave += strTemp + strDelimeter;

		// Total MatchingError Defect
		strTemp.Format(_T("%d"), iTotalMatchingError);
		strSave += strTemp + strDelimeter;

		// Total LightError Defect
		strTemp.Format(_T("%d"), iTotalLightError);
		strSave += strTemp + strDelimeter;

		strSave += "\r\n";

		file.Write(strSave, strSave.GetLength());
		file.Close();
	}
	catch (CFileException *pEx)
	{
		pEx->Delete();
	}

	LeaveCriticalSection(&CS_LOT_SUMMARY_LOG);
}

void CuScanApp::SaveLotResultLog(BOOL bSaveLas, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode, double dInspectTime, CString strResult,
								 CString strBarrelOKNG, CString strLensOKNG, CString sBLInspType, CString sBLDefectType, double dBLDefectArea, int iBLCenterX, int iBLCenterY,
								 CString strExtraResult, CString sExInspType, CString sExDefectType, double dExDefectArea, int iExCenterX, int iExCenterY,
								 CString strFAIResult, CString sBarcodeShiftResult, CString sBarcodeRotationResult, double dBarcodePosOffsetX, double dBarcodePosOffsetY, double dBarcodePosOffsetRotation,
								 CString sBlackCoatingResult, double dBlackCoatingDiameter, double dBlackCoatingPosX, double dBlackCoatingPosY, double dO1O2Distance, double dDiameterMin,
								 CString sModuleMixResult, CString sBarcodeErrorResult, CString sMatchingErrorResult, CString sLightErrorResult, int iDefectCode)
{

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strPath1, strPath2;
	CString strDelimeter, strHeader, strSave, strTemp;

	//////////
	strDelimeter = _T(",");

	for (long n = 0; n < LOT_RESULT_HEADER_END; n++)
	{
		strHeader += g_sLotResultLogHeader[LOT_RESULT_HEADER_DATE + n] + strDelimeter;
	}

	// Data
	strTemp.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
	strSave += strTemp + strDelimeter;

	// Time
	strTemp.Format("\'%04d-%02d-%02d %02d:%02d:%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	strSave += strTemp + strDelimeter;

	// Station
	strSave += GetPCID() + strDelimeter;

	// Machine No
	strSave += Struct_PreferenceStruct.m_strEquipNo + strDelimeter;

	// Login Mode
	strSave += m_sLoginMode + strDelimeter;

	// Operator
	strSave += m_sOperator + strDelimeter;

	// SW Version
	strTemp.Format(_T("%d"), m_iAppVersion);
	strSave += strTemp + strDelimeter;

	// Lot ID
	strSave += strLotID + strDelimeter;

	// Model
	strSave += m_pModelDataManager->m_sModelName + strDelimeter;

	// Model Version
	strSave += _T("1000") + strDelimeter;

	// Tray
	strTemp.Format(_T("%d"), iTrayNo);
	strSave += strTemp + strDelimeter;

	// Barcode
	strSave += sBarcode + strDelimeter;

	// Module
	strTemp.Format(_T("%d"), iModuleNo);
	strSave += strTemp + strDelimeter;

	// Cycle Time
	strTemp.Format(_T("%.3f"), dInspectTime);
	strSave += strTemp + strDelimeter;

	// Final Result
	strSave += strResult + strDelimeter;

	// Barrel Result
	strSave += strBarrelOKNG + strDelimeter;

	// Lens Result
	strSave += strLensOKNG + strDelimeter;

	// BL Insp Type Result
	strSave += sBLInspType + strDelimeter;

	// BL Defect Type Result
	strSave += sBLDefectType + strDelimeter;

	// BL Defect Area
	strTemp.Format(_T("%.3f"), dBLDefectArea);
	strSave += strTemp + strDelimeter;

	// BL Defect Center X
	strTemp.Format(_T("%d"), iBLCenterX);
	strSave += strTemp + strDelimeter;

	// BL Defect Center Y
	strTemp.Format(_T("%d"), iBLCenterY);
	strSave += strTemp + strDelimeter;

	// Extra Result
	strSave += strExtraResult + strDelimeter;

	// BL Insp Type Result
	strSave += sExInspType + strDelimeter;

	// BL Defect Type Result
	strSave += sExDefectType + strDelimeter;

	// Extra Defect Area
	strTemp.Format(_T("%.3f"), dExDefectArea);
	strSave += strTemp + strDelimeter;

	// Extra Defect  Center X
	strTemp.Format(_T("%d"), iExCenterX);
	strSave += strTemp + strDelimeter;

	// Extra Defect  Center Y
	strTemp.Format(_T("%d"), iExCenterY);
	strSave += strTemp + strDelimeter;

	// FAI Result
	strSave += strFAIResult + strDelimeter;

	// Extra Result Barcdoe Shift
	strSave += sBarcodeShiftResult + strDelimeter;

	// Extra Result Barcdoe Shift
	strSave += sBarcodeRotationResult + strDelimeter;

	// Barcode Offset X
	strTemp.Format(_T("%.3f"), dBarcodePosOffsetX);
	strSave += strTemp + strDelimeter;

	// Barcode Offset Y
	strTemp.Format(_T("%.3f"), dBarcodePosOffsetY);
	strSave += strTemp + strDelimeter;

	// Barcode Offset R
	strTemp.Format(_T("%.3f"), dBarcodePosOffsetRotation);
	strSave += strTemp + strDelimeter;

	// BlackCoating Result
	strSave += sBlackCoatingResult + strDelimeter;

	// BlackCoating Diameter
	strTemp.Format(_T("%.3f"), dBlackCoatingDiameter);
	strSave += strTemp + strDelimeter;

	// BlackCoating Pos X
	strTemp.Format(_T("%.3f"), dBlackCoatingPosX);
	strSave += strTemp + strDelimeter;

	// BlackCoating Pos Y
	strTemp.Format(_T("%.3f"), dBlackCoatingPosY);
	strSave += strTemp + strDelimeter;

	// O1O2Distance
	strTemp.Format(_T("%.3f"), dO1O2Distance);
	strSave += strTemp + strDelimeter;

	// Diameter Min
	strTemp.Format(_T("%.3f"), dDiameterMin);
	strSave += strTemp + strDelimeter;

	// Module Mix Result
	strSave += sModuleMixResult + strDelimeter;

	// Barcode Error Result
	strSave += sBarcodeErrorResult + strDelimeter;

	// Matching Error Result
	strSave += sMatchingErrorResult + strDelimeter;

	// Light Error Result
	strSave += sLightErrorResult + strDelimeter;

	// Defect Code
	strTemp.Format(_T("%d"), iDefectCode);
	strSave += strTemp + strDelimeter;

	//////////
	strPath1.Format(m_FileBase.m_strResultFolderPrev + "\\%s\\LotResult_%s_%04d%02d%02d.csv", strLotID, strLotID, time.wYear, time.wMonth, time.wDay);
	strPath2.Format(m_FileBase.m_strResultFolderPrev + "\\DayLotResult_%04d%02d%02d.csv", time.wYear, time.wMonth, time.wDay);

	m_pSaveManager->SaveResult(strHeader, strSave, strPath1);
	m_pSaveManager->SaveResult(strHeader, strSave, strPath2);

	if (bSaveLas)
	{
		strPath1.Format("%s\\%s_%04d%02d%02d%02d_Inspection_Log.csv", m_szLASDataFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
		strPath2.Format("%s\\%s_%04d%02d%02d%02d_LOT_TIME.csv", m_szLASDataFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);

		m_pSaveManager->SaveResult(strHeader, strSave, strPath1);
		m_pSaveManager->SaveResult(strHeader, strSave, strPath2);
	}
}

// 2024.09.24 - v2660 - Defect shape feature Log 추가 - LeeGW
void CuScanApp::SaveDefectFeatureLog(BOOL bSaveLas, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode, int iImageIdx, int iTabIdx, int iDefectCode, Hobject HDefectImg, Hobject HDefectRgn)
{
	if (Struct_PreferenceStruct.m_bSaveDefectFeatureLog == FALSE)
		return;

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strFile, strHeader, strSave, strTemp, strDelimeter;
	strDelimeter = _T(",");

	for (long n = 0; n < DSF_HEADER_END; n++)
	{
		strHeader += g_sDefectFeatureLogHeader[DSF_HEADER_DATE + n] + strDelimeter;
	}
	// moments_m11,moments_m20,moments_m02,moments_ia,moments_ib,moments_m11_invar,moments_m20_invar,moments_m02_invar,moments_phi1,moments_phi2,moments_m21,moments_m12,moments_m03,moments_m30,moments_m21_invar,moments_m12_invar,moments_m03_invar,moments_m30_invar,moments_i1,moments_i2,moments_i3,moments_i4,moments_psi1,moments_psi2,moments_psi3,moments_psi4

	HTuple HParam1, HParam2, HParam3, HParam4, HParam5;

	int center_x, center_y;
	int area, width, height, row1, column1, row2, column2, inner_width, inner_height, connect_num, holes_num, d_area_holes, d_euler_number;
	double row, column, dCircularity, dCompactness, dContlength, dConvexity, dRectangularity, dRa, dRb, dPhi, dAnisometry, dBulkiness, struct_factor, outer_radius, inner_radius, width_height_ratio;
	double dist_mean, dist_deviation, dRoundness, num_sides, max_diameter, dOrientation;
	double rect2_phi, rect2_len1, rect2_len2, moments_m11, moments_m20, moments_m02, moments_ia, moments_ib, moments_m11_invar, moments_m20_invar, moments_m02_invar;
	double moments_phi1, moments_phi2, moments_m21, moments_m12, moments_m03, moments_m30, moments_m21_invar, moments_m12_invar, moments_m03_invar, moments_m30_invar;
	double moments_i1, moments_i2, moments_i3, moments_i4, moments_psi1, moments_psi2, moments_psi3, moments_psi4;
	int gv_min, gv_max, gv_median;
	double gv_mean, gv_dev;

	Hobject SelectRgn;
	HTuple OriSize, OriArea, Row, Column;
	HTuple CountDefect;
	Hobject ConnectionsRegion;

	connection(HDefectRgn, &ConnectionsRegion);
	count_obj(ConnectionsRegion, &CountDefect); // Total Defect Blobs

	CString sShapeFeatureValue;
	sShapeFeatureValue = _T("");

	for (int i = 0; i < CountDefect[0].I(); i++)
	{

		select_obj(ConnectionsRegion, &SelectRgn, i + 1);

		if (m_pGFunction->ValidHRegion(SelectRgn) == FALSE)
			continue;

		min_max_gray(SelectRgn, HDefectImg, 1, &HParam1, &HParam2, &HParam3);
		gv_min = HParam1[0].D();
		gv_max = HParam2[0].D();

		min_max_gray(SelectRgn, HDefectImg, 50, &HParam1, &HParam2, &HParam3);
		gv_median = HParam1[0].D();

		intensity(SelectRgn, HDefectImg, &HParam1, &HParam2);
		gv_mean = HParam1[0].D();
		gv_dev = HParam2[0].D();

		area_center(SelectRgn, &HParam1, &HParam2, &HParam3);
		area = HParam1[0].L();
		row = HParam2[0].D();
		column = HParam3[0].D();

		smallest_rectangle1(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
		row1 = HParam1[0].L();
		column1 = HParam2[0].L();
		row2 = HParam3[0].L();
		column2 = HParam4[0].L();
		width = column2 - column1 + 1;
		height = row2 - row1 + 1;
		center_x = row1 + abs(row1 - row2) * 0.5;
		center_y = column1 + abs(column1 - column2) * 0.5;

		circularity(SelectRgn, &HParam1);
		dCircularity = HParam1[0].D();

		compactness(SelectRgn, &HParam1);
		dCompactness = HParam1[0].D();

		contlength(SelectRgn, &HParam1);
		dContlength = HParam1[0].D();

		convexity(SelectRgn, &HParam1);
		dConvexity = HParam1[0].D();

		rectangularity(SelectRgn, &HParam1);
		dRectangularity = HParam1[0].D();

		elliptic_axis(SelectRgn, &HParam1, &HParam2, &HParam3);
		dRa = HParam1[0].D();
		dRb = HParam2[0].D();
		dPhi = HParam3[0].D();

		eccentricity(SelectRgn, &HParam1, &HParam2, &HParam3);
		dAnisometry = HParam1[0].D();
		dBulkiness = HParam2[0].D();
		struct_factor = HParam3[0].D();

		smallest_circle(SelectRgn, &HParam1, &HParam2, &HParam3);
		outer_radius = HParam3[0].D();

		inner_circle(SelectRgn, &HParam1, &HParam2, &HParam3);
		inner_radius = HParam3[0].D();

		width_height_ratio = inner_radius / outer_radius;

		inner_rectangle1(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
		inner_width = HParam4[0].L() - HParam2[0].L() + 1;
		inner_height = HParam3[0].L() - HParam1[0].L() + 1;

		roundness(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
		dist_mean = HParam1[0].D();
		dist_deviation = HParam2[0].D();
		dRoundness = HParam3[0].D();
		num_sides = HParam4[0].D();

		connect_and_holes(SelectRgn, &HParam1, &HParam2);
		connect_num = HParam1[0].L();
		holes_num = HParam2[0].L();

		area_holes(SelectRgn, &HParam1);
		d_area_holes = HParam1[0].L();

		diameter_region(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
		max_diameter = HParam5[0].D();

		orientation_region(SelectRgn, &HParam1);
		dOrientation = HParam1[0].D();

		euler_number(SelectRgn, &HParam1);
		d_euler_number = HParam1[0].L();

		smallest_rectangle2(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
		rect2_phi = HParam3[0].D();
		rect2_len1 = HParam4[0].D();
		rect2_len2 = HParam5[0].D();

		moments_region_2nd(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
		moments_m11 = HParam1[0].D();
		moments_m20 = HParam2[0].D();
		moments_m02 = HParam3[0].D();
		moments_ia = HParam4[0].D();
		moments_ib = HParam5[0].D();

		moments_region_2nd_invar(SelectRgn, &HParam1, &HParam2, &HParam3);
		moments_m11_invar = HParam1[0].D();
		moments_m20_invar = HParam2[0].D();
		moments_m02_invar = HParam3[0].D();

		moments_region_2nd_rel_invar(SelectRgn, &HParam1, &HParam2);
		moments_phi1 = HParam1[0].D();
		moments_phi2 = HParam2[0].D();

		moments_region_3rd(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
		moments_m21 = HParam1[0].D();
		moments_m12 = HParam2[0].D();
		moments_m03 = HParam3[0].D();
		moments_m30 = HParam4[0].D();

		moments_region_3rd_invar(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
		moments_m21_invar = HParam1[0].D();
		moments_m12_invar = HParam2[0].D();
		moments_m03_invar = HParam3[0].D();
		moments_m30_invar = HParam4[0].D();

		moments_region_central(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
		moments_i1 = HParam1[0].D();
		moments_i2 = HParam2[0].D();
		moments_i3 = HParam3[0].D();
		moments_i4 = HParam4[0].D();

		moments_region_central_invar(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
		moments_psi1 = HParam1[0].D();
		moments_psi2 = HParam2[0].D();
		moments_psi3 = HParam3[0].D();
		moments_psi4 = HParam4[0].D();

		// Data
		strTemp.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
		strSave = strTemp + strDelimeter;

		// Time
		strTemp.Format("\'%04d-%02d-%02d %02d:%02d:%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		strSave += strTemp + strDelimeter;

		// Station
		strSave += GetPCID() + strDelimeter;

		// Machine No
		strSave += Struct_PreferenceStruct.m_strEquipNo + strDelimeter;

		// Lot ID
		strSave += strLotID + strDelimeter;

		// Model
		strSave += m_pModelDataManager->m_sModelName + strDelimeter;

		// Tray
		strTemp.Format(_T("%d"), iTrayNo);
		strSave += strTemp + strDelimeter;

		// Barcode
		strSave += sBarcode + strDelimeter;

		// Module == ModuleNo
		strTemp.Format(_T("%d"), iModuleNo);
		strSave += strTemp + strDelimeter;

		// Vision
		strTemp.Format(_T("T1"));
		strSave += strTemp + strDelimeter;

		// Auto Param. Reorder - 250912, jhkim ////////////////////////////////////////
		// Position == NG Area
		strSave += g_sDefectPosName[iDefectCode] + strDelimeter;

		// Image Type
		strSave += g_sImageName[m_iMachineInspType][iImageIdx] + strDelimeter;

		// NG Name == NG Defect Name
		strSave += g_sDefectName[iDefectCode] + strDelimeter;

		// (Auto Param.) Image No 추가 - 250912, jhkim
		strTemp.Format(_T("%d"), iImageIdx);
		strSave += strTemp + strDelimeter;

		// Insp Tab No == Inspection Tab No
		strTemp.Format(_T("%d"), iTabIdx);
		strSave += strTemp + strDelimeter;
		//////////////////////////////////////////////////////////////////////////////

		// Gray Value == gv min/max/median/mean/dev
		strTemp.Format(_T("%d"), gv_min);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), gv_max);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), gv_median);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), gv_mean);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), gv_dev);
		strSave += strTemp + strDelimeter;

		// Shape == area/row/column/legth_x ...
		strTemp.Format(_T("%d"), area);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), row);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), column);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), width); // X길이
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), height); // Y길이
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), row1);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), column1);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), row2);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), column2);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), center_x);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), center_y);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dCircularity);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dCompactness);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dContlength);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dConvexity);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dRectangularity);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dRa);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dRb);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dPhi);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dAnisometry);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dBulkiness);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), struct_factor);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), inner_radius); // 너비
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), outer_radius); // 높이
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), width_height_ratio); // 폭/길이 비율
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), inner_width);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), inner_height);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dist_mean);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dist_deviation);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dRoundness);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), num_sides);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), connect_num);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), holes_num);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), d_area_holes);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), max_diameter);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), dOrientation);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%d"), d_euler_number);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), rect2_phi);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), rect2_len1);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), rect2_len2);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m11);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m20);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m02);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_ia);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_ib);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m11_invar);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m20_invar);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m02_invar);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_phi1);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_phi2);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m21);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m12);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m03);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m30);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m21_invar);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m12_invar);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m03_invar);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_m30_invar);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_i1);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_i2);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_i3);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_i4);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_psi1);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_psi2);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_psi3);
		strSave += strTemp + strDelimeter;

		strTemp.Format(_T("%.3lf"), moments_psi4);
		strSave += strTemp + strDelimeter;

		strFile.Format("%s\\%s\\DefectFeature_%s_%04d%02d%02d.csv", m_FileBase.m_strResultFolderPrev, strLotID, strLotID, time.wYear, time.wMonth, time.wDay);
		m_pSaveManager->SaveResult(strHeader, strSave, strFile);

		if (bSaveLas)
		{

			strFile.Format("%s\\%s_%04d%02d%02d%02d_Defect_Shape_Feature.csv", m_szLASDataFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
			m_pSaveManager->SaveResult(strHeader, strSave, strFile);

			// 아래 저장 후 나중에 EVENT 폴더에 옮겨짐 (SaveManager.cpp의 SaveThread 참조)
			strFile.Format("%s\\%s_%04d%02d%02d%02d_Defect_Shape_Feature.csv", m_szLASTempFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
			m_pSaveManager->SaveResult(strHeader, strSave, strFile);

			// strFile.Format("%s\\%s_%04d%02d%02d%02d_AVI_MFG_DEFECT_AVIMONITOR_RESULT.csv", m_szLASDataFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
			// m_pSaveManager->SaveResult(strHeader, strSave, strFile);

			// strFile.Format("%s\\%s_%04d%02d%02d%02d_AVI_MFG_DEFECT_AVIMONITOR_RESULT.csv", m_szLASSectionFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
			// m_pSaveManager->SaveResult(strHeader, strSave, strFile);
		}
	}

	HDefectRgn.Reset();
	ConnectionsRegion.Reset();
}

void CuScanApp::SaveDefectFeatureLog(BOOL bSaveLas, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode, CString sDefectCode, Hobject HDefectImg, Hobject HDefectRgn)
{
	if (Struct_PreferenceStruct.m_bSaveDefectFeatureLog == FALSE)
		return;

	if (m_pGFunction->ValidHRegion(HDefectRgn) == FALSE)
		return;

	if (m_pGFunction->ValidHImage(HDefectImg) == FALSE)
		return;

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strFile, strHeader, strSave, strTemp, strDelimeter;
	strDelimeter = _T(",");

	for (long n = 0; n < DSF_HEADER_END; n++)
	{
		strHeader += g_sDefectFeatureLogHeader[DSF_HEADER_DATE + n] + strDelimeter;
	}
	// moments_m11,moments_m20,moments_m02,moments_ia,moments_ib,moments_m11_invar,moments_m20_invar,moments_m02_invar,moments_phi1,moments_phi2,moments_m21,moments_m12,moments_m03,moments_m30,moments_m21_invar,moments_m12_invar,moments_m03_invar,moments_m30_invar,moments_i1,moments_i2,moments_i3,moments_i4,moments_psi1,moments_psi2,moments_psi3,moments_psi4

	HTuple HParam1, HParam2, HParam3, HParam4, HParam5;

	int center_x, center_y;
	int area, width, height, row1, column1, row2, column2, inner_width, inner_height, connect_num, holes_num, d_area_holes, d_euler_number;
	double row, column, dCircularity, dCompactness, dContlength, dConvexity, dRectangularity, dRa, dRb, dPhi, dAnisometry, dBulkiness, struct_factor, outer_radius, inner_radius, width_height_ratio;
	double dist_mean, dist_deviation, dRoundness, num_sides, max_diameter, dOrientation;
	double rect2_phi, rect2_len1, rect2_len2, moments_m11, moments_m20, moments_m02, moments_ia, moments_ib, moments_m11_invar, moments_m20_invar, moments_m02_invar;
	double moments_phi1, moments_phi2, moments_m21, moments_m12, moments_m03, moments_m30, moments_m21_invar, moments_m12_invar, moments_m03_invar, moments_m30_invar;
	double moments_i1, moments_i2, moments_i3, moments_i4, moments_psi1, moments_psi2, moments_psi3, moments_psi4;
	int gv_min, gv_max, gv_median;
	double gv_mean, gv_dev;

	Hobject SelectRgn;
	HTuple OriSize, OriArea, Row, Column;
	HTuple CountDefect;
	Hobject ConnectionsRegion;

	try
	{
		connection(HDefectRgn, &ConnectionsRegion);
		count_obj(ConnectionsRegion, &CountDefect); // Total Defect Blobs

		CString sShapeFeatureValue;
		sShapeFeatureValue = _T("");

		for (int i = 0; i < CountDefect[0].I(); i++)
		{

			select_obj(ConnectionsRegion, &SelectRgn, i + 1);

			if (m_pGFunction->ValidHRegion(SelectRgn) == FALSE)
				continue;

			min_max_gray(SelectRgn, HDefectImg, 1, &HParam1, &HParam2, &HParam3);
			gv_min = HParam1[0].D();
			gv_max = HParam2[0].D();

			min_max_gray(SelectRgn, HDefectImg, 50, &HParam1, &HParam2, &HParam3);
			gv_median = HParam1[0].D();

			intensity(SelectRgn, HDefectImg, &HParam1, &HParam2);
			gv_mean = HParam1[0].D();
			gv_dev = HParam2[0].D();

			area_center(SelectRgn, &HParam1, &HParam2, &HParam3);
			area = HParam1[0].L();
			row = HParam2[0].D();
			column = HParam3[0].D();

			smallest_rectangle1(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
			row1 = HParam1[0].L();
			column1 = HParam2[0].L();
			row2 = HParam3[0].L();
			column2 = HParam4[0].L();
			width = column2 - column1 + 1;
			height = row2 - row1 + 1;
			center_x = row1 + abs(row1 - row2) * 0.5;
			center_y = column1 + abs(column1 - column2) * 0.5;

			circularity(SelectRgn, &HParam1);
			dCircularity = HParam1[0].D();

			compactness(SelectRgn, &HParam1);
			dCompactness = HParam1[0].D();

			contlength(SelectRgn, &HParam1);
			dContlength = HParam1[0].D();

			convexity(SelectRgn, &HParam1);
			dConvexity = HParam1[0].D();

			rectangularity(SelectRgn, &HParam1);
			dRectangularity = HParam1[0].D();

			elliptic_axis(SelectRgn, &HParam1, &HParam2, &HParam3);
			dRa = HParam1[0].D();
			dRb = HParam2[0].D();
			dPhi = HParam3[0].D();

			eccentricity(SelectRgn, &HParam1, &HParam2, &HParam3);
			dAnisometry = HParam1[0].D();
			dBulkiness = HParam2[0].D();
			struct_factor = HParam3[0].D();

			smallest_circle(SelectRgn, &HParam1, &HParam2, &HParam3);
			outer_radius = HParam3[0].D();

			inner_circle(SelectRgn, &HParam1, &HParam2, &HParam3);
			inner_radius = HParam3[0].D();

			width_height_ratio = inner_radius / outer_radius;

			inner_rectangle1(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
			inner_width = HParam4[0].L() - HParam2[0].L() + 1;
			inner_height = HParam3[0].L() - HParam1[0].L() + 1;

			roundness(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
			dist_mean = HParam1[0].D();
			dist_deviation = HParam2[0].D();
			dRoundness = HParam3[0].D();
			num_sides = HParam4[0].D();

			connect_and_holes(SelectRgn, &HParam1, &HParam2);
			connect_num = HParam1[0].L();
			holes_num = HParam2[0].L();

			area_holes(SelectRgn, &HParam1);
			d_area_holes = HParam1[0].L();

			diameter_region(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			max_diameter = HParam5[0].D();

			orientation_region(SelectRgn, &HParam1);
			dOrientation = HParam1[0].D();

			euler_number(SelectRgn, &HParam1);
			d_euler_number = HParam1[0].L();

			smallest_rectangle2(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			rect2_phi = HParam3[0].D();
			rect2_len1 = HParam4[0].D();
			rect2_len2 = HParam5[0].D();

			moments_region_2nd(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4, &HParam5);
			moments_m11 = HParam1[0].D();
			moments_m20 = HParam2[0].D();
			moments_m02 = HParam3[0].D();
			moments_ia = HParam4[0].D();
			moments_ib = HParam5[0].D();

			moments_region_2nd_invar(SelectRgn, &HParam1, &HParam2, &HParam3);
			moments_m11_invar = HParam1[0].D();
			moments_m20_invar = HParam2[0].D();
			moments_m02_invar = HParam3[0].D();

			moments_region_2nd_rel_invar(SelectRgn, &HParam1, &HParam2);
			moments_phi1 = HParam1[0].D();
			moments_phi2 = HParam2[0].D();

			moments_region_3rd(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
			moments_m21 = HParam1[0].D();
			moments_m12 = HParam2[0].D();
			moments_m03 = HParam3[0].D();
			moments_m30 = HParam4[0].D();

			moments_region_3rd_invar(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
			moments_m21_invar = HParam1[0].D();
			moments_m12_invar = HParam2[0].D();
			moments_m03_invar = HParam3[0].D();
			moments_m30_invar = HParam4[0].D();

			moments_region_central(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
			moments_i1 = HParam1[0].D();
			moments_i2 = HParam2[0].D();
			moments_i3 = HParam3[0].D();
			moments_i4 = HParam4[0].D();

			moments_region_central_invar(SelectRgn, &HParam1, &HParam2, &HParam3, &HParam4);
			moments_psi1 = HParam1[0].D();
			moments_psi2 = HParam2[0].D();
			moments_psi3 = HParam3[0].D();
			moments_psi4 = HParam4[0].D();

			// Data
			strTemp.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
			strSave = strTemp + strDelimeter;

			// Time
			strTemp.Format("\'%04d-%02d-%02d %02d:%02d:%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
			strSave += strTemp + strDelimeter;

			// Station
			strSave += GetPCID() + strDelimeter;

			// Machine No
			strSave += Struct_PreferenceStruct.m_strEquipNo + strDelimeter;

			// Lot ID
			strSave += strLotID + strDelimeter;

			// Model
			strSave += m_pModelDataManager->m_sModelName + strDelimeter;

			// Tray
			strTemp.Format(_T("%d"), iTrayNo);
			strSave += strTemp + strDelimeter;

			// Barcode
			strSave += sBarcode + strDelimeter;

			// Module
			strTemp.Format(_T("%d"), iModuleNo);
			strSave += strTemp + strDelimeter;

			// Vision
			strTemp.Format(_T("T1"));
			strSave += strTemp + strDelimeter;

			// Defect
			strSave += sDefectCode + strDelimeter;

			// Gray Value
			strTemp.Format(_T("%d"), gv_min);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), gv_max);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), gv_median);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), gv_mean);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), gv_dev);
			strSave += strTemp + strDelimeter;

			// Shape
			strTemp.Format(_T("%d"), area);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), row);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), column);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), width); // X길이
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), height); // Y길이
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), row1);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), column1);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), row2);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), column2);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), center_x);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), center_y);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dCircularity);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dCompactness);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dContlength);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dConvexity);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dRectangularity);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dRa);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dRb);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dPhi);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dAnisometry);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dBulkiness);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), struct_factor);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), inner_radius); // 너비
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), outer_radius); // 높이
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), width_height_ratio); // 폭/길이 비율
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), inner_width);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), inner_height);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dist_mean);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dist_deviation);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dRoundness);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), num_sides);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), connect_num);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), holes_num);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), d_area_holes);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), max_diameter);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), dOrientation);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%d"), d_euler_number);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), rect2_phi);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), rect2_len1);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), rect2_len2);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m11);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m20);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m02);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_ia);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_ib);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m11_invar);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m20_invar);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m02_invar);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_phi1);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_phi2);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m21);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m12);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m03);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m30);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m21_invar);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m12_invar);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m03_invar);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_m30_invar);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_i1);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_i2);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_i3);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_i4);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_psi1);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_psi2);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_psi3);
			strSave += strTemp + strDelimeter;

			strTemp.Format(_T("%.3lf"), moments_psi4);
			strSave += strTemp + strDelimeter;

			strFile.Format("%s\\%s\\DefectFeature_%s_%04d%02d%02d.csv", m_FileBase.m_strResultFolderPrev, strLotID, strLotID, time.wYear, time.wMonth, time.wDay);
			m_pSaveManager->SaveResult(strHeader, strSave, strFile);

			if (bSaveLas)
			{

				strFile.Format("%s\\%s_%04d%02d%02d%02d_Defect_Shape_Feature.csv", m_szLASDataFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
				m_pSaveManager->SaveResult(strHeader, strSave, strFile);

				// 아래 저장 후 나중에 EVENT 폴더에 옮겨짐 (SaveManager.cpp의 SaveThread 참조)
				strFile.Format("%s\\%s_%04d%02d%02d%02d_Defect_Shape_Feature.csv", m_szLASTempFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
				m_pSaveManager->SaveResult(strHeader, strSave, strFile);

				// strFile.Format("%s\\%s_%04d%02d%02d%02d_AVI_MFG_DEFECT_AVIMONITOR_RESULT.csv", m_szLASDataFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
				// m_pSaveManager->SaveResult(strHeader, strSave, strFile);

				// strFile.Format("%s\\%s_%04d%02d%02d%02d_AVI_MFG_DEFECT_AVIMONITOR_RESULT.csv", m_szLASSectionFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
				// m_pSaveManager->SaveResult(strHeader, strSave, strFile);
			}
		}

		HDefectRgn.Reset();
		ConnectionsRegion.Reset();
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CInspectAdminViewHideDlg WriteResultFileExtra] : %s", except.message);
		THEAPP.SaveLog(str);

		HDefectRgn.Reset();
		ConnectionsRegion.Reset();
	}
}
// 2024.09.24 - v2660 - Defect shape feature Log 추가 - LeeGW

// 2025.03.27 - v3011 - Defect Param Log 추가 - LeeGW
void CuScanApp::SaveDefectParamLog(BOOL bSaveLas, BOOL bBarrelLens, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode, int iImageIdx, int iTabIdx, CAlgorithmParam AlgorithmParam)
{
	if (Struct_PreferenceStruct.m_bSaveDetectParamLog == FALSE)
		return;

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strPath, strHeader, strSave, strTemp, strDelimeter;
	strDelimeter = _T(",");

	for (long n = 0; n < PARAM_HEADER_END; n++)
	{
		strHeader += g_sDetectParamLogHeader[PARAM_HEADER_TIME + n] + strDelimeter;
	}

	// Time
	strTemp.Format("\'%04d-%02d-%02d %02d:%02d:%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	strSave += strTemp + strDelimeter;

	// Station
	strSave += GetPCID() + strDelimeter;

	// Machine No
	strSave += Struct_PreferenceStruct.m_strEquipNo + strDelimeter;

	// Login Mode
	strSave += m_sLoginMode + strDelimeter;

	// Operator
	strSave += m_sOperator + strDelimeter;

	// Recipe
	strSave += m_pModelDataManager->m_sModelName + strDelimeter;

	// SW Version
	strTemp.Format(_T("%d"), m_iAppVersion);
	strSave += strTemp + strDelimeter;

	// Lot ID
	strSave += strLotID + strDelimeter;

	// Tray
	strTemp.Format(_T("%d"), iTrayNo);
	strSave += strTemp + strDelimeter;

	// Module
	strTemp.Format(_T("%d"), iModuleNo);
	strSave += strTemp + strDelimeter;

	// Sensor ID
	// MEMO : What for? 250912, jhkim
	strTemp.Format(_T(" "));
	strSave += strTemp + strDelimeter;

	// Barcode
	strSave += sBarcode + strDelimeter;

	// Auto Param. Reorr - 250912, jhkim ////////////////////////////////////////
	// Position == NG Area
	strSave += g_sDefectPosName[AlgorithmParam.m_iDefectNameIdx] + strDelimeter;

	// Image Type
	strSave += g_sImageName[m_iMachineInspType][iImageIdx] + strDelimeter;

	// NG Name == NG Defect Name
	strSave += g_sDefectName[AlgorithmParam.m_iDefectNameIdx] + strDelimeter;

	// (Auto Param.) Image No 추가 - 250912, jhkim
	strTemp.Format(_T("%d"), iImageIdx);
	strSave += strTemp + strDelimeter;

	// Insp Tab No == Inspection Tab No
	strTemp.Format(_T("%d"), iTabIdx);
	strSave += strTemp + strDelimeter;
	//////////////////////////////////////////////////////////////////////////////

	// Z Axis
	strTemp.Format(_T("%.2lf"), m_pModelDataManager->m_mdInspPosZ[iImageIdx]);
	strSave += strTemp + strDelimeter;

	// Light Dome Page
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iLightPageNo[iImageIdx][0]);
	strSave += strTemp + strDelimeter;

	// Light Ring Page
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iLightPageNo[iImageIdx][1]);
	strSave += strTemp + strDelimeter;

	// Light Dome
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iInspLightBright[iImageIdx][0]);
	strSave += strTemp + strDelimeter;

	// Light Ring
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iInspLightBright[iImageIdx][1]);
	strSave += strTemp + strDelimeter;

	// Light 55
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iInspLightBright[iImageIdx][2]);
	strSave += strTemp + strDelimeter;

	// Light 65
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iInspLightBright[iImageIdx][3]);
	strSave += strTemp + strDelimeter;

	// Light 75
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iInspLightBright[iImageIdx][4]);
	strSave += strTemp + strDelimeter;

	// B/L ROI Refer Center X
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iRefContCol);
	strSave += strTemp + strDelimeter;

	// B/L ROI Refer Center Y
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iRefContRow);
	strSave += strTemp + strDelimeter;

	// Barrel ROI Refer Radius
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iRefContRad3);
	strSave += strTemp + strDelimeter;

	// Lens ROI Refer Radius
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iRefContRad4);
	strSave += strTemp + strDelimeter;

	// Matching Scale Use
	strSave += (m_pModelDataManager->m_bUseMatchingScale ? "TRUE" : "FALSE") + strDelimeter;

	// Matching Scale Min
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iMatchingScaleMin);
	strSave += strTemp + strDelimeter;

	// Matching Scale Max
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iMatchingScaleMax);
	strSave += strTemp + strDelimeter;

	// B/L Matching Image No
	strSave += g_sImageName[m_iMachineInspType][m_pModelDataManager->m_iBlackMatchingImageType] + strDelimeter;

	// Barrel Matching Contrast
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iMatchingContrastC3);
	strSave += strTemp + strDelimeter;

	// Lens Matching Contrast
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iMatchingContrastC4);
	strSave += strTemp + strDelimeter;

	// Inner Barrel Magin_Outer
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iDecisonTopBarrelInner_RoiOuterMargin);
	strSave += strTemp + strDelimeter;

	// Inner Barrel Magin_Inner
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iDecisonTopBarrelInner_RoiInnerMargin);
	strSave += strTemp + strDelimeter;

	// Barcode Shift Use
	strSave += (m_pModelDataManager->m_bInspectBarcodeShift ? "TRUE" : "FALSE") + strDelimeter;

	// Barcode Shift Ref X
	strTemp.Format(_T("%.2lf"), m_pModelDataManager->m_dBarcodeShiftRefX);
	strSave += strTemp + strDelimeter;

	// Barcode Shift Tol X
	strTemp.Format(_T("%.2lf"), m_pModelDataManager->m_dBarcodeShiftToleranceX);
	strSave += strTemp + strDelimeter;

	// Barcode Shift Ref Y
	strTemp.Format(_T("%.2lf"), m_pModelDataManager->m_dBarcodeShiftRefY);
	strSave += strTemp + strDelimeter;

	// Barcode Shift Tol Y
	strTemp.Format(_T("%.2lf"), m_pModelDataManager->m_dBarcodeShiftToleranceY);
	strSave += strTemp + strDelimeter;

	// Barcode Shift Use
	strSave += (m_pModelDataManager->m_bInspectBarcodeRotation ? "TRUE" : "FALSE") + strDelimeter;

	// Barcode Shift Ref R
	strTemp.Format(_T("%.2lf"), m_pModelDataManager->m_dBarcodeShiftRefRotation);
	strSave += strTemp + strDelimeter;

	// Barcode Shift Tol R
	strTemp.Format(_T("%.2lf"), m_pModelDataManager->m_dBarcodeShiftToleranceRotation);
	strSave += strTemp + strDelimeter;

	// Global Align Use
	strSave += (m_pModelDataManager->m_bUseLocalAlignMatching[iImageIdx] ? "TRUE" : "FALSE") + strDelimeter;

	// Global Align Image
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (m_pModelDataManager->m_bLocalAlignImage[iImageIdx][i] == TRUE)
		{
			strSave += g_sImageName[m_iMachineInspType][iImageIdx];

			if (i < MAX_IMAGE_TAB - 1)
				strSave += '/';
		}

		if (i == MAX_IMAGE_TAB - 1)
			strSave += strDelimeter;
	}

	// Global Align Pyramid
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iLocalMatchingPyramidLevel[iImageIdx]);
	strSave += strTemp + strDelimeter;

	// Global Align Angle
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iLocalMatchingAngleRange[iImageIdx]);
	strSave += strTemp + strDelimeter;

	// Global Align Score
	strTemp.Format(_T("%.2lf"), m_pModelDataManager->m_dLocalMatchingScore[iImageIdx]);
	strSave += strTemp + strDelimeter;

	// Global Align Search X
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iLocalMatchingSearchMarginX[iImageIdx]);
	strSave += strTemp + strDelimeter;

	// Global Align Search Y
	strTemp.Format(_T("%d"), m_pModelDataManager->m_iLocalMatchingSearchMarginY[iImageIdx]);
	strSave += strTemp + strDelimeter;

	// Insp Area 1
	strSave += (AlgorithmParam.m_bUseInspectArea[0] ? "TRUE" : "FALSE") + strDelimeter;

	// Insp Area 1
	if (bBarrelLens)
	{
		if (AlgorithmParam.m_iInspectAreaContour1Number[0] < 4)
			strSave += _T("Barrel ROI") + strDelimeter;
		else
			strSave += _T("Lens ROI") + strDelimeter;
	}
	else
		strSave += _T("User ROI") + strDelimeter;

	// Insp Area 1
	strTemp.Format(_T("%d"), AlgorithmParam.m_iInspectAreaContour1Margin[0]);
	strSave += strTemp + strDelimeter;

	// Insp Area 1
	if (bBarrelLens)
	{
		if (AlgorithmParam.m_iInspectAreaContour2Number[0] < 4)
			strSave += _T("Barrel ROI") + strDelimeter;
		else
			strSave += _T("Lens ROI") + strDelimeter;
	}
	else
		strSave += _T("User ROI") + strDelimeter;

	// Insp Area 1
	strTemp.Format(_T("%d"), AlgorithmParam.m_iInspectAreaContour2Margin[0]);
	strSave += strTemp + strDelimeter;

	// Insp Area 2
	strSave += (AlgorithmParam.m_bUseInspectArea[1] ? "TRUE" : "FALSE") + strDelimeter;

	// Insp Area 2
	if (bBarrelLens)
	{
		if (AlgorithmParam.m_iInspectAreaContour1Number[1] < 4)
			strSave += _T("Barrel ROI") + strDelimeter;
		else
			strSave += _T("Lens ROI") + strDelimeter;
	}
	else
		strSave += _T("User ROI") + strDelimeter;

	// Insp Area 2
	strTemp.Format(_T("%d"), AlgorithmParam.m_iInspectAreaContour1Margin[1]);
	strSave += strTemp + strDelimeter;

	// Insp Area 2
	if (bBarrelLens)
	{
		if (AlgorithmParam.m_iInspectAreaContour2Number[1] < 4)
			strSave += _T("Barrel ROI") + strDelimeter;
		else
			strSave += _T("Lens ROI") + strDelimeter;
	}
	else
		strSave += _T("User ROI") + strDelimeter;

	// Insp Area 2
	strTemp.Format(_T("%d"), AlgorithmParam.m_iInspectAreaContour2Margin[1]);
	strSave += strTemp + strDelimeter;

	// Insp Area 3
	strSave += (AlgorithmParam.m_bUseInspectArea[2] ? "TRUE" : "FALSE") + strDelimeter;

	// Insp Area 3
	if (bBarrelLens)
	{
		if (AlgorithmParam.m_iInspectAreaContour1Number[2] < 4)
			strSave += _T("Barrel ROI") + strDelimeter;
		else
			strSave += _T("Lens ROI") + strDelimeter;
	}
	else
		strSave += _T("User ROI") + strDelimeter;

	// Insp Area 3
	strTemp.Format(_T("%d"), AlgorithmParam.m_iInspectAreaContour1Margin[2]);
	strSave += strTemp + strDelimeter;

	// Insp Area 3
	if (bBarrelLens)
	{
		if (AlgorithmParam.m_iInspectAreaContour2Number[2] < 4)
			strSave += _T("Barrel ROI") + strDelimeter;
		else
			strSave += _T("Lens ROI") + strDelimeter;
	}
	else
		strSave += _T("User ROI") + strDelimeter;

	// Insp Area 3
	strTemp.Format(_T("%d"), AlgorithmParam.m_iInspectAreaContour2Margin[2]);
	strSave += strTemp + strDelimeter;

	// Fix Bright Thres Use
	strSave += (AlgorithmParam.m_bUseBrightFixedThres ? "TRUE" : "FALSE") + strDelimeter;

	// Fix Bright Thres Lower
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightLowerThres);
	strSave += strTemp + strDelimeter;

	// Fix Bright Thres Upper
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightUpperThres);
	strSave += strTemp + strDelimeter;

	// DT Bright Thres Use
	strSave += (AlgorithmParam.m_bUseBrightDT ? "TRUE" : "FALSE") + strDelimeter;

	// DT Bright Thres Nom Use
	strSave += (AlgorithmParam.m_bUseBrightLineNormalization ? "TRUE" : "FALSE") + strDelimeter;

	// DT Bright Median Filter Size
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightMedianFilterSize);
	strSave += strTemp + strDelimeter;

	// DT Bright Closing Filter Size
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightClosingFilterSize);
	strSave += strTemp + strDelimeter;

	// DT Bright Filter Size
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightDTFilterSize);
	strSave += strTemp + strDelimeter;

	// DT Bright Value
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightDTValue);
	strSave += strTemp + strDelimeter;

	// Uni Bright Thres Use
	strSave += (AlgorithmParam.m_bUseBrightUniformityCheck ? "TRUE" : "FALSE") + strDelimeter;

	// Uni Bright Offset
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightUniformityOffset);
	strSave += strTemp + strDelimeter;

	// Uni Bright Hyst
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightUniformityHystLength);
	strSave += strTemp + strDelimeter;

	// Uni Bright Hyst Offset
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightUniformityHystOffset);
	strSave += strTemp + strDelimeter;

	// Hyst Bright Thres Use
	strSave += (AlgorithmParam.m_bUseBrightHystThres ? "TRUE" : "FALSE") + strDelimeter;

	// Hyst Bright Hyst Secure
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightHystSecureThres);
	strSave += strTemp + strDelimeter;

	// Hyst Bright Hyst Potential
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightHystPotentialThres);
	strSave += strTemp + strDelimeter;

	// Hyst Bright Hyst Length
	strTemp.Format(_T("%d"), AlgorithmParam.m_iBrightHystPotentialLength);
	strSave += strTemp + strDelimeter;

	// Fix Dark Thres Use
	strSave += (AlgorithmParam.m_bUseDarkFixedThres ? "TRUE" : "FALSE") + strDelimeter;

	// Fix Dark Thres Lower
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkLowerThres);
	strSave += strTemp + strDelimeter;

	// Fix Dark Thres Upper
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkUpperThres);
	strSave += strTemp + strDelimeter;

	// DT Dark Thres Use
	strSave += (AlgorithmParam.m_bUseDarkDT ? "TRUE" : "FALSE") + strDelimeter;

	// DT Dark Thres Nom Use
	strSave += (AlgorithmParam.m_bUseDarkLineNormalization ? "TRUE" : "FALSE") + strDelimeter;

	// DT Dark Median Filter Size
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkMedianFilterSize);
	strSave += strTemp + strDelimeter;

	// DT Dark Closing Filter Size
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkClosingFilterSize);
	strSave += strTemp + strDelimeter;

	// DT Dark Filter Size
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkDTFilterSize);
	strSave += strTemp + strDelimeter;

	// DT Dark Value
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkDTValue);
	strSave += strTemp + strDelimeter;

	// Uni Dark Thres Use
	strSave += (AlgorithmParam.m_bUseDarkUniformityCheck ? "TRUE" : "FALSE") + strDelimeter;

	// Uni Dark Offset
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkUniformityOffset);
	strSave += strTemp + strDelimeter;

	// Uni Dark Hyst
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkUniformityHystLength);
	strSave += strTemp + strDelimeter;

	// Uni Dark Hyst Offset
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkUniformityHystOffset);
	strSave += strTemp + strDelimeter;

	// Hyst Dark Thres Use
	strSave += (AlgorithmParam.m_bUseDarkHystThres ? "TRUE" : "FALSE") + strDelimeter;

	// Hyst Dark Hyst Secure
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkHystSecureThres);
	strSave += strTemp + strDelimeter;

	// Hyst Dark Hyst Potential
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkHystPotentialThres);
	strSave += strTemp + strDelimeter;

	// Hyst Dark Hyst Length
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDarkHystPotentialLength);
	strSave += strTemp + strDelimeter;

	// Combine Bright Dark Blob
	strSave += (AlgorithmParam.m_bCombineBrightDarkBlob ? "TRUE" : "FALSE") + strDelimeter;

	// Opening Size
	strTemp.Format(_T("%d"), AlgorithmParam.m_iOpeningSize);
	strSave += strTemp + strDelimeter;

	// Closing Size
	strTemp.Format(_T("%d"), AlgorithmParam.m_iClosingSize);
	strSave += strTemp + strDelimeter;

	// Fill Up Use
	strSave += (AlgorithmParam.m_bFillup ? "TRUE" : "FALSE") + strDelimeter;

	// Connection Use
	strSave += (AlgorithmParam.m_bUseConnection ? "TRUE" : "FALSE") + strDelimeter;

	// Connection Min Size
	strTemp.Format(_T("%d"), AlgorithmParam.m_iConnectionMinSize);
	strSave += strTemp + strDelimeter;

	// Connection Length
	strTemp.Format(_T("%d"), AlgorithmParam.m_iConnectionLength);
	strSave += strTemp + strDelimeter;

	// Connection Angle Use
	strSave += (AlgorithmParam.m_bUseAngle ? "TRUE" : "FALSE") + strDelimeter;

	// Connection Width
	strTemp.Format(_T("%d"), AlgorithmParam.m_iConnectionMaxWidth);
	strSave += strTemp + strDelimeter;

	// Condition Area Use
	strSave += (AlgorithmParam.m_bUseDefectConditionArea ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Area Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionAreaMin);
	strSave += strTemp + strDelimeter;

	// Condition Area Max
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionAreaMax);
	strSave += strTemp + strDelimeter;

	// Condition Length Use
	strSave += (AlgorithmParam.m_bUseDefectConditionLength ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Length Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionLengthMin);
	strSave += strTemp + strDelimeter;

	// Condition Length Max
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionLengthMax);
	strSave += strTemp + strDelimeter;

	// Condition Width Use
	strSave += (AlgorithmParam.m_bUseDefectConditionWidth ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Width Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionWidthMin);
	strSave += strTemp + strDelimeter;

	// Condition Width Max
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionWidthMax);
	strSave += strTemp + strDelimeter;

	// Condition X Use
	strSave += (AlgorithmParam.m_bDefectConditionXLength ? "TRUE" : "FALSE") + strDelimeter;

	// Condition X Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionXLengthMin);
	strSave += strTemp + strDelimeter;

	// Condition X Max
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionXLengthMax);
	strSave += strTemp + strDelimeter;

	// Condition Y Use
	strSave += (AlgorithmParam.m_bDefectConditionYLength ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Y Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionYLengthMin);
	strSave += strTemp + strDelimeter;

	// Condition Y Max
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionYLengthMax);
	strSave += strTemp + strDelimeter;

	// Condition Mean Use
	strSave += (AlgorithmParam.m_bUseDefectConditionMean ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Mean Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionMeanMin);
	strSave += strTemp + strDelimeter;

	// Condition Mean Max
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionMeanMax);
	strSave += strTemp + strDelimeter;

	// Condition Std Use
	strSave += (AlgorithmParam.m_bUseDefectConditionStdev ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Std Min
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionStdevMin);
	strSave += strTemp + strDelimeter;

	// Condition Std Max
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionStdevMax);
	strSave += strTemp + strDelimeter;

	// Condition Aniso Use
	strSave += (AlgorithmParam.m_bUseDefectConditionAnisometry ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Aniso Min
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionAnisometryMin);
	strSave += strTemp + strDelimeter;

	// Condition Aniso Max
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionAnisometryMax);
	strSave += strTemp + strDelimeter;

	// Condition Circularity Use
	strSave += (AlgorithmParam.m_bUseDefectConditionCircularity ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Circularity Min
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionCircularityMin);
	strSave += strTemp + strDelimeter;

	// Condition Circularity Max
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionCircularityMax);
	strSave += strTemp + strDelimeter;

	// Condition Convexity Use
	strSave += (AlgorithmParam.m_bUseDefectConditionConvexity ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Convexity Min
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionConvexityMin);
	strSave += strTemp + strDelimeter;

	// Condition Convexity Max
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionConvexityMax);
	strSave += strTemp + strDelimeter;

	// Condition Ellipse Use
	strSave += (AlgorithmParam.m_bUseDefectConditionEllipseRatio ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Ellipse Min
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionEllipseRatioMin);
	strSave += strTemp + strDelimeter;

	// Condition Ellipse Max
	strTemp.Format(_T("%.2lf"), AlgorithmParam.m_dDefectConditionEllipseRatioMax);
	strSave += strTemp + strDelimeter;

	// Condition Contrast Use
	strSave += (AlgorithmParam.m_bDefectConditionContrast ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Contrast Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionContrastMin);
	strSave += strTemp + strDelimeter;

	// Condition Contrast Max
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionContrastMax);
	strSave += strTemp + strDelimeter;

	// Condition Outer Dist Use
	strSave += (AlgorithmParam.m_bUseDefectConditionOuterDist ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Outer Dist Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionOuterDist);
	strSave += strTemp + strDelimeter;

	// Condition Inner Dist Use
	strSave += (AlgorithmParam.m_bUseDefectConditionInnerDist ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Inner Dist Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iDefectConditionInnerDist);
	strSave += strTemp + strDelimeter;

	// Condition FT Connect Use
	strSave += (AlgorithmParam.m_bUseFTConnected ? "TRUE" : "FALSE") + strDelimeter;

	// Condition FT Connect Area Use
	strSave += (AlgorithmParam.m_bUseFTConnectedArea ? "TRUE" : "FALSE") + strDelimeter;

	// Condition  FT Connect Area Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iFTConnectedAreaMin);
	strSave += strTemp + strDelimeter;

	// Condition FT Connect Length Use
	strSave += (AlgorithmParam.m_bUseFTConnectedLength ? "TRUE" : "FALSE") + strDelimeter;

	// Condition  FT Connect Length Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iFTConnectedLengthMin);
	strSave += strTemp + strDelimeter;

	// Condition FT Connect Width Use
	strSave += (AlgorithmParam.m_bUseFTConnectedWidth ? "TRUE" : "FALSE") + strDelimeter;

	// Condition  FT Connect Width Min
	strTemp.Format(_T("%d"), AlgorithmParam.m_iFTConnectedWidthMin);
	strSave += strTemp + strDelimeter;

	// Condition Dont Care Apply
	strSave += (AlgorithmParam.m_bUseApplyDontCare ? "TRUE" : "FALSE") + strDelimeter;

	// Condition Dont Care Use
	strSave += (AlgorithmParam.m_bUseDontCare ? "TRUE" : "FALSE") + strDelimeter;

	strPath.Format("%s\\%s\\DetectParam_%s_%04d%02d%02d.csv", m_FileBase.m_strResultFolderPrev, strLotID, strLotID, time.wYear, time.wMonth, time.wDay);
	m_pSaveManager->SaveResult(strHeader, strSave, strPath);

	if (bSaveLas)
	{
		strPath.Format("%s\\%s_%04d%02d%02d%02d_Detect_Vision_Para.csv", m_szLASDataFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
		m_pSaveManager->SaveResult(strHeader, strSave, strPath);

		// TEMP 폴더 저장 주석 처리 - 251103, jhkim
		// strPath.Format("%s\\%s_%04d%02d%02d%02d_Detect_Vision_Para.csv", m_szLASTempFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
		// m_pSaveManager->SaveResult(strHeader, strSave, strPath);

		// strPath.Format("%s\\%s_%04d%02d%02d%02d_AVI_MFG_SPEC_AVIMONITOR_RESULT.csv", m_szLASDataFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
		// m_pSaveManager->SaveResult(strHeader, strSave, strPath);

		// strPath.Format("%s\\%s_%04d%02d%02d%02d_AVI_MFG_SPEC_AVIMONITOR_RESULT.csv", m_szLASSectionFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
		// m_pSaveManager->SaveResult(strHeader, strSave, strPath);
	}
}

// FAI 로그 추가
void CuScanApp::SaveFAIResultLog(BOOL bSaveLas, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode) // ParkSW 20240626
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strFile, strDelimeter, strHeader;

	//////////
	strDelimeter = _T(",");

	if (m_iMachineInspType == MACHINE_WELDING)
	{
		for (long n = 0; n < FAI_WD_RESULT_HEADER_END; n++)
		{
			strHeader += g_sFAIResultLogHeader_Welding[FAI_WD_RESULT_HEADER_DATE + n];
			strHeader += strDelimeter;
		}
	}
	else
	{
		for (long n = 0; n < FAI_RESULT_HEADER_END; n++)
		{
			strHeader += g_sFAIResultLogHeader[FAI_RESULT_HEADER_DATE + n];
			strHeader += strDelimeter;
		}
	}
	CString strTemp = _T("");
	CString strSave = _T("");

	// Data
	strTemp.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
	strSave += strTemp + strDelimeter;

	// Time
	strTemp.Format("\'%04d-%02d-%02d %02d:%02d:%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	strSave += strTemp + strDelimeter;

	// Station
	strSave += GetPCID() + strDelimeter;

	// Machine No
	strSave += Struct_PreferenceStruct.m_strEquipNo + strDelimeter;

	// Lot ID
	strSave += strLotID + strDelimeter;

	// Model
	strSave += m_pModelDataManager->m_sModelName + strDelimeter;

	// Tray
	strTemp.Format(_T("%d"), iTrayNo);
	strSave += strTemp + strDelimeter;

	// Module
	strTemp.Format(_T("%d"), iModuleNo);
	strSave += strTemp + strDelimeter;

	// IndexNo
	strTemp.Format("%d", m_pInspectService->m_iIndexNo_H);
	strSave += strTemp + strDelimeter;

	// PocketNo
	int iPocketNo = m_iModuleNumberOneLine * (m_pInspectService->m_iIndexNo_H - 1) + (iModuleNo - 1) % m_iModuleNumberOneLine;
	strTemp.Format("%d", iPocketNo + 1);
	strSave += strTemp + strDelimeter;

	// Barcode
	strSave += sBarcode + strDelimeter;

	if (m_iMachineInspType == MACHINE_WELDING)
	{
		// FAI-14 Result
		strSave += m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[14][0] + strDelimeter;

		// FAI-14
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[14][0]);
		strSave += strTemp + strDelimeter;

		// FAI-14-X-보
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[15][0]);
		strSave += strTemp + strDelimeter;

		// FAI-14-X-원
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[15][1]);
		strSave += strTemp + strDelimeter;

		// FAI-14-Y-보
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[16][0]);
		strSave += strTemp + strDelimeter;

		// FAI-14-Y-원
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[16][1]);
		strSave += strTemp + strDelimeter;

		// Bracket Rotation Result
		strSave += m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[123][0] + strDelimeter;

		// Rotation-R-보
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[123][0]);
		strSave += strTemp + strDelimeter;

		// Rotation-R-원
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[123][1]);
		strSave += strTemp + strDelimeter;

		// FAI-EA Result
		strSave += m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[124][0] + strDelimeter;

		// FAI-EA
		strTemp.Format("%d", (int)m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[124][0]);
		strSave += strTemp + strDelimeter;

		// FAI-Diameter Result
		strSave += m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[125][0] + strDelimeter;

		// FAI-S1-Diameter
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[125][1]);
		strSave += strTemp + strDelimeter;

		// FAI-S2-Diameter
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[125][2]);
		strSave += strTemp + strDelimeter;

		// FAI-S3-Diameter
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[125][3]);
		strSave += strTemp + strDelimeter;

		// FAI-S4-Diameter
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[125][4]);
		strSave += strTemp + strDelimeter;

		// FAI-S1-Position
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[126][0]);
		strSave += strTemp + strDelimeter;

		// FAI-S2-Position
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[127][0]);
		strSave += strTemp + strDelimeter;

		// FAI-S3-Position
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[128][0]);
		strSave += strTemp + strDelimeter;

		// FAI-S4-Position
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[129][0]);
		strSave += strTemp + strDelimeter;

		// FAI-S5-Position
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[130][0]);
		strSave += strTemp + strDelimeter;

		// FAI-14-Inner-Rad
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[14][2]);
		strSave += strTemp + strDelimeter;

		// FAI-14-Inner-X
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[15][2]);
		strSave += strTemp + strDelimeter;

		// FAI-14-Inner-Y
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[16][2]);
		strSave += strTemp + strDelimeter;

		// Chasis North/South
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleCenterX[14]);
		strSave += strTemp + strDelimeter;

		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleCenterY[14]);
		strSave += strTemp + strDelimeter;

		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumEllipseCenterX[14]);
		strSave += strTemp + strDelimeter;

		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumEllipseCenterY[14]);
		strSave += strTemp + strDelimeter;

		// Inner1 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[14][0]);
		strSave += strTemp + strDelimeter;

		// Inner2 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[14][1]);
		strSave += strTemp + strDelimeter;

		// Inner3 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[14][2]);
		strSave += strTemp + strDelimeter;

		// Inner4 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[14][3]);
		strSave += strTemp + strDelimeter;

		// Inner5 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[14][4]);
		strSave += strTemp + strDelimeter;

		// Inner6 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[14][5]);
		strSave += strTemp + strDelimeter;

		// Inner7 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[14][6]);
		strSave += strTemp + strDelimeter;

		// Inner8 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[14][7]);
		strSave += strTemp + strDelimeter;
	}
	else
	{
		// FAI-26 Result
		strSave += m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_strFAI_OKNG[26][0] + strDelimeter;

		// FAI-26
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[26][0]);
		strSave += strTemp + strDelimeter;

		// FAI-26-X
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[27][0]);
		strSave += strTemp + strDelimeter;

		// FAI-26-X-ORG
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[27][1]);
		strSave += strTemp + strDelimeter;

		// FAI-26-Y
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[28][0]);
		strSave += strTemp + strDelimeter;

		// FAI-26-Y-ORG
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[28][1]);
		strSave += strTemp + strDelimeter;

		// FAI-26-X
		strTemp.Format("%.8f", THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[51][0]);
		strSave += strTemp + strDelimeter;

		// FAI-26
		strTemp.Format("%.8f", THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[52][0]);
		strSave += strTemp + strDelimeter;

		// FAI-26-X-ORG
		strTemp.Format("%.8f", THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[52][1]);
		strSave += strTemp + strDelimeter;

		// FAI-26-Y
		strTemp.Format("%.8f", THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[53][0]);
		strSave += strTemp + strDelimeter;

		// FAI-26-Y-ORG
		strTemp.Format("%.8f", THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[53][1]);
		strSave += strTemp + strDelimeter;

		// Inner X
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterX[51]);
		strSave += strTemp + strDelimeter;

		// Inner Y
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterY[51]);
		strSave += strTemp + strDelimeter;

		// Outer X
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterCircleCenterX[51]);
		strSave += strTemp + strDelimeter;

		// Outer Y
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterCircleCenterY[51]);
		strSave += strTemp + strDelimeter;

		// DatumA X
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleCenterX[26]);
		strSave += strTemp + strDelimeter;

		// DatumA Y
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleCenterY[26]);
		strSave += strTemp + strDelimeter;

		// DatumB X
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumEllipseCenterX[26]);
		strSave += strTemp + strDelimeter;

		// DatumB Y
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumEllipseCenterY[26]);
		strSave += strTemp + strDelimeter;

		// Inner Diameter
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[51][1]);
		strSave += strTemp + strDelimeter;

		// Outer Diameter
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[51][2]);
		strSave += strTemp + strDelimeter;

		// DatumA Diameter
		strTemp.Format("%.8f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[26][1]);
		strSave += strTemp + strDelimeter;

		// Inner1 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[51][0]);
		strSave += strTemp + strDelimeter;

		// Inner2 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[51][1]);
		strSave += strTemp + strDelimeter;

		// Inner3 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[51][2]);
		strSave += strTemp + strDelimeter;

		// Inner4 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[51][3]);
		strSave += strTemp + strDelimeter;

		// Inner5 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[51][4]);
		strSave += strTemp + strDelimeter;

		// Inner6 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[51][5]);
		strSave += strTemp + strDelimeter;

		// Inner7 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[51][6]);
		strSave += strTemp + strDelimeter;

		// Inner8 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleES[51][7]);
		strSave += strTemp + strDelimeter;

		// Outer1 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleES[51][0]);
		strSave += strTemp + strDelimeter;

		// Outer2 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleES[51][1]);
		strSave += strTemp + strDelimeter;

		// Outer3 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleES[51][2]);
		strSave += strTemp + strDelimeter;

		// Outer4 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleES[51][3]);
		strSave += strTemp + strDelimeter;

		// Outer5 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleES[51][4]);
		strSave += strTemp + strDelimeter;

		// Outer6 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleES[51][5]);
		strSave += strTemp + strDelimeter;

		// Outer7 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleES[51][6]);
		strSave += strTemp + strDelimeter;

		// Outer8 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleES[51][7]);
		strSave += strTemp + strDelimeter;

		// DatumA1 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleES[26][0]);
		strSave += strTemp + strDelimeter;

		// DatumA2 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleES[26][1]);
		strSave += strTemp + strDelimeter;

		// DatumA3 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleES[26][2]);
		strSave += strTemp + strDelimeter;

		// DatumA4 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleES[26][3]);
		strSave += strTemp + strDelimeter;

		// DatumA5 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleES[26][4]);
		strSave += strTemp + strDelimeter;

		// DatumA6 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleES[26][5]);
		strSave += strTemp + strDelimeter;

		// DatumA7 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleES[26][6]);
		strSave += strTemp + strDelimeter;

		// DatumA8 Edge Strength
		strTemp.Format("%.4f", m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleES[26][7]);
		strSave += strTemp + strDelimeter;
	}

	strFile.Format("%s\\%s\\FAILotResult_%s_%04d%02d%02d.csv", m_FileBase.m_strResultFolderPrev, strLotID, strLotID, time.wYear, time.wMonth, time.wDay);
	m_pSaveManager->SaveResult(strHeader, strSave, strFile);

	if (bSaveLas)
	{
		strFile.Format("%s\\%s_%04d%02d%02d%02d_FAI_Log.csv", m_szLASDataFolder, strLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
		m_pSaveManager->SaveResult(strHeader, strSave, strFile);
	}
}
//

void CuScanApp::SaveOnlyADJLotResultLog(BOOL bSaveLas, CString sLotID, int iTrayNo, CString sBarcode, int iModuleNo, CString sDeepModelName, CString sDefectName, CString sADJResult, CString sAddStr)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	// ADJDaySummary.txt
	CString strPath, strDayPath, strHeader, strDelimeter;

	strDelimeter = ",";
	strHeader.Format("Date,Time,Machine_Code,LotNum,Config,Tray,Barcode,ModuleNo,Model ID,불량 항목,ADJ Unit 결과,ADJ 로그");

	CString strSave, strTemp;

	// Data
	strTemp.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
	strSave += strTemp + strDelimeter;

	// Time
	strTemp.Format("\'%04d-%02d-%02d %02d:%02d:%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	strSave += strTemp + strDelimeter;

	// Station
	strSave += GetPCID() + strDelimeter;

	// Machine No
	strSave += Struct_PreferenceStruct.m_strEquipNo + strDelimeter;

	// Lot ID
	strSave += sLotID + strDelimeter;

	// Model
	strSave += m_pModelDataManager->m_sModelName + strDelimeter;

	// Tray
	strTemp.Format(_T("%d"), iTrayNo);
	strSave += strTemp + strDelimeter;

	// Barcode
	strSave += sBarcode + strDelimeter;

	// Module
	strTemp.Format(_T("%d"), iModuleNo);
	strSave += strTemp + strDelimeter;

	// Model ID
	strSave += sDeepModelName + strDelimeter;

	// 불량 항목
	strSave += sDefectName + strDelimeter;

	// AI Result
	strSave += sADJResult + strDelimeter;

	// 파일명, 확률값 등등
	strSave += sAddStr + strDelimeter;

	strPath.Format("%s\\ADJOnlyDayLotReult_%04d%02d%02d.csv", m_FileBase.m_strResultFolderPrev, time.wYear, time.wMonth, time.wDay);
	strDayPath.Format("%s\\%s\\ADJOnlyLotReult_%04d%02d%02d.csv", m_FileBase.m_strResultFolderPrev, sLotID, time.wYear, time.wMonth, time.wDay);

	m_pSaveManager->SaveResult(strHeader, strSave, strPath);
	m_pSaveManager->SaveResult(strHeader, strSave, strDayPath);

	if (bSaveLas)
	{
		strPath.Format("%s\\%s_%04d%02d%02d%02d_ADJLotResult_Log.csv", m_szLASDataFolder, sLotID, time.wYear, time.wMonth, time.wDay, time.wHour);
		m_pSaveManager->SaveResult(strHeader, strSave, strPath);
	}
}

void CuScanApp::SaveADJDaySummaryINI(CString sSectionModel, BOOL bResultNG, BOOL bResultSkip)
{
	EnterCriticalSection(&m_csADJDaySummaryLog);

	SYSTEMTIME time;
	GetLocalTime(&time);

	// ADJDaySummary.txt
	CString sDaySummaryFileName;
	sDaySummaryFileName.Format("%s\\ADJDaySummary_%04d%02d%02d.txt", m_FileBase.m_strResultFolderPrev, time.wYear, time.wMonth, time.wDay);

	CIniFileCS INI_DaySummary(sDaySummaryFileName);
	int iTotalOK, iTotalNG;
	int iTotal, iTotalSkip;
	double dFilter, dSkipFilter;
	CString strFilter, strSkipFilter;

	iTotalOK = INI_DaySummary.Get_Integer(sSectionModel, "양품", 0);
	iTotalNG = INI_DaySummary.Get_Integer(sSectionModel, "불량", 0);
	iTotal = INI_DaySummary.Get_Integer(sSectionModel, "딥러닝요청수량", 0);
	iTotalSkip = INI_DaySummary.Get_Integer(sSectionModel, "딥러닝스킵수량", 0);

	iTotal++;

	if (bResultSkip)
		iTotalSkip++;

	if (bResultNG)
		iTotalNG++;
	else
		iTotalOK++;

	if ((iTotal) == 0)
	{
		dSkipFilter = 0;
		dFilter = 0;
	}
	else
	{
		dSkipFilter = ((double)iTotalSkip / (double)(iTotal)) * 100.0;
		dFilter = ((double)iTotalOK / (double)(iTotalOK + iTotalNG)) * 100.0;
	}

	strSkipFilter.Format("%.3lf", dSkipFilter);
	strFilter.Format("%.3lf", dFilter);

	INI_DaySummary.Set_Integer(sSectionModel, "총량", iTotal);
	INI_DaySummary.Set_Integer(sSectionModel, "양품", iTotalOK);
	INI_DaySummary.Set_Integer(sSectionModel, "불량", iTotalNG);
	INI_DaySummary.Set_String(sSectionModel, "필터링률(%)", strFilter);
	INI_DaySummary.Set_Integer(sSectionModel, "딥러닝요청수량", iTotal);
	INI_DaySummary.Set_Integer(sSectionModel, "딥러닝스킵수량(inNG)", iTotalSkip);
	INI_DaySummary.Set_String(sSectionModel, "딥러닝스킵률(%)", strSkipFilter);

	LeaveCriticalSection(&m_csADJDaySummaryLog);
}
// ADJ 로그 추가 - LeegW

// PC Status Log 추가 - LeeGW START
void CuScanApp::SavePCStatusLog(BOOL bSaveLas)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	// PCStatus.txt
	CString strPath;

	CString strHeader;
	strHeader.Format("Date,Time,Station,Machine_Code,CPU,MemoryTotal(GB),MemoryUse(GB),MemoryUse(%%),DriveType(C:),DriveTotal(C:)(GB),DriveUse(C:)(GB),DriveUse(C:)(%%),DriveType(D:),DriveTotal(D:)(GB),DriveUse(D:)(GB),DriveUse(D:)(%%)");
	// strHeader.Format("Date,Time,Machine_Code,CPU,Memory,Drive(C:),Drive(D:)");

	CString strSave, strTemp, strDelimeter;
	strDelimeter = _T(",");

	// Data
	strTemp.Format("%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
	strSave += strTemp + strDelimeter;

	// Time
	strTemp.Format("\'%04d-%02d-%02d %02d:%02d:%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	strSave += strTemp + strDelimeter;

	// Station
	strSave += GetPCID() + strDelimeter;

	// Machine No
	strSave += Struct_PreferenceStruct.m_strEquipNo + strDelimeter;

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	HKEY hKey;
	CString strCPU;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		char buffer[256];
		DWORD bufferSize = sizeof(buffer);
		if (RegQueryValueEx(hKey, _T("ProcessorNameString"), NULL, NULL, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS)
		{
			strCPU = (CString)buffer;
		}
		RegCloseKey(hKey);
	}

	switch (sysInfo.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
		strCPU += " x64";
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		strCPU += " x86";
		break;
	case PROCESSOR_ARCHITECTURE_ARM64:
		strCPU += " ARM64";
		break;
	default:
		strCPU += " Unknown";
		break;
	}

	// CPU
	strSave += strCPU + strDelimeter;

	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(MEMORYSTATUSEX);

	double totalMemoryGB = 0, usedMemoryGB = 0, usedMemoryPercent = 0;

	if (GlobalMemoryStatusEx(&memStatus))
	{
		totalMemoryGB = (double)memStatus.ullTotalPhys / (1024 * 1024 * 1024);
		usedMemoryGB = (double)(memStatus.ullTotalPhys - memStatus.ullAvailPhys) / (1024 * 1024 * 1024);
		usedMemoryPercent = (double)usedMemoryGB / totalMemoryGB * 100;
	}

	// Memory Total
	strTemp.Format("%0.3fGB", totalMemoryGB);
	strSave += strTemp + strDelimeter;

	// Memory Use(GB)
	strTemp.Format("%0.3fGB", usedMemoryGB);
	strSave += strTemp + strDelimeter;

	// Memory Use(%)
	strTemp.Format("%0.3f%%", usedMemoryPercent);
	strSave += strTemp + strDelimeter;

	HANDLE hDevice;
	ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
	CString drive, devicePath;
	double totalGB = 0, usedGB = 0, usedPercent;

	strTemp = _T("");
	drive = _T("C:\\");
	if (GetDiskFreeSpaceEx(drive, &freeBytesAvailable, &totalBytes, &totalFreeBytes))
	{
		totalGB = (double)totalBytes.QuadPart / (1024 * 1024 * 1024);
		usedGB = (double)(totalBytes.QuadPart - freeBytesAvailable.QuadPart) / (1024 * 1024 * 1024);
		usedPercent = (double)usedGB / totalGB * 100.0;
	}

	char volumeName[MAX_PATH] = {0};
	char fileSystemName[MAX_PATH] = {0};
	DWORD serialNumber = 0, maxComponentLength = 0, fileSystemFlags = 0;

	if (GetVolumeInformation(drive, volumeName, sizeof(volumeName) / sizeof(char), &serialNumber, &maxComponentLength, &fileSystemFlags, fileSystemName, sizeof(fileSystemName) / sizeof(char)))
	{
		strTemp = volumeName;
	}
	else
	{
		strTemp = _T("");
	}

	// Drive Type(C:)
	strSave += strTemp + strDelimeter;

	// Drive Total(C:)(GB)
	strTemp.Format("%0.3fGB", totalGB);
	strSave += strTemp + strDelimeter;

	// Drive Use(C:)(GB)
	strTemp.Format("%0.3fGB", usedGB);
	strSave += strTemp + strDelimeter;

	// Drive Use(C:)(%)
	strTemp.Format("%0.3f%%", usedPercent);
	strSave += strTemp + strDelimeter;

	drive = _T("D:\\");
	if (GetDiskFreeSpaceEx(drive, &freeBytesAvailable, &totalBytes, &totalFreeBytes))
	{
		totalGB = (double)totalBytes.QuadPart / (1024 * 1024 * 1024);
		usedGB = (double)(totalBytes.QuadPart - freeBytesAvailable.QuadPart) / (1024 * 1024 * 1024);
		usedPercent = (double)usedGB / totalGB * 100.0;
	}

	char volumeName2[MAX_PATH] = {};
	char fileSystemName2[MAX_PATH] = {0};
	serialNumber = 0, maxComponentLength = 0, fileSystemFlags = 0;

	if (GetVolumeInformation(drive, volumeName2, sizeof(volumeName2) / sizeof(char), &serialNumber, &maxComponentLength, &fileSystemFlags, fileSystemName2, sizeof(fileSystemName2) / sizeof(char)))
	{
		strTemp = volumeName2;
	}
	else
	{
		strTemp = _T("");
	}

	// Drive Type(D:)
	strSave += strTemp + strDelimeter;

	// Drive Total(D:)(GB)
	strTemp.Format("%0.3fGB", totalGB);
	strSave += strTemp + strDelimeter;

	// Drive Use(D:)(GB)
	strTemp.Format("%0.3fGB", usedGB);
	strSave += strTemp + strDelimeter;

	// Drive Use(D:)(%)
	strTemp.Format("%0.3f%%", usedPercent);
	strSave += strTemp + strDelimeter;

	strPath.Format("%s\\PCStatus_%04d%02d%02d.csv", m_FileBase.m_strResultFolderPrev, time.wYear, time.wMonth, time.wDay);
	m_pSaveManager->SaveResult(strHeader, strSave, strPath);

	if (bSaveLas)
	{
		strPath.Format("%s\\PCSTATUS_%04d%02d%02d%02d_PCStatus_Log.csv", m_szLASDataFolder, time.wYear, time.wMonth, time.wDay, time.wHour);
		m_pSaveManager->SaveResult(strHeader, strSave, strPath);
	}
}
// PC Status Log 추가 - LeeGW END

// 2024.09.24 - v2660 - LAS ZIP 추가 - LeeGW
void CuScanApp::CreateLasZip(CString strLotID, CString sBarcode)
{
	CString strTemp;
	CString strZipPath;
	CString TempLog;
	CString strLasZipName;
	CString strTime;
	SYSTEMTIME systemLasTime;

	GetLocalTime(&systemLasTime);
	strTime.Format("%04d%02d%02d%02d%02d%02d", systemLasTime.wYear, systemLasTime.wMonth, systemLasTime.wDay, systemLasTime.wHour, systemLasTime.wMinute, systemLasTime.wSecond);

	CString strTempZipFileName = GetLASTempDirectory() + "\\" + strLotID + "__Result_" + sBarcode + '_' + strTime + '_' + Struct_PreferenceStruct.m_strEquipNo + ".zip";
	strTemp = _T(strTempZipFileName);

	const char *ccharpZipFileName = (char *)(LPCSTR)strTemp;
	hz[Struct_BarcodeVirtualMagazine.GetVirtualMagazineNo(sBarcode)] = CreateZip(ccharpZipFileName, 0);
}
void CuScanApp::AddLasZip(CString sBarcodeResult, CString sFilePath, CString sLasFileName)
{
	int magazineNo = Struct_BarcodeVirtualMagazine.GetVirtualMagazineNo(sBarcodeResult);
	zr[magazineNo] = NULL;

	HZIP hZip = hz[magazineNo];
	zr[magazineNo] = ZipAdd(
		hZip,
		sLasFileName,
		sFilePath);
	int zipAddResult = zr[magazineNo];

	if (zipAddResult != 0) // Assuming 0 is success, non-zero is failure
	{
		CString errMsg;
		errMsg.Format(_T("ZipAdd failed for barcode: %s, file: %s, error code: %d"), sBarcodeResult, sFilePath, zipAddResult);

		SaveLog(errMsg);
		// Optionally, handle cleanup or retry here
	}
}

void CuScanApp::MoveLasZip(CString strLotID, CString sBarcode)
{
	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;

	CString sSrcPath, sFilename, sDstPath;
	sSrcPath.Format("%s\\%s__Result_%s*", GetLASTempDirectory(), strLotID, sBarcode);

	if ((hFindFile = FindFirstFile(sSrcPath, &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		FindClose(hFindFile);

		sFilename = (LPCTSTR)FindFileData.cFileName;
		sSrcPath.Format("%s\\%s", GetLASTempDirectory(), sFilename);

		sDstPath.Format("%s\\%s", GetLASImageDirectory(), sFilename);
		MoveFile(sSrcPath, sDstPath);
	}
}

void CuScanApp::CloseLasZip(CString sBarcode)
{
	zr[Struct_BarcodeVirtualMagazine.GetVirtualMagazineNo(sBarcode)] = CloseZip(hz[Struct_BarcodeVirtualMagazine.GetVirtualMagazineNo(sBarcode)]);
}
// 2024.09.24 - v2660 - LAS ZIP 추가 - LeeGW

void CuScanApp::SetViewStatusText(int idx, CString szMsg)
{
	if (m_bInitComplete)
	{
		CuScanView *pView = (CuScanView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
		pView->SetStatusText(idx, szMsg);
	}
}

void CuScanApp::UpdateCurMode()
{
	if (m_bInitComplete)
	{
		CuScanView *pView = (CuScanView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
		pView->UpdateCurMode();
	}
}

void CuScanApp::SetExitProgram()
{
	if (m_bInitComplete)
	{
		CuScanView *pView = (CuScanView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
		pView->SetExitProgram();
	}
}

void CuScanApp::SetInspectionReady()
{
	CuScanView *pView = (CuScanView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
	pView->TeachingShow();
}

void CuScanApp::DoEvents()
{
	MSG msg;
	if (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

BOOL CuScanApp::IsRun()
{
	return m_bRun;
}

#include <tlhelp32.h>

int CuScanApp::CheckProcessExist()
{
	HANDLE hSnap = NULL;
	PROCESSENTRY32 pe;
	char str[256];
	BOOL f_Find = FALSE;
	int iFound = 0;

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);

	CString sFileName = GetProgramFileName();

	if (Process32First(hSnap, &pe))
	{
		do
		{
			wsprintf(str, "%s", pe.szExeFile);
			if (stricmp(str, (LPSTR)(LPCTSTR)sFileName) == 0)
			{
				iFound++;
			}
		} while (Process32Next(hSnap, &pe));
	}
	if (iFound != 0)
	{
		ExitProcess(sFileName, GetCurrentProcessId());
	}

	return iFound;
}

void CuScanApp::ExitProcess(CString strTargetProcName, DWORD dwExceptProcId)
{
	DWORD aProcesses[1024] = {
		0,
	};
	DWORD cbNeeded;
	DWORD cProcesses;

	// 실행중인 프로세스를 모두 구한다
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return;

	// 실행중인 프로세스의 개수를 구한다
	cProcesses = cbNeeded / sizeof(DWORD);
	// 각 프로세스에 대한 이름 및 프로세스 아이디를 구하고 타겟 프로세스를 강제로 종료시킨다
	for (UINT i = 0; i < cProcesses; i++)
	{
		TCHAR szProcessName[MAX_PATH] = _T("unknown");
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

		if (NULL != hProcess)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
				GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName)); // 프로세스 이름을 구한다(szProcessName)
		}

		BOOL bRet;
		CString strComp = szProcessName;
		// 실행중인 프로세스(strComp)와 죽이려는 프로세스(strTargetProcName) 이름을 비교하여 같으면 해당 프로세스 종료
		// 단 실행중인 나 자신은 죽이려는 프로세스에서 제외시킨다
		if ((strComp.MakeLower() == strTargetProcName.MakeLower()) && (aProcesses[i] != dwExceptProcId))
		{
			HANDLE hKillProc = OpenProcess(PROCESS_TERMINATE, FALSE, aProcesses[i]);

			if (hKillProc != NULL)
			{
				DWORD ExitCode = 0;

				GetExitCodeProcess(hKillProc, &ExitCode);
				bRet = TerminateProcess(hKillProc, ExitCode);
				if (bRet)
					WaitForSingleObject(hKillProc, INFINITE);

				CloseHandle(hKillProc);
			}
		}
		CloseHandle(hProcess);
	}
}

// 프로세스 초기화 Restart - LeeGW
// 목적: 현재 실행 중인 검사 프로그램을 종료 후 동일 경로에서 재실행하여 메모리 누수 등 시스템 안정성 확보
// 사용 시점: InspectSummary 의 OnTimer (100ms 마다 재시작 검사)
void CuScanApp::RestartProcess()
{
	char exePath[MAX_PATH];

	THEAPP.DoubleLogOut("[CuScanApp::RestartProcess]Process Restarting...");
	if (GetModuleFileName(NULL, exePath, MAX_PATH) == 0)
	{
		THEAPP.DoubleLogOut("Failed to get executable path!");
		return;
	}

	STARTUPINFO si = {sizeof(STARTUPINFO)};
	PROCESS_INFORMATION pi;

	// TODO : 현재 해당 프로젝트는 단일 프로세스만 허용. 따라서 아래 Createprocess 호출 시 바로 기존 프로세스 종료됨.
	if (CreateProcess(exePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		// 추가: 새 프로세스가 정상적으로 실행되는지 확인
		DWORD waitResult = WaitForInputIdle(pi.hProcess, 1000); // 1초 대기
		if (waitResult == 0)
		{
			THEAPP.DoubleLogOut("New process started and is responsive.");
		}
		else
		{
			THEAPP.DoubleLogOut("Warning: New process may not be responsive (WaitForInputIdle timeout).");
		}

#ifdef INLINE_MODE
		m_pHandlerService->Set_StatusUpdate(HS_STOP);
		Sleep(200);
		m_pHandlerService->Terminate_Handler();
		m_pHandlerService->DeleteInstance();
#endif
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		THEAPP.DoubleLogOut("Current process exiting for restart.");
		PostQuitMessage(0); // 현재 프로세스 종료
	}
	else
	{
		SaveLog("Failed to create process!");
	}
}

CString CuScanApp::GetProgramFileName()
{
	char szBuffer[MAX_PATH * 2];
	int nPathLength = 0, nFileNameLength = 0;
	char *FirstIdx, *SecIdx;
	CString sFileName;

	nPathLength = GetModuleFileName(NULL, szBuffer, MAX_PATH);
	FirstIdx = strrchr(szBuffer, '\\');
	SecIdx = strrchr(szBuffer, '\0');
	nFileNameLength = SecIdx - FirstIdx;

	sFileName.Format("%s", FirstIdx + 1);

	return sFileName;
}

CCalDataService *CuScanApp::GetCalDataService(int nCamIdx)
{
	if (nCamIdx == 1)
		return m_pCalDataService;
	return NULL;
}

void CuScanApp::DeletePath(CString strPath)
{
	CFileFind finder;
	BOOL bContinue = TRUE;

	if (strPath.Right(1) != _T("\\"))
		strPath += _T("\\");

	strPath += _T("*.*");
	bContinue = finder.FindFile(strPath);
	while (bContinue)
	{
		bContinue = finder.FindNextFile();
		if (finder.IsDots()) // Ignore this item.
		{
			continue;
		}
		else if (finder.IsDirectory()) // Delete all sub item.
		{
			DeletePath(finder.GetFilePath());
			::RemoveDirectory((LPCTSTR)finder.GetFilePath());
		}
		else // Delete file.
		{
			::DeleteFile((LPCTSTR)finder.GetFilePath());
		}
	}

	finder.Close();

	strPath = strPath.Left(strPath.ReverseFind('\\'));
	::RemoveDirectory((LPCTSTR)strPath);
}

void CuScanApp::CleanDiskbyCapacity(int iCapacity)
{
	CString sSaveRootFolderName = Struct_PreferenceStruct.m_strBasic_FolderPath + "\\CMI_Results";

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString FolderSub, sDeleteFolderName;
	WORD wStartYear, wEndYear, wCurYear, wCurMonth, wCurDay;
	wStartYear = time.wYear - 1; // 2 year
	wEndYear = time.wYear;

	int i, j, k;
	double dCurDiskPer;
	BOOL bDiskCleanDone = FALSE;

	for (i = wStartYear; i <= wEndYear; i++)
	{
		wCurYear = i;

		for (j = 1; j <= 12; j++)
		{
			wCurMonth = j;

			for (k = 1; k <= 31; k++)
			{
				wCurDay = k;

				FolderSub.Format("\\%d", wCurYear);
				sDeleteFolderName = sSaveRootFolderName + FolderSub;

				FolderSub.Format("\\%d", wCurMonth);
				sDeleteFolderName = sDeleteFolderName + FolderSub;

				FolderSub.Format("\\%d", wCurDay);
				sDeleteFolderName = sDeleteFolderName + FolderSub;

				DeletePath(sDeleteFolderName);

				dCurDiskPer = GetDiskCapacityPercent();
				if (dCurDiskPer <= (double)iCapacity)
				{
					bDiskCleanDone = TRUE;
					break;
				}
			}

			FolderSub.Format("\\%d", wCurYear);
			sDeleteFolderName = sSaveRootFolderName + FolderSub;

			FolderSub.Format("\\%d", wCurMonth);
			sDeleteFolderName = sDeleteFolderName + FolderSub;

			if (CheckFolderHasItem(sDeleteFolderName) == FALSE)
			{
				::RemoveDirectory((LPCTSTR)sDeleteFolderName);
			}

			if (bDiskCleanDone)
				break;
		}

		FolderSub.Format("\\%d", wCurYear);
		sDeleteFolderName = sSaveRootFolderName + FolderSub;

		if (CheckFolderHasItem(sDeleteFolderName) == FALSE)
		{
			::RemoveDirectory((LPCTSTR)sDeleteFolderName);
		}

		if (bDiskCleanDone)
			break;
	}
}

void CuScanApp::CleanDiskbyDay()
{
	CString sSaveRootFolderName = Struct_PreferenceStruct.m_strBasic_FolderPath + "\\CMI_Results";

	int iDayTerm = Struct_PreferenceStruct.m_iDiskTerm;

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString FolderSub, sDeleteFolderName;
	WORD wStartYear, wEndYear, wEndMonth, wEndDay;
	WORD wCurYear, wCurMonth, wCurDay;
	wStartYear = time.wYear - 1; // 2 year

	SYSTEMTIME yesterDaytime;
	int iDayCnt = 1;
	yesterDaytime = FindingYesterDay(time);

	while (1)
	{
		if (iDayCnt > iDayTerm)
			break;

		yesterDaytime = FindingYesterDay(yesterDaytime);
		++iDayCnt;
	}

	wEndYear = yesterDaytime.wYear;
	wEndMonth = yesterDaytime.wMonth;
	wEndDay = yesterDaytime.wDay;

	int i, j, k;
	BOOL bDiskCleanDone = FALSE;

	for (i = wStartYear; i <= wEndYear; i++)
	{
		wCurYear = i;

		for (j = 1; j <= 12; j++)
		{
			wCurMonth = j;

			for (k = 1; k <= 31; k++)
			{
				wCurDay = k;

				FolderSub.Format("\\%d", wCurYear);
				sDeleteFolderName = sSaveRootFolderName + FolderSub;

				FolderSub.Format("\\%d", wCurMonth);
				sDeleteFolderName = sDeleteFolderName + FolderSub;

				FolderSub.Format("\\%d", wCurDay);
				sDeleteFolderName = sDeleteFolderName + FolderSub;

				DeletePath(sDeleteFolderName);

				if (wCurYear == wEndYear && wCurMonth == wEndMonth && wCurDay == wEndDay)
				{
					bDiskCleanDone = TRUE;
					break;
				}
			}

			FolderSub.Format("\\%d", wCurYear);
			sDeleteFolderName = sSaveRootFolderName + FolderSub;

			FolderSub.Format("\\%d", wCurMonth);
			sDeleteFolderName = sDeleteFolderName + FolderSub;

			if (CheckFolderHasItem(sDeleteFolderName) == FALSE)
			{
				::RemoveDirectory((LPCTSTR)sDeleteFolderName);
			}

			if (bDiskCleanDone)
				break;
		}

		FolderSub.Format("\\%d", wCurYear);
		sDeleteFolderName = sSaveRootFolderName + FolderSub;

		if (CheckFolderHasItem(sDeleteFolderName) == FALSE)
		{
			::RemoveDirectory((LPCTSTR)sDeleteFolderName);
		}

		if (bDiskCleanDone)
			break;
	}
}

double CuScanApp::GetDiskCapacityPercent()
{
	CString str;
	float TotalBytes, FreeBytes;
	float fTotal, fFree, fUsed;
	int iThres = Struct_PreferenceStruct.m_iDiskMax;

	ULARGE_INTEGER ulUserFree, ulTotal, ulRealFree;

	CString sDir;
	CString sSaveRootFolderName = Struct_PreferenceStruct.m_strBasic_FolderPath + "\\CMI_Results";
	sDir = sSaveRootFolderName.Left(sSaveRootFolderName.ReverseFind('\\') + 1);

	if (GetDiskFreeSpaceEx(sDir, &ulUserFree, &ulTotal, &ulRealFree))
	{
		TotalBytes = ulTotal.LowPart + (double)ulTotal.HighPart * (double)4294967296;
		fTotal = (float)(TotalBytes / 1024 / 1024 / 1024); // Convert (GB)
		FreeBytes = ulRealFree.LowPart + (double)ulRealFree.HighPart * (double)4294967296;
		fFree = (float)(FreeBytes / 1024 / 1024 / 1024); // Convert (GB)
		fUsed = fTotal - fFree;
	}

	double dDiskCapacityPer;
	dDiskCapacityPer = fUsed / fTotal * 100.0;

	return dDiskCapacityPer;
}

BOOL CuScanApp::CheckFolderHasItem(CString strPath)
{
	BOOL bFolderHasItem = FALSE;

	CFileFind finder;
	BOOL bContinue = TRUE;

	if (strPath.Right(1) != _T("\\"))
		strPath += _T("\\");

	strPath += _T("*.*");
	bContinue = finder.FindFile(strPath);
	while (bContinue)
	{
		bContinue = finder.FindNextFile();
		if (finder.IsDots()) // Ignore this item.
		{
			continue;
		}

		bFolderHasItem = TRUE;
	}

	finder.Close();

	return bFolderHasItem;
}

SYSTEMTIME CuScanApp::FindingYesterDay(SYSTEMTIME Todaytime)
{
	SYSTEMTIME YesterDayTime;

	if (Todaytime.wMonth == 1 && Todaytime.wDay == 1) // 1 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear - 1;
		YesterDayTime.wMonth = 12;
		YesterDayTime.wDay = 31;
	}
	else if (Todaytime.wMonth == 2 && Todaytime.wDay == 1) // 2 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 1;
		YesterDayTime.wDay = 31;
	}
	else if (Todaytime.wMonth == 3 && Todaytime.wDay == 1) // 3 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 2;
		YesterDayTime.wDay = 28;
	}
	else if (Todaytime.wMonth == 4 && Todaytime.wDay == 1) // 4 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 3;
		YesterDayTime.wDay = 31;
	}
	else if (Todaytime.wMonth == 5 && Todaytime.wDay == 1) // 5 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 4;
		YesterDayTime.wDay = 30;
	}
	else if (Todaytime.wMonth == 6 && Todaytime.wDay == 1) // 6 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 5;
		YesterDayTime.wDay = 31;
	}
	else if (Todaytime.wMonth == 7 && Todaytime.wDay == 1) // 7 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 6;
		YesterDayTime.wDay = 30;
	}
	else if (Todaytime.wMonth == 8 && Todaytime.wDay == 1) // 8 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 7;
		YesterDayTime.wDay = 31;
	}
	else if (Todaytime.wMonth == 9 && Todaytime.wDay == 1) // 9 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 8;
		YesterDayTime.wDay = 31;
	}
	else if (Todaytime.wMonth == 10 && Todaytime.wDay == 1) // 10 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 9;
		YesterDayTime.wDay = 30;
	}
	else if (Todaytime.wMonth == 11 && Todaytime.wDay == 1) // 11 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 10;
		YesterDayTime.wDay = 31;
	}
	else if (Todaytime.wMonth == 12 && Todaytime.wDay == 1) // 12 월 1일 일 경우
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = 11;
		YesterDayTime.wDay = 30;
	}
	else
	{
		YesterDayTime.wYear = Todaytime.wYear;
		YesterDayTime.wMonth = Todaytime.wMonth;
		YesterDayTime.wDay = Todaytime.wDay - 1;
	}

	return YesterDayTime;
}

// Multiple Defect Start
CString CuScanApp::GetVisionAndDefectName(int nDefectCode)
{
	CString strRetrunVal;

	switch (nDefectCode)
	{
	case DEFECT_NAME_DIRT:
		strRetrunVal = "배럴 이물";
		break;
	case DEFECT_NAME_SCRATCH:
		strRetrunVal = "배럴 스크래치";
		break;
	case DEFECT_NAME_STAIN:
		strRetrunVal = "배럴 얼룩";
		break;
	case DEFECT_NAME_DENT:
		strRetrunVal = "배럴 Dent";
		break;
	case DEFECT_NAME_CHIPPING:
		strRetrunVal = "배럴 칩핑";
		break;
	case DEFECT_NAME_EPOXY:
		strRetrunVal = "배럴 에폭시";
		break;
	case DEFECT_NAME_OUTSIDE_CT:
		strRetrunVal = "배럴 외곽오염";
		break;
	case DEFECT_NAME_PEEL_OFF:
		strRetrunVal = "배럴 Peel Off";
		break;
	case DEFECT_NAME_WHITEDOT:
		strRetrunVal = "배럴 WhiteDot";
		break;
	case DEFECT_NAME_LENS_CONTAMINATION:
		strRetrunVal = "Lens 오염";
		break; // 24.05.09 - v2646 - 불량명 변경(LENS_CONTAMINATION) - LeeGW
	case DEFECT_NAME_LENS_SCRATCH:
		strRetrunVal = "Lens 스크래치";
		break;
	case DEFECT_NAME_LENS_DIRT:
		strRetrunVal = "Lens 이물";
		break; // 24.05.09 - v2646 - 불량명 변경(LENS_DIRT) - LeeGW
	case DEFECT_NAME_LENS_WHITEDOT:
		strRetrunVal = "Lens WhiteDot";
		break;
	case DEFECT_NAME_EPOXYHOLE:
		strRetrunVal = "Epoxy Hole";
		break;
	case DEFECT_NAME_NORTH_SPRINGNG:
		strRetrunVal = "North Spring NG";
		break;
	case DEFECT_NAME_EAST_SPRINGNG:
		strRetrunVal = "East Spring NG";
		break;
	default:
		strRetrunVal = "UNKNOWN";
		break;
	}

	return strRetrunVal;
}

void CuScanApp::SaveMultipleDefectListLog(CString sPath, CString strLog)
{
	EnterCriticalSection(&m_csMULTIPLEDEFECTLOG);

	CString strFile;
	m_FileBase.CreatePath(sPath);
	CString strFileName;
	strFileName.Format("\\MultipleDefectList.txt");
	strFile.Format(sPath + strFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		LeaveCriticalSection(&m_csMULTIPLEDEFECTLOG);
		return;
	}

	try
	{
		CString strSave;
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			// Date / Time / Station / Model /  LOTNUM / Tray / Module / SN /* Vision */ Defect Name / Defect Count
			strSave.Format("Date\tTime\tStation\tModel\tLOTNUM\tTrayNo\tModuleNo\tSN\tDefect Name\tDefect Count\r\n");
			file.Write(strSave, strSave.GetLength());
		}

		SYSTEMTIME time;
		GetLocalTime(&time);

		CString strCurTime;
		strCurTime.Format("%04d-%02d-%02d\t%02d:%02d:%02d %03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

		strSave.Format("%s\t%s\r\n", strCurTime, strLog);

		file.Write(strSave, strSave.GetLength());
		file.Close();
	}
	catch (CFileException *pEx)
	{
		pEx->Delete();
	}

	LeaveCriticalSection(&m_csMULTIPLEDEFECTLOG);
}

void CuScanApp::ResetMultipleDefectInfo(int iModuleIdx)
{
	for (int iDefectIdx = 0; iDefectIdx < MAX_MULTIPLE_DEFECT_NUMBER; iDefectIdx++)
		m_stMultipleDefectInfo[iModuleIdx][iDefectIdx].Reset();
}

// Multiple Defect End
