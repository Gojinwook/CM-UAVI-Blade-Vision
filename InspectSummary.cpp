// InspectSummary.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "InspectSummary.h"
#include "afxdialogex.h"
#include "IniFileCS.h"

#include <thread>

constexpr UINT RESTART_CHECK_TIME = 1000;
constexpr UINT PCSTATUS_CHECK_TIME = 1000;
constexpr UINT PCSTATUS_SAVE_TIME = 14400000; 	// 4 hours in milliseconds
constexpr UINT ADJ_CONNECT_CHECK_TIME = 60000; 	// 1 minute in milliseconds

constexpr BOOL DEBUG_RESTART_PROCESS = FALSE;

// CInspectSummary 대화 상자입니다.
CInspectSummary* CInspectSummary::m_pInstance = NULL;

CInspectSummary* CInspectSummary::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CInspectSummary();
		if(!m_pInstance->m_hWnd) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			m_pInstance->Create(IDD_INSPECT_SUMMARY, pFrame->GetActiveView());
			if (bShowFlag) m_pInstance->Show();
		}
	}
	return m_pInstance;
}
void CInspectSummary::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();
	SAFE_DELETE(m_pInstance);
}

void CInspectSummary::Show()
{
	ShowWindow(SW_HIDE);
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	ShowWindow(SW_SHOW);
}

IMPLEMENT_DYNAMIC(CInspectSummary, CDialog)

	CInspectSummary::CInspectSummary(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectSummary::IDD, pParent)
{
	SetPosition(VIEW1_DLG3_LEFT+520, VIEW1_DLG3_TOP+40, VIEW1_DLG3_WIDTH-530, VIEW1_DLG3_HEIGHT+10);
	//m_strOKNG = "";

	m_iDxTrayNo = 1;
	m_iDxLineNo = 1;
	m_bDxAllignImg = TRUE;					// added for CMI 3000 2000
	m_bUseAllignImg = TRUE;					// added for CMI 3000 2000
	m_bDxAccelMatching = TRUE;				// added for CMI 3000 2000
	m_bUseAccelMatching = TRUE;				// added for CMI 3000 2000
	m_bDxFobTest = FALSE;
	m_bUseFobTest = FALSE;
	m_bRestartBtnClicked = FALSE;
	bIsRestart = FALSE;
	dProcessStartTime = 0;
	m_bUniformityCheckMode = FALSE;
	bIsSatusCheckRetry = FALSE;

	m_bUseBLInsp = TRUE;
	m_iIndexNo = 0;
	m_iPocketNo = 0;
	m_sInspStatus = "Ready";
}

CInspectSummary::~CInspectSummary()
{
}

void CInspectSummary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_USE_BL_INSP, m_chkUseBLInsp);
	DDX_Check(pDX, IDC_CHK_UNIFORMITY_CHECK, m_bUniformityCheckMode);
	DDX_Check(pDX, IDC_CHK_FOBTEST, m_bDxFobTest);
	DDX_Check(pDX, IDC_CHK_ALLIGN_IMG, m_bDxAllignImg);
	DDX_Check(pDX, IDC_CHK_ACCEL_MATCHING, m_bDxAccelMatching);
	DDX_Text(pDX, IDC_EDIT_TRAY_NO, m_iDxTrayNo);
	DDX_Text(pDX, IDC_EDIT_STAGE_NO, m_iDxLineNo);
	DDX_Control(pDX, IDC_PROGRESS_DISK_HDDC, m_ctrlProgressHddC);
	DDX_Control(pDX, IDC_PROGRESS_DISK_HDDD, m_ctrlProgressHddD);
	DDX_Control(pDX, IDC_STATIC_ADJ_CONNECT, m_LabelADJConnected);
	DDX_Control(pDX, IDC_STATIC_INDEX_NO, m_lbIndexNo);
	DDX_Control(pDX, IDC_STATIC_POCKET_NO, m_lbPocketNo);
	DDX_Control(pDX, IDC_STATIC_INSPECT_STATUS, m_lbInspStatus);
}


BEGIN_MESSAGE_MAP(CInspectSummary, CDialog)
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_MFCBUTTON_INSPECT_START, &CInspectSummary::OnBnClickedMfcbuttonInspectStart)
	ON_MESSAGE(WM_INSPECTION_LOOP, OnInspectionLoop)
	ON_BN_CLICKED(IDC_BN_FOBSET, &CInspectSummary::OnBnClickedBnFobset)
	ON_BN_CLICKED(IDC_CHK_FOBTEST, &CInspectSummary::OnClickedChkFobtest)
	ON_BN_CLICKED(IDC_CHK_ALLIGN_IMG, &CInspectSummary::OnBnClickedChkAllignImg)
	ON_BN_CLICKED(IDC_CHK_ACCEL_MATCHING, &CInspectSummary::OnBnClickedChkAccelMatching)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRAY_NO, &CInspectSummary::OnDeltaposSpinTrayNo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STAGE_NO, &CInspectSummary::OnDeltaposSpinStageNo)
	ON_BN_CLICKED(IDC_MFCBUTTON_STATUS_CHECK, &CInspectSummary::OnBnClickedMfcbuttonStatusCheck)
	ON_BN_CLICKED(IDC_CHK_USE_BL_INSP, &CInspectSummary::OnBnClickedChkBlInspUse)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CInspectSummary 메시지 처리기입니다.
void CInspectSummary::CallbuttonInspectStart()
{
	OnBnClickedMfcbuttonInspectStart();
}

void CInspectSummary::OnBnClickedMfcbuttonInspectStart()
{
#ifdef INLINE_MODE
	if (m_bRestartBtnClicked==FALSE)
		m_bRestartBtnClicked = TRUE;
	else
	{
		if(IDNO == AfxMessageBox("재시작 버튼을 이미 누른 상태입니다!!!\n작업을 진행하시겠습니까?", MB_YESNO))
			return;
	}
#endif

	UpdateData(TRUE);
	if (m_iDxTrayNo < 1) m_iDxTrayNo = 1;
	if (m_iDxLineNo < 1) m_iDxLineNo = 1;
	UpdateData(FALSE);

	THEAPP.m_pInspectSummary->SetDlgItemText(IDC_STATIC_INSPECT_STATUS, "Inspection Started");
	THEAPP.SaveLog("InspectSummary Button - InspectStart");

#ifdef INLINE_MODE

	THEAPP.m_pHandlerService->Set_StatusUpdate(HS_RUN);

	if (THEAPP.m_pInspectService->m_bInspectDone == TRUE || THEAPP.m_pInspectService->m_bInspectRepeat == TRUE)
	{
		/////// 비전 다운 등의 이유로 메뉴얼로 다시 시작할 때 마지막 검사상태를 로드한다
		CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
		CIniFileCS TXT_Status(strStatusFileName);
		CString sSection = "Status";
		THEAPP.m_pModelDataManager->m_sModelName = TXT_Status.Get_String(sSection, "ModelID", "Default");
		THEAPP.m_pInspectService->m_sLotID_H = TXT_Status.Get_String(sSection, "LotID", "Default");
		THEAPP.m_pInspectService->m_iLotTrayAmt_H = TXT_Status.Get_Integer(sSection, "TrayAmount", 0);
		THEAPP.m_pInspectService->m_iLotModuleAmt_H = TXT_Status.Get_Integer(sSection, "ModuleAmount", 0);
		THEAPP.m_pInspectService->m_iTrayNo_H = m_iDxTrayNo;
		THEAPP.m_pInspectService->m_iLineNo_H = m_iDxLineNo;

		THEAPP.m_pHandlerService->SetLotId(THEAPP.m_pInspectService->m_sLotID_H);
		THEAPP.m_pHandlerService->SetTrayNumber(THEAPP.m_pInspectService->m_iTrayNo_H);
		THEAPP.m_pHandlerService->SetLineNumber(THEAPP.m_pInspectService->m_iLineNo_H);

		THEAPP.m_pHandlerService->Set_PositionRequest();

		BOOL bBarcodeScanEnd = TXT_Status.Get_Bool(sSection, "BarcodeScanComplete", FALSE);
		BOOL bInspectScanEnd = TXT_Status.Get_Bool(sSection, "InspectScanComplete", FALSE);

		if (bBarcodeScanEnd == FALSE || bInspectScanEnd == FALSE)		// 트레이를 별도로 마무리해야 할 필요가 없으면 / Scan Complete 전에 다운 / 자리에 모듈있으므로 Load Complete 받은 것 처럼 검사 처리
		{
			THEAPP.m_pInspectService->ReadyToStart(TRUE);		// CMI3000 Full-Auto는 처음부터 다시 검사 시작한다. semi-auto는 GetLoadComplete 호출

			THEAPP.m_pInspectService->m_iPrevTrayNo_H = THEAPP.m_pInspectService->m_iTrayNo_H;

			THEAPP.m_pInspectService->InspectionMove(TRUE);		// 스캔과 검사 시작
		}
		else					// ScanComplete와 InspectComplete 사이에 비전 종료/재시작 시 트레이를 별도로 마무리하고 LoadComplete기다린다
		{
			const int iTrayModuleMax = THEAPP.m_iModuleNumberOneLine;
			CString sTrayResult = TXT_Status.Get_String(sSection, "TrayResult", "MM");
			CString sNGCode="";
			CString sBadName="";
			CString sTrayBarcodeResult = "";

			// 모두 불량 처리
			sTrayResult = "";
			for (int ii = 0; ii < THEAPP.m_iModuleNumberOneLine; ii++)
			{
				if (ii) {
					sTrayResult += ",";
					sNGCode += ",";
					sBadName += ",";
					sTrayBarcodeResult += ",";
				}
				sTrayResult += "N";

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_BARCODE_ERROR);

				sNGCode += sNGCodeTemp;
				sBadName += "Barcode 미인식";
				sTrayBarcodeResult += BARCODE_STATUS_DEFAULT;
			}

			THEAPP.m_pHandlerService->Set_InspectComplete(THEAPP.m_pInspectService->m_sLotID_H, 
				THEAPP.m_pInspectService->m_iTrayNo_H, THEAPP.m_pInspectService->m_iLineNo_H, THEAPP.m_iModuleNumberOneLine, 
				sTrayResult, sTrayBarcodeResult); // 검사 종료 신호 날림. PC 검사 ready 상태 set함.  //연산 쪽에 있어야함.

			THEAPP.m_pInspectService->m_bInspectDone = TRUE;		// 이게 끝나야 GetLoadComplete 가 완전히 수행된다
			THEAPP.SaveLog("++inspect done");

			// TXT_Status.Set_Bool (sSection, "TrayMustEnd", FALSE);
			TXT_Status.Set_Bool (sSection, "BarcodeScanComplete", TRUE);
			TXT_Status.Set_Bool (sSection, "InspectScanComplete", TRUE);
			TXT_Status.Set_String (sSection, "TrayResult", "NN");
			TXT_Status.Set_String (sSection, "Status", "Inspection Started");
		}
	}

	THEAPP.SaveLog("모션 시작");

#else	// end INLINE_MODE	

	THEAPP.m_pAlgorithm->m_tInspectStartTime = GetTickCount();
	////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
	if(THEAPP.m_pInspectResultDlg->CurrentLotID != THEAPP.m_pInspectResultDlg->LastLotID)
	{
		THEAPP.m_pInspectResultDlg->SetDlgItemText (IDC_EDIT_COUNTTRAY,"0");
	}
	THEAPP.m_pInspectResultDlg->LastLotID = THEAPP.m_pInspectResultDlg->CurrentLotID;

	CheckLotIDAndChangeModel (THEAPP.m_pInspectResultDlg->CurrentLotID);

	CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS INI(strStatusFileName);
	CString strSection = "Status";
	INI.Set_String (strSection, "LotID", THEAPP.m_pInspectResultDlg->CurrentLotID);
	INI.Set_String (strSection, "ModelID", THEAPP.m_pModelDataManager->m_sModelName);
	////////////////////////////////// <==== changed for CMI3000 2000 ///////////////////////////////////////////////////

	THEAPP.iModuleCountOneLot = 0;
	for (int i=0; i<MAX_MODULE_ONE_LOT; i++)
	{
		THEAPP.m_iModuleDefectCenterX[i] = -1;
		THEAPP.m_iModuleDefectCenterY[i] = -1;
		THEAPP.m_sModuleDefectName[i] = _T("OK");
	}

	THEAPP.m_pInspectService->OfflineInspection();

#endif	// OFFLINE
	THEAPP.m_pInspectService->m_bInspectRepeat = FALSE;


	//Multiple Defect Test Start
#ifndef INLINE_MODE // == OFFLINE MODE
	if (THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarm)
	{
		int nVirtualMzIdx = THEAPP.m_stVirtualMagazineNo.GetVirtualMagazineNo(THEAPP.m_pInspectResultDlg->CurrentLotID);
		THEAPP.m_pInspectService->LotDetailDefectError(nVirtualMzIdx,THEAPP.m_pInspectResultDlg->CurrentLotID);
	}
#endif
	//Multiple Defect Test End
}

void CInspectSummary::OnBnClickedMfcbuttonInspectPass()
{
	return;

	THEAPP.m_pInspectSummary->SetDlgItemText(IDC_STATIC_INSPECT_STATUS, "Tray reset");
	THEAPP.SaveLog("InspectSummery Button - InspectReset");
	GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);

#ifdef INLINE_MODE
	THEAPP.m_pCameraManager->GrabErrorPostProcess();
	THEAPP.m_pHandlerService->Set_StatusUpdate(HS_STOP);	//// added for CMI3000 2000
	Sleep(1);
	CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS TXT_Status(strStatusFileName);
	CString sSection = "Status";
	TXT_Status.Set_Bool (sSection, "BarcodeScanComplete", FALSE);
	TXT_Status.Set_Bool (sSection, "InspectScanComplete", FALSE);
	TXT_Status.Set_String (sSection, "TrayResult", "NN");
	TXT_Status.Set_String (sSection, "Status", "Reset");
	THEAPP.m_pHandlerService->Set_StatusUpdate(HS_RESET);
	THEAPP.m_pInspectService->m_bInspectRepeat = TRUE;		// 이게 끝나야 다음 트레이를 위한 GetLoadComplete 가 완전히 수행된다
#endif
}

void CInspectSummary::OnBnClickedMfcbuttonStatusCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (IDNO == AfxMessageBox("알람 발생 시에만 버튼을 눌러주세요.\n작업을 진행하시겠습니까?", MB_YESNO))
		return;

	UpdateData(TRUE);
	if (m_iDxTrayNo < 1) m_iDxTrayNo = 1;
	if (m_iDxLineNo < 1) m_iDxLineNo = 1;
	UpdateData(FALSE);

	CString strLog;

	THEAPP.m_pInspectSummary->SetDlgItemText(IDC_STATIC_INSPECT_STATUS, "Status Check");
	THEAPP.SaveLog("InspectSummery Button - Status Check");

	THEAPP.m_pLogDlg->WriteLog("ㅣ ****** 상태감지 시작 ****** ㅣ", LOG_COLOR_BLACK, 260, FALSE, TRUE);

#ifdef INLINE_MODE
	THEAPP.m_pInspectService->m_iTrayNo_H = m_iDxTrayNo;
	THEAPP.m_pInspectService->m_iLineNo_H = m_iDxLineNo;

	THEAPP.m_pHandlerService->SetLotId(THEAPP.m_pInspectService->m_sLotID_H);
	THEAPP.m_pHandlerService->SetTrayNumber(THEAPP.m_pInspectService->m_iTrayNo_H);
	THEAPP.m_pHandlerService->SetLineNumber(THEAPP.m_pInspectService->m_iLineNo_H);
	
	strLog.Format("[상태] LotID: %s, TrayNo : %d, LineNo : %d", THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, THEAPP.m_pInspectService->m_iLineNo_H);
	THEAPP.m_pLogDlg->WriteLog(strLog, LOG_COLOR_BLACK, 260, FALSE, FALSE);

	THEAPP.m_pInspectService->SetCycleStopStatus(TRUE);
	THEAPP.m_pHandlerService->Set_StatusUpdate(HS_STOP);

	Sleep(200);

	THEAPP.m_pHandlerService->Terminate_Handler();
	THEAPP.m_pHandlerService->DeleteInstance();
	THEAPP.m_pLogDlg->WriteLog("[요청] Handler 통신 초기화...", LOG_COLOR_ORANGE, 260, FALSE, FALSE);

	Sleep(100);

	THEAPP.m_pHandlerService = CHandlerService::GetInstance();
	THEAPP.m_pHandlerService->Initialize_Handler();
	THEAPP.m_pLogDlg->WriteLog("[요청] Handler 통신 재연결...", LOG_COLOR_ORANGE, 260, FALSE, FALSE);

	Sleep(200);

	CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS TXT_Status(strStatusFileName);

	CString sSection = "Status";
	BOOL bBarcodeScanEnd = TXT_Status.Get_Bool(sSection, "BarcodeScanComplete", FALSE);
	BOOL bInspectScanEnd = TXT_Status.Get_Bool(sSection, "InspectScanComplete", FALSE);

	if (bBarcodeScanEnd==TRUE)
		THEAPP.m_pLogDlg->WriteLog("[상태] 바코드 이미지 스캔 완료", LOG_COLOR_BLACK, 260, FALSE, FALSE);
	else
		THEAPP.m_pLogDlg->WriteLog("[상태] 바코드 이미지 스캔 미완료", LOG_COLOR_BLACK, 260, FALSE, FALSE);
	
	if (bInspectScanEnd == TRUE)
		THEAPP.m_pLogDlg->WriteLog("[상태] 검사 이미지 스캔 완료", LOG_COLOR_BLACK, 260, FALSE, FALSE);
	else
		THEAPP.m_pLogDlg->WriteLog("[상태] 검사 이미지 스캔 미완료", LOG_COLOR_BLACK, 260, FALSE, FALSE);

	THEAPP.m_pInspectService->SetCycleStopStatus(FALSE);
	THEAPP.m_pHandlerService->Set_StatusUpdate(HS_RUN);

	if ((bBarcodeScanEnd == TRUE) && (bInspectScanEnd == TRUE))
	{
		if ((IDYES == AfxMessageBox("카메라 위치가 시작 위치일 경우 *예*를 눌러주세요.", MB_YESNO)))
		{
			if (IDYES == AfxMessageBox("검사미완료 알람일 경우 *예*를 눌러주세요.", MB_YESNO))
			{
				THEAPP.m_pLogDlg->WriteLog("[요청] Handler 검사완료 확인 요청 시도...", LOG_COLOR_ORANGE, 260, FALSE, FALSE);

				THEAPP.m_pHandlerService->ResetReplyReceived();
				THEAPP.m_pHandlerService->Set_InspectComplete();
			}
			else
			{
				if (THEAPP.m_pInspectService->m_iLineNo_H == THEAPP.m_iTrayLineNumber)
				{
					THEAPP.m_pInspectService->m_iTrayNo_H++;
					THEAPP.m_pInspectService->m_iLineNo_H = 1;
				}
				else
					THEAPP.m_pInspectService->m_iLineNo_H++;

				THEAPP.m_pHandlerService->SetTrayNumber(THEAPP.m_pInspectService->m_iTrayNo_H);
				THEAPP.m_pHandlerService->SetLineNumber(THEAPP.m_pInspectService->m_iLineNo_H);

				TXT_Status.Set_Integer(sSection, "TrayNo", THEAPP.m_pInspectService->m_iTrayNo_H);
				TXT_Status.Set_Integer(sSection, "LineNo", THEAPP.m_pInspectService->m_iLineNo_H);

				strLog.Format("[상태](수정) LotID: %s, TrayNo : %d, LineNo : %d", THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, THEAPP.m_pInspectService->m_iLineNo_H);
				THEAPP.m_pLogDlg->WriteLog(strLog, LOG_COLOR_BLACK, 260, FALSE, FALSE);

				Sleep(200);

				THEAPP.m_pLogDlg->WriteLog("[요청] Handler 스캔요청 시도...", LOG_COLOR_ORANGE, 260, FALSE, FALSE);

				THEAPP.m_pInspectService->ReadyToStart(TRUE);
				THEAPP.m_pInspectService->m_iPrevTrayNo_H = THEAPP.m_pInspectService->m_iTrayNo_H;
				THEAPP.m_pInspectService->InspectionMove(TRUE);
			}
		}
		else
		{
			Sleep(200);

			THEAPP.m_pLogDlg->WriteLog("[요청] Handler 스캔 요청 시도...", LOG_COLOR_ORANGE, 260, FALSE, FALSE);

			THEAPP.m_pInspectService->ReadyToStart(TRUE);
			THEAPP.m_pInspectService->m_iPrevTrayNo_H = THEAPP.m_pInspectService->m_iTrayNo_H;
			THEAPP.m_pInspectService->InspectionMove(TRUE);

		}
	}
	else
	{
		Sleep(200);

		THEAPP.m_pLogDlg->WriteLog("[요청] Handler 스캔 요청 시도...", LOG_COLOR_ORANGE, 260, FALSE, FALSE);

		THEAPP.m_pInspectService->ReadyToStart(TRUE);
		THEAPP.m_pInspectService->m_iPrevTrayNo_H = THEAPP.m_pInspectService->m_iTrayNo_H;
		THEAPP.m_pInspectService->InspectionMove(TRUE);

	}

	bIsSatusCheckRetry = FALSE;
#endif
}

void CInspectSummary::OnBnClickedMfcbuttonInspectPause()
{
	return;

	THEAPP.m_pInspectSummary->SetDlgItemText(IDC_STATIC_INSPECT_STATUS, "Pause");
	THEAPP.SaveLog("InspectSummary Button - InspectStop");
	GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
#ifdef INLINE_MODE
	THEAPP.m_pHandlerService->Set_StatusUpdate(HS_PAUSE);
#endif
}

void CInspectSummary::OnBnClickedMfcbuttonInspectCyclestop()
{
	//GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
	THEAPP.m_pHandlerService->Set_StatusUpdate(HS_CYCLE_STOP);	//// added for CMI3000 2000
	THEAPP.m_pHandlerService->m_nInspectPCStatus = HS_CYCLE_STOP;
}

void CInspectSummary::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	MoveWindow(m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	lpwndpos->flags |= SWP_NOMOVE; 
	CDialog::OnWindowPosChanging(lpwndpos); 
}

// Lot 폴더 내의 Tray-xx 폴더 및 하부 폴더를 생성하는 함수
void CInspectSummary::CheckPrevSaveFolder()
{
	CString FolderName = THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath+"\\CMI_Results";
	
	CString FolderSub;
	THEAPP.m_FileBase.CreateFolder(FolderName);

	SYSTEMTIME time;
	GetLocalTime(&time);
	
	FolderSub.Format("\\%d",time.wYear);
	THEAPP.m_FileBase.CreateFolder(FolderName+FolderSub);
	FolderName = FolderName+FolderSub;

	FolderSub.Format("\\%d",time.wMonth);
	THEAPP.m_FileBase.CreateFolder(FolderName+FolderSub);
	FolderName = FolderName+FolderSub;

	FolderSub.Format("\\%d",time.wDay);
	THEAPP.m_FileBase.CreateFolder(FolderName+FolderSub);
	FolderName = FolderName+FolderSub;

	THEAPP.m_FileBase.m_strResultFolderPrev.Format(FolderName);

	// ******************* 어제 파일을 접근 하기 위함 *************************
	SYSTEMTIME yesterDaytime;
	CString FolderYesterDaySub;
	CString FolderYesterDayName = THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath+"\\CMI_Results";
	THEAPP.m_FileBase.CreateFolder(FolderYesterDayName);
	yesterDaytime = THEAPP.m_pInspectResultDlg->FindingYesterDay(time);

	FolderYesterDaySub.Format("\\%d",yesterDaytime.wYear);
	THEAPP.m_FileBase.CreateFolder(FolderYesterDayName+FolderYesterDaySub);
	FolderYesterDayName = FolderYesterDayName+FolderYesterDaySub;

	FolderYesterDaySub.Format("\\%d",yesterDaytime.wMonth);
	THEAPP.m_FileBase.CreateFolder(FolderYesterDayName+FolderYesterDaySub);
	FolderYesterDayName = FolderYesterDayName+FolderYesterDaySub;

	FolderYesterDaySub.Format("\\%d",yesterDaytime.wDay);
	THEAPP.m_FileBase.CreateFolder(FolderYesterDayName+FolderYesterDaySub);
	FolderYesterDayName = FolderYesterDayName+FolderYesterDaySub;

	THEAPP.m_FileBase.m_strYesterDayResultFolderPrev.Format(FolderYesterDayName);
	// *************************************************************************

	CString LotID;
	CString TrayNumber;
	THEAPP.m_pInspectResultDlg->GetDlgItemText(IDC_EDIT_LOT_ID, LotID);
	THEAPP.m_pInspectResultDlg->GetDlgItemText(IDC_EDIT_COUNTTRAY, TrayNumber);
	int iTrayNumber = atoi((LPSTR)(LPCSTR)TrayNumber);
	TrayNumber.Format("%d", iTrayNumber+1);			// 누적 tray 숫자를 증가시킨다
	THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_COUNTTRAY, TrayNumber);

	if(iTrayNumber == 0)
	{
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_OKCOUNT,0);
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NGCOUNT,0);
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_BARCODE_ERROR_LOT,0);
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NUMBER_OF_MODULE,0);
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_YIELD,"0.0");
	}

	///////////// 저장 폴더를 생성한다 /////////////////////////////////////////////
	FolderSub.Format("\\%s",LotID);
	THEAPP.m_FileBase.CreateFolder(FolderName+FolderSub);
	THEAPP.m_FileBase.m_strCurrentLotFolder = FolderName+FolderSub;
	THEAPP.m_FileBase.CreateFolder(FolderName+FolderSub+"\\Tray-"+TrayNumber);
	
	CString FolderTray;
	FolderTray = FolderName+FolderSub+"\\Tray-"+TrayNumber;

	THEAPP.m_FileBase.m_strTrayResultFolderPrev = FolderTray;
	THEAPP.m_FileBase.m_strOverayImageFolderPrev.Format(FolderTray + "\\ResultImage");
	THEAPP.m_FileBase.m_strOriImageFolderPrev.Format(FolderTray + "\\RawImage");
	THEAPP.m_FileBase.m_strReviewImageFolder.Format(FolderTray + "\\ReviewImage");	// 24.06.12 - v2604 - Review Image 저장 - LeeGW
	THEAPP.m_FileBase.m_strFAIImageFolder.Format(FolderTray + "\\FAIImage");
	THEAPP.m_FileBase.m_strADJRstPrev.Format(FolderTray + "\\ADJImage");	// AI 크롭 이미지 - LeeGW
	THEAPP.m_FileBase.m_strADJOKRstPrev.Format(THEAPP.m_FileBase.m_strADJRstPrev + "\\OKImage");
	THEAPP.m_FileBase.m_strADJNGRstPrev.Format(THEAPP.m_FileBase.m_strADJRstPrev + "\\NGImage");
	THEAPP.m_FileBase.m_strADJSkipRstPrev.Format(THEAPP.m_FileBase.m_strADJRstPrev + "\\SKIPImage");

	if (THEAPP.Struct_PreferenceStruct.m_bSaveResultImage == TRUE)
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strOverayImageFolderPrev);

	if (THEAPP.Struct_PreferenceStruct.m_bSaveRawImage == TRUE)
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strOriImageFolderPrev);

	if (THEAPP.Struct_PreferenceStruct.m_bSaveReviewImage == TRUE)
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strReviewImageFolder);

	if (THEAPP.Struct_PreferenceStruct.m_bSaveFAIImage == TRUE)
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strFAIImageFolder);
		
	if (THEAPP.Struct_PreferenceStruct.m_bIsUseAIInsp == TRUE)
	{
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strADJRstPrev);
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strADJOKRstPrev);
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strADJNGRstPrev);
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strADJSkipRstPrev);
	}	

	///////////// NG 저장 폴더를 생성한다 /////////////////////////////////////////////

	if (THEAPP.Struct_PreferenceStruct.m_bSaveNGImage)
	{
		CString sNGFolder;
		sNGFolder = FolderName + "\\NG";
		THEAPP.m_FileBase.CreateFolder(sNGFolder);

		FolderSub.Format("\\%s",LotID);
		THEAPP.m_FileBase.CreateFolder(sNGFolder+FolderSub);
		THEAPP.m_FileBase.CreateFolder(sNGFolder+FolderSub+"\\Tray-"+TrayNumber);

		FolderTray = sNGFolder+FolderSub+"\\Tray-"+TrayNumber;

		THEAPP.m_FileBase.CreateFolder(FolderTray+"\\ResultImage");
		THEAPP.m_FileBase.CreateFolder(FolderTray+"\\RawImage");
		THEAPP.m_FileBase.CreateFolder(FolderTray+"\\ReviewImage");	// 24.06.12 - v2604 - Review Image 저장 - LeeGW

		THEAPP.m_FileBase.m_strNGOverayImageFolderPrev.Format(FolderTray+"\\ResultImage");
		THEAPP.m_FileBase.m_strNGOriImageFolderPrev.Format(FolderTray+"\\RawImage");
		THEAPP.m_FileBase.m_strNGReviewImageFolder.Format(FolderTray+"\\ReviewImage");	// 24.06.12 - v2604 - Review Image 저장 - LeeGW

	}
}

void CInspectSummary::CheckPrevSaveFolder(CString sLotID, CString sTrayNo)
{

	CString FolderName = THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath + "\\CMI_Results";


	CString FolderSub;
	THEAPP.m_FileBase.CreateFolder(FolderName);

	SYSTEMTIME time;
	GetLocalTime(&time);

	FolderSub.Format("\\%d", time.wYear);
	THEAPP.m_FileBase.CreateFolder(FolderName + FolderSub);
	FolderName = FolderName + FolderSub;

	FolderSub.Format("\\%d", time.wMonth);
	THEAPP.m_FileBase.CreateFolder(FolderName + FolderSub);
	FolderName = FolderName + FolderSub;

	FolderSub.Format("\\%d", time.wDay);
	THEAPP.m_FileBase.CreateFolder(FolderName + FolderSub);
	FolderName = FolderName + FolderSub;

	THEAPP.m_FileBase.m_strResultFolderPrev.Format(FolderName);

	// ******************* 어제 파일을 접근 하기 위함 *************************
	SYSTEMTIME yesterDaytime;
	CString FolderYesterDaySub;
	CString FolderYesterDayName = THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath + "\\CMI_Results";
	THEAPP.m_FileBase.CreateFolder(FolderYesterDayName);
	yesterDaytime = THEAPP.m_pInspectResultDlg->FindingYesterDay(time);

	FolderYesterDaySub.Format("\\%d", yesterDaytime.wYear);
	THEAPP.m_FileBase.CreateFolder(FolderYesterDayName + FolderYesterDaySub);
	FolderYesterDayName = FolderYesterDayName + FolderYesterDaySub;

	FolderYesterDaySub.Format("\\%d", yesterDaytime.wMonth);
	THEAPP.m_FileBase.CreateFolder(FolderYesterDayName + FolderYesterDaySub);
	FolderYesterDayName = FolderYesterDayName + FolderYesterDaySub;

	FolderYesterDaySub.Format("\\%d", yesterDaytime.wDay);
	THEAPP.m_FileBase.CreateFolder(FolderYesterDayName + FolderYesterDaySub);
	FolderYesterDayName = FolderYesterDayName + FolderYesterDaySub;

	THEAPP.m_FileBase.m_strYesterDayResultFolderPrev.Format(FolderYesterDayName);
	// *************************************************************************

	int iTrayNumber = atoi((LPSTR)(LPCSTR)sTrayNo);
	sTrayNo.Format("%d", iTrayNumber);			// 누적 tray 숫자를 증가시킨다

	///////////// 저장 폴더를 생성한다 /////////////////////////////////////////////
	FolderSub.Format("\\%s", sLotID);
	THEAPP.m_FileBase.CreateFolder(FolderName + FolderSub);
	THEAPP.m_FileBase.m_strCurrentLotFolder = FolderName + FolderSub;
	THEAPP.m_FileBase.CreateFolder(FolderName + FolderSub + "\\Tray-" + sTrayNo);

	CString FolderTray;
	FolderTray = FolderName + FolderSub + "\\Tray-" + sTrayNo;

	THEAPP.m_FileBase.m_strTrayResultFolderPrev = FolderTray;
	THEAPP.m_FileBase.m_strOverayImageFolderPrev.Format(FolderTray + "\\ResultImage");
	THEAPP.m_FileBase.m_strOriImageFolderPrev.Format(FolderTray + "\\RawImage");
	THEAPP.m_FileBase.m_strReviewImageFolder.Format(FolderTray + "\\ReviewImage");	// 24.06.12 - v2604 - Review Image 저장 - LeeGW
	THEAPP.m_FileBase.m_strFAIImageFolder.Format(FolderTray + "\\FAIImage");
	THEAPP.m_FileBase.m_strADJRstPrev.Format(FolderTray + "\\ADJImage");	// AI 크롭 이미지 - LeeGW
	THEAPP.m_FileBase.m_strADJOKRstPrev.Format(THEAPP.m_FileBase.m_strADJRstPrev + "\\OKImage");
	THEAPP.m_FileBase.m_strADJNGRstPrev.Format(THEAPP.m_FileBase.m_strADJRstPrev + "\\NGImage");
	THEAPP.m_FileBase.m_strADJSkipRstPrev.Format(THEAPP.m_FileBase.m_strADJRstPrev + "\\SKIPImage");

	if(THEAPP.Struct_PreferenceStruct.m_bSaveResultImage == TRUE)
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strOverayImageFolderPrev);

	if (THEAPP.Struct_PreferenceStruct.m_bSaveRawImage == TRUE)
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strOriImageFolderPrev);

	if (THEAPP.Struct_PreferenceStruct.m_bSaveReviewImage == TRUE)
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strReviewImageFolder);

	if (THEAPP.Struct_PreferenceStruct.m_bSaveFAIImage == TRUE)
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strFAIImageFolder);
	
	if (THEAPP.Struct_PreferenceStruct.m_bIsUseAIInsp == TRUE)
	{
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strADJRstPrev);
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strADJOKRstPrev);
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strADJNGRstPrev);
		THEAPP.m_FileBase.CreateFolder(THEAPP.m_FileBase.m_strADJSkipRstPrev);
	}

	///////////// NG 저장 폴더를 생성한다 /////////////////////////////////////////////

	if (THEAPP.Struct_PreferenceStruct.m_bSaveNGImage)
	{
		CString sNGFolder;
		sNGFolder = FolderName + "\\NG";
		THEAPP.m_FileBase.CreateFolder(sNGFolder);

		FolderSub.Format("\\%s", sLotID);
		THEAPP.m_FileBase.CreateFolder(sNGFolder + FolderSub);
		THEAPP.m_FileBase.CreateFolder(sNGFolder + FolderSub + "\\Tray-" + sTrayNo);

		FolderTray = sNGFolder + FolderSub + "\\Tray-" + sTrayNo;

		THEAPP.m_FileBase.CreateFolder(FolderTray + "\\ResultImage");
		THEAPP.m_FileBase.CreateFolder(FolderTray + "\\RawImage");
		THEAPP.m_FileBase.CreateFolder(FolderTray + "\\ReviewImage");	// 24.06.12 - v2604 - Review Image 저장 - LeeGW

		THEAPP.m_FileBase.m_strNGOverayImageFolderPrev.Format(FolderTray + "\\ResultImage");
		THEAPP.m_FileBase.m_strNGOriImageFolderPrev.Format(FolderTray + "\\RawImage");
		THEAPP.m_FileBase.m_strNGReviewImageFolder.Format(FolderTray + "\\ReviewImage");	// 24.06.12 - v2604 - Review Image 저장 - LeeGW

	}
}

// Lot 폴더 내의 Tray-xx 폴더 개수 카운트해서 "작업Tray개수" 에디트박스에 기입하는 함수
void CInspectSummary::CheckLastTrayFolder()
{
	CFileFind ff;
	
	CString FolderName = THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath+"\\CMI_Results";

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString FolderSub;
	FolderSub.Format("\\%d",time.wYear);
	THEAPP.m_FileBase.CreateFolder(FolderName+FolderSub);
	FolderName = FolderName+FolderSub;

	FolderSub.Format("\\%d",time.wMonth);
	THEAPP.m_FileBase.CreateFolder(FolderName+FolderSub);
	FolderName = FolderName+FolderSub;

	FolderSub.Format("\\%d",time.wDay);
	THEAPP.m_FileBase.CreateFolder(FolderName+FolderSub);
	FolderName = FolderName+FolderSub;


	CString LotID;
	THEAPP.m_pInspectResultDlg->GetDlgItemText(IDC_EDIT_LOT_ID, LotID);


	if(THEAPP.m_pInspectResultDlg->LastLotID!=LotID)
	{
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_COUNTTRAY, "0");

		
		UpdateData(FALSE);
	}


	CString strModelFile = FolderName+"\\"+LotID+"\\*.*";
	
	if (!ff.FindFile(strModelFile, 0)) 
		return;
	
	BOOL bExist = ff.FindNextFile();

	int nCnt = 0, nNameLen;
	CString strExt, strName;
	int CNT=0;
	while (TRUE) {
		if (ff.IsDirectory()){
			strExt = ff.GetFileName();
			if(strExt.GetLength()>2)
			{
				CNT+=1;
			}
		}
		if (!bExist) break;
		bExist = ff.FindNextFile();
	}

	//////////////// changed for CMI3000 2000 ====> ////////////
	CString strTrayCount;
	if (THEAPP.m_pInspectService->m_bInspectDone == TRUE)
		strTrayCount.Format("%d",CNT);
	else	// 검사가 끝나지 않고 멈춰서 다시 시작할 땐 트레이 수를 낮춰서 CheckPrevSaveFolder에서 트레이 수 증가 안되게 한다
	{
		if (CNT > 0) strTrayCount.Format("%d", CNT-1);
		else strTrayCount.Format("%d", 0);
	}	
	//////////////// <==== changed for CMI3000 2000 ////////////


	THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_COUNTTRAY, strTrayCount);
	UpdateData(FALSE);
}

LRESULT CInspectSummary::OnInspectionLoop(WPARAM wParam,LPARAM lParam)
{
	CallbuttonInspectStart();
	return 0;
}

void CInspectSummary::Initialize_one_line()
{
	//set_system("global_mem_cache", "false");
	//set_system("temporary_mem_cache", "false");

	if(THEAPP.m_pInspectService->m_pInspectAlgorithm)
	{
		delete[] THEAPP.m_pInspectService->m_pInspectAlgorithm;
		THEAPP.m_pInspectService->m_pInspectAlgorithm = NULL;
		if(THEAPP.m_pInspectService->m_pInspectAlgorithm==NULL)
		{
			THEAPP.m_pInspectService->m_pInspectAlgorithm = new Algorithm[THEAPP.m_iModuleNumberOneLine];
		}
	}

	THEAPP.m_pInspectAdminViewDlg->m_pHImage.Reset();

	THEAPP.m_pCameraManager->ImageGrabCount=0;
	gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->m_pHImage);

	THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.InvalidateRect(false);
	THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.InvalidateRect(false);
	THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.InvalidateRect(false);
	THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.InvalidateRect(false);

	if(THEAPP.Struct_PreferenceStruct.m_bCheckBeamProject==TRUE)
	{
		THEAPP.m_pTrayBeamProjectViewDlg->ShowWindow(SW_HIDE);
		Sleep(500);
		THEAPP.m_pTrayBeamProjectViewDlg->ShowWindow(SW_SHOW);
	}

	THEAPP.m_pTrayAdminViewDlg->Show();
	THEAPP.m_pTrayOperatorViewPrevDlg->Show();
	THEAPP.m_pTrayOperatorViewNowDlg->Show();
	if(THEAPP.m_iModeSwitch==MODE_INSPECT_VIEW_ADMIN)
	{
		THEAPP.m_pTrayOperatorViewPrevDlg->ShowWindow(SW_HIDE);
		THEAPP.m_pTrayOperatorViewNowDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		THEAPP.m_pTrayAdminViewDlg->ShowWindow(SW_HIDE);
	}

	//set_system("global_mem_cache", "exclusive");
	//set_system("temporary_mem_cache", "true");
}

void CInspectSummary::Initailize_for_Inspection()
{

	CheckLastTrayFolder();		// Lot 폴더 내의 Tray-xx 폴더 개수 카운트해서 "작업Tray개수" 에디트박스에 기입하는 함수
	CheckPrevSaveFolder();		// Lot 폴더 내의 Tray-xx 폴더 및 하부 폴더를 생성하는 함수
	
	char* CopyOKNG;
	CopyOKNG = new char[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
	for(int i=0;i<THEAPP.m_pModelDataManager->m_iTrayModuleMax;i++)
	{
		CopyOKNG[i] = THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[i];
	}

	if(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg)
	{
		delete[] THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg;
		THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg=NULL;
		if(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg==NULL)
		{
			THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg = new char[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}
	if(THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg)
	{
		delete[] THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg;
		THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg=NULL;
		if(THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg==NULL)
		{
			THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg = new char[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}

	
	if(THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg)
	{
		delete[] THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg;
		THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg=NULL;
		if(THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg==NULL)
		{
			THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg = new char[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}


	if(THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg)
	{
		delete[] THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg;
		THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg=NULL;
		if(THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg==NULL)
		{
			THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg = new char[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}

	if(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion)
	{
		delete[] THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion;
		THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion=NULL;
		if(THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion==NULL)
		{
			THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion = new BOOL[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}
	if(THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion)
	{
		delete[] THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion;
		THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion=NULL;
		if(THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion==NULL)
		{
			THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion = new BOOL[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}
	if(THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion)
	{
		delete[] THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion;
		THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion=NULL;
		if(THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion==NULL)
		{
			THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion = new BOOL[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}
	if(THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion)
	{
		delete[] THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion;
		THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion=NULL;
		if(THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion==NULL)
		{
			THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion = new BOOL[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}

	if(THEAPP.m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG)
	{
		delete[] THEAPP.m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG;
		THEAPP.m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG=NULL;
		if(THEAPP.m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG==NULL)
		{
			THEAPP.m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG = new BOOL[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}

	if(THEAPP.m_pInspectAdminViewHideDlg->m_bLensDefectOKNG)
	{
		delete[] THEAPP.m_pInspectAdminViewHideDlg->m_bLensDefectOKNG;
		THEAPP.m_pInspectAdminViewHideDlg->m_bLensDefectOKNG=NULL;
		if(THEAPP.m_pInspectAdminViewHideDlg->m_bLensDefectOKNG==NULL)
		{
			THEAPP.m_pInspectAdminViewHideDlg->m_bLensDefectOKNG = new BOOL[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}	

	if(THEAPP.m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG)
	{
		delete[] THEAPP.m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG;
		THEAPP.m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG=NULL;
		if(THEAPP.m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG==NULL)
		{
			THEAPP.m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG = new BOOL[THEAPP.m_pModelDataManager->m_iTrayModuleMax];
		}
	}	

	int TrayNumber;
	TrayNumber = THEAPP.m_pInspectResultDlg->GetDlgItemInt(IDC_EDIT_COUNTTRAY);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(int k=0;k<THEAPP.m_pModelDataManager->m_iTrayModuleMax;k++)
	{
		THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[k] = DEFECT_TYPE_PROCESSING_ERROR;		// $$$ 에러인지 초기화인지 다시 살펴봐라
		THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.OkNg[k] = CopyOKNG[k];
		THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.OkNg[k] = CopyOKNG[k];
		THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[k] = DEFECT_TYPE_PROCESSING_ERROR;	// $$$ 에러인지 초기화인지 다시 살펴봐라
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	


	for(int i=0;i<THEAPP.m_pModelDataManager->m_iTrayModuleMax;i++)
	{
		for (int j = 0; j < MAX_IMAGE_TAB; j++)
		{
			THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[i][j].Reset();
			gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[i][j]);
		}
		THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[i].Reset();
		gen_empty_obj(&THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[i]);
		
		THEAPP.m_pInspectAdminViewHideDlg->m_bBarrelDefectOKNG[i] = FALSE;
		THEAPP.m_pInspectAdminViewHideDlg->m_bLensDefectOKNG[i] = FALSE;
		THEAPP.m_pInspectAdminViewHideDlg->m_bExtraDefectOKNG[i] = FALSE;

		THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.ClickRegion[i] = FALSE;
		THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.ClickRegion[i] = FALSE;
		THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.ClickRegion[i] = FALSE;
		THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.ClickRegion[i] = FALSE;
	}


	for (int iTrayIdx = 0; iTrayIdx < MAX_TRAY_LOT; iTrayIdx++)
	{
		for (int iModuleIdx = 0; iModuleIdx < MAX_MODULE_ONE_TRAY; iModuleIdx++)
		{
			// TODO : Need to fix here for memory leak
			THEAPP.m_StructFaiMeasure[iTrayIdx][iModuleIdx].ResetFAI();
			for (int iDefectIdx = 0; iDefectIdx < MAX_DEFECT_NAME; iDefectIdx++)
			{
				THEAPP.m_vDetectParamInfo[iTrayIdx][iModuleIdx][iDefectIdx].clear();
			}
		}
	}

	delete[] CopyOKNG;

	THEAPP.m_pDefectListDlg->m_conDefectList.DeleteAllItems();
}

void CInspectSummary::CheckLotIDAndChangeModel(CString LotID)
{
	double tchecks = 0, tchecke = 0;
	int iSetLength = LotID.GetLength();

	if(iSetLength > 10)
	{
		CString strShortID = LotID;
		strShortID.Delete(0,2);
		strShortID = strShortID.Left(6);


		CString strDataFolder;
		strDataFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
		CString FileName = "ModelRecipe.ini";
		CString strSection;
		CIniFileCS INI(strDataFolder+FileName);
		strSection = "Model Maker";


		CString strModelName;
		while (TRUE)			// ModelRecipe.ini 에서 LotID가 일치하는 것을 찾을 때까지 뒤에서 한글자씩 줄여가면서 체크한다
		{
			strModelName = INI.Get_String(strSection, strShortID, "NA");
			int length = strShortID.GetLength();
			if (strModelName.GetLength() && strModelName != "NA") break;
			if (length < 3) 
				break;
			strShortID = strShortID.Left(length-1);
		}

		if (strModelName == "NA") 
			return;											// LotID 에 일치하는 모델이 없으면 이전 모델로 간주하고 skip
		
		////////////////////////////////// <==== Changed for CMI3000 2000 ///////////////////////////////////////////////////
		if (THEAPP.m_pModelDataManager->m_sModelName == strModelName) 
			return;	// 모델이 같으면 skip

		tchecks = GetTickCount();

		THEAPP.m_pModelDataManager->m_sModelName = strModelName;
		THEAPP.m_pModelDataManager->LoadModel();
		
		tchecke = GetTickCount();
		
		CString sTimeCheck; sTimeCheck.Format(" -Model Loading Time: %.0lf ms", tchecke - tchecks); THEAPP.SaveLog(sTimeCheck); THEAPP.SaveDetectLog(sTimeCheck);
	}
}

#include "DlgInspectFobTest.h"
void CInspectSummary::OnBnClickedBnFobset()
{
	CDlgInspectFobTest dlg;
	dlg.DoModal();
}


#include "FobModeChangeDlg.h"

void CInspectSummary::OnClickedChkFobtest()
{
	GetDlgItem(IDC_BN_FOBSET)->EnableWindow(TRUE);
	UpdateData();

	m_bUseFobTest = m_bDxFobTest;

	if (m_bDxFobTest == FALSE)
	{
		GetDlgItem(IDC_BN_FOBSET)->EnableWindow(FALSE);
	
		//FOB Mode Change Add Start
		CString strLog;
		if( THEAPP.m_nFOBModeUser == FOB_MODE_ADMIN )
		{
			strLog.Format("[FOB Mode(Admin)]No Use FOB Mode");
		}
		else
		{
			strLog.Format("[FOB Mode(Operator)]No Use FOB Mode (Operator ID) : %s",THEAPP.m_strFOBModeOperatorID);
		}
		THEAPP.m_pHandlerService->Set_FOBUpdate((int)m_bDxFobTest);
		THEAPP.SaveLog(strLog);
		//FOB Mode Change Add End
	}
	else //else 이하 //FOB Mode Change 추가
	{
		CFobModeChangeDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			THEAPP.m_pHandlerService->Set_FOBUpdate((int)m_bDxFobTest);
		}
		else
		{
			m_bUseFobTest = m_bDxFobTest = FALSE;
			GetDlgItem(IDC_BN_FOBSET)->EnableWindow(FALSE);
			UpdateData(FALSE);
		}
	}

	THEAPP.UpdateCurMode();
}



void CInspectSummary::OnBnClickedChkAllignImg()
{
	UpdateData();
	m_bUseAllignImg = m_bDxAllignImg;
}


void CInspectSummary::OnBnClickedChkAccelMatching()
{
	UpdateData();
	m_bUseAccelMatching = m_bDxAccelMatching;
}


void CInspectSummary::ShowDiskCapacity()
{
	CString str; 
	float TotalBytes, FreeBytes;
	float fTotal, fFree, fUsed;
	int iThres = THEAPP.Struct_PreferenceStruct.m_iDiskMax;

	ULARGE_INTEGER ulUserFree, ulTotal, ulRealFree; 

	CString sDir="C:\\";
	if(GetDiskFreeSpaceEx(sDir, &ulUserFree, &ulTotal, &ulRealFree)) 
	{
		TotalBytes = ulTotal.LowPart + (double)ulTotal.HighPart*(double)4294967296; 
		fTotal = (float)(TotalBytes/1024/1024/1024); // Convert (GB)     
		FreeBytes = ulRealFree.LowPart + (double)ulRealFree.HighPart*(double)4294967296; 
		fFree = (float)(FreeBytes/1024/1024/1024); // Convert (GB) 
		fUsed = fTotal - fFree;
	}
	int iPos = int(fUsed/fTotal*100);
	if (iPos < 0) iPos = 0; if (iPos > 100) iPos = 100;
	SetDlgItemInt(IDC_STATIC_DISK_HDDC_PRO, iPos);
	m_ctrlProgressHddC.SetRange(0, 100);
	m_ctrlProgressHddC.SetPos(iPos);
	if(iPos >= iThres) 
		m_ctrlProgressHddC.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(255, 0, 0));
	else 
		m_ctrlProgressHddC.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(0, 127, 255));

	sDir="D:\\";
	if(GetDiskFreeSpaceEx(sDir, &ulUserFree, &ulTotal, &ulRealFree)) 
	{ 
		TotalBytes = ulTotal.LowPart + (double)ulTotal.HighPart*(double)4294967296; 
		fTotal = (float)(TotalBytes/1024/1024/1024); // Convert (GB)     
		FreeBytes = ulRealFree.LowPart + (double)ulRealFree.HighPart*(double)4294967296; 
		fFree = (float)(FreeBytes/1024/1024/1024); // Convert (GB) 
		fUsed = fTotal - fFree;
	}
	iPos = int(fUsed/fTotal*100);
	if (iPos < 0) iPos = 0; if (iPos > 100) iPos = 100;
	SetDlgItemInt(IDC_STATIC_DISK_HDDD_PRO, iPos);
	m_ctrlProgressHddD.SetRange(0, 100);
	m_ctrlProgressHddD.SetPos(iPos);
	if(iPos >= iThres) 
		m_ctrlProgressHddD.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(255, 0, 0));
	else 
		m_ctrlProgressHddD.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(0, 127, 255));
}

void CInspectSummary::OnDeltaposSpinTrayNo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);
	if (pNMUpDown->iDelta < 0)
	{
		if (m_iDxTrayNo > 19) m_iDxTrayNo = 1;
		else m_iDxTrayNo++;
	}
	else
	{
		if (m_iDxTrayNo < 2) m_iDxTrayNo = 20;
		else m_iDxTrayNo--;
	}
	UpdateData(FALSE);

	*pResult = 0;
}


void CInspectSummary::OnDeltaposSpinStageNo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	UpdateData(TRUE);
	if (pNMUpDown->iDelta < 0)
	{
		if (m_iDxLineNo > 5) m_iDxLineNo = 6;
		else m_iDxLineNo++;
	}
	else
	{
		if (m_iDxLineNo < 2) m_iDxLineNo = 1;
		else m_iDxLineNo--;
	}
	UpdateData(FALSE);

	*pResult = 0;
}


void CInspectSummary::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		case 0:
		{
			KillTimer(0);

			if (THEAPP.Struct_PreferenceStruct.m_bIsUseAIInsp)
			{
				if (THEAPP.m_ADJClientService.m_bConnect)
				{
					m_LabelADJConnected.SetWindowText("Connected");
				}
				else
				{
					m_LabelADJConnected.SetWindowText("Not Connected");

					THEAPP.m_ADJClientService.ReStart();
					THEAPP.m_ADJClientService.Initialize();

					if (THEAPP.m_ADJClientService.m_arrClient.TCPConnect())
					{
						THEAPP.m_ADJClientService.m_bConnect = TRUE;
					}
					else
					{
						THEAPP.m_ADJClientService.m_bConnect = FALSE;
					}
				}
			}
			else
			{
				m_LabelADJConnected.SetWindowText("Not Use");
			}


			// 1분 주기로 ADJ 연결 체크
			SetTimer(0, ADJ_CONNECT_CHECK_TIME, NULL);

			break;
		}
		case 1:
		{
			CString temp;

			////////// 상태 업데이트 //////////
			temp.Format("%d", m_iIndexNo > 0 ? m_iIndexNo : 0);
			m_lbIndexNo.SetWindowText(temp);

			temp.Format("%d", m_iPocketNo > 0 ? m_iPocketNo : 0);
			m_lbPocketNo.SetWindowText(temp);

			m_lbInspStatus.SetWindowText(m_sInspStatus);
	
			////////// 프로세스 초기화 확인 //////////
			BOOL bRestartOn = THEAPP.Struct_PreferenceStruct.m_bUseProcessRestart;
			if (bRestartOn && (bIsRestart == FALSE))
			{
				// 검사 완료 후에만 프로세스 재시작 체크
				// 검사 중에는 메모리 사용량이 많을 수 있으므로 검사 완료 후에 체크하도록 함 (Maybe) - 251001 , jhkim
				int iModuleNumberOneLine = THEAPP.m_iModuleNumberOneLine;
				int iTrayLineNumber = THEAPP.m_iTrayLineNumber;
				if ((iModuleNumberOneLine > 0) && (iTrayLineNumber > 0))
				{
					int totalLines = ((THEAPP.m_pInspectService->m_iLotModuleAmt_H - 1) / THEAPP.m_iModuleNumberOneLine) + 1;
					int iLastLineNo = ((totalLines-1) % THEAPP.m_iTrayLineNumber) + 1;

					bool isInspectDone = (THEAPP.m_pInspectService->m_bInspectDone == TRUE);
					bool isPCStatusReady = (THEAPP.m_pHandlerService->m_nInspectPCStatus == HS_RUN);
					bool isTrayComplete = (THEAPP.m_pInspectService->m_iTrayNo_H >= THEAPP.m_pInspectService->m_iLotTrayAmt_H);
					bool isLineComplete = (THEAPP.m_pInspectService->m_iLineNo_H >= iLastLineNo);
					
					if (isInspectDone && isPCStatusReady && isTrayComplete && isLineComplete)
					{
						try
						{
							MEMORYSTATUSEX memInfo;
							memInfo.dwLength = sizeof(MEMORYSTATUSEX);
							GlobalMemoryStatusEx(&memInfo);

							DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;  // 사용 중인 물리 메모리
							double physMemUsedMegaByte = (double)physMemUsed / (1024 * 1024);	// 현재 프로세스가 사용중인 메모리
							double physMemUsedPercent = ((double)physMemUsed / (double)memInfo.ullTotalPhys) * 100;

							double dDurationTime = (GetTickCount() - dProcessStartTime) / 60000;

							BOOL bMemoryOver = (physMemUsedPercent > THEAPP.Struct_PreferenceStruct.m_dProcessRestartMemoryLimit);	// 메모리 상한선 검사 (%)
							BOOL bMinTimeOver = (dDurationTime >= THEAPP.Struct_PreferenceStruct.m_iProcessRestartTimeMin);			// 최소시간 검사 (분)
							BOOL bMaxTimeOver = (dDurationTime > THEAPP.Struct_PreferenceStruct.m_iProcessRestartTimeMax);			// 최대시간 검사 (분)

							if ((bMemoryOver && bMinTimeOver) || bMaxTimeOver)
							{
								Sleep(THEAPP.Struct_PreferenceStruct.m_iProcessRestartDelayTime); // 초기화 전 지연시간 (ms)

								THEAPP.RestartProcess();
								bIsRestart = TRUE;
							}
						}
						catch (exception& ex)
						{
							THEAPP.SaveLog("[CInspectSummary::OnTimer] Process Restart Failed. Exception: " + CString(ex.what()));
						}
					}
				}
			}
			break;
		}
		case 2:	// PCSatus 로그 추가 - LeeGW
		{
			KillTimer(2);

			THEAPP.SavePCStatusLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog);

			// Save PCStatusLog (4시간 주기) - 251014 - jhkim
			SetTimer(2, PCSTATUS_SAVE_TIME, NULL);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CInspectSummary::OnBnClickedChkBlInspUse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// BL 검사 유무 체크 - LeeGW

	UpdateData();

	CString sMsg;
	if (m_chkUseBLInsp.GetCheck() == TRUE)
		sMsg = _T("배럴/렌즈 검사를 시작하시겠습니까?");
	else
		sMsg = _T("배럴/렌즈 검사를 중지하시겠습니까?");

	if (IDYES == AfxMessageBox(sMsg, MB_YESNO))
	{
		m_bUseBLInsp = m_chkUseBLInsp.GetCheck();

		CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
		CIniFileCS TXT_Status(strStatusFileName);

		TXT_Status.Set_Bool("Status", "BarrelLensInspection", m_bUseBLInsp);
	}
	else
	{
		m_chkUseBLInsp.SetCheck(m_bUseBLInsp);
	}


	if (m_bUseBLInsp == TRUE)
	{
		m_chkUseBLInsp.Init_Ctrl(_T("Arial"), 9, TRUE, BLACK, CYAN, 0, 0);
	}
	else
	{
		m_chkUseBLInsp.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, DARK_GRAY, 0, 0);
	}

	UpdateData(FALSE);
}

BOOL CInspectSummary::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// BL 검사 유무 체크 - LeeGW
	CString sStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS TXT_Status(sStatusFileName);

	m_bUseBLInsp = TXT_Status.Get_Bool("Status", "BarrelLensInspection", TRUE);	
	m_chkUseBLInsp.SetCheck(m_bUseBLInsp);

	if(m_bUseBLInsp == TRUE)
		m_chkUseBLInsp.Init_Ctrl(_T("Arial"), 9, TRUE, BLACK, CYAN, 0, 0);
	else
		m_chkUseBLInsp.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, DARK_GRAY, 0, 0);

	UpdateData(FALSE);

	//SetTimer(0, 60000, NULL); -> ADJ 지원 중지.

	SetTimer(1, RESTART_CHECK_TIME, NULL);
	if (THEAPP.m_iMachineInspType != MACHINE_WELDING)	// MOL팀 요청
		SetTimer(2, PCSTATUS_CHECK_TIME, NULL);

	dProcessStartTime = GetTickCount();

	// ADJ Disable - 251021, jhkim
	for (UINT id : ADJConditions)
	{
		CWnd* pWnd = GetDlgItem(id);
		if (pWnd)
		{
			pWnd->EnableWindow(FALSE);
			pWnd->ShowWindow(SW_HIDE);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CInspectSummary::SetBtnEnable(BOOL bEnable)
{
	// 버튼 ID를 배열로 관리하여 반복문으로 처리
	const UINT maintBtnIDs[] = {
		IDC_MFCBUTTON_INSPECT_START,
		IDC_MFCBUTTON_STATUS_CHECK,
		IDC_CHK_USE_BL_INSP
	};

	for (UINT id : maintBtnIDs)
	{
		CWnd* pWnd = GetDlgItem(id);
		if (pWnd)
			pWnd->EnableWindow(bEnable);
		else
		{
			CString strMsg;
			strMsg.Format(_T("CInspectSummary::SetBtnEnable - Failed to get control with ID: %d\n"), id);
			AfxOutputDebugString(strMsg);
		}
	}
}