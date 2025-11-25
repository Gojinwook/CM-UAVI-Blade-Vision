#include "stdafx.h"
#include "InspectService.h"
#include "uScan.h"
#include "IniFileCS.h"


CInspectService *CInspectService::m_pInstance = NULL;

CInspectService *CInspectService::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new CInspectService();
	}
	return m_pInstance;
}

void CInspectService::DeleteInstance()
{
	if (m_pInstance) delete m_pInstance;
	m_pInstance = NULL;
}

CInspectService::CInspectService(void)
{
	m_sLotID_H = "";
	m_iLotTrayAmt_H = 0;			// added for CMI 3000 2000
	m_iLotModuleAmt_H = 0;			// added for CMI 3000 2000

	m_iPrevTrayNo_H = -1;
	m_iTrayNo_H = 0;				// added for CMI 3000 2000
	m_iLineNo_H = 0;				// added for CMI 3000 2000
	m_iIndexNo_H = 0;				// added for CMI 3000 2000
	m_iMboCount_H = 0;				// added for CMI 3000 2000

	m_bInspectDone = FALSE;			// added for CMI 3000 2000
	m_bInspectRepeat = FALSE;
	m_bInspectRun = FALSE;

	m_bLineInspectDone =  FALSE;

	m_iFobTestG = 100;				// added for CMI 3000 2000
	m_iFobTestN = 0;				// added for CMI 3000 2000
	m_iFobTestE = 0;				// added for CMI 3000 2000

	m_bCycleStopSignaled = FALSE;
}


CInspectService::~CInspectService(void)
{
}

//////////////////////// added for CMI3000 2000 ====>//////////////////////////////// 
void CInspectService::QuitInspect()
{
	int iStatus = THEAPP.m_pHandlerService->m_nInspectPCStatus;

	CString sStr;
	sStr.Format("QuitInspect 들어옴: 상태번호 %d", iStatus);
	THEAPP.SaveLog(sStr);

	THEAPP.m_pInspectSummary->m_sInspStatus = _T("Reset");

	THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
	THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;
	m_bInspectRepeat = TRUE;		// 이게 끝나야 다음 트레이를 위한 GetLoadComplete 가 완전히 수행된다
	m_bInspectRun = FALSE;

	THEAPP.m_pInspectSummary->m_sInspStatus = _T("Ready");
}

// 핸들러로부터 Get_LotStart 콜 되었을 때... 핸들러로부터 정보는 LOT_ID, Tray수량, 검사모듈갯수
void CInspectService::ReadyLot ()
{
#ifdef INLINE_MODE
	THEAPP.m_pInspectSummary->m_sInspStatus = _T("Lot Ready");

	THEAPP.m_pModelDataManager->SetControllerLightInfo();
	THEAPP.SaveLog("조명 Controller 셋팅 완료");

	CString sInspectLog;
	CString sLotID_H = THEAPP.m_pInspectService->m_sLotID_H;
	int iLotTrayAmt_H = THEAPP.m_pInspectService->m_iLotTrayAmt_H;
	int iLotModuleAmt_H = THEAPP.m_pInspectService->m_iLotModuleAmt_H;

	sInspectLog.Format("******************** Lot %s, %d Trays, %d Modules 검사 준비 ******************\n", sLotID_H, iLotTrayAmt_H, iLotModuleAmt_H);
	THEAPP.SaveLog(sInspectLog);
	THEAPP.SaveDetectLog(sInspectLog);

	if(THEAPP.m_pInspectResultDlg->CurrentLotID != THEAPP.m_pInspectResultDlg->LastLotID)
	{
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_COUNTTRAY,"0");
	}

	THEAPP.m_pInspectResultDlg->LastLotID = THEAPP.m_pInspectResultDlg->CurrentLotID;
	THEAPP.m_pInspectSummary->CheckLotIDAndChangeModel(THEAPP.m_pInspectResultDlg->CurrentLotID);

	THEAPP.m_pInspectSummary->m_iDxTrayNo = 1;
	THEAPP.m_pInspectSummary->m_iDxLineNo = 0;

	CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS TXT_Status(strStatusFileName);
	CString sSection = "Status";
	TXT_Status.Set_String (sSection, "LotID", m_sLotID_H);
	TXT_Status.Set_Integer (sSection, "TrayAmount", m_iLotTrayAmt_H);
	TXT_Status.Set_Integer (sSection, "ModuleAmount", m_iLotModuleAmt_H);
	//	TXT_Status.Set_Bool (sSection, "TrayMustEnd", FALSE);
	TXT_Status.Set_Bool (sSection, "BarcodeScanComplete", FALSE);
	TXT_Status.Set_Bool (sSection, "InspectScanComplete", FALSE);
	TXT_Status.Set_String (sSection, "Status", "Lot Ready");

	SYSTEMTIME LotStartTime;
	CString strLotStartTime;
	GetLocalTime(&LotStartTime);
	strLotStartTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d", LotStartTime.wYear, LotStartTime.wMonth, LotStartTime.wDay,
		LotStartTime.wHour, LotStartTime.wMinute, LotStartTime.wSecond, LotStartTime.wMilliseconds);

	TXT_Status.Set_String (sSection, "LotStartTime", strLotStartTime);

	THEAPP.m_dLotStartTickCount = GetTickCount();

	int i, j;

	for (i=0; i<MAX_TRAY_LOT; i++)
	{
		for (j=0; j<MAX_MODULE_ONE_TRAY; j++)
		{
			THEAPP.m_iBarcodeShiftPos[i][j] = 0;
		}
	}

	THEAPP.iModuleCountOneLot = 0;
	for (i=0; i<MAX_MODULE_ONE_LOT; i++)
	{
		THEAPP.m_iModuleDefectCenterX[i] = -1;
		THEAPP.m_iModuleDefectCenterY[i] = -1;
		THEAPP.m_sModuleDefectName[i] = _T("OK");
	}

	THEAPP.m_pLogDlg->ClearView();
#endif
}

// 핸들러로부터 Load Complete 받았을 때 처리...
void CInspectService::ReadyToStart (BOOL bErrorStatus)
{
	CString sInspectLog; 
	sInspectLog.Format("******************** 트레이 %d / Line %d 검사 준비 ******************", THEAPP.m_pInspectService->m_iTrayNo_H, THEAPP.m_pInspectService->m_iLineNo_H);
	THEAPP.SaveLog(sInspectLog); 
	THEAPP.SaveDetectLog(sInspectLog);
	THEAPP.m_pAlgorithm->m_tInspectStartTime = GetTickCount();

	if((THEAPP.m_iModeSwitch==MODE_INSPECT_VIEW_ADMIN) || (THEAPP.m_iModeSwitch==MODE_INSPECT_VIEW_OPERATOR))
	{
		////// Ready 상태가 아닌 상태에서 다시 run이면 빠져나간다: 조사 필요
		if(THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay && THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay) {
			THEAPP.SaveLog("out~~~~~");
			return;
		}

		THEAPP.m_pHandlerService->Set_PositionRequest();

		THEAPP.m_pCameraManager->ImageGrabCount=0;

		if(THEAPP.m_pInspectResultDlg->CurrentLotID != THEAPP.m_pInspectResultDlg->LastLotID)
		{
			THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_COUNTTRAY,"0");
		}

		double tstart = 0, tend = 0;
		THEAPP.m_pInspectResultDlg->LastLotID = THEAPP.m_pInspectResultDlg->CurrentLotID;
		
		if (bErrorStatus)
		{
			THEAPP.m_pInspectSummary->CheckLotIDAndChangeModel(THEAPP.m_pInspectResultDlg->CurrentLotID);		// 모션 위치 설정
		}
		else
		{
			if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_X)
			{
				if (m_iLineNo_H==1)		// 첫 라인에서만 (알고리즘 & 모션 제외)
					THEAPP.m_pInspectSummary->CheckLotIDAndChangeModel(THEAPP.m_pInspectResultDlg->CurrentLotID);		// 모션 위치 설정
			}
			else
			{
				if (m_iTrayNo_H!=m_iPrevTrayNo_H)		// 트레이 번호가 바뀔때
					THEAPP.m_pInspectSummary->CheckLotIDAndChangeModel(THEAPP.m_pInspectResultDlg->CurrentLotID);		// 모션 위치 설정
			}
		}

		THEAPP.m_pModelDataManager->ModelPreSet(m_iLineNo_H);

		CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
		CIniFileCS TXT_Status(strStatusFileName);
		CString sSection = "Status";

		TXT_Status.Set_String (sSection, "LotID", m_sLotID_H);
		TXT_Status.Set_Integer (sSection, "TrayNo", m_iTrayNo_H);
		TXT_Status.Set_Integer (sSection, "LineNo", m_iLineNo_H);
		TXT_Status.Set_String (sSection, "ModelID", THEAPP.m_pModelDataManager->m_sModelName);
		TXT_Status.Set_Integer (sSection, "TrayAmount", m_iLotTrayAmt_H);
		// TXT_Status.Set_Integer (sSection, "ModuleAmount", m_iLotModuleAmt_H);
		// TXT_Status.Set_Bool (sSection, "TrayMustEnd", FALSE);
		TXT_Status.Set_Bool (sSection, "BarcodeScanComplete", FALSE);
		TXT_Status.Set_Bool (sSection, "InspectScanComplete", FALSE);
		TXT_Status.Set_String (sSection, "Status", "Load Completed");

		THEAPP.m_pInspectSummary->m_sInspStatus = _T("Load Completed");

		m_iLotModuleAmt_H = TXT_Status.Get_Integer(sSection, "ModuleAmount", 0);	// 검사 시마다 Lot 모듈 수량 리딩

		THEAPP.m_pInspectSummary->m_iDxTrayNo = m_iTrayNo_H;
		THEAPP.m_pInspectSummary->m_iDxLineNo = m_iLineNo_H;
		THEAPP.m_pInspectSummary->m_iIndexNo = m_iIndexNo_H;
		THEAPP.m_pInspectSummary->m_iPocketNo = THEAPP.m_iModuleNumberOneLine * (m_iIndexNo_H - 1) + 1;

		THEAPP.m_pInspectSummary->UpdateData(FALSE);

		////////////////// <==== changed for CMI3000 2000 ///////////////////////////////////////////////////

		CString sTimeCheck;	sTimeCheck.Format("---tray %d, line %d", m_iTrayNo_H, m_iLineNo_H); THEAPP.SaveLog(sTimeCheck);

		THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = TRUE;
		THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = TRUE;

		if (bErrorStatus)
		{
			THEAPP.m_pInspectSummary->Initailize_for_Inspection();	//WCS 2020/05/17
		}
		else
		{

			if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_X)
			{
				if (m_iLineNo_H==1)		// 첫 라인에서만 (알고리즘 & 모션 제외)
					THEAPP.m_pInspectSummary->Initailize_for_Inspection();
			}
			else
			{
				if (m_iTrayNo_H!=m_iPrevTrayNo_H)		// 트레이 번호가 바뀔때
					THEAPP.m_pInspectSummary->Initailize_for_Inspection();
			}
		}

		THEAPP.m_pInspectSummary->Initialize_one_line();

		m_bInspectDone = FALSE;			// CMI3000, 이거 안해주면 데이터 저장을 위한 트레이 수가 증가하지 않는다
										// CheckLastTrayFolder 다음에 FALSE로 바꿔준다
	}

}
//////////////////////// <==== added for CMI3000 2000 //////////////////////////////// 

UINT BarcodeScanThread(LPVOID lp)
{
	try
	{
		if (THEAPP.m_pModelDataManager->m_sModelName == ".")
			return 0;

		CInspectService *pInspectService = (CInspectService *)lp;

		CString sLineNo = "";
		sLineNo.Format("Line %d", pInspectService->m_iLineNo_H);

		double dTimeStart = 0, dTimeEnd = 0;
		dTimeStart = GetTickCount();

#ifdef INLINE_MODE
		double dInspPosX = 0;
		double dInspPosY = 0;
		// TODO : 배열 0:0, 0:1 이 무슨 뜻?
		dInspPosX = THEAPP.m_pModelDataManager->m_mdBarcodeScanPosXY[0][0];
		dInspPosY = THEAPP.m_pModelDataManager->m_mdBarcodeScanPosXY[0][1];

		if (dInspPosX == 0)
		{
			THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
			THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;

			THEAPP.SaveLog("###바코드### 모션 좌표 0으로 인해 종료");
			return 0;
		}
#endif // INLINE_MODE

		BOOL bResetCameraDone = FALSE;
		BOOL bResetCameraRet;

		double StartTime_Ins = 0, EndTime_Ins = 0, CurrentTime_Ins = 0;
		double StartTime = 0, EndTime = 0, CurrentTime = 0;
		CString strInspectionTime;

		DWORD dwGrabStart = 0, dwGrabEnd = 0;
		DWORD dwGrabElapsedTime = 0;

		Hobject HBarcodeGrabImage;

		// Scan counter  (0. 1. 2. 3. 4. 5)
		int nInspCnt = 0;

		////////////////////////////// Barcode 스캔 시작 ////////////////////////////////////
		while (TRUE)
		{
			if (nInspCnt >= THEAPP.m_iModuleNumberOneLine)
				break; // 현재 트레이 상의 시료 갯수만큼만 inspection 진행

			int iModuleNo = nInspCnt + 1; // 초기화

#ifdef INLINE_MODE
			iModuleNo = THEAPP.m_pModelDataManager->m_viInspectModuleNo[nInspCnt];
#endif // INLINE_MODE

			double Xposition, Yposition;
			CString sGrabTime, sGrabTime_total;
			double tGrabStart = 0, tGrabEnd = 0;
			double tGrabStart_total = 0, tGrabEnd_total = 0;

			CString strLog;

#ifdef INLINE_MODE
			StartTime = GetTickCount();

			dInspPosX = THEAPP.m_pModelDataManager->m_mdBarcodeScanPosXY[nInspCnt][0];
			// TODO : Y는 매번 0으로 초기화?
			dInspPosY = 0;

			THEAPP.m_pHandlerService->m_bBarcodeMoveCompleteFlag = FALSE;
			THEAPP.m_pHandlerService->Set_AMoveRequest(dInspPosX, dInspPosY, 1, VISION_TYPE_BARCODE); // 위치이동

			// 무브 컴플리트 기다림
			while (!THEAPP.m_pHandlerService->m_bBarcodeMoveCompleteFlag)
			{
				// LOT 시작 명령 수신 시 핸들러 처리를 대기하는 부분 모두 Clear 하도록 코드 추가
				if (pInspectService->GetCycleStopStatus() == TRUE)
					return 0;

				Sleep(1);
			}
			////////////////////////////////// <==== changed for CMI3000 2000 ///////////////////////////////////////////////////

			CurrentTime_Ins = GetTickCount();

			strLog.Format("###바코드### 모션 %2d, 모듈 %2d 위치 도착, 이동시간: %.0lf ms", nInspCnt + 1, iModuleNo, (CurrentTime_Ins - StartTime));
			THEAPP.SaveLog(strLog);
			THEAPP.SaveDetectLog(strLog);

			CString sDelay;
			int K = 0;

			BOOL bGrabSuccess = FALSE;
			dwGrabStart = GetTickCount();

			THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[nInspCnt].Reset();

			for (K = 0; K < 6; K++) // 6번 Retry
			{
				HBarcodeGrabImage.Reset();
				THEAPP.m_pCameraManager->GrabBarcodeImage(&HBarcodeGrabImage);
				if (THEAPP.m_pGFunction->ValidHImage(HBarcodeGrabImage) == TRUE)
				{
					sDelay.Format("###바코드### 모션 %d 번째 / %d 번째 시도 성공", nInspCnt + 1, K + 1);
					THEAPP.SaveLog(sDelay);
					bGrabSuccess = TRUE;
					break;
				}
			}

			if (bGrabSuccess == FALSE && bResetCameraDone == FALSE)
			{
				bResetCameraRet = THEAPP.m_pCameraManager->ResetBarcodeCamera();
				if (bResetCameraRet)
				{
					THEAPP.SaveLog("바코드 카메라 초기화 성공");
					bResetCameraDone = TRUE;

					bGrabSuccess = FALSE;
					dwGrabStart = GetTickCount();

					for (K = 0; K < 6; K++) // 6번 Retry
					{
						HBarcodeGrabImage.Reset();
						THEAPP.m_pCameraManager->GrabBarcodeImage(&HBarcodeGrabImage);
						if (THEAPP.m_pGFunction->ValidHImage(HBarcodeGrabImage) == TRUE)
						{
							sDelay.Format("###바코드### 모션 %d 번째 / %d 번째 시도 성공", nInspCnt + 1, K + 1);
							THEAPP.SaveLog(sDelay);
							bGrabSuccess = TRUE;
							break;
						}
					}
				}
				else
				{
					THEAPP.SaveLog("바코드 카메라 초기화 오류");
					AfxMessageBox("바코드 카메라를 초기화할 수 없습니다. 비전 프로그램을 다시 시작해 주세요.", MB_SYSTEMMODAL);
				}
			}

			if (bGrabSuccess)
			{
				copy_image(HBarcodeGrabImage, &(THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[nInspCnt]));
				THEAPP.m_pInspectAdminViewDlg->m_iBarcodeGrabStatus[nInspCnt] = BARCODE_GRAB_STATUS_GRAB_DONE;
			}
			else
			{
				THEAPP.m_pInspectAdminViewDlg->m_iBarcodeGrabStatus[nInspCnt] = BARCODE_GRAB_STATUS_NO_GRAB;
			}

			dwGrabEnd = GetTickCount();
			sGrabTime.Format("###바코드### Module %2d 그랩 시간: %.0lf ms (x%d)", iModuleNo, dwGrabEnd - dwGrabStart, K + 1);
			THEAPP.SaveLog(sGrabTime);

			while (THEAPP.m_pInspectAdminViewDlg->m_iInspectGrabStatus[nInspCnt] == INSPECT_GRAB_STATUS_NOT_READY) // Inspect Grab 완료 기다림
			{
				Sleep(1);
			}

#else // Offline Mode =>

#endif // Offline Mode

			nInspCnt++;
		}

#ifdef INLINE_MODE
		// 2025.06.25 - 핸들러 응답 체크 - LeeGW
		THEAPP.m_pHandlerService->ResetReplyReceived();

		int iHandlerRetryNum = 0;
		while (TRUE)
		{
			iHandlerRetryNum++;

			if ((THEAPP.Struct_PreferenceStruct.m_bUseMboMode == FALSE) || (THEAPP.Struct_PreferenceStruct.m_bUseMboMode == TRUE && THEAPP.m_pInspectService->m_iMboCount_H == THEAPP.Struct_PreferenceStruct.m_iMboModeCount))
				THEAPP.m_pHandlerService->Set_ScanComplete(pInspectService->m_iLineNo_H, VISION_TYPE_BARCODE);

			if (THEAPP.Struct_PreferenceStruct.m_bUseHandlerRetry == TRUE)
				Sleep(THEAPP.Struct_PreferenceStruct.m_iHandlerReplyWaitTime);

			if (THEAPP.Struct_PreferenceStruct.m_bUseHandlerRetry == FALSE || THEAPP.m_pHandlerService->Is_ReplyReceived() == TRUE || iHandlerRetryNum >= THEAPP.Struct_PreferenceStruct.m_iHandlerRetryCount)
				break;

			Sleep(THEAPP.Struct_PreferenceStruct.m_iHandlerRetryWaitTime);

			// TODO : 핸들러가 두개? 핸들러 A도 있나?
			THEAPP.SaveLog("********** Handler B 스캔완료 송신 재시도 ******************");
		}

#endif

		CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
		CIniFileCS TXT_Status(strStatusFileName);
		CString sSection = "Status";
		TXT_Status.Set_Bool(sSection, "BarcodeScanComplete", TRUE);

#ifdef LOOPINSPECTION
		Sleep(3000);
		THEAPP.m_pInspectSummary->SendMessage(WM_INSPECTION_LOOP, 0, 0);
#endif

		return 0;
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [BarcodeScanThread] : %s", except.message);
		THEAPP.SaveLog(str);
		return 0;
	}
}

// about 800 lines - 251014, jhkim
UINT InspectionThread(LPVOID lp)
{
try{
	if(THEAPP.m_pModelDataManager->m_sModelName == ".") {return 0;}

	CInspectService* pInspectService = (CInspectService*)lp;

	THEAPP.m_pInspectService->m_bInspectRun = TRUE;

	CString sLineNo = ""; sLineNo.Format("Line %d", pInspectService->m_iLineNo_H); 
	CString sInspectLog; sInspectLog.Format("******************** 트레이 %d / Line %d 검사 시작 ******************", THEAPP.m_pInspectService->m_iTrayNo_H, THEAPP.m_pInspectService->m_iLineNo_H);
	THEAPP.SaveLog(sInspectLog); THEAPP.SaveDetectLog(sInspectLog);

	double dTimeStart = 0, dTimeEnd = 0;
	dTimeStart = GetTickCount();

	//////////////////////////////////////////////////////////////////모션 테스트 위치 입력 (array x * array y개 트레이)

	BOOL bGrabFail = FALSE;

	int nInspCnt = 0;
	int iTrayModuleAmount = THEAPP.m_iModuleNumberOneLine;

#ifdef INLINE_MODE

	THEAPP.m_pInspectSummary->m_sInspStatus = _T("Scanning...");

	double dInspPosX = 0;
	double dInspPosY = 0;

	dInspPosX = THEAPP.m_pModelDataManager->m_mdInspectPosXY[0][0];
	dInspPosY = THEAPP.m_pModelDataManager->m_mdInspectPosXY[0][1];

	if(dInspPosX == 0) {
		THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
		THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;
		return 0;
	}

	double dInspPosZ[MAX_IMAGE_TAB] = { 0.0 };
	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		dInspPosZ[iImageIdx] = THEAPP.m_pModelDataManager->m_mdInspPosZ[iImageIdx];
	}
#endif	// INLINE_MODE


	////////////////////////////////////////////////////////////////// Inspection
	double StartTime_Ins = 0, EndTime_Ins = 0, CurrentTime_Ins=0;
	double StartTime = 0, EndTime = 0, CurrentTime=0;
	CString strInspectionTime;

	DWORD dwGrabStart = 0, dwGrabEnd = 0;
	DWORD dwGrabElapsedTime = 0;


	StartTime_Ins = GetTickCount();
	CurrentTime_Ins = GetTickCount();
	CString sTimeCheck; sTimeCheck.Format("초기화 시간 total: %.0lf ms", CurrentTime_Ins - THEAPP.m_pAlgorithm->m_tInspectStartTime); THEAPP.SaveLog(sTimeCheck); THEAPP.SaveDetectLog(sTimeCheck);

	CurrentTime_Ins = CurrentTime_Ins-StartTime_Ins;
	strInspectionTime.Format("%.3lf", CurrentTime_Ins/1000);

	////////////////////////////// Initislizing /////////////////////////////////////
	for(nInspCnt = 0; nInspCnt < THEAPP.m_iModuleNumberOneLine; nInspCnt++) {
		pInspectService->m_pInspectAlgorithm[nInspCnt].ThreadFinishFlag = FALSE;
	}
	nInspCnt = 0;						// Inspection counter  (0. 1. 2. 3. 4. 5)

	//0524
	int iRetryCnt;

	////////////////////////////// Tray 스캔 시작 ////////////////////////////////////
	while(TRUE)
	{
		if(nInspCnt >= THEAPP.m_iModuleNumberOneLine) 
			break;  // 현재 트레이 상의 시료 갯수만큼만 inspection 진행

		int iModuleNo = nInspCnt+1; // 초기화

		CString Temp;
		Temp.Format("nInspCnt : %2d, iTrayModuleAmount : %2d , iModuleNo : %2d",nInspCnt, iTrayModuleAmount, iModuleNo);
		THEAPP.SaveLog(Temp);

		iModuleNo = THEAPP.m_pModelDataManager->m_viInspectModuleNo[nInspCnt];
		
		GetLocalTime(&(THEAPP.m_InspectStartTime[iModuleNo]));	// 검사시작시간

		CurrentTime_Ins = GetTickCount();
		CurrentTime_Ins = CurrentTime_Ins - StartTime_Ins;
		strInspectionTime.Format("%.3lf",CurrentTime_Ins/1000);

		THEAPP.m_pInspectSummary->m_iPocketNo = THEAPP.m_iModuleNumberOneLine * (THEAPP.m_pInspectService->m_iIndexNo_H - 1) + (iModuleNo - 1) % THEAPP.m_iModuleNumberOneLine + 1;

		Temp.Format("%2d 번째 검사 - 모듈 %2d 검사시작", nInspCnt+1, iModuleNo);
		THEAPP.SaveLog(Temp);

		double Xposition,Yposition;

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		THEAPP.m_pCameraManager->ImageGrabCount=0;
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		CString sGrabTime, sGrabTime_total;
		double tGrabStart = 0, tGrabEnd = 0;
		double tGrabStart_total = 0, tGrabEnd_total = 0;

		CString strLog;

#ifdef INLINE_MODE

		tGrabStart_total = GetTickCount();

		StartTime = GetTickCount();

		dInspPosX = THEAPP.m_pModelDataManager->m_mdInspectPosXY[nInspCnt][0];
		dInspPosY = THEAPP.m_pModelDataManager->m_mdInspectPosXY[nInspCnt][1];

		THEAPP.m_pHandlerService->m_bMoveCompleteFlag = FALSE;
		THEAPP.m_pHandlerService->Set_AMoveRequest(dInspPosX, dInspPosY, 1); // 위치이동
		while(!THEAPP.m_pHandlerService->m_bMoveCompleteFlag) // 무브 컴플리트 기다림
		{
			// LOT 시작 명령 수신 시 핸들러 처리를 대기하는 부분 모두 Clear 하도록 코드 추가
			if (pInspectService->GetCycleStopStatus() == TRUE)
				return 0;

			Sleep(1);
		}

		bGrabFail = FALSE;

		CurrentTime_Ins = GetTickCount();

		strLog.Format("모션 %2d, 모듈 %2d 위치 도착, 이동시간: %.0lf ms", nInspCnt+1, iModuleNo, CurrentTime_Ins - StartTime); THEAPP.SaveLog(strLog); THEAPP.SaveDetectLog(strLog);

		int iGrabCnt = THEAPP.m_iMaxInspImageNo;
		double dPrevZpos = 0.0;
		for (int igc = 0; igc < iGrabCnt; igc++)
		{
			BOOL bGrabSuccess = FALSE;

			if (igc == 0) 
				Sleep(5);
			else 
				Sleep(THEAPP.Struct_PreferenceStruct.m_iGrabDelayTime); // 130403 --> 조명 이상 문제로 interval 을 늘림, 현재는 0!!

			if (dPrevZpos != dInspPosZ[igc])
			{
				THEAPP.m_pHandlerService->m_bMoveCompleteFlag = FALSE;
				THEAPP.m_pHandlerService->Set_ZMoveRequest(dInspPosZ[igc]);
				while(!THEAPP.m_pHandlerService->m_bMoveCompleteFlag) // 무브 컴플리트 기다림
				{
					// LOT 시작 명령 수신 시 핸들러 처리를 대기하는 부분 모두 Clear 하도록 코드 추가
					if (pInspectService->GetCycleStopStatus() == TRUE)
						return 0;

					Sleep(1);
				}

				dPrevZpos = dInspPosZ[igc];
			}

			tGrabStart = GetTickCount();
				
			CString sDelay;
			double dWattingGrab_StartTime=0, dWattingGrab_EndtTime=0;

			int K=0;

			//0524
			iRetryCnt = 0;
			for(K=0; K<THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry N회
			{
				//0524
				++iRetryCnt;

				if (THEAPP.m_pCameraManager->ImageGrabCount != igc)	// ImageGrabCount는 igc 와 같아야 한다
				{
					sDelay.Format("  --그랩 %d 실패, GrabCount = %d Error", igc+1, THEAPP.m_pCameraManager->ImageGrabCount);
					THEAPP.SaveLog(sDelay); 
					bGrabFail = TRUE; 
					break;
				}

				sDelay.Format("  ****-- 한번 보자 %d 번째 시도", K+1); 
				THEAPP.SaveLog(sDelay);
				
				// Grab Start
				THEAPP.m_pCameraManager->CallHookFunction(igc);		

				THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, igc);


				strLog.Format("모션 %2d 조명 %d On", nInspCnt+1, igc+1);
				THEAPP.SaveLog(strLog);
				
				Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pCameraManager->m_bGrabDone)
						break;

					dwGrabEnd = GetTickCount();

					if((dwGrabEnd-dwGrabStart) > THEAPP.Struct_PreferenceStruct.m_iGrabDoneWaitTime)
					{
						THEAPP.m_pCameraManager->GrabErrorPostProcess();
						THEAPP.SaveLog("GrabDone 대기시간 초과");
						break;
					}

					Sleep(1);
				}

				// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW START
				if (THEAPP.m_pCameraManager->m_bGrabDone==TRUE && THEAPP.m_pCameraManager->m_bReGrab==FALSE)
				{
					if(THEAPP.m_pCameraManager->GrabErrorCheck(igc)==TRUE)
					{
						THEAPP.m_pCameraManager->ImageGrabCount--;
						THEAPP.m_pCameraManager->GrabErrorPostProcess();
					}
				}					
				// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW END

				dWattingGrab_StartTime=0; dWattingGrab_EndtTime=0;
				dWattingGrab_StartTime = GetTickCount();

				while(TRUE)  // 영상1 그랩을 기다림
				{
					if(THEAPP.m_pCameraManager->ImageGrabCount==igc+1 || THEAPP.m_pCameraManager->m_bReGrab==TRUE)		// 그랩 됐으면 igc가 증가하므로
					{
						if (THEAPP.m_pCameraManager->ImageGrabCount==igc+1 && THEAPP.m_pCameraManager->m_bReGrab==FALSE)
						{
							bGrabSuccess = TRUE;
						}
						break;
					}

					dWattingGrab_EndtTime = GetTickCount();

					if((dWattingGrab_EndtTime-dWattingGrab_StartTime) > 1000)
					{
						THEAPP.SaveLog(" 대기시간 1초 초과 - 다시 찍어라");
						THEAPP.m_pCameraManager->m_bReGrab=TRUE;
						break;
					}

					Sleep(1);
				}
				
				if(THEAPP.m_pCameraManager->m_bReGrab == FALSE) 
				{
					THEAPP.SaveLog("  --찍기 성공한 듯"); 
					break;
				}

				Sleep(THEAPP.Struct_PreferenceStruct.m_iGrabDelayTime);

			}	// for(K=0; K<THEAPP.Struct_PreferenceStruct.m_iGrabErrRetryNo; K++)		// Grab Retry N회		//0524

			if (!bGrabSuccess) 
				bGrabFail = TRUE;		// 그랩이 한번이라도 성공하지 못하면 bGrabFail 활성화 시킨다
			
			tGrabEnd = GetTickCount();
			sGrabTime.Format("* Module %2d 영상%d 그랩 시간: %.0lf ms (시도횟수 %d)", iModuleNo, igc+1, tGrabEnd - tGrabStart, iRetryCnt); 
			THEAPP.SaveLog(sGrabTime);

		}	// for (int igc = 0; igc < iGrabCnt; igc++)

		THEAPP.m_pInspectAdminViewDlg->m_iInspectGrabStatus[nInspCnt] = INSPECT_GRAB_STATUS_GRAB_DONE;

		if (bGrabFail == TRUE) 
		{
			THEAPP.SaveLog("  **-- 영상 그랩 실패. 신호 없음");
						
			THEAPP.m_pInspectSummary->OnBnClickedMfcbuttonInspectPass();
		}

#else	// Offline Mode =>

		Sleep(THEAPP.Struct_PreferenceStruct.m_iGrabDelayTime);

		if(THEAPP.Struct_PreferenceStruct.m_bSaveRawImageMerge == FALSE)
		{
			tGrabStart_total = GetTickCount();
			CString FolderName,ImageName;
			if(THEAPP.Struct_PreferenceStruct.m_strDebug_FolderPath!="")
			{
				FolderName.Format(THEAPP.Struct_PreferenceStruct.m_strDebug_FolderPath+"\\");
			}
			else
			{
				return 0;
			}

			bGrabFail = TRUE;

			try{

				HANDLE hFindFile;
				WIN32_FIND_DATA FindFileData;
				CString sReadFileName, strRawImageFileFullName;
				BOOL bFileFindFail = FALSE;

				// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW START
				BYTE *pImageData;
				char type[30];
				Hlong lImageWidth, lImageHeight;

				int iCamImageWidth = THEAPP.m_pCameraManager->GetCamImageWidth();
				int iCamImageHeight = THEAPP.m_pCameraManager->GetCamImageHeight();
				// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW END

				ImageName.Format("Module%d_Barcode*.jpg", iModuleNo);
				sReadFileName = FolderName + ImageName;
				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);
					FindClose(hFindFile);
					read_image(&THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[nInspCnt], strRawImageFileFullName);
				}
				else
				{
					THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[nInspCnt].Reset();

					bFileFindFail = TRUE;
				}

				for (int imgIdx = 0; imgIdx < MAX_IMAGE_TAB; imgIdx++)
				{	
					if (imgIdx >= THEAPP.m_iMaxInspImageNo)
						break;

					ImageName.Format("Module%d_%s*", iModuleNo, g_sImageName[THEAPP.m_iMachineInspType][imgIdx]);
					sReadFileName = FolderName + ImageName;
					if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
					{
						strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);
						FindClose(hFindFile);
						read_image(&THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[nInspCnt][imgIdx], strRawImageFileFullName);
					}
					else
					{
						bFileFindFail = TRUE;
					}

				}
				
				// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
				for (int i = 0; i < MAX_IMAGE_TAB; i++)
				{
					if (i >= THEAPP.m_iMaxInspImageNo)
						break;

					if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[nInspCnt][i]))
					{
						get_image_pointer1(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[nInspCnt][i], (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

						if (lImageWidth != iCamImageWidth || lImageHeight != iCamImageHeight)
							zoom_image_size(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[nInspCnt][i], &THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[nInspCnt][i], iCamImageWidth,iCamImageHeight, "constant");
					}
					else
					{
						bFileFindFail = TRUE;
					}
					
				}
				// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

				THEAPP.m_pCameraManager->ImageGrabCount = THEAPP.m_iMaxInspImageNo;

				if (bFileFindFail == FALSE)
					bGrabFail = FALSE;
			}
			catch(HException &except)
			{
				CString str;
				str.Format("Halcon Exception [Debug Read Image]: %s", except.message);
				THEAPP.SaveLog(str);
			}

			////////// 가상으로 모션 인터벌을 준다. 그러지 않으면 8개 이상의 스레드가 실행 되어 느려진다
			while (TRUE) 
			{
				tGrabEnd = GetTickCount();
				if ((tGrabEnd - tGrabStart_total) > THEAPP.Struct_PreferenceStruct.m_iGrabDelayTime)
					break;

				// 1 ms 동안 sleep? 
				Sleep(1);
			}
		}
		else
		{
			tGrabStart_total = GetTickCount();
			CString FolderName, ImageName;
			if (THEAPP.Struct_PreferenceStruct.m_strDebug_FolderPath != "")
			{
				FolderName.Format(THEAPP.Struct_PreferenceStruct.m_strDebug_FolderPath+"\\");
			}
			else
			{
				// TODO : 그냥 return 해도 괜찮은건가?
				return 0;
			}

			bGrabFail = TRUE;

			try
			{
				HANDLE hFindFile;
				WIN32_FIND_DATA FindFileData;
				CString sReadFileName, strRawImageFileFullName;
				BOOL bFileFindFail = FALSE;

				ImageName.Format("Module%d_Barcode*.jpg", iModuleNo);
				sReadFileName = FolderName + ImageName;
				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);
					FindClose(hFindFile);
					read_image(&THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[nInspCnt], strRawImageFileFullName);
				}
				else
				{
					THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[nInspCnt].Reset();

					bFileFindFail = TRUE;
				}

				int iCamImageWidth = THEAPP.m_pCameraManager->GetCamImageWidth();
				int iCamImageHeight = THEAPP.m_pCameraManager->GetCamImageHeight();

				int iNoInspectImage = THEAPP.m_iMaxInspImageNo;

				Hobject HCombineImage;
				gen_empty_obj(&HCombineImage);

				BYTE *pImageData;
				char type[30];
				Hlong lImageWidth, lImageHeight;

				int iNoImageYDir, iModular;
				int iCropImageSizeX, iCropImageSizeY;

				iNoImageYDir = iNoInspectImage / MAX_COMBINE_IMAGE_NUMBER;
				iModular = iNoInspectImage % MAX_COMBINE_IMAGE_NUMBER;
				if (iModular > 0)
					iNoImageYDir += 1;

				ImageName.Format("Module%d_Combine*", iModuleNo);
				sReadFileName = FolderName + ImageName;
				if ((hFindFile = FindFirstFile(sReadFileName, &FindFileData)) != INVALID_HANDLE_VALUE)
				{
					strRawImageFileFullName.Format("%s%s", FolderName, (LPCTSTR)FindFileData.cFileName);
					FindClose(hFindFile);
					read_image(&HCombineImage, strRawImageFileFullName);
				}
				else
				{
					bFileFindFail = TRUE;
				}

				if (THEAPP.m_pGFunction->ValidHImage(HCombineImage) == TRUE)
				{
					get_image_pointer1(HCombineImage, (Hlong *)&pImageData, type, &lImageWidth, &lImageHeight);

					int wd = (int)lImageWidth;
					int ht = (int)lImageHeight;

					iCropImageSizeX = wd / MAX_COMBINE_IMAGE_NUMBER;
					iCropImageSizeY = ht / iNoImageYDir;

					POINT CropLTPoint, CropRBPoint;
					int iImageIndexX, iImageIndexY;
					Hobject HRawImage;

					for (int i = 0; i < iNoInspectImage; i++)
					{
						iImageIndexX = i % MAX_COMBINE_IMAGE_NUMBER;
						iImageIndexY = i / MAX_COMBINE_IMAGE_NUMBER;

						CropLTPoint.x = iImageIndexX * iCropImageSizeX;
						CropRBPoint.x = iImageIndexX * iCropImageSizeX + iCropImageSizeX - 1;
						CropLTPoint.y = iImageIndexY * iCropImageSizeY;
						CropRBPoint.y = iImageIndexY * iCropImageSizeY + iCropImageSizeY - 1;

						crop_rectangle1(HCombineImage, &HRawImage, CropLTPoint.y, CropLTPoint.x, CropRBPoint.y, CropRBPoint.x);

						if (THEAPP.m_pGFunction->ValidHImage(HRawImage))
						{
							get_image_pointer1(HRawImage, (Hlong *)&pImageData, type, &lImageWidth, &lImageHeight);

							if (lImageWidth != iCamImageWidth || lImageHeight != iCamImageHeight)
								zoom_image_size(HRawImage, &HRawImage, iCamImageWidth, iCamImageHeight, "constant");

							copy_image(HRawImage, &THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[nInspCnt][i]);
						}
						else
						{
							bFileFindFail = TRUE;
						}
					}
				}
				else
				{
					bFileFindFail = TRUE;
				}

				THEAPP.m_pCameraManager->ImageGrabCount = THEAPP.m_iMaxInspImageNo;

				if (bFileFindFail == FALSE)
					bGrabFail = FALSE;
			}
			catch(HException &except)
			{
				CString str;
				str.Format("Halcon Exception [Debug Read Image]: %s", except.message);
				THEAPP.SaveLog(str);
			}

			////////// 가상으로 모션 인터벌을 준다. 그러지 않으면 8개 이상의 스레드가 실행 되어 느려진다
			while (TRUE) {
				tGrabEnd = GetTickCount();
				if ((tGrabEnd-tGrabStart_total) > THEAPP.Struct_PreferenceStruct.m_iGrabDelayTime) break;
				Sleep(1);
			}
		}

#endif	// Offline Mode

		tGrabEnd_total = GetTickCount();
		sGrabTime.Format("* Module %2d 영상 그랩 시간 total: %.0lf ms", iModuleNo, tGrabEnd_total - tGrabStart_total);
		THEAPP.SaveLog(sGrabTime);
		THEAPP.SaveDetectLog(sGrabTime);

#ifdef INLINE_MODE

		Hobject HCropPartImage;

		if (bGrabFail == FALSE)
		{
			try
			{
				if (THEAPP.Struct_PreferenceStruct.m_iCamFOVType==CAM_FOV_CROP)
				{
					for (int k = 0; k < MAX_IMAGE_TAB; k++)
					{
						if (k >= THEAPP.m_iMaxInspImageNo)
							break;

						crop_part(THEAPP.m_pCameraManager->m_hoCallBackImage[k], &HCropPartImage, GRAB_Y_OFFSET, GRAB_X_OFFSET, GRAB_Y_MAX, GRAB_X_MAX);
						copy_image(HCropPartImage, &(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[nInspCnt][k]));
					}
				}
				else
				{
					for (int k = 0; k < MAX_IMAGE_TAB; k++)
					{
						if (k >= THEAPP.m_iMaxInspImageNo)
							break;

						copy_image(THEAPP.m_pCameraManager->m_hoCallBackImage[k], &(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[nInspCnt][k]));
					}
				}
			}
			catch(HException &except)
			{
				bGrabFail = TRUE;
			}
		}

		while(THEAPP.m_pInspectAdminViewDlg->m_iBarcodeGrabStatus[nInspCnt] == BARCODE_GRAB_STATUS_NOT_READY) // 바코드 Grab 완료 기다림
		{
			Sleep(1);
		}

#endif

		/////////// 모듈 검출 시작 /////////////////////////////////////////
		strInspectionTime.Format("* Module %2d Defect 검출 시작", iModuleNo); THEAPP.SaveLog(strInspectionTime);
		double tDetectStart, tDetectEnd;
		tDetectStart = GetTickCount();

		if (bGrabFail == TRUE)		// 그랩 실패이면 검사 안하고 Align Error 판정 한다
		{
			//////////////////////////////////////////////////////////////////////////
			//		Barcode Inspection

			CString sBarcodeResult;
			Hobject HBarcodeImage;
			double dBarcodePosX, dBarcodePosY;

			if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[nInspCnt])==TRUE)
				copy_image(THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[nInspCnt], &HBarcodeImage);
			else
				HBarcodeImage.Reset();
			
			sBarcodeResult = _T("");
			dBarcodePosX = dBarcodePosY = 0;

			double dBarcodePosRotation = 0;
			pInspectService->m_pInspectAlgorithm[nInspCnt].InspectBarcode(&HBarcodeImage, &sBarcodeResult, &dBarcodePosX, &dBarcodePosY, &dBarcodePosRotation, THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX, THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY ); //Barcode Rotation Inspection
			
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_sBarcodeResult = sBarcodeResult;
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_dBarcodePosOffsetX = dBarcodePosX;
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_dBarcodePosOffsetY = dBarcodePosY;
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeShiftNG = FALSE;
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeModuleMixNG = FALSE;

			//Barcode Rotation Log
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_dBarcodePosOffsetRotation = dBarcodePosRotation;
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeRotationNG = FALSE;

			if (sBarcodeResult == BARCODE_STATUS_NOGRAB || sBarcodeResult == BARCODE_STATUS_DEFAULT)
				pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeError = TRUE;
			else
			{
				pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeError = FALSE;

				if (THEAPP.m_pModelDataManager->m_bInspectBarcodeShift)
				{
					if (abs(dBarcodePosX) >= THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceX || abs(dBarcodePosY) >= THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceY)
						pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeShiftNG = TRUE;
					else
						pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeShiftNG = FALSE;
				}

				//Barcode Rotation Inspection Start
				if (THEAPP.m_pModelDataManager->m_bInspectBarcodeRotation)
				{
					if (abs(dBarcodePosRotation) >= THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceRotation )
					{
						pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeShiftNG = TRUE;
						pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeRotationNG = TRUE; //Barcode Rotation Log
					}
				}
				
				if(pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeShiftNG == TRUE)
				{
					CString strBarcodeShiftLog;
					strBarcodeShiftLog.Format("Barcode Shift NG X:%.2lf Y:%.2lf Deg:%.2lf",dBarcodePosX,dBarcodePosY,dBarcodePosRotation);
					THEAPP.SaveLog(strBarcodeShiftLog);
					THEAPP.m_pHandlerService->Set_BarcodeAlarmRequest("Barcode",THEAPP.m_pInspectResultDlg->CurrentLotID, HANDLER_ALARM_BARCODE_NG, dBarcodePosX, dBarcodePosY, dBarcodePosRotation );
				}
				//Barcode Rotation Inspection End

				// Add Module Mix
				CString sEEER = sBarcodeResult;
				sEEER = sEEER.Right(6);
				sEEER = sEEER.Mid(0,4);

				CString sDataFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
				CIniFileCS EEER_INI(sDataFolder + "\\ModelBarcode.ini");
				CString strSection = "EEER Code";

				CString sModelName;
				sModelName = EEER_INI.Get_String(strSection,sEEER,"Undefined");

				int iRetIndex = -1;
				if (sModelName != "Undefined")
				{
					iRetIndex = THEAPP.m_pModelDataManager->m_sModelName.Find(sModelName, 0);

					if (iRetIndex < 0)
						pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeModuleMixNG = TRUE;
					else
						pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBarcodeModuleMixNG = FALSE;
				}
			}	

			//////////////////////////////////////////////////////////////////////////

			pInspectService->m_pInspectAlgorithm[nInspCnt].m_nModuleNo = iModuleNo;
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_bMatchingSuccess = FALSE;
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_bModuleEmpty = FALSE;
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_bLightDisorder = FALSE;
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_bBlackCoatingDiameterNG = FALSE;
			pInspectService->m_pInspectAlgorithm[nInspCnt].ThreadFinishFlag = 1;
		}
		else
		{
			pInspectService->m_pInspectAlgorithm[nInspCnt].m_nModuleNo = iModuleNo;
			pInspectService->m_pInspectAlgorithm[nInspCnt].CopyInspectInfomation(
				THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[nInspCnt],
				&THEAPP.m_pInspectAdminViewDlg->pHModuleBarcodeGrabImage[nInspCnt],
				THEAPP.m_pModelDataManager->Contours,
				THEAPP.m_pModelDataManager->ModelID,
				THEAPP.m_pModelDataManager->m_lLAlignModelID);
		}

		CurrentTime_Ins = GetTickCount();
		strInspectionTime.Format("%.3lf", (CurrentTime_Ins-StartTime_Ins)/1000);

		CString strTimeCheck;
		strTimeCheck.Format("*************************************nInspCnt %d/ 모션 이동시간 Module %2d : %.0lf ms", nInspCnt, iModuleNo, strInspectionTime);
		THEAPP.SaveLog(strTimeCheck);

		nInspCnt++;
	}

	THEAPP.m_pInspectSummary->m_sInspStatus = _T("Scan Completed");

#ifdef INLINE_MODE
	// 2025.06.25 - 핸들러 응답 체크 - LeeGW
	THEAPP.m_pHandlerService->ResetReplyReceived();

	int iHandlerRetryNum = 0;
	while (TRUE)
	{
		iHandlerRetryNum++;

		if ((THEAPP.Struct_PreferenceStruct.m_bUseMboMode == FALSE) || (THEAPP.Struct_PreferenceStruct.m_bUseMboMode == TRUE && THEAPP.m_pInspectService->m_iMboCount_H == THEAPP.Struct_PreferenceStruct.m_iMboModeCount))
			THEAPP.m_pHandlerService->Set_ScanComplete(pInspectService->m_iLineNo_H);

		if (THEAPP.Struct_PreferenceStruct.m_bUseHandlerRetry == TRUE)
			Sleep(THEAPP.Struct_PreferenceStruct.m_iHandlerReplyWaitTime);

		if (THEAPP.Struct_PreferenceStruct.m_bUseHandlerRetry == FALSE || 
			THEAPP.m_pHandlerService->Is_ReplyReceived() == TRUE || 
			iHandlerRetryNum >= THEAPP.Struct_PreferenceStruct.m_iHandlerRetryCount)
			{
				break;
			}

		Sleep(THEAPP.Struct_PreferenceStruct.m_iHandlerRetryWaitTime);

		THEAPP.SaveLog("********** Handler I 스캔완료 송신 재시도 ******************");
	}

	CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
	CIniFileCS TXT_Status(strStatusFileName);
	CString sSection = "Status";
	TXT_Status.Set_Bool (sSection, "InspectScanComplete", TRUE);
	TXT_Status.Set_String (sSection, "TrayResult", "NNNNNN");
	TXT_Status.Set_String (sSection, "Status", "Scan Completed");

	dTimeEnd = GetTickCount();
	sTimeCheck.Format("********** 트레이 스캔 시간 : %.4lf s", (dTimeEnd - dTimeStart)/1000);
	THEAPP.SaveLog(sTimeCheck);
	THEAPP.SaveDetectLog(sTimeCheck);

#endif

	CurrentTime_Ins = GetTickCount();
	CurrentTime_Ins = CurrentTime_Ins - StartTime_Ins;
	strInspectionTime.Format("%.3lf", CurrentTime_Ins / 1000);

	EndTime = GetTickCount();


#ifdef LOOPINSPECTION
	Sleep(3000);
	THEAPP.m_pInspectSummary->SendMessage(WM_INSPECTION_LOOP,0,0);
#endif

	return 0;
}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [InspectionThread] : %s", except.message); THEAPP.SaveLog(str);
	return 0;
}
}

// about 1800 lines - 251014, jhkim
UINT ResultSaveThread(LPVOID lp)
{
	try
	{
		CInspectService* pInspectService = (CInspectService*)lp;

		// Multiple Defect
		// Vision 에서 최대 3개의 LotID 를 저장함.
		// 다른 설비에서는 투입 Port 가 여러개여서, 매거진이 1개 이상임.
		// 이런 매거진을 검사하기 위한 변수.
		int nVirtualMzIdx = THEAPP.m_stVirtualMagazineNo.GetVirtualMagazineNo(pInspectService->m_sLotID_H);

		const int iTrayModuleMax = THEAPP.m_pModelDataManager->m_iTrayModuleMax;
		int iLineNo = pInspectService->m_iLineNo_H;	// Add
		int iModuleNumber;

		int iSum = 0;
		int iThreadCount = 0;
		char* ResultSaveDisplay;
		ResultSaveDisplay = new char[THEAPP.m_iModuleNumberOneLine];

		for (iThreadCount = 0; iThreadCount < THEAPP.m_iModuleNumberOneLine; iThreadCount++)
		{
			ResultSaveDisplay[iThreadCount] = 0;
		}

		const int iTrayModuleAmount = THEAPP.m_pModelDataManager->m_iTrayModuleAmt;

		CString sStatusFileName = THEAPP.m_FileBase.m_strTrayResultFolderPrev + "\\TrayResult.txt";
		CIniFileCS fResult(sStatusFileName);
		CString sSection = "Result", sKey;

		for (int k = 0; k < THEAPP.m_iModuleNumberOneLine; k++)
		{
			if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_X)
				iModuleNumber = iLineNo + THEAPP.m_iTrayLineNumber * k;
			else
				iModuleNumber = (THEAPP.m_iTrayLineNumber - iLineNo) * THEAPP.m_iModuleNumberOneLine + k + 1;

			sKey.Format("%02d", iModuleNumber);
			fResult.Set_String (sSection, sKey, "nan");
		}

		CString temp;
		temp.Format("ResultSaveThread, Tray %d, Module %d", pInspectService->m_iTrayNo_H == 1, iTrayModuleAmount);
		THEAPP.SaveLog(temp);

		// Barrel Defects & Lens & Barcode Shift & Sidefill Epoxy Hole / 기존: MAX_DEFECT_NUMBER+3
		int iDefectCount[MAX_DEFECT_NUMBER];
		for (int i = 0; i < (MAX_DEFECT_NUMBER); i++)
			iDefectCount[i] = 0;

		int iDefectCntRet = DEFECT_CODE_GOOD;
		int iFAISpecialNGCnt = 0;

		Hobject HDefectIntersectRgn;
		int iImageIdx;

		// ReviewImage 저장 - LeeGW
		CString strImageFileName, strImageNo;
		Hobject HInspectImage;

		BOOL bBarrelDefectExist, bLensDefectExist;
		Hobject HZoomImageSurface, HZoomImageEdge, HZoomImageLens1, HZoomImageLens2;
		BYTE *pImageData[BARREL_LENS_IMAGE_TAB];
		char type[30];
		Hlong lImageWidth, lImageHeight;

		// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW START
		int* m_bSpecialNGSortFlag;
		m_bSpecialNGSortFlag = new int[THEAPP.m_iModuleNumberOneLine];
		for (int i = 0; i < THEAPP.m_iModuleNumberOneLine; i++)
		{
			m_bSpecialNGSortFlag[i] = FALSE;
		}
		// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW END

		while(TRUE)
		{
			iSum = 0;
			for(iThreadCount=0; iThreadCount<THEAPP.m_iModuleNumberOneLine; iThreadCount++)
			{
				if(pInspectService->m_pInspectAlgorithm[iThreadCount].ThreadFinishFlag == TRUE)
				{
					if(ResultSaveDisplay[iThreadCount]==0)
					{
						double tInspectStart_total, tInspectEnd_total;
						tInspectStart_total = GetTickCount();

						int iModuleNo = iThreadCount + 1; // 초기화, offline 모드일 땐 검사순서(+1)와 모듈번호가 같다

						iModuleNo = pInspectService->m_pInspectAlgorithm[iThreadCount].m_nModuleNo;

						// 800 줄 짜리 if 구문
						if (pInspectService->m_pInspectAlgorithm[iThreadCount].m_bMatchingSuccess) // 검사한 경우
						{
#pragma region Init Variables
							///////// 결과저장여기
							bBarrelDefectExist = FALSE;
							bLensDefectExist = FALSE;

							THEAPP.m_pInspectAdminViewHideDlg->m_iModuleNo = iModuleNo;
							THEAPP.m_pInspectAdminViewHideDlg->iDirtDefectSortingNumber = -1;
							THEAPP.m_pInspectAdminViewHideDlg->iScratchDefectSortingNumber = -1;
							THEAPP.m_pInspectAdminViewHideDlg->iStainDefectSortingNumber = -1;
							THEAPP.m_pInspectAdminViewHideDlg->iDentDefectSortingNumber = -1;
							THEAPP.m_pInspectAdminViewHideDlg->iChippingDefectSortingNumber = -1;
							THEAPP.m_pInspectAdminViewHideDlg->iEpoxyDefectSortingNumber = -1;
							THEAPP.m_pInspectAdminViewHideDlg->iOutsideCTDefectSortingNumber = -1;
							THEAPP.m_pInspectAdminViewHideDlg->iPeelOffDefectSortingNumber = -1;
							THEAPP.m_pInspectAdminViewHideDlg->iWhiteDotDefectSortingNumber = -1;
#pragma endregion

							//////////////// 결과 저장 및 출력을 위해(WriteResultFile) 베럴 판정 MinArea 통과한 영역을 InspectAdminViewHideDlg 로 보낸다
#pragma region Apply Inspection condition for Barrel Defects & Save Result
							Hobject HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn;
							if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
							{
								if (!THEAPP.m_pModelDataManager->m_bOneBarrel)
								{
									THEAPP.m_pAlgorithm->GetInspectArea_Barrel(pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType][MATCHING_CONTOUR_5], pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType][MATCHING_CONTOUR_2], 0, -1, &HTopBarrelRgn);
									THEAPP.m_pAlgorithm->GetInspectArea_Barrel(pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType][MATCHING_CONTOUR_2], pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType][MATCHING_CONTOUR_1], 0, 0, &HBottomBarrelRgn);
									THEAPP.m_pAlgorithm->GetInspectArea_Inner(pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType][MATCHING_CONTOUR_4],
										THEAPP.m_pModelDataManager->m_iDecisonTopBarrelInner_RoiInnerMargin,
										THEAPP.m_pModelDataManager->m_iDecisonTopBarrelInner_RoiOuterMargin, &HInnerBarrelRgn);
								}
								else
								{
									THEAPP.m_pAlgorithm->GetInspectArea_Barrel(pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType][MATCHING_CONTOUR_5], pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType][MATCHING_CONTOUR_3], 0, 0, &HTopBarrelRgn);
									gen_empty_obj(&HBottomBarrelRgn);
									THEAPP.m_pAlgorithm->GetInspectArea_Inner(pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[THEAPP.m_pModelDataManager->m_iBlackMatchingImageType][MATCHING_CONTOUR_4],
										THEAPP.m_pModelDataManager->m_iDecisonTopBarrelInner_RoiInnerMargin,
										THEAPP.m_pModelDataManager->m_iDecisonTopBarrelInner_RoiOuterMargin, &HInnerBarrelRgn);
								}
							}

							Hobject HInspectImage;
							gen_empty_obj(&HInspectImage);

							gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDirt);
							if(THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectDirtRgn))
							{
								THEAPP.m_pInspectAdminViewHideDlg->iDirtDefectSortingNumber = THEAPP.m_pAlgorithm->ApplyInspectionCondition(
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectDirtRgn, HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDirt,
									THEAPP.m_pModelDataManager->m_dDirtMinArea_Top, THEAPP.m_pModelDataManager->m_dDirtMidArea_Top, THEAPP.m_pModelDataManager->m_dDirtMaxArea_Top,
									THEAPP.m_pModelDataManager->m_iDirtMinNumAccept_Top, THEAPP.m_pModelDataManager->m_iDirtMidNumAccept_Top, THEAPP.m_pModelDataManager->m_iDirtMaxNumAccept_Top,
									THEAPP.m_pModelDataManager->m_dDirtMinArea_Btm, THEAPP.m_pModelDataManager->m_dDirtMidArea_Btm, THEAPP.m_pModelDataManager->m_dDirtMaxArea_Btm,
									THEAPP.m_pModelDataManager->m_iDirtMinNumAccept_Btm, THEAPP.m_pModelDataManager->m_iDirtMidNumAccept_Btm, THEAPP.m_pModelDataManager->m_iDirtMaxNumAccept_Btm,
									THEAPP.m_pModelDataManager->m_dDirtMinArea_Inner, THEAPP.m_pModelDataManager->m_dDirtMidArea_Inner, THEAPP.m_pModelDataManager->m_dDirtMaxArea_Inner,
									THEAPP.m_pModelDataManager->m_iDirtMinNumAccept_Inner, THEAPP.m_pModelDataManager->m_iDirtMidNumAccept_Inner, THEAPP.m_pModelDataManager->m_iDirtMaxNumAccept_Inner);

								if(THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDirt))
								{
									auto* pCopyTarget = &THEAPP.m_stMultipleDefectInfo[nVirtualMzIdx][DEFECT_NAME_DIRT].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1];

									copy_obj(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDirt, pCopyTarget, 1, -1); // Multiple Defect

									bBarrelDefectExist = TRUE;
									for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
									{
										intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDirt, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
										if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
										{
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
											copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
											strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];

											strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_DIRT]);
											THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
										}
									}
								}
								
							}	// m_HRgnDirt: MinArea 을 통과한 Dirt 영역들

							gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnScratch);
							if(THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectScratchRgn))
							{
								THEAPP.m_pInspectAdminViewHideDlg->iScratchDefectSortingNumber = THEAPP.m_pAlgorithm->ApplyInspectionCondition(
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectScratchRgn, HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnScratch,
									THEAPP.m_pModelDataManager->m_dScratchMinArea_Top, THEAPP.m_pModelDataManager->m_dScratchMidArea_Top, THEAPP.m_pModelDataManager->m_dScratchMaxArea_Top,
									THEAPP.m_pModelDataManager->m_iScratchMinNumAccept_Top, THEAPP.m_pModelDataManager->m_iScratchMidNumAccept_Top, THEAPP.m_pModelDataManager->m_iScratchMaxNumAccept_Top,
									THEAPP.m_pModelDataManager->m_dScratchMinArea_Btm, THEAPP.m_pModelDataManager->m_dScratchMidArea_Btm, THEAPP.m_pModelDataManager->m_dScratchMaxArea_Btm,
									THEAPP.m_pModelDataManager->m_iScratchMinNumAccept_Btm, THEAPP.m_pModelDataManager->m_iScratchMidNumAccept_Btm, THEAPP.m_pModelDataManager->m_iScratchMaxNumAccept_Btm,
									THEAPP.m_pModelDataManager->m_dScratchMinArea_Inner, THEAPP.m_pModelDataManager->m_dScratchMidArea_Inner, THEAPP.m_pModelDataManager->m_dScratchMaxArea_Inner,
									THEAPP.m_pModelDataManager->m_iScratchMinNumAccept_Inner, THEAPP.m_pModelDataManager->m_iScratchMidNumAccept_Inner, THEAPP.m_pModelDataManager->m_iScratchMaxNumAccept_Inner);

								if(THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnScratch))
								{
									auto* pCopyTarget = &THEAPP.m_stMultipleDefectInfo[nVirtualMzIdx][DEFECT_NAME_SCRATCH].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1];

									copy_obj(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnScratch, pCopyTarget, 1, -1); // Multiple Defect

									bBarrelDefectExist = TRUE;
									for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
									{
										intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnScratch, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
										if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
										{
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
											copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
											strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


											strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_SCRATCH]);
											THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
										}
									}
								}

							}	// m_HRgnScratch: MinArea 을 통과한 Scratch 영역들

							gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnStain);
							if(THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectStainRgn))
							{
								THEAPP.m_pInspectAdminViewHideDlg->iStainDefectSortingNumber = THEAPP.m_pAlgorithm->ApplyInspectionCondition(
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectStainRgn, HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnStain,
									THEAPP.m_pModelDataManager->m_dStainMinArea_Top, THEAPP.m_pModelDataManager->m_dStainMidArea_Top, THEAPP.m_pModelDataManager->m_dStainMaxArea_Top,
									THEAPP.m_pModelDataManager->m_iStainMinNumAccept_Top, THEAPP.m_pModelDataManager->m_iStainMidNumAccept_Top, THEAPP.m_pModelDataManager->m_iStainMaxNumAccept_Top,
									THEAPP.m_pModelDataManager->m_dStainMinArea_Btm, THEAPP.m_pModelDataManager->m_dStainMidArea_Btm, THEAPP.m_pModelDataManager->m_dStainMaxArea_Btm,
									THEAPP.m_pModelDataManager->m_iStainMinNumAccept_Btm, THEAPP.m_pModelDataManager->m_iStainMidNumAccept_Btm, THEAPP.m_pModelDataManager->m_iStainMaxNumAccept_Btm,
									THEAPP.m_pModelDataManager->m_dStainMinArea_Inner, THEAPP.m_pModelDataManager->m_dStainMidArea_Inner, THEAPP.m_pModelDataManager->m_dStainMaxArea_Inner,
									THEAPP.m_pModelDataManager->m_iStainMinNumAccept_Inner, THEAPP.m_pModelDataManager->m_iStainMidNumAccept_Inner, THEAPP.m_pModelDataManager->m_iStainMaxNumAccept_Inner);

								if(THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnStain))
								{
									copy_obj(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnStain,
										&THEAPP.m_stMultipleDefectInfo[nVirtualMzIdx][DEFECT_NAME_STAIN].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1], 
										1 , -1 ); //Multiple Defect

									bBarrelDefectExist = TRUE;
									for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
									{
										intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnStain, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
										if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
										{
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
											copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
											strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


											strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_STAIN]);
											THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
										}
									}
								}

							}	// m_HRgnStain: MinArea 을 통과한 Stain 영역들

							gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDent);
							if(THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectDentRgn))
							{
								THEAPP.m_pInspectAdminViewHideDlg->iDentDefectSortingNumber = THEAPP.m_pAlgorithm->ApplyInspectionCondition(
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectDentRgn, HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDent,
									THEAPP.m_pModelDataManager->m_dDentMinArea_Top, THEAPP.m_pModelDataManager->m_dDentMidArea_Top, THEAPP.m_pModelDataManager->m_dDentMaxArea_Top,
									THEAPP.m_pModelDataManager->m_iDentMinNumAccept_Top, THEAPP.m_pModelDataManager->m_iDentMidNumAccept_Top, THEAPP.m_pModelDataManager->m_iDentMaxNumAccept_Top,
									THEAPP.m_pModelDataManager->m_dDentMinArea_Btm, THEAPP.m_pModelDataManager->m_dDentMidArea_Btm, THEAPP.m_pModelDataManager->m_dDentMaxArea_Btm,
									THEAPP.m_pModelDataManager->m_iDentMinNumAccept_Btm, THEAPP.m_pModelDataManager->m_iDentMidNumAccept_Btm, THEAPP.m_pModelDataManager->m_iDentMaxNumAccept_Btm,
									THEAPP.m_pModelDataManager->m_dDentMinArea_Inner, THEAPP.m_pModelDataManager->m_dDentMidArea_Inner, THEAPP.m_pModelDataManager->m_dDentMaxArea_Inner,
									THEAPP.m_pModelDataManager->m_iDentMinNumAccept_Inner, THEAPP.m_pModelDataManager->m_iDentMidNumAccept_Inner, THEAPP.m_pModelDataManager->m_iDentMaxNumAccept_Inner);

								if(THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDent))
								{
									copy_obj(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDent,
										&THEAPP.m_stMultipleDefectInfo[nVirtualMzIdx][DEFECT_NAME_DENT].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1], 
										1 , -1 ); //Multiple Defect

									bBarrelDefectExist = TRUE;
									for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
									{
										intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnDent, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
										if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
										{
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
											copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
											strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


											strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_DENT]);
											THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
										}
									}
								}

							}	// m_HRgnDent: MinArea 을 통과한 Dent 영역들

							gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnChipping);
							if(THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectChippingRgn))
							{
								THEAPP.m_pInspectAdminViewHideDlg->iChippingDefectSortingNumber = THEAPP.m_pAlgorithm->ApplyInspectionCondition(
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectChippingRgn, HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnChipping,
									THEAPP.m_pModelDataManager->m_dChipMinArea_Top, THEAPP.m_pModelDataManager->m_dChipMidArea_Top, THEAPP.m_pModelDataManager->m_dChipMaxArea_Top,
									THEAPP.m_pModelDataManager->m_iChipMinNumAccept_Top, THEAPP.m_pModelDataManager->m_iChipMidNumAccept_Top, THEAPP.m_pModelDataManager->m_iChipMaxNumAccept_Top,
									THEAPP.m_pModelDataManager->m_dChipMinArea_Btm, THEAPP.m_pModelDataManager->m_dChipMidArea_Btm, THEAPP.m_pModelDataManager->m_dChipMaxArea_Btm,
									THEAPP.m_pModelDataManager->m_iChipMinNumAccept_Btm, THEAPP.m_pModelDataManager->m_iChipMidNumAccept_Btm, THEAPP.m_pModelDataManager->m_iChipMaxNumAccept_Btm,
									THEAPP.m_pModelDataManager->m_dChipMinArea_Inner, THEAPP.m_pModelDataManager->m_dChipMidArea_Inner, THEAPP.m_pModelDataManager->m_dChipMaxArea_Inner,
									THEAPP.m_pModelDataManager->m_iChipMinNumAccept_Inner, THEAPP.m_pModelDataManager->m_iChipMidNumAccept_Inner, THEAPP.m_pModelDataManager->m_iChipMaxNumAccept_Inner);

								if(THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnChipping))
								{
									copy_obj(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnChipping,
										&THEAPP.m_stMultipleDefectInfo[nVirtualMzIdx][DEFECT_NAME_CHIPPING].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1], 
										1 , -1 ); //Multiple Defect

									bBarrelDefectExist = TRUE;
									for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
									{
										intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnChipping, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
										if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
										{
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
											copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
											strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


											strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_CHIPPING]);
											THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
										}
									}
								}

							}	// m_HRgnChipping: MinArea 을 통과한 Chipping 영역들

							gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxy);
							if(THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectEpoxyRgn))
							{
								THEAPP.m_pInspectAdminViewHideDlg->iEpoxyDefectSortingNumber = THEAPP.m_pAlgorithm->ApplyInspectionCondition(
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectEpoxyRgn, HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxy,
									THEAPP.m_pModelDataManager->m_dEpoxyMinArea_Top, THEAPP.m_pModelDataManager->m_dEpoxyMidArea_Top, THEAPP.m_pModelDataManager->m_dEpoxyMaxArea_Top,
									THEAPP.m_pModelDataManager->m_iEpoxyMinNumAccept_Top, THEAPP.m_pModelDataManager->m_iEpoxyMidNumAccept_Top, THEAPP.m_pModelDataManager->m_iEpoxyMaxNumAccept_Top,
									THEAPP.m_pModelDataManager->m_dEpoxyMinArea_Btm, THEAPP.m_pModelDataManager->m_dEpoxyMidArea_Btm, THEAPP.m_pModelDataManager->m_dEpoxyMaxArea_Btm,
									THEAPP.m_pModelDataManager->m_iEpoxyMinNumAccept_Btm, THEAPP.m_pModelDataManager->m_iEpoxyMidNumAccept_Btm, THEAPP.m_pModelDataManager->m_iEpoxyMaxNumAccept_Btm,
									THEAPP.m_pModelDataManager->m_dEpoxyMinArea_Inner, THEAPP.m_pModelDataManager->m_dEpoxyMidArea_Inner, THEAPP.m_pModelDataManager->m_dEpoxyMaxArea_Inner,
									THEAPP.m_pModelDataManager->m_iEpoxyMinNumAccept_Inner, THEAPP.m_pModelDataManager->m_iEpoxyMidNumAccept_Inner, THEAPP.m_pModelDataManager->m_iEpoxyMaxNumAccept_Inner);

								if(THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxy))
								{
									copy_obj(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxy,
										&THEAPP.m_stMultipleDefectInfo[nVirtualMzIdx][DEFECT_NAME_EPOXY].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1], 
										1 , -1 ); //Multiple Defect

									bBarrelDefectExist = TRUE;
									for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
									{
										intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxy, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
										if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
										{
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
											copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1 ,1);
											strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


											strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_EPOXY]);
											THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
										}
									}
								}

							}	// m_HRgnEpoxy: MinArea 을 통과한 Epoxy 영역들

							gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnOutsideCT);
							if(THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectOutsideCTRgn))
							{
								THEAPP.m_pInspectAdminViewHideDlg->iOutsideCTDefectSortingNumber = THEAPP.m_pAlgorithm->ApplyInspectionCondition(
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectOutsideCTRgn, HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnOutsideCT,
									THEAPP.m_pModelDataManager->m_dRes1MinArea_Top, THEAPP.m_pModelDataManager->m_dRes1MidArea_Top, THEAPP.m_pModelDataManager->m_dRes1MaxArea_Top,
									THEAPP.m_pModelDataManager->m_iRes1MinNumAccept_Top, THEAPP.m_pModelDataManager->m_iRes1MidNumAccept_Top, THEAPP.m_pModelDataManager->m_iRes1MaxNumAccept_Top,
									THEAPP.m_pModelDataManager->m_dRes1MinArea_Btm, THEAPP.m_pModelDataManager->m_dRes1MidArea_Btm, THEAPP.m_pModelDataManager->m_dRes1MaxArea_Btm,
									THEAPP.m_pModelDataManager->m_iRes1MinNumAccept_Btm, THEAPP.m_pModelDataManager->m_iRes1MidNumAccept_Btm, THEAPP.m_pModelDataManager->m_iRes1MaxNumAccept_Btm,
									THEAPP.m_pModelDataManager->m_dRes1MinArea_Inner, THEAPP.m_pModelDataManager->m_dRes1MidArea_Inner, THEAPP.m_pModelDataManager->m_dRes1MaxArea_Inner,
									THEAPP.m_pModelDataManager->m_iRes1MinNumAccept_Inner, THEAPP.m_pModelDataManager->m_iRes1MidNumAccept_Inner, THEAPP.m_pModelDataManager->m_iRes1MaxNumAccept_Inner);

								if(THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnOutsideCT))
								{
									copy_obj(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnOutsideCT,
										&THEAPP.m_stMultipleDefectInfo[nVirtualMzIdx][DEFECT_NAME_OUTSIDE_CT].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1], 
										1 , -1 ); //Multiple Defect

									bBarrelDefectExist = TRUE;
									for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
									{
										intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnOutsideCT, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
										if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
										{
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
											copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
											strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


											strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_OUTSIDE_CT]);
											THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
										}
									}
								}

							}	// m_HRgnOutsideCT: MinArea 을 통과한 Res1 영역들

							gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnPeelOff);
							if(THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectPeelOffRgn))
							{
								THEAPP.m_pInspectAdminViewHideDlg->iPeelOffDefectSortingNumber = THEAPP.m_pAlgorithm->ApplyInspectionCondition(
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectPeelOffRgn, HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnPeelOff,
									THEAPP.m_pModelDataManager->m_dRes2MinArea_Top, THEAPP.m_pModelDataManager->m_dRes2MidArea_Top, THEAPP.m_pModelDataManager->m_dRes2MaxArea_Top,
									THEAPP.m_pModelDataManager->m_iRes2MinNumAccept_Top, THEAPP.m_pModelDataManager->m_iRes2MidNumAccept_Top, THEAPP.m_pModelDataManager->m_iRes2MaxNumAccept_Top,
									THEAPP.m_pModelDataManager->m_dRes2MinArea_Btm, THEAPP.m_pModelDataManager->m_dRes2MidArea_Btm, THEAPP.m_pModelDataManager->m_dRes2MaxArea_Btm,
									THEAPP.m_pModelDataManager->m_iRes2MinNumAccept_Btm, THEAPP.m_pModelDataManager->m_iRes2MidNumAccept_Btm, THEAPP.m_pModelDataManager->m_iRes2MaxNumAccept_Btm,
									THEAPP.m_pModelDataManager->m_dRes2MinArea_Inner, THEAPP.m_pModelDataManager->m_dRes2MidArea_Inner, THEAPP.m_pModelDataManager->m_dRes2MaxArea_Inner,
									THEAPP.m_pModelDataManager->m_iRes2MinNumAccept_Inner, THEAPP.m_pModelDataManager->m_iRes2MidNumAccept_Inner, THEAPP.m_pModelDataManager->m_iRes2MaxNumAccept_Inner);

								if(THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnPeelOff))
								{
									copy_obj(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnPeelOff,
										&THEAPP.m_stMultipleDefectInfo[nVirtualMzIdx][DEFECT_NAME_PEEL_OFF].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1], 
										1 , -1 ); //Multiple Defect

									bBarrelDefectExist = TRUE;
									for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
									{
										intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnPeelOff, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
										if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
										{
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
											copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
											strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


											strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_PEEL_OFF]);
											THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
										}
									}
								}

							}	// m_HRgnPeelOff: MinArea 을 통과한 Res2 영역들

							// White Dot - LeeGW
							gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnWhiteDot);
							if(THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectWhiteDotRgn))
							{
								THEAPP.m_pInspectAdminViewHideDlg->iWhiteDotDefectSortingNumber = THEAPP.m_pAlgorithm->ApplyInspectionCondition(
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectWhiteDotRgn, HTopBarrelRgn, HBottomBarrelRgn, HInnerBarrelRgn, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnWhiteDot,
									THEAPP.m_pModelDataManager->m_dWhiteDotMinArea_Top, THEAPP.m_pModelDataManager->m_dWhiteDotMidArea_Top, THEAPP.m_pModelDataManager->m_dWhiteDotMaxArea_Top,
									THEAPP.m_pModelDataManager->m_iWhiteDotMinNumAccept_Top, THEAPP.m_pModelDataManager->m_iWhiteDotMidNumAccept_Top, THEAPP.m_pModelDataManager->m_iWhiteDotMaxNumAccept_Top,
									THEAPP.m_pModelDataManager->m_dWhiteDotMinArea_Btm, THEAPP.m_pModelDataManager->m_dWhiteDotMidArea_Btm, THEAPP.m_pModelDataManager->m_dWhiteDotMaxArea_Btm,
									THEAPP.m_pModelDataManager->m_iWhiteDotMinNumAccept_Btm, THEAPP.m_pModelDataManager->m_iWhiteDotMidNumAccept_Btm, THEAPP.m_pModelDataManager->m_iWhiteDotMaxNumAccept_Btm,
									THEAPP.m_pModelDataManager->m_dWhiteDotMinArea_Inner, THEAPP.m_pModelDataManager->m_dWhiteDotMidArea_Inner, THEAPP.m_pModelDataManager->m_dWhiteDotMaxArea_Inner,
									THEAPP.m_pModelDataManager->m_iWhiteDotMinNumAccept_Inner, THEAPP.m_pModelDataManager->m_iWhiteDotMidNumAccept_Inner, THEAPP.m_pModelDataManager->m_iWhiteDotMaxNumAccept_Inner);

								if(THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnWhiteDot))
								{
									copy_obj(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnWhiteDot,
										&THEAPP.m_stMultipleDefectInfo[nVirtualMzIdx][DEFECT_NAME_WHITEDOT].m_HMultipleDefectRgn[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1], 
										1 , -1 ); //Multiple Defect

									bBarrelDefectExist = TRUE;
									for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
									{
										intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnWhiteDot, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
										if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
										{
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
											copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
											strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


											strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_WHITEDOT]);
											THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
											// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
										}
									}
								}

							}	// White Dot: MinArea 을 통과한 White Dot 영역들

#pragma endregion
							//////////////// 결과 저장 및 출력을 위해(WriteResultFile) 렌즈 defect 검출 영역을 InspectAdminViewHideDlg 로 보낸다
							// 각 검사영상에서 Lens 불량 발생 영역 복사		
							bLensDefectExist = FALSE;

							// v3004 - 불량 디스플레이 시 누적되는 현상 해결 - LeeGW
							{
								gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensDirt);
								gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensScratch);
								gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensCT);
								gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensWhiteDot);
								gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensDefectMin);
							}

							// 각 이미지별로 Lens 불량 영역 확인
							for (iImageIdx = 0; iImageIdx < BARREL_LENS_IMAGE_TAB; iImageIdx++)
							{
								// 24.05.16 - v2602 - Lens 불량 표기 세분화로 변경 - LeeGW
								if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectLensDirtRgn_EachImage[iImageIdx]))
								{	
									bLensDefectExist = TRUE;
									concat_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectLensDirtRgn_EachImage[iImageIdx], THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensDirt, &(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensDirt));
									
									intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensDirt, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
									if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
									{
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
										copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
										strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


										strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_LENS_DIRT]);
										THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
									}
								}

								if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectLensScratchRgn_EachImage[iImageIdx]))
								{
									bLensDefectExist = TRUE;
									concat_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectLensScratchRgn_EachImage[iImageIdx], THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensScratch, &(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensScratch));

									intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensScratch, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
									if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
									{
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
										copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
										strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


										strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_LENS_SCRATCH]);
										THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
									}
								}

								if (THEAPP.m_pGFunction->ValidHRegion (pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectLensCTRgn_EachImage[iImageIdx]))
								{
									bLensDefectExist = TRUE;
									concat_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectLensCTRgn_EachImage[iImageIdx], THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensCT, &(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensCT));
								
									intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensCT, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
									if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
									{
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
										copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
										strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


										strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_LENS_CONTAMINATION]);
										THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
									}
								}
								
								// 24.06.07 - v2647 - LeeGW End
								// White Dot - LeeGW
								if (THEAPP.m_pGFunction->ValidHRegion (pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectLensWhiteDotRgn_EachImage[iImageIdx]))
								{
									bLensDefectExist = TRUE;
									concat_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectLensWhiteDotRgn_EachImage[iImageIdx], THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensWhiteDot, &(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensWhiteDot));
									
									intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnLensCT, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx], &HDefectIntersectRgn);
									if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
									{
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
										copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
										strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


										strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_LENS_WHITEDOT]);
										THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
									}
								}

							}

							// *** ROI 검사 결과 확인 - LeeGW ***
							{
								gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxyHole);
								gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnNorthSpringNG);
								gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEastSpringNG);
							}

							// 각 이미지별로 ROI 불량 영역 확인
							for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
							{
								copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
								strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];

								if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectEpoxyHole[iImageIdx]))
									concat_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectEpoxyHole[iImageIdx], THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxyHole, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxyHole);
								
								if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectNorthSpringNG[iImageIdx]))
									concat_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectNorthSpringNG[iImageIdx], THEAPP.m_pInspectAdminViewHideDlg->m_HRgnNorthSpringNG, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnNorthSpringNG);
								
								if (THEAPP.m_pGFunction->ValidHRegion(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectEastSpringNG[iImageIdx]))
									concat_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HDefectEastSpringNG[iImageIdx], THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEastSpringNG, &THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEastSpringNG);
							}

							// 각 ROI 불량 영역에 대하여 Review Image 저장 (m_HRgnEpoxyHole)
							if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxyHole))
							{
								union1(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxyHole, &(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxyHole));
							
								for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
								{
									intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEpoxyHole, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectExtra[iImageIdx], &HDefectIntersectRgn);
									if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
									{
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
										copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
										strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


										strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_EPOXYHOLE]);
										THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
									}
								}
							}

							// 각 ROI 불량 영역에 대하여 Review Image 저장 (m_HRgnNorthSpringNG)
							if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnNorthSpringNG))
							{
								union1(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnNorthSpringNG, &(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnNorthSpringNG));

								for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
								{
									intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnNorthSpringNG, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectExtra[iImageIdx], &HDefectIntersectRgn);
									if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
									{
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
										copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
										strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


										strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_NORTH_SPRINGNG]);
										THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
									}
								}
							}

							// 각 ROI 불량 영역에 대하여 Review Image 저장 (m_HRgnEastSpringNG)
							if (THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEastSpringNG))
							{
								union1(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEastSpringNG, &(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEastSpringNG));

								for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
								{
									intersection(THEAPP.m_pInspectAdminViewHideDlg->m_HRgnEastSpringNG, pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectExtra[iImageIdx], &HDefectIntersectRgn);
									if (THEAPP.m_pGFunction->ValidHRegion(HDefectIntersectRgn))
									{
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW START
										copy_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx], &HInspectImage, 1, 1);
										strImageNo = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];


										strImageFileName.Format("Module%d__%s_%s_%s", iModuleNo, strImageNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult, g_sDefectFileName[DEFECT_NAME_EAST_SPRINGNG]);
										THEAPP.m_pInspectAdminViewHideDlg->WriteReviewImage(HInspectImage, HDefectIntersectRgn, strImageFileName);
										// 24.06.12 - v2604 - ReviewImage 저장을 위해 추가 - LeeGW END
									}
								}
							}
							// *** END ***


							// ********* FAI 측정 결과 확인 *********
							for (int i = 0; i < MAX_FAI_ITEM; i++)
							{
								for (int j = 0; j < MAX_ONE_FAI_MEASURE_VALUE; j++)
								{
									if (THEAPP.m_pGFunction->ValidHXLD(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HReviewXLD_FAI_Item[i][j]))
										THEAPP.m_pInspectAdminViewHideDlg->m_HReviewXLD_FAI_Item[i][j] = pInspectService->m_pInspectAlgorithm[iThreadCount].m_HReviewXLD_FAI_Item[i][j];
									else
										gen_empty_obj(&THEAPP.m_pInspectAdminViewHideDlg->m_HReviewXLD_FAI_Item[i][j]);
								}
							}

							THEAPP.m_pInspectAdminViewHideDlg->WriteResultFileFAI(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage,
								THEAPP.m_pInspectService->m_iTrayNo_H, iModuleNo, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult);

							if (THEAPP.m_pInspectAdminViewHideDlg->bIsFAISpecialNG == TRUE)
								iFAISpecialNGCnt++;
							// ********* FAI 측정 결과 확인 *********



							// ** Mark and store Extra Inspection Results **	
#pragma region Mark and store Extra Inspection Results
							CString		strExtraDefectPosition = _T("");
							double		dExtraArea	= 0.0;
							double		dExtraDefectCenterX = 0.0;
							double      dExtraDefectCenterY = 0.0;
							BOOL		bExtraResult = FALSE;

							int iExtraDefectType = DEFECT_CODE_GOOD;

							THEAPP.m_pInspectAdminViewHideDlg->OverlaySetViewportManager(THEAPP.Struct_PreferenceStruct.m_iExtraResultImageX, THEAPP.Struct_PreferenceStruct.m_iExtraResultImageY, THEAPP.Struct_PreferenceStruct.m_dExtraResultImageZoom);

							int iExImageStartNo = 0;
							if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
								iExImageStartNo = BARREL_LENS_IMAGE_TAB;

							for (iImageIdx = iExImageStartNo; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
							{
								if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
									break;

								int iExtraDefectTypeTemp = THEAPP.m_pInspectAdminViewHideDlg->WriteResultFileExtra(iImageIdx,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx],
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectExtra[iImageIdx],
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HROIInspectAreaRgn,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult,
									&strExtraDefectPosition,
									&dExtraArea,
									&dExtraDefectCenterX,
									&dExtraDefectCenterY
								);

								if (iExtraDefectTypeTemp > DEFECT_CODE_GOOD)
									iExtraDefectType = iExtraDefectTypeTemp;
							}
#pragma endregion							
							// ** **

#pragma region Write Result File
							THEAPP.m_pInspectAdminViewHideDlg->OverlaySetViewportManager(THEAPP.Struct_PreferenceStruct.m_iLensResultImageX, THEAPP.Struct_PreferenceStruct.m_iLensResultImageY, THEAPP.Struct_PreferenceStruct.m_dLensResultImageZoom);

							CString		strDefectPosition = _T("");
							double		dblArea	= 0.0;
							double		dDefectCenterX = 0.0;
							double      dDefectCenterY = 0.0;

							for (iImageIdx = 0; iImageIdx < BARREL_LENS_IMAGE_TAB; iImageIdx++)
							{

								if(THEAPP.m_iMachineInspType == MACHINE_WELDING)
									iImageIdx = WELDING_IMAGE_TAB - 1;

								if(THEAPP.Struct_PreferenceStruct.m_bSaveResultMerge)
								{
									
									if (iImageIdx == TRIGGER_SURFACE || iImageIdx == TRIGGER_LENS2)
										continue;

									if (iImageIdx == TRIGGER_EDGE)
									{
										concat_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[TRIGGER_EDGE],
											pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[TRIGGER_SURFACE],
											&(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[TRIGGER_EDGE]));
									}
									else if (iImageIdx == TRIGGER_LENS1)
									{
										concat_obj(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[TRIGGER_LENS1],
											pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[TRIGGER_LENS2],
											&(pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[TRIGGER_LENS1]));
									}

									
								}

								iDefectCntRet = THEAPP.m_pInspectAdminViewHideDlg->WriteResultFile (iImageIdx,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HAlgoInspectImage[iImageIdx],
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_HdefectBarrelLens[iImageIdx],
									pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[iImageIdx][MATCHING_CONTOUR_1],
									pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[iImageIdx][MATCHING_CONTOUR_3],
									pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[iImageIdx][MATCHING_CONTOUR_4],
									pInspectService->m_pInspectAlgorithm[iThreadCount].InspectContour[iImageIdx][MATCHING_CONTOUR_5],
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_dBarcodePosOffsetX,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_dBarcodePosOffsetY,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeShiftNG,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_dBarcodePosOffsetRotation,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeRotationNG,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeModuleMixNG,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_dBlackCoatingOuterDiameter,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_dBlackCoatingPosX,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_dBlackCoatingPosY,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_dO1O2Distance,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_dDiameterMin,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBlackCoatingDiameterNG,
									&strDefectPosition,
									&dblArea,
									&dDefectCenterX,
									&dDefectCenterY,
									iExtraDefectType,
									&dExtraArea,
									&dExtraDefectCenterX,
									&dExtraDefectCenterY,
									bBarrelDefectExist | bLensDefectExist,
									bBarrelDefectExist,
									bLensDefectExist,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeError,
									!(pInspectService->m_pInspectAlgorithm[iThreadCount].m_bMatchingSuccess),
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_bLightDisorder,
									pInspectService->m_pInspectAlgorithm[iThreadCount].m_bModuleEmpty
									);
							}
#pragma endregion
							THEAPP.SaveLog("WriteResultFile Finish");
							//////////////////////////////////////////////////////////////////////////

							// 검사 결과에 따른 OK/NG 판정
#pragma region 검사 결과에 따른 OK/NG 판정
							if (pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeModuleMixNG)	/// Module 혼입 (MX)
							{
								THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_MODULE_MIX_ERROR;
								THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_MODULE_MIX_ERROR;
								THEAPP.SaveLog("pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeModuleMixNG = TRUE");
							}
							else if (pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeError)	/// Barcode 오류일 경우
							{
								THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_BARCODE_ERROR;
								THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_BARCODE_ERROR;
								THEAPP.SaveLog("pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeError = TRUE");
							}		
							else if (pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeShiftNG) //Ver 2630 modify
							{
								THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_BARCODE_SHIFT;
								THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_BARCODE_SHIFT;
								THEAPP.m_iBarcodeShiftPos[pInspectService->m_iTrayNo_H][iModuleNo] = 1;
								THEAPP.SaveLog("pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeShiftNG = TRUE");
							}
#pragma endregion

							// Defect Count 저장	
							if ((iDefectCntRet >= 0) && (iDefectCntRet < MAX_DEFECT_NUMBER)) // Defect (Module 단위)
							{
								iDefectCount[iDefectCntRet] += 1;
							}

							m_bSpecialNGSortFlag[iThreadCount] = pInspectService->m_pInspectAlgorithm[iThreadCount].m_bSpecialNGSortFlag;	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW
						}
						else		// 매칭 실패이면
						{
							THEAPP.SaveLog("매칭 실패");
							////////////////////////////////// added for CMI3000 2000 ====> ///////////////////////////////////////////////////
							if (pInspectService->m_pInspectAlgorithm[iThreadCount].m_bModuleEmpty)		/// 모듈이 없을 때 (EM)
							{
								THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_EMPTY;
								THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_EMPTY;
							}
							else
							{
								if (pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeModuleMixNG)	/// Module 혼입 (MX)
								{
									THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_MODULE_MIX_ERROR;
									THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_MODULE_MIX_ERROR;
								}
								else if (pInspectService->m_pInspectAlgorithm[iThreadCount].m_bLightDisorder)	/// 조명 이상일 경우 (LE)
								{
									THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_LIGHT_ERROR;
									THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_LIGHT_ERROR;
								}
								else if (pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeError)	/// Barcode 오류일 경우 (BE)
								{
									THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_BARCODE_ERROR;
									THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_BARCODE_ERROR;
								}
								else if (pInspectService->m_pInspectAlgorithm[iThreadCount].m_bBarcodeShiftNG) //Ver 2630 modify
								{
									THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_BARCODE_SHIFT;
									THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_BARCODE_SHIFT;
									THEAPP.m_iBarcodeShiftPos[pInspectService->m_iTrayNo_H][iModuleNo] = 1;
								}
								else																	/// 매칭 실패인 경우 (MC)
								{
									THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_ALIGN_ERROR;
									THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[iModuleNo-1] = DEFECT_TYPE_ALIGN_ERROR;
								}
							}
						////////////////////////////////// added for CMI3000 2000 ====> ///////////////////////////////////////////////////
						}

						///////// * InspectComplete와 LoadComplete 사이에 SW다운 돼서 재시작 할 경우에 대비해서 미리 추가로 Tray 결과에 기록해준다 *////////////////////////////////
						if (iModuleNo == 0) 
							THEAPP.SaveLog("--------------!!!! ModuleNo is 0000 !!!!");
						if (iModuleNo > iTrayModuleAmount) 
							THEAPP.SaveLog("--------------!!!! ModuleNo exceeds maxNo !!!!");
						
						// TrayResult.txt => B, L, BL, O, EM, BE(Barcode Error), AL, NN

						sKey.Format("%02d", iModuleNo);
						if (iModuleNo > iTrayModuleAmount) 
						{
							fResult.Set_String (sSection, sKey, "EM"); 
							continue;
						}

						if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] == DEFECT_TYPE_MODULE_MIX_ERROR) 
							fResult.Set_String (sSection, sKey, "MX");		
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] == DEFECT_TYPE_EMPTY) 
							fResult.Set_String (sSection, sKey, "EM");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] == DEFECT_TYPE_BARCODE_ERROR) 
							fResult.Set_String (sSection, sKey, "BE");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] == DEFECT_TYPE_BARCODE_SHIFT) 
							fResult.Set_String (sSection, sKey, "BS");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo-1] == DEFECT_TYPE_ALIGN_ERROR) 
							fResult.Set_String (sSection, sKey, "AL");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo - 1] == DEFECT_TYPE_SPRINGNG)
							fResult.Set_String(sSection, sKey, "SP");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo - 1] == DEFECT_TYPE_EPOXYHOLE)
							fResult.Set_String(sSection, sKey, "EH");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo - 1] == DEFECT_TYPE_BARREL)
							fResult.Set_String(sSection, sKey, "B");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo - 1] == DEFECT_TYPE_LENS)
							fResult.Set_String(sSection, sKey, "L");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo - 1] == DEFECT_TYPE_BARREL_LENS)
							fResult.Set_String(sSection, sKey, "BL");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo - 1] == DEFECT_TYPE_FAING)
							fResult.Set_String(sSection, sKey, "FAI");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo - 1] == DEFECT_TYPE_BLACK_COATING_DIAMETER)
							fResult.Set_String(sSection, sKey, "CD");
						else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[iModuleNo - 1] == DEFECT_TYPE_GOOD)
							fResult.Set_String(sSection, sKey, "O");
						else 
							fResult.Set_String (sSection, sKey, "NN");

						sKey.Format("%02d_BARCODE", iModuleNo);
						fResult.Set_String (sSection, sKey, pInspectService->m_pInspectAlgorithm[iThreadCount].m_sBarcodeResult);

						//// 검사 진행 다이어그램 업데이트
						if (THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)
							THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.InvalidateRect(false);
						else
							THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.InvalidateRect(false);

						ResultSaveDisplay[iThreadCount] += 1;

						CString sInspectTime;
						tInspectEnd_total = GetTickCount();
						sInspectTime.Format("* Module %2d 영상 판정 시간 total: %.0lf ms", iModuleNo, tInspectEnd_total - tInspectStart_total);
						THEAPP.SaveDetectLog(sInspectTime);
					} // <-- if(ResultSaveDisplay[iThreadCount]==0)
				}
				
				iSum += ResultSaveDisplay[iThreadCount];	// 저장 완료 카운트
			}

	//		if (iSum >= iTrayModuleAmount) break;	// Error: 종료된 쓰레드의 개수가 모듈 수 보다 많으면 안됨
			if (iSum >= THEAPP.m_iModuleNumberOneLine) 
				break;	// Error: 종료된 쓰레드의 개수가 모듈 수 보다 많으면 안됨
			
			Sleep(1);
		}

		double tResultStart_total, tResultEnd_total;
		tResultStart_total = GetTickCount();

		CString ResultTextB="";
		CString ResultTextL="";
		CString ResultTextBL="";

		CString sTrayResult = ""; // Handler 와 통신 parameter
		CString sTrayBarcodeResult = "";
		CString sNGCode=""; // mes 결과 데이터
		CString sBadName="";

		CString sBarcodeResult;

		CString s_dBlackCoatingOuterDiameter;
		CString s_dBlackCoatingPosX;
		CString s_dBlackCoatingPosY;
		CString s_dO1O2Distance;
		CString s_dDiameterMin;

		int iOK=0, iNG=0, iBarcodeError=0, iBlackCoatingDiameterNG = 0, iSpringNGNG = 0, iEpoxyHoleNG = 0, iModuleMixError = 0;
		int iMatchingError=0, iLightError=0, iEmptyError=0;		//20180210
		int iNoBarrelDefect, iNoLensDefect, iNoBarrelLensDefect;
		iNoBarrelDefect = iNoLensDefect = iNoBarrelLensDefect = 0;

		sSection = "Result";

		for (int iModNo=0; iModNo<THEAPP.m_iModuleNumberOneLine; iModNo++)
		{

			if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_X)
				iModuleNumber = iLineNo + THEAPP.m_iTrayLineNumber * iModNo;
			else
				iModuleNumber = (THEAPP.m_iTrayLineNumber - iLineNo) * THEAPP.m_iModuleNumberOneLine + iModNo + 1;
				//iModuleNumber = (iLineNo-1) * THEAPP.m_iModuleNumberOneLine + iModNo + 1;

			int RealModuleNum = iModuleNumber-1;

			if (iModNo)
			{
				sTrayResult += ",";
				sTrayBarcodeResult += ",";
				sNGCode +=",";
				sBadName +=",";

				s_dBlackCoatingOuterDiameter +=",";
				s_dBlackCoatingPosX +=",";
				s_dBlackCoatingPosY +=",";
				s_dO1O2Distance +=",";
				s_dDiameterMin +=",";
			}

			// 바코드
			sKey.Format("%02d_BARCODE", iModuleNumber);
			sBarcodeResult = fResult.Get_String (sSection, sKey, BARCODE_STATUS_DEFAULT);
			sTrayBarcodeResult += sBarcodeResult;

			sKey.Format("%02d", iModuleNumber);

			if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_EMPTY)
			{	// Empty
				sTrayResult += "E";
				fResult.Set_String (sSection, sKey, "EM");

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_EMPTY);
				sNGCode += sNGCodeTemp;

				sBadName +="모듈 Empty";
			}
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_MODULE_MIX_ERROR)
			{	// Module Mix
				iNG++;
				sTrayResult += "N";
				fResult.Set_String (sSection, sKey, "MX");

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_MODULE_MIX_ERROR);
				sNGCode += sNGCodeTemp;

				sBadName +="모듈 혼입";
			}
			////////////////////////////////// added for CMI3000 2000 ====> ///////////////////////////////////////////////////
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_LIGHT_ERROR)
			{	// 조명 이상
				iNG++;
				sTrayResult += "N";
				fResult.Set_String(sSection, sKey, "LE");

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_LIGHT_ERROR);
				sNGCode += sNGCodeTemp;

				sBadName += "조명 Error";
			}
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_BARCODE_ERROR)
			{	// Barcode Error
				iNG++;
				sTrayResult += "B"; //Ver 2630(Defect Code)
				fResult.Set_String(sSection, sKey, "BE");

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_BARCODE_ERROR);
				sNGCode += sNGCodeTemp;

				sBadName += "Barcode 미인식";
			}
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_BARCODE_SHIFT)
			{	// Barcode Shift
				iNG++;
				sTrayResult += "B"; //Ver 2630(Defect Code)
				fResult.Set_String(sSection, sKey, "BS");

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_BARCODE_SHIFT);
				sNGCode += sNGCodeTemp;

				sBadName += "Barcode Shift";
			}
			////////////////////////////////// <==== added for CMI3000 2000 ///////////////////////////////////////////////////
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_ALIGN_ERROR)
			{	// 매칭 오류
				iNG++;
				sTrayResult += "N";
				fResult.Set_String(sSection, sKey, "MC");

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_MATCHING_ERROR);
				sNGCode += sNGCodeTemp;

				sBadName += "Matching Error";
			}
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_SPRINGNG)
			{
				iNG++;
				if (THEAPP.Struct_PreferenceStruct.m_bSpecialNGSort == TRUE && m_bSpecialNGSortFlag[iModNo] == TRUE)
					sTrayResult += "W";
				else
					sTrayResult += "N";
				fResult.Set_String(sSection, sKey, "SP");

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_SPRINGNG);
				sNGCode += sNGCodeTemp;

				sBadName += "Spring NG";
			}
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_EPOXYHOLE)
			{	// Sidefill Epoxy Hole
				iNG++;
				if (THEAPP.Struct_PreferenceStruct.m_bSpecialNGSort == TRUE && m_bSpecialNGSortFlag[iModNo] == TRUE)
					sTrayResult += "W";
				else
					sTrayResult += "N";
				fResult.Set_String (sSection, sKey, "EH");
				
				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_EPOXYHOLE);
				sNGCode += sNGCodeTemp;
				
				sBadName +="Epoxy Hole NG";
			}
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_BARREL ||
					 THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_LENS ||
					 THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_BARREL_LENS)
			{	// Defect
				iNG++;

				if (THEAPP.Struct_PreferenceStruct.m_bSpecialNGSort == TRUE && m_bSpecialNGSortFlag[iModNo] == TRUE)
					sTrayResult += "W";
				else
					sTrayResult += "A";

				if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_BARREL)
				{
					CString temp;
					temp.Format("%d, ",RealModuleNum+1);
					ResultTextB += temp;
					fResult.Set_String (sSection, sKey, "B");

					++iNoBarrelDefect;
				}
				else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_LENS)
				{
					CString temp;
					temp.Format("%d, ",RealModuleNum+1);
					ResultTextL += temp;
					fResult.Set_String (sSection, sKey, "L");

					++iNoLensDefect;
				}
				else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_BARREL_LENS)
				{
					CString temp;
					temp.Format("%d, ",RealModuleNum+1);
					ResultTextBL += temp;
					fResult.Set_String (sSection, sKey, "BL");

					++iNoBarrelLensDefect;
				}
				//////////////////////////////////////////////////////////////////////////////
				// mes 데이터 보내는 곳 iDefectCount ( Barrel ) + InspectAdminViewHideDlg.LensOkNG ( Lens )  처음 걸리는 불량을 대표 불량으로 표시

				CString sNGCodeTemp;
				if( iDefectCntRet == DEFECT_CODE_DIRT) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Barrel 이물";
				}
				else if( iDefectCntRet == DEFECT_CODE_SCRATCH) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Barrel 스크래치";
				}
				else if( iDefectCntRet == DEFECT_CODE_STAIN) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Barrel 얼룩";
				}
				else if( iDefectCntRet == DEFECT_CODE_DENT) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Barrel 덴트";
				}
				else if( iDefectCntRet == DEFECT_CODE_CHIPPING) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Barrel 치핑";
				}
				else if( iDefectCntRet == DEFECT_CODE_EPOXY) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Barrel 에폭시";
				}
				else if( iDefectCntRet == DEFECT_CODE_OUTSIDE_CT) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Barrel 외곽오염";
				}
				else if( iDefectCntRet == DEFECT_CODE_PEEL_OFF) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Barrel Peel Off";
				}
				else if( iDefectCntRet == DEFECT_CODE_WHITEDOT) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Barrel WhiteDot";
				}
				else if (iDefectCntRet == DEFECT_CODE_LENS_CONTAMINATION) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Lens 오염";	// 24.05.09 - v2646 - 불량명 변경 - LeeGW
				}
				else if (iDefectCntRet == DEFECT_CODE_LENS_SCRATCH) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Lens 스크래치";
				}
				else if (iDefectCntRet == DEFECT_CODE_LENS_DIRT) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Lens 이물";	// 24.05.09 - v2646 - 불량명 변경 - LeeGW
				}
				else if (iDefectCntRet == DEFECT_CODE_LENS_WHITEDOT) {
					sNGCodeTemp.Format("%d", iDefectCntRet);
					sNGCode += sNGCodeTemp;
					sBadName +="Lens WhiteDot";
				}
				//////////////////////////////////////////////////////////////////////////////
			}
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_FAING)
			{
				iNG++;
				if (THEAPP.m_pModelDataManager->m_bFAISpecialNGUse[THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][RealModuleNum].m_iNGFAICode])
					sTrayResult += "S";
				else
					sTrayResult += "N";
				fResult.Set_String(sSection, sKey, "FAI");

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_FAING);
				sNGCode += sNGCodeTemp;

				CString sBadNameTemp;
				if (THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][RealModuleNum].m_iNGFAICode == 26)
					sBadNameTemp = _T("Lens Center - OMM 측정 필요");
				else
					sBadNameTemp.Format("FAI-%d NG", THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][RealModuleNum].m_iNGFAICode);

				sBadName += sBadNameTemp;
			}
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_BLACK_COATING_DIAMETER)
			{	// Black Coating Diameter Error	// 나중에 블랙코팅 삭제 예정
				iNG++;
				sTrayResult += "N";
				fResult.Set_String(sSection, sKey, "CD");

				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_BLACKCOATING_ERROR);
				sNGCode += sNGCodeTemp;

				sBadName += "Black Coating 직경 NG";
			}
			else if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[RealModuleNum] == DEFECT_TYPE_GOOD)
			{	// GOOD
				iOK++;
				sTrayResult += "G";
				fResult.Set_String (sSection, sKey, "O");
				
				CString sNGCodeTemp;
				sNGCodeTemp.Format("%d", DEFECT_CODE_GOOD);
				sNGCode += sNGCodeTemp;
				sBadName +="양품";
			}
			////////////////////////////////// added for CMI3000 2000 ====> ///////////////////////////////////////////////////
			else		// normally 0
			{
				sTrayResult += "N";
				fResult.Set_String (sSection, sKey, "NN");
			}

			CString sBlackCoating;
			sBlackCoating.Format("%.3lf", THEAPP.m_pTrayAdminViewDlg->mp_dBlackCoatingOuterDiameter[RealModuleNum]);
			s_dBlackCoatingOuterDiameter += sBlackCoating;
			sBlackCoating.Format("%.3lf", THEAPP.m_pTrayAdminViewDlg->mp_dBlackCoatingPosX[RealModuleNum]);
			s_dBlackCoatingPosX += sBlackCoating;
			sBlackCoating.Format("%.3lf", THEAPP.m_pTrayAdminViewDlg->mp_dBlackCoatingPosY[RealModuleNum]);
			s_dBlackCoatingPosY += sBlackCoating;
			sBlackCoating.Format("%.3lf", THEAPP.m_pTrayAdminViewDlg->mp_dO1O2Distance[RealModuleNum]);
			s_dO1O2Distance += sBlackCoating;
			sBlackCoating.Format("%.3lf", THEAPP.m_pTrayAdminViewDlg->mp_dDiameterMin[RealModuleNum]);
			s_dDiameterMin += sBlackCoating;

			////////////////////////////////// <==== added for CMI3000 2000 ///////////////////////////////////////////////////
		}

		THEAPP.SaveLog(sTrayResult);
		THEAPP.SaveLog(sTrayBarcodeResult);
		THEAPP.SaveLog(s_dBlackCoatingOuterDiameter);
		THEAPP.SaveLog(s_dBlackCoatingPosX);
		THEAPP.SaveLog(s_dBlackCoatingPosY);
		THEAPP.SaveLog(s_dO1O2Distance);
		THEAPP.SaveLog(s_dDiameterMin);

		///////////// ** FOB Test: 검사 결과에 상관 없이 인위적인 결과를 Pick & Place로 넘겨준다 /////////////////////////
		if (THEAPP.m_pInspectSummary->m_bUseFobTest)
		{
			int iAmt = THEAPP.m_iModuleNumberOneLine;		// 검사 수량
			int nN = (int)((double)THEAPP.m_iModuleNumberOneLine * double(pInspectService->m_iFobTestN)*PERCENTAGE_FACTOR);
			int nG = THEAPP.m_iModuleNumberOneLine - nN;

			if (nG < 0) nG = 0; if (nN < 0) nN = 0; //if (nE < 0) nE = 0;

			int nvFOB[2];
			nvFOB[0] = nG; nvFOB[1] = nN; //nvFOB[2] = nE;
			BOOL bG = FALSE, bN = FALSE, bE = FALSE;

			sTrayResult = "";
			for(int k=0; k<THEAPP.m_iModuleNumberOneLine; k++)
			{
				if (k) sTrayResult += ",";

				if (THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[k] == DEFECT_TYPE_EMPTY) {sTrayResult += "E"; continue;}	// Empty 인 포켓은 Empty 처리

				if (k >= THEAPP.m_iModuleNumberOneLine) {sTrayResult += "E"; continue;}					// 모듈 개수 넘으면 Empty 처리
				if (pInspectService->m_iFobTestG == 100) {sTrayResult += "G"; continue;}	// Good 100%면 포켓이 비어있어도 상관 없이 good 처리
				if (pInspectService->m_iFobTestN == 100) {sTrayResult += "N"; continue;}
				if (pInspectService->m_iFobTestE == 100) {sTrayResult += "E"; continue;}

				if (nvFOB[0] < 1 && nvFOB[1] < 1) {sTrayResult += "E"; continue;}

				int tmp = (3*k+rand())%2;
				if (nvFOB[tmp] > 0) {
					nvFOB[tmp]--;
					if (tmp == 0) sTrayResult += "G";
					else if (tmp == 1) sTrayResult += "N";
				}
				else {
					if (tmp == 0) {
						if (nvFOB[1] > 0) {nvFOB[1]--; sTrayResult += "N";}
					}
					if (tmp == 1) {
						if (nvFOB[0] > 0 ) {nvFOB[0]--; sTrayResult += "G";}
					}
				}
			} //for
		}

		if (THEAPP.m_iModuleNumberOneLine*2 != sTrayResult.GetLength()+1) 
			THEAPP.SaveLog("Tray Result Count Error!! - Inspect Amount is wrong");
		else 
			THEAPP.SaveLog("Tray Result Count Good!! - Inspect Amount is in order");

		THEAPP.m_pInspectSummary->m_sInspStatus = _T("Inspection Completed");

	#ifdef INLINE_MODE
		CString strStatusFileName = THEAPP.GetWorkingDirectory() + "\\Data\\" + "LastStatus.txt";
		CIniFileCS TXT_Status(strStatusFileName);
		sSection = "Status";
		TXT_Status.Set_String (sSection, "TrayResult", sTrayResult);
		TXT_Status.Set_String(sSection, "Status", "Inspection Completed");
	#endif

		/////////////////////////////////////////////////////////// 양불수율 저장

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////// Lot 단위
		SYSTEMTIME time;
		GetLocalTime(&time);

		CString sPerfomFolder = THEAPP.m_FileBase.m_strCurrentLotFolder;

		CString sPerformFile;
		sPerformFile.Format("%s\\LotSummary_%s_%04d%02d%02d.txt", sPerfomFolder, pInspectService->m_sLotID_H, time.wYear, time.wMonth, time.wDay);

		CIniFileCS LPINI(sPerformFile);
		CString sPerfomSection = "Results";

		int iTotalOK = LPINI.Get_Integer(sPerfomSection, "양품", 0) + iOK;
		int iTotalNG = LPINI.Get_Integer(sPerfomSection, "불량", 0) + iNG;
		int iTotalBarrelDefect = LPINI.Get_Integer(sPerfomSection, "배럴불량", 0) + iNoBarrelDefect;
		int iTotalLensDefect = LPINI.Get_Integer(sPerfomSection, "렌즈불량", 0) + iNoLensDefect;
		int iTotalBarrelLensDefect = LPINI.Get_Integer(sPerfomSection, "배럴렌즈불량", 0) + iNoBarrelLensDefect;
		int iTotalDefectCount_Dirt = LPINI.Get_Integer(sPerfomSection, "Barrel_Dirt", 0) + iDefectCount[DEFECT_CODE_DIRT];
		int iTotalDefectCount_Scratch = LPINI.Get_Integer(sPerfomSection, "Barrel_Scratch", 0) + iDefectCount[DEFECT_CODE_SCRATCH];
		int iTotalDefectCount_Stain = LPINI.Get_Integer(sPerfomSection, "Barrel_Stain", 0) + iDefectCount[DEFECT_CODE_STAIN];
		int iTotalDefectCount_Dent = LPINI.Get_Integer(sPerfomSection, "Barrel_Dent", 0) + iDefectCount[DEFECT_CODE_DENT];
		int iTotalDefectCount_Chipping = LPINI.Get_Integer(sPerfomSection, "Barrel_Chipping", 0) + iDefectCount[DEFECT_CODE_CHIPPING];
		int iTotalDefectCount_Epoxy = LPINI.Get_Integer(sPerfomSection, "Barrel_Epoxy", 0) + iDefectCount[DEFECT_CODE_EPOXY];
		int iTotalDefectCount_OutsideCT = LPINI.Get_Integer(sPerfomSection, "Barrel_외곽오염", 0) + iDefectCount[DEFECT_CODE_OUTSIDE_CT];
		int iTotalDefectCount_Peeloff = LPINI.Get_Integer(sPerfomSection, "Barrel_Peeloff", 0) + iDefectCount[DEFECT_CODE_PEEL_OFF];
		int iTotalDefectCount_WhiteDot = LPINI.Get_Integer(sPerfomSection, "Barrel_WhiteDot", 0) + iDefectCount[DEFECT_CODE_WHITEDOT];
		int iTotalDefectCount_Lens_Contamination = LPINI.Get_Integer(sPerfomSection, "Lens_오염", 0) + iDefectCount[DEFECT_CODE_LENS_CONTAMINATION];	// 24.06.07 - v2647 - 로그 렌즈 불량명 추가 - LeeGW Start
		int iTotalDefectCount_Lens_Scratch = LPINI.Get_Integer(sPerfomSection, "Lens_스크래치", 0) + iDefectCount[DEFECT_CODE_LENS_SCRATCH];	// 24.06.07 - v2647 - 로그 렌즈 불량명 추가 - LeeGW Start
		int iTotalDefectCount_Lens_Dirt = LPINI.Get_Integer(sPerfomSection, "Lens_이물", 0) + iDefectCount[DEFECT_CODE_LENS_DIRT];	// 24.06.07 - v2647 - 로그 렌즈 불량명 추가 - LeeGW Start
		int iTotalDefectCount_Lens_WhiteDot = LPINI.Get_Integer(sPerfomSection, "Lens_WhiteDot", 0) + iDefectCount[DEFECT_CODE_LENS_WHITEDOT];	// 24.06.07 - v2647 - 로그 렌즈 불량명 추가 - LeeGW Start
		int iTotalEpoxyHole = LPINI.Get_Integer(sPerfomSection, "EpoxyHole", 0) + iDefectCount[DEFECT_CODE_EPOXYHOLE];
		int iTotalSpringNG = LPINI.Get_Integer(sPerfomSection, "SpringNG", 0) + iDefectCount[DEFECT_CODE_SPRINGNG];
		int iTotalFAING = LPINI.Get_Integer(sPerfomSection, "FAING", 0) + iDefectCount[DEFECT_CODE_FAING];
		int iTotalFAISpecialNG = LPINI.Get_Integer(sPerfomSection, "FAISpecialNG", 0) + iFAISpecialNGCnt;
		int iTotalBlackCoatingNG = LPINI.Get_Integer(sPerfomSection, "BlackCoatingNG", 0) + iDefectCount[DEFECT_CODE_BLACKCOATING_ERROR];
		int iTotalBarcodeShift = LPINI.Get_Integer(sPerfomSection, "바코드Shift", 0) + iDefectCount[DEFECT_CODE_BARCODE_SHIFT];
		int iTotalBarcodeError = LPINI.Get_Integer(sPerfomSection, "바코드미인식", 0) + iDefectCount[DEFECT_CODE_BARCODE_ERROR];
		int iTotalModuleMixError = LPINI.Get_Integer(sPerfomSection, "모듈혼입", 0) + iDefectCount[DEFECT_CODE_MODULE_MIX_ERROR];
		int iTotalMatchingError = LPINI.Get_Integer(sPerfomSection, "매칭실패", 0) + iDefectCount[DEFECT_CODE_MATCHING_ERROR];;
		int iTotalLightError = LPINI.Get_Integer(sPerfomSection, "조명이상", 0) + iDefectCount[DEFECT_CODE_LIGHT_ERROR];
		int iTotalEmptyError = LPINI.Get_Integer(sPerfomSection, "Empty", 0) + iDefectCount[DEFECT_CODE_EMPTY];
		//	iTotalDefectCount_BarcodeShift: LotSummary.txt 에 기록할 Barcode Shift 수량 (배럴/렌즈 불량을 제외한 Barcode Shift 수량)
		//  iTotalBarcodeShift: Lot 전체 Barcode Shift 수량

		double yield = 0;
		double dNGRatio = 0;
		UINT iTotal = iTotalOK + iTotalNG;

		if (iTotal > 0)
		{
			yield = ((double)iTotalOK / (double)(iTotal)) * 100.0;
			dNGRatio = ((double)iTotalNG / (double)(iTotal)) * 100.0;
		}

		CString strYield;
		strYield.Format("%.2lf", yield);

		double dBarrelDefectRatio, dLensDefectRatio, dBarrelLensDefectRatio;

		dBarrelDefectRatio = 0;
		dLensDefectRatio = 0;
		dBarrelLensDefectRatio = 0;

		if (iTotalNG > 0) 
		{
			dBarrelDefectRatio = ((double)iTotalBarrelDefect / (double)iTotalNG) * 100.0;
			dLensDefectRatio = ((double)iTotalLensDefect / (double)iTotalNG) * 100.0;
			dBarrelLensDefectRatio = ((double)iTotalBarrelLensDefect / (double)iTotalNG) * 100.0;
		}

		// 위에서 Get 했는데 왜 다시 Set 하는지?
		// 필요한 것만 Set 하면 안되나?
		LPINI.Set_Integer(sPerfomSection, "총량", iTotal);
		LPINI.Set_Integer(sPerfomSection, "양품", iTotalOK);
		LPINI.Set_Integer(sPerfomSection, "불량", iTotalNG);
		LPINI.Set_Double(sPerfomSection, "수율", yield);
		LPINI.Set_Integer(sPerfomSection, "배럴불량", iTotalBarrelDefect);
		LPINI.Set_Double(sPerfomSection, "배럴불량수율", dBarrelDefectRatio);
		LPINI.Set_Integer(sPerfomSection, "렌즈불량", iTotalLensDefect);
		LPINI.Set_Double(sPerfomSection, "렌즈불량수율", dLensDefectRatio);
		LPINI.Set_Integer(sPerfomSection, "배럴렌즈불량", iTotalBarrelLensDefect);
		LPINI.Set_Double(sPerfomSection, "배럴렌즈불량수율", dBarrelLensDefectRatio);
		LPINI.Set_Integer(sPerfomSection, "Barrel_Dirt", iTotalDefectCount_Dirt);
		LPINI.Set_Integer(sPerfomSection, "Barrel_Scratch", iTotalDefectCount_Scratch);
		LPINI.Set_Integer(sPerfomSection, "Barrel_Stain", iTotalDefectCount_Stain);
		LPINI.Set_Integer(sPerfomSection, "Barrel_Dent", iTotalDefectCount_Dent);
		LPINI.Set_Integer(sPerfomSection, "Barrel_Chipping", iTotalDefectCount_Chipping);
		LPINI.Set_Integer(sPerfomSection, "Barrel_Epoxy", iTotalDefectCount_Epoxy);
		LPINI.Set_Integer(sPerfomSection, "Barrel_외곽오염", iTotalDefectCount_OutsideCT);
		LPINI.Set_Integer(sPerfomSection, "Barrel_Peeloff", iTotalDefectCount_Peeloff);
		LPINI.Set_Integer(sPerfomSection, "Barrel_WhiteDot", iTotalDefectCount_WhiteDot);	// White Dot - LeeGW
		LPINI.Set_Integer(sPerfomSection, "Lens_오염", iTotalDefectCount_Lens_Contamination);	// 24.06.07 - v2647 - 로그 렌즈 불량명 추가 - LeeGW Start
		LPINI.Set_Integer(sPerfomSection, "Lens_스크래치", iTotalDefectCount_Lens_Scratch);	// 24.06.07 - v2647 - 로그 렌즈 불량명 추가 - LeeGW Start
		LPINI.Set_Integer(sPerfomSection, "Lens_이물", iTotalDefectCount_Lens_Dirt);		// 24.06.07 - v2647 - 로그 렌즈 불량명 추가 - LeeGW Start
		LPINI.Set_Integer(sPerfomSection, "Lens_WhiteDot", iTotalDefectCount_Lens_WhiteDot);	// White Dot - LeeGW
		LPINI.Set_Integer(sPerfomSection, "EpoxyHole", iTotalEpoxyHole);
		LPINI.Set_Integer(sPerfomSection, "SpringNG", iTotalSpringNG);
		LPINI.Set_Integer(sPerfomSection, "FAING", iTotalFAING);
		LPINI.Set_Integer(sPerfomSection, "FAISpecialNG", iTotalFAISpecialNG);
		LPINI.Set_Integer(sPerfomSection, "BlackCoatingNG", iTotalBlackCoatingNG);
		LPINI.Set_Integer(sPerfomSection, "바코드Shift", iTotalBarcodeShift);
		LPINI.Set_Integer(sPerfomSection, "바코드미인식", iTotalBarcodeError);
		LPINI.Set_Integer(sPerfomSection, "모듈혼입", iTotalModuleMixError);
		LPINI.Set_Integer(sPerfomSection, "매칭실패", iTotalMatchingError);
		LPINI.Set_Integer(sPerfomSection, "조명이상", iTotalLightError);
		LPINI.Set_Integer(sPerfomSection, "Empty", iTotalEmptyError);

		CString strDefectRatio;
		strDefectRatio.Format("%d/%d/%d (%%)", (int)dBarrelDefectRatio, (int)dLensDefectRatio, (int)dBarrelLensDefectRatio);

	#ifdef INLINE_MODE
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_OKCOUNT, iTotalOK);
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NGCOUNT, iTotalNG);
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_BARCODE_ERROR_LOT, iTotalBarcodeError);
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NUMBER_OF_MODULE, iTotal);
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_YIELD, strYield);
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_DEFECT_RATIO_LOT, strDefectRatio);

		BOOL bChangeBGColor = FALSE;
		if (iTotalBarcodeShift > 5 || iTotalModuleMixError > 5 || iTotalMatchingError > 5 || iTotalFAISpecialNG > 5)
			bChangeBGColor = TRUE;

		CString LogBuffer;

		// Lot ID 를 못찾은 것을 검사하나?
		if(THEAPP.m_pLogDlg->m_sCurrentLotID.Find(pInspectService->m_sLotID_H) == -1)
		{
			THEAPP.m_pLogDlg->m_sBefore2LotID = THEAPP.m_pLogDlg->m_sBeforeLotID;
			THEAPP.m_pLogDlg->m_sBefore2TotalModuleMix = THEAPP.m_pLogDlg->m_sBeforeTotalModuleMix;
			THEAPP.m_pLogDlg->m_sBefore2TotalBarcodeShift = THEAPP.m_pLogDlg->m_sBeforeTotalBarcodeShift;
			THEAPP.m_pLogDlg->m_sBefore2TotalMatchingError = THEAPP.m_pLogDlg->m_sBeforeTotalMatchingError;
			THEAPP.m_pLogDlg->m_sBefore2TotalFAISpecialNG = THEAPP.m_pLogDlg->m_sBeforeTotalFAISpecialNG;

			THEAPP.m_pLogDlg->m_sBeforeLotID = THEAPP.m_pLogDlg->m_sCurrentLotID;
			THEAPP.m_pLogDlg->m_sBeforeTotalModuleMix = THEAPP.m_pLogDlg->m_sCurrentTotalModuleMix;
			THEAPP.m_pLogDlg->m_sBeforeTotalBarcodeShift = THEAPP.m_pLogDlg->m_sCurrentTotalBarcodeShift;
			THEAPP.m_pLogDlg->m_sBeforeTotalMatchingError = THEAPP.m_pLogDlg->m_sCurrentTotalMatchingError;
			THEAPP.m_pLogDlg->m_sBeforeTotalFAISpecialNG = THEAPP.m_pLogDlg->m_sCurrentTotalFAISpecialNG;

			LogBuffer.Format("[%s]", pInspectService->m_sLotID_H);
			THEAPP.m_pLogDlg->m_sCurrentLotID = LogBuffer;

			LogBuffer.Format("Barcode Shift: %d", iTotalBarcodeShift);
			THEAPP.m_pLogDlg->m_sCurrentTotalBarcodeShift = LogBuffer;
			
			LogBuffer.Format("Module Mix: %d", iTotalModuleMixError);
			THEAPP.m_pLogDlg->m_sCurrentTotalModuleMix = LogBuffer;

			LogBuffer.Format("Matching Error: %d", iTotalMatchingError);
			THEAPP.m_pLogDlg->m_sCurrentTotalMatchingError = LogBuffer;

			if (iTotalFAISpecialNG >= 5)
				LogBuffer.Format("FAI Special NG: %d   === NG NG NG!! ===", iTotalFAISpecialNG);
			else
				LogBuffer.Format("FAI Special NG: %d", iTotalFAISpecialNG);
			THEAPP.m_pLogDlg->m_sCurrentTotalFAISpecialNG = LogBuffer;

			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentLotID, LOG_COLOR_BLACK, 340, bChangeBGColor, TRUE);
			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentTotalFAISpecialNG, LOG_COLOR_RED, 340, FALSE, FALSE);
			if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
			{
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentTotalModuleMix, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentTotalBarcodeShift, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentTotalMatchingError, LOG_COLOR_RED, 340, FALSE, FALSE);
			}

			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeLotID, LOG_COLOR_BLACK, 340, FALSE, FALSE);
			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeTotalFAISpecialNG, LOG_COLOR_RED, 340, FALSE, FALSE);
			if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
			{
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeTotalModuleMix, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeTotalBarcodeShift, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeTotalMatchingError, LOG_COLOR_RED, 340, FALSE, FALSE);
			}
			

			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2LotID, LOG_COLOR_BLACK, 340, FALSE, FALSE);
			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2TotalFAISpecialNG, LOG_COLOR_RED, 340, FALSE, FALSE);
			if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
			{
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2TotalModuleMix, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2TotalBarcodeShift, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2TotalMatchingError, LOG_COLOR_RED, 340, FALSE, FALSE);
			}

		}
		else
		{
			LogBuffer.Format("[%s]", pInspectService->m_sLotID_H);
			THEAPP.m_pLogDlg->m_sCurrentLotID = LogBuffer;

			LogBuffer.Format("Module Mix: %d", iTotalModuleMixError);
			THEAPP.m_pLogDlg->m_sCurrentTotalModuleMix = LogBuffer;

			LogBuffer.Format("Barcode Shift: %d", iTotalBarcodeShift);
			THEAPP.m_pLogDlg->m_sCurrentTotalBarcodeShift = LogBuffer;

			LogBuffer.Format("Matching Error: %d", iTotalMatchingError);
			THEAPP.m_pLogDlg->m_sCurrentTotalMatchingError = LogBuffer;

			if (iTotalFAISpecialNG >= 5)
				LogBuffer.Format("FAI Special NG: %d   === NG NG NG!! ===", iTotalFAISpecialNG);
			else
				LogBuffer.Format("FAI Special NG: %d", iTotalFAISpecialNG);
			THEAPP.m_pLogDlg->m_sCurrentTotalFAISpecialNG = LogBuffer;

			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentLotID, LOG_COLOR_BLACK, 340, bChangeBGColor, TRUE);
			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentTotalFAISpecialNG, LOG_COLOR_RED, 340, FALSE, FALSE);
			if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
			{
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentTotalModuleMix, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentTotalBarcodeShift, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sCurrentTotalMatchingError, LOG_COLOR_RED, 340, FALSE, FALSE);
			}

			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeLotID, LOG_COLOR_BLACK, 340, FALSE, FALSE);
			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeTotalFAISpecialNG, LOG_COLOR_RED, 340, FALSE, FALSE);
			if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
			{
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeTotalModuleMix, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeTotalBarcodeShift, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBeforeTotalMatchingError, LOG_COLOR_RED, 340, FALSE, FALSE);
			}

			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2LotID, LOG_COLOR_BLACK, 340, FALSE, FALSE);
			THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2TotalFAISpecialNG, LOG_COLOR_RED, 340, FALSE, FALSE);
			if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
			{
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2TotalModuleMix, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2TotalBarcodeShift, LOG_COLOR_RED, 340, FALSE, FALSE);
				THEAPP.m_pLogDlg->WriteLog(THEAPP.m_pLogDlg->m_sBefore2TotalMatchingError, LOG_COLOR_RED, 340, FALSE, FALSE);
			}
		}
	#endif

		THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.InvalidateRect(false);
		THEAPP.m_pTrayBeamProjectViewDlg->m_TrayImagePB.InvalidateRect(false);
		THEAPP.m_pTrayOperatorViewPrevDlg->m_TrayImagePB.InvalidateRect(false);
		THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.InvalidateRect(false);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////// 1Day Lot Result 통합 (1라인)
		// LotSummary,txt : Lot 종료 시 저장

	#ifdef INLINE_MODE

		CString strLotSave;
		CString sLotStartTime, sLotEndTime;
		SYSTEMTIME LotEndTime;

		sSection = "Status";
		int iLotModuleAmt_H = TXT_Status.Get_Integer(sSection, "ModuleAmount", 0);
		int iNoModuleInTray;

		iNoModuleInTray = THEAPP.m_iTrayLineNumber * THEAPP.m_iModuleNumberOneLine;

		int iLastTrayNo, iLastLineNo, iMod, iMod2;

		if (iNoModuleInTray > 0)
		{
			iLastTrayNo = iLotModuleAmt_H / iNoModuleInTray;
			iMod = iLotModuleAmt_H % iNoModuleInTray;
		}
		
		if (iMod > 0)
		{
			++iLastTrayNo;
			if (THEAPP.m_iModuleNumberOneLine > 0)
			{
				iLastLineNo = iMod / THEAPP.m_iModuleNumberOneLine;
				iMod2 = iMod % THEAPP.m_iModuleNumberOneLine;
			}

			if (iMod2 > 0)
				++iLastLineNo;
		}
		else
		{
			iLastLineNo = THEAPP.m_iTrayLineNumber;
		}

		// Lot End
		if ((iLastTrayNo == THEAPP.m_pInspectService->m_iTrayNo_H) && (iLastLineNo == THEAPP.m_pInspectService->m_iLineNo_H))
		{
			// Start Time
			sLotStartTime = TXT_Status.Get_String(sSection, "LotStartTime", "Default");

			// End Time
			GetLocalTime(&LotEndTime);
			sLotEndTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d", LotEndTime.wYear, LotEndTime.wMonth, LotEndTime.wDay, LotEndTime.wHour, LotEndTime.wMinute, LotEndTime.wSecond, LotEndTime.wMilliseconds);

			// Cycle Time
			double dInspectTime;
			double dLotEndTickCount = GetTickCount();
			if (THEAPP.m_dLotStartTickCount>0)
				dInspectTime = (dLotEndTickCount - THEAPP.m_dLotStartTickCount) / 1000.0;
			else
				dInspectTime = 0;

			THEAPP.SaveLotSummaryLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, sLotStartTime, sLotEndTime, pInspectService->m_sLotID_H, dInspectTime, iTotal, iTotalOK, yield, iTotalNG, dNGRatio,
									 iTotalBarrelDefect, iTotalLensDefect, iTotalBarrelLensDefect, iTotalDefectCount_Dirt, iTotalDefectCount_Scratch, iTotalDefectCount_Stain,
									 iTotalDefectCount_Dent, iTotalDefectCount_Chipping, iTotalDefectCount_Epoxy, iTotalDefectCount_OutsideCT, iTotalDefectCount_Peeloff, iTotalDefectCount_WhiteDot,
									 iTotalDefectCount_Lens_Contamination, iTotalDefectCount_Lens_Scratch, iTotalDefectCount_Lens_Dirt, iTotalDefectCount_Lens_WhiteDot, iTotalEpoxyHole, iTotalSpringNG,
									 iTotalFAING, iTotalBlackCoatingNG, iTotalModuleMixError, iTotalBarcodeError, iTotalBarcodeShift, iTotalMatchingError, iTotalLightError);

			// LAS 저장
			if (THEAPP.Struct_PreferenceStruct.m_bSaveLasLog)
				THEAPP.m_pSaveManager->SaveLasSection(pInspectService->m_sLotID_H, THEAPP.GetLASTempDirectory(), THEAPP.GetLASSectionDirectory(), FALSE);

			//Multiple Defect Start
			if (THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarm)
				THEAPP.m_pInspectService->LotDetailDefectError(nVirtualMzIdx, pInspectService->m_sLotID_H);
			//Multiple Defect End
		}	
	#endif

		sPerformFile.Format("%s\\DaySummary_%04d%02d%02d.txt", THEAPP.m_FileBase.m_strResultFolderPrev, time.wYear, time.wMonth, time.wDay);

		CIniFileCS DPINI(sPerformFile);

		sPerfomSection = "Today";

		strYield.Format("%.2lf", DPINI.Get_Double(sPerfomSection, "수율", 0.0));

		dBarrelDefectRatio = DPINI.Get_Double(sPerfomSection, "배럴불량수율", 0.0);
		dLensDefectRatio = DPINI.Get_Double(sPerfomSection, "렌즈불량수율", 0.0);
		dBarrelLensDefectRatio = DPINI.Get_Double(sPerfomSection, "배럴렌즈불량수율", 0.0);

		strDefectRatio.Format("%d/%d/%d (%%)", (int)dBarrelDefectRatio, (int)dLensDefectRatio, (int)dBarrelLensDefectRatio);

	#ifdef INLINE_MODE
		CString strShortID = THEAPP.m_pInspectResultDlg->CurrentLotID.Left(3);
		iTotalOK = DPINI.Get_Integer(sPerfomSection, "양품", 0) + iOK;
		iTotalNG = DPINI.Get_Integer(sPerfomSection, "불량", 0) + iNG;

		DOUBLE yield_Day = 0;
		iTotal = iTotalOK + iTotalNG;

		if (iTotal > 0)
			yield_Day = ((double)iTotalOK / (iTotal)) * 100;

		strYield.Format("%.2lf", yield_Day);

		iTotalBarrelDefect = LPINI.Get_Integer(sPerfomSection, "배럴불량", 0) + iNoBarrelDefect;
		iTotalLensDefect = LPINI.Get_Integer(sPerfomSection, "렌즈불량", 0) + iNoLensDefect;
		iTotalBarrelLensDefect = LPINI.Get_Integer(sPerfomSection, "배럴렌즈불량", 0) + iNoBarrelLensDefect;

		if (iTotalNG > 0) 
		{
			dBarrelDefectRatio = ((double)iTotalBarrelDefect/(double)iTotalNG)*100.0;
			dLensDefectRatio = ((double)iTotalLensDefect/(double)iTotalNG)*100.0;
			dBarrelLensDefectRatio = ((double)iTotalBarrelLensDefect/(double)iTotalNG)*100.0;
		}

		strDefectRatio.Format("%d/%d/%d (%%)", (int)dBarrelDefectRatio, (int)dLensDefectRatio, (int)dBarrelLensDefectRatio);

		DPINI.Set_Integer(sPerfomSection, "총량", iTotal);
		DPINI.Set_Integer(sPerfomSection, "양품", iTotalOK);
		DPINI.Set_Integer(sPerfomSection, "불량", iTotalNG);
		DPINI.Set_Double(sPerfomSection, "수율", yield_Day);
		LPINI.Set_Integer(sPerfomSection, "배럴불량", iTotalBarrelDefect);
		LPINI.Set_Integer(sPerfomSection, "렌즈불량", iTotalLensDefect);
		LPINI.Set_Integer(sPerfomSection, "배럴렌즈불량", iTotalBarrelLensDefect);
		LPINI.Set_Double(sPerfomSection, "배럴불량수율", dBarrelDefectRatio);
		LPINI.Set_Double(sPerfomSection, "렌즈불량수율", dLensDefectRatio);
		LPINI.Set_Double(sPerfomSection, "배럴렌즈불량수율", dBarrelLensDefectRatio);

		DPINI.Set_Integer(sPerfomSection,"기준시간", THEAPP.m_pInspectResultDlg->m_iResetTime);
	#endif

	#ifdef INLINE_MODE
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_OKCOUNT_DAY, DPINI.Get_Integer(sPerfomSection, "양품", 0));
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NGCOUNT_DAY, DPINI.Get_Integer(sPerfomSection, "불량", 0));
		THEAPP.m_pInspectResultDlg->SetDlgItemInt(IDC_EDIT_NUMBER_OF_MODULE_DAY, DPINI.Get_Integer(sPerfomSection, "총량", 0));
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_YIELD_DAY, strYield);
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_DEFECT_RATIO_DAY, strDefectRatio);
	#endif
		
		iSum=0;
		iThreadCount=0;
		
		double tstart = 0, tend = 0;
		tstart = GetTickCount();
		THEAPP.SaveLog("검사 threads 완료 시작");
		while(TRUE)		// 현 tray 의 모든 module에 대한 Thread 가 다 끝나기를 기다림
		{
			iSum = 0;
			for(iThreadCount=0; iThreadCount < THEAPP.m_iModuleNumberOneLine; iThreadCount++) {
				iSum += pInspectService->m_pInspectAlgorithm[iThreadCount].ThreadFinishFlag;
			}
			if(iSum >= THEAPP.m_iModuleNumberOneLine) 
				break;	// Error: 종료된 쓰레드의 개수가 모듈 수 보다 많으면 안됨
			
			Sleep(1);
		}

		for(iThreadCount=0; iThreadCount < THEAPP.m_iModuleNumberOneLine; iThreadCount++) {
			pInspectService->m_pInspectAlgorithm[iThreadCount].ThreadFinishFlag = FALSE;
		}

		tend = GetTickCount();
		CString sTimeCheck;
		sTimeCheck.Format("검사 threads 완료 %.0lf ms", tend - tstart); THEAPP.SaveLog(sTimeCheck);


		THEAPP.m_pAlgorithm->m_tInspectEndTime = GetTickCount();;
		sTimeCheck.Format("********** 트레이 %d 검출 시간 : %.4lf s", THEAPP.m_pInspectService->m_iTrayNo_H, (THEAPP.m_pAlgorithm->m_tInspectEndTime - THEAPP.m_pAlgorithm->m_tInspectStartTime)/1000);
		THEAPP.SaveLog(sTimeCheck); THEAPP.SaveDetectLog(sTimeCheck);
		
		THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
		THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;

		CString sResultRecordTime;
		tResultEnd_total = GetTickCount();
		sResultRecordTime.Format("* 수율 기록 시간 total: %.0lf ms", tResultEnd_total - tResultStart_total);
		THEAPP.SaveDetectLog(sResultRecordTime);

		THEAPP.m_pAlgorithm->m_tInspectEndTime = tResultEnd_total;
		sTimeCheck.Format("********** 트레이 %d 검사+결과저장 시간 : %.4lf s", THEAPP.m_pInspectService->m_iTrayNo_H, (THEAPP.m_pAlgorithm->m_tInspectEndTime - THEAPP.m_pAlgorithm->m_tInspectStartTime)/1000);
		THEAPP.SaveLog(sTimeCheck); THEAPP.SaveDetectLog(sTimeCheck);

		////////////////////////////////// Added for CMI3000 2000 ====> ///////////////////////////////////////////////////

	#ifdef INLINE_MODE
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
	#endif

	#ifdef INLINE_MODE
		THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
		THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;

		THEAPP.SaveLog(sTrayResult);
		
		// 2025.06.25 - 핸들러 응답 체크 - LeeGW
		THEAPP.m_pHandlerService->ResetReplyReceived();

		int iHandlerRetryNum = 0;
		while (TRUE)
		{
			iHandlerRetryNum++;

			if ((THEAPP.Struct_PreferenceStruct.m_bUseMboMode == FALSE) || 
			((THEAPP.Struct_PreferenceStruct.m_bUseMboMode == TRUE) && (THEAPP.m_pInspectService->m_iMboCount_H == THEAPP.Struct_PreferenceStruct.m_iMboModeCount)))
				THEAPP.m_pHandlerService->Set_InspectComplete(pInspectService->m_sLotID_H, 
					pInspectService->m_iTrayNo_H, pInspectService->m_iLineNo_H, THEAPP.m_iModuleNumberOneLine, 
					sTrayResult, sTrayBarcodeResult, sNGCode, 
					sBadName, s_dBlackCoatingOuterDiameter, s_dBlackCoatingPosX, 
					s_dBlackCoatingPosY, s_dO1O2Distance, s_dDiameterMin); // 검사 종료 신호 날림. PC 검사 ready 상태 set함.  //연산 쪽에 있어야함.

			if (THEAPP.Struct_PreferenceStruct.m_bUseHandlerRetry == TRUE)
				Sleep(THEAPP.Struct_PreferenceStruct.m_iHandlerReplyWaitTime);

			if (THEAPP.Struct_PreferenceStruct.m_bUseHandlerRetry == FALSE || 
				THEAPP.m_pHandlerService->Is_ReplyReceived() == TRUE || iHandlerRetryNum >= THEAPP.Struct_PreferenceStruct.m_iHandlerRetryCount)
				break;

			Sleep(THEAPP.Struct_PreferenceStruct.m_iHandlerRetryWaitTime);

			THEAPP.SaveLog("********** Handler 검사완료 송신 재시도 ******************");
		}

		THEAPP.m_pInspectService->m_bInspectDone = TRUE;		// 이게 끝나야 GetLoadComplete 가 완전히 수행된다
		THEAPP.SaveLog("********** Tray 검사 종료 ******************");

		TXT_Status.Set_String (sSection, "TrayResult", "NN");
	#endif

	#ifndef INLINE_MODE
		THEAPP.m_pHandlerService->Set_InspectComplete(pInspectService->m_sLotID_H, pInspectService->m_iTrayNo_H, pInspectService->m_iLineNo_H, THEAPP.m_iModuleNumberOneLine, sTrayResult, sTrayBarcodeResult, sNGCode, sBadName, s_dBlackCoatingOuterDiameter, s_dBlackCoatingPosX, s_dBlackCoatingPosY, s_dO1O2Distance, s_dDiameterMin); //Ver 2630 add(For Test)
	#endif

		THEAPP.m_pInspectService->m_bInspectRun = FALSE;		// 이게 끝나야 GetLoadComplete 가 완전히 수행된다

	#ifdef INLINE_MODE
		THEAPP.m_pInspectSummary->ShowDiskCapacity();
	#endif
		////////////////////////////////// <==== Added for CMI3000 2000 ///////////////////////////////////////////////////

		delete m_bSpecialNGSortFlag;
		delete ResultSaveDisplay;

	#ifndef INLINE_MODE
		pInspectService->m_bLineInspectDone = TRUE;
	#endif

		return 0;
	}
	catch(HException &except)
	{
		CString str; str.Format("Halcon Exception [ResultSaveThread] : %s", except.message); THEAPP.SaveLog(str);
		return 0;
	}
}

// about 600 lines - 251014, jhkim
UINT UniformityCheckThread(LPVOID lp)
{
	CInspectService* pInspectService = (CInspectService*)lp;

	double dInspPosX = 0;
	double dInspPosY = 0;
	
	dInspPosX = THEAPP.m_pModelDataManager->m_mdInspectPosXY[0][0]; 
	dInspPosY = THEAPP.m_pModelDataManager->m_mdInspectPosXY[0][1];

	if(dInspPosX == 0) {
		THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
		THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;
		return 0;
	}

	int nInspCnt=0;

	//////////////////////////////////////////////////////////////////모션 테스트 위치 입력 (array x * array y개 트레이)
	double StartTime_Ins = 0 ,EndTime_Ins = 0,CurrentTime_Ins=0;
	double StartTime = 0 ,EndTime = 0,CurrentTime=0;
	CString strInspectionTime;

	DWORD dwGrabStart = 0, dwGrabEnd = 0;
	DWORD dwGrabElapsedTime = 0;

	StartTime_Ins = GetTickCount();
	CurrentTime_Ins = GetTickCount();
	CurrentTime_Ins = CurrentTime_Ins-StartTime_Ins;
	strInspectionTime.Format("%.3lf",CurrentTime_Ins/1000);
	THEAPP.m_pInspectSummary->SetDlgItemText(IDC_STATIC_INSPECTSUMMARY_TIME, strInspectionTime);
	THEAPP.m_pInspectSummary->m_staticProgress.SetRange(0, THEAPP.m_pModelDataManager->m_iTrayModuleAmt);

#ifdef INLINE_MODE

	StartTime = GetTickCount();
	THEAPP.m_pHandlerService->m_bMoveCompleteFlag = FALSE;
	THEAPP.m_pHandlerService->Set_AMoveRequest(dInspPosX, dInspPosY, 1); // 위치이동

	nInspCnt = 0;
	while(!THEAPP.m_pHandlerService->m_bMoveCompleteFlag) // 무브 컴플리트 기다림
	{
		// 만약 스탑 상태면 빠져나와야함. 
	////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
		if(THEAPP.m_pHandlerService->m_nInspectPCStatus != 2 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 4 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 5)
		{
			THEAPP.m_pInspectService->QuitInspect();
			return 0;
		}
		Sleep(1);
	}
	while (THEAPP.m_pHandlerService->m_nInspectPCStatus == 4) Sleep(1);



	CString strLog; strLog.Format("모션 %d 위치 도착",1);
	THEAPP.SaveLog(strLog);
	int K=0;
	Sleep(5);
	double dWattingGrab_StartTime=0, dWattingGrab_EndtTime=0;
	for(K=0;K<3;K++)
	{
		THEAPP.m_pCameraManager->CallHookFunction();

		THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, TRIGGER_SURFACE);

		strLog.Format("모션 %d 조명 1 On",nInspCnt+1);
		THEAPP.SaveLog(strLog);

		Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (THEAPP.m_pCameraManager->m_bGrabDone)
				break;

			dwGrabEnd = GetTickCount();

			if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
			{
				THEAPP.m_pCameraManager->GrabErrorPostProcess();
				break;
			}
		}

		dWattingGrab_StartTime=0; dWattingGrab_EndtTime=0;
		dWattingGrab_StartTime = GetTickCount();
		while(TRUE)  // 영상1 그랩을 기다림
		{

			if (THEAPP.m_pCameraManager->ImageGrabCount == 1 || THEAPP.m_pCameraManager->m_bReGrab == TRUE)
			{
				break;
			}
			else
			{
				////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
				if (THEAPP.m_pHandlerService->m_nInspectPCStatus != 2 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 4 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 5)
				{
					THEAPP.m_pInspectService->QuitInspect();
					return 0;
				}
				Sleep(1);
			}
			while (THEAPP.m_pHandlerService->m_nInspectPCStatus == 4)
				Sleep(1);

			dWattingGrab_EndtTime = GetTickCount();
			if ((dWattingGrab_EndtTime - dWattingGrab_StartTime) > 3000)
			{
				THEAPP.m_pCameraManager->m_bReGrab = TRUE;
				break;
			}
		}
		if(THEAPP.m_pCameraManager->m_bReGrab==FALSE){break;}
	}



	Sleep(5);
	for(K=0;K<3;K++)
	{
		THEAPP.m_pCameraManager->CallHookFunction();

		THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, TRIGGER_EDGE);

		strLog.Format("모션 %d 조명 2 On",nInspCnt+1);
		THEAPP.SaveLog(strLog);

		Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (THEAPP.m_pCameraManager->m_bGrabDone)
				break;

			dwGrabEnd = GetTickCount();

			if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
			{
				THEAPP.m_pCameraManager->GrabErrorPostProcess();
				break;
			}
		}

		dWattingGrab_StartTime=0; dWattingGrab_EndtTime=0;
		dWattingGrab_StartTime = GetTickCount();
		while(TRUE)  // 영상2 그랩을 기다림
		{
			if(THEAPP.m_pCameraManager->ImageGrabCount==2 || THEAPP.m_pCameraManager->m_bReGrab==TRUE){break;}
			else
			{
			////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
				if(THEAPP.m_pHandlerService->m_nInspectPCStatus != 2 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 4 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 5)
				{
					THEAPP.m_pInspectService->QuitInspect();
					return 0;
				}
				Sleep(1);
			}
			while (THEAPP.m_pHandlerService->m_nInspectPCStatus == 4) Sleep(1);

			dWattingGrab_EndtTime = GetTickCount();
			if((dWattingGrab_EndtTime-dWattingGrab_StartTime)>3000)
			{
				THEAPP.m_pCameraManager->m_bReGrab=TRUE;
				break;
			}
		}
		if(THEAPP.m_pCameraManager->m_bReGrab==FALSE){break;}
	}



	Sleep(5);
	for(K=0;K<3;K++)
	{
		THEAPP.m_pCameraManager->CallHookFunction();

		THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, TRIGGER_LENS1);

		strLog.Format("모션 %d 조명 3 On",nInspCnt+1);
		THEAPP.SaveLog(strLog);

		Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (THEAPP.m_pCameraManager->m_bGrabDone)
				break;

			dwGrabEnd = GetTickCount();

			if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
			{
				THEAPP.m_pCameraManager->GrabErrorPostProcess();
				break;
			}
		}

		dWattingGrab_StartTime=0; dWattingGrab_EndtTime=0;
		dWattingGrab_StartTime = GetTickCount();
		while(TRUE)  // 영상3 그랩을 기다림
		{
			if(THEAPP.m_pCameraManager->ImageGrabCount==3 || THEAPP.m_pCameraManager->m_bReGrab==TRUE){break;}
			else
			{
			////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
				if(THEAPP.m_pHandlerService->m_nInspectPCStatus != 2 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 4 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 5)
				{
					THEAPP.m_pInspectService->QuitInspect();
					return 0;
				}
				Sleep(1);
			}
			while (THEAPP.m_pHandlerService->m_nInspectPCStatus == 4) Sleep(1);

			dWattingGrab_EndtTime = GetTickCount();
			if((dWattingGrab_EndtTime-dWattingGrab_StartTime)>3000)
			{
				THEAPP.m_pCameraManager->m_bReGrab=TRUE;
				break;
			}
		}
		if(THEAPP.m_pCameraManager->m_bReGrab==FALSE){break;}
	}



	Sleep(5);
	for(K=0;K<3;K++)
	{
		THEAPP.m_pCameraManager->CallHookFunction();

		THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, TRIGGER_LENS2);

		strLog.Format("모션 %d 조명 4 On",nInspCnt+1);
		THEAPP.SaveLog(strLog);

		Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (THEAPP.m_pCameraManager->m_bGrabDone)
				break;

			dwGrabEnd = GetTickCount();

			if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
			{
				THEAPP.m_pCameraManager->GrabErrorPostProcess();
				break;
			}
		}

		dWattingGrab_StartTime=0; dWattingGrab_EndtTime=0;
		dWattingGrab_StartTime = GetTickCount();
		while(TRUE)  // 영상4 그랩을 기다림
		{
			if(THEAPP.m_pCameraManager->ImageGrabCount==4 || THEAPP.m_pCameraManager->m_bReGrab==TRUE){break;}
			else
			{
			////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
				if(THEAPP.m_pHandlerService->m_nInspectPCStatus != 2 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 4 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 5)
				{
					THEAPP.m_pInspectService->QuitInspect();
					return 0;
				}
				Sleep(1);
			}
			while (THEAPP.m_pHandlerService->m_nInspectPCStatus == 4) Sleep(1);

			dWattingGrab_EndtTime = GetTickCount();
			if((dWattingGrab_EndtTime-dWattingGrab_StartTime)>3000)
			{
				THEAPP.m_pCameraManager->m_bReGrab=TRUE;
				break;
			}
		}
		if(THEAPP.m_pCameraManager->m_bReGrab==FALSE){break;}
	}
#else
	Sleep(50);

	CString FolderNameK,ImageNameK;
	if(THEAPP.Struct_PreferenceStruct.m_strDebug_FolderPath!="")
	{
		FolderNameK.Format(THEAPP.Struct_PreferenceStruct.m_strDebug_FolderPath+"\\");
	}
	else
	{
		return 0;
	}


	if(THEAPP.Struct_PreferenceStruct.m_bSaveBMP==TRUE)
	{
		try{

			for (int imgType = 0; imgType < BARREL_LENS_IMAGE_TAB; imgType++)
			{
				ImageNameK.Format("Module%d_%s.bmp", 1, g_sImageName[THEAPP.m_iMachineInspType][imgType]);
				read_image(&THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][imgType], FolderNameK + ImageNameK);
			}

			THEAPP.m_pCameraManager->ImageGrabCount = BARREL_LENS_IMAGE_TAB;
		}
		catch(HException &except)
		{
			CString str;
			str.Format("Halcon Exception [Debug Read Image] : %s", except.message);
			THEAPP.SaveLog(str);
			if(THEAPP.Struct_PreferenceStruct.m_bSaveBMP==TRUE)
			{
				THEAPP.m_pInspectResultDlg->MessageBox("BMP 파일이 있는 RawImage 폴더가 아닙니다. Debug 폴더를 다시 설정해주세요.");
			}
			else
			{
				THEAPP.m_pInspectResultDlg->MessageBox("JPG 파일이 있는 RawImage 폴더가 아닙니다. Debug 폴더를 다시 설정해주세요.");
			}

			THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
			THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;
			return 0;
		}
	}
	else
	{
		try
		{
			for (int imgType = 0; imgType < BARREL_LENS_IMAGE_TAB; imgType++)
			{
				ImageNameK.Format("Module%d_%s.jpg", 1, g_sImageName[THEAPP.m_iMachineInspType][imgType]);
				read_image(&THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][0], FolderNameK + ImageNameK);
			}

			THEAPP.m_pCameraManager->ImageGrabCount = BARREL_LENS_IMAGE_TAB;
		}
		catch(HException &except)
		{
			CString str;
			str.Format("Halcon Exception [Debug Read Image] : %s", except.message);
			THEAPP.SaveLog(str);
			if(THEAPP.Struct_PreferenceStruct.m_bSaveBMP==TRUE)
			{
				THEAPP.m_pInspectResultDlg->MessageBox("BMP 파일이 있는 RawImage 폴더가 아닙니다. Debug 폴더를 다시 설정해주세요.");
			}
			else
			{
				THEAPP.m_pInspectResultDlg->MessageBox("JPG 파일이 있는 RawImage 폴더가 아닙니다. Debug 폴더를 다시 설정해주세요.");
			}

			THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
			THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;
			return 0;
		}
	}
#endif

	THEAPP.m_pModelDataManager->ContoursAffinTrans1_Uniformity_Ori;
			
	HTuple  Row1, Column1, Angle1, ScaleR1, ScaleC1, Score1;
	HTuple  HomMat2DIdentity1;
	HTuple  HomMat2DTranslate1;
	HTuple  HomMat2DRotate1;
	HTuple  HomMat2DScale1;
			
	get_shape_model_contours(&THEAPP.m_pModelDataManager->ContoursAffinTrans1_Uniformity_Ori, THEAPP.m_pModelDataManager->ModelID1_Uniformity, 1);

	find_aniso_shape_model(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][0], THEAPP.m_pModelDataManager->ModelID1_Uniformity, 0, 0, 0.98, 1.02, 0.98, 1.02, 0.65, 1, 0.3, 
		"interpolation", 3, 0.9, &Row1, &Column1, &Angle1, &ScaleR1, &ScaleC1, &Score1);
	hom_mat2d_identity(&HomMat2DIdentity1);
	hom_mat2d_translate(HomMat2DIdentity1, Row1, Column1, &HomMat2DTranslate1);
	hom_mat2d_rotate(HomMat2DTranslate1, Angle1, Row1, Column1, &HomMat2DRotate1);
	hom_mat2d_scale(HomMat2DRotate1, ScaleR1, ScaleC1, Row1, Column1, &HomMat2DScale1);
	affine_trans_contour_xld(THEAPP.m_pModelDataManager->ContoursAffinTrans1_Uniformity_Ori, &THEAPP.m_pModelDataManager->ContoursAffinTrans1_Uniformity_Ori, HomMat2DScale1);	

	Hobject Reduced_InspectImage1,Reduced_InspectImage2,Reduced_InspectImage3,Reduced_InspectImage4;
	Hobject reduceedRegion;

	HTuple FitRow, FitColumn, FitRadius, FitStartPhi, FitEndPhi, FitPointOrder;
	fit_circle_contour_xld(THEAPP.m_pModelDataManager->ContoursAffinTrans1_Uniformity_Ori, "algebraic", -1, 0, 0, 3, 2, &FitRow, &FitColumn, &FitRadius, &FitStartPhi, &FitEndPhi, &FitPointOrder);

	gen_region_contour_xld(THEAPP.m_pModelDataManager->ContoursAffinTrans1_Uniformity_Ori,&reduceedRegion,"filled");

	opening_circle(reduceedRegion,&reduceedRegion,5);

	reduce_domain(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][0],reduceedRegion,&Reduced_InspectImage1);
	reduce_domain(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][1],reduceedRegion,&Reduced_InspectImage2);
	reduce_domain(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][2],reduceedRegion,&Reduced_InspectImage3);
	reduce_domain(THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][3],reduceedRegion,&Reduced_InspectImage4);

	Hobject Domain_InspectImage1,Domain_InspectImage2,Domain_InspectImage3,Domain_InspectImage4;
	Hobject Partition_Region1,Partition_Region2,Partition_Region3,Partition_Region4;
	get_domain(Reduced_InspectImage1,&Domain_InspectImage1);
	get_domain(Reduced_InspectImage2,&Domain_InspectImage2);
	get_domain(Reduced_InspectImage3,&Domain_InspectImage3);
	get_domain(Reduced_InspectImage4,&Domain_InspectImage4);


	Reduced_InspectImage1.Reset();
	Reduced_InspectImage2.Reset();
	Reduced_InspectImage3.Reset();
	Reduced_InspectImage4.Reset();
	gen_empty_obj(&Reduced_InspectImage1);
	gen_empty_obj(&Reduced_InspectImage2);
	gen_empty_obj(&Reduced_InspectImage3);
	gen_empty_obj(&Reduced_InspectImage4);


	HTuple Row1_1,Column1_1,Row2_1,Column2_1;
	HTuple Row1_2,Column1_2,Row2_2,Column2_2;
	HTuple Row1_3,Column1_3,Row2_3,Column2_3;
	HTuple Row1_4,Column1_4,Row2_4,Column2_4;
	Hobject Rectangle1,Rectangle2,Rectangle3,Rectangle4;

	inner_rectangle1(Domain_InspectImage1,&Row1_1,&Column1_1,&Row2_1,&Column2_1);
	gen_rectangle1(&Domain_InspectImage1,Row1_1,Column1_1,Row2_1,Column2_1);

	inner_rectangle1(Domain_InspectImage2,&Row1_2,&Column1_2,&Row2_2,&Column2_2);
	gen_rectangle1(&Domain_InspectImage2,Row1_2,Column1_2,Row2_2,Column2_2);

	inner_rectangle1(Domain_InspectImage3,&Row1_3,&Column1_3,&Row2_3,&Column2_3);
	gen_rectangle1(&Domain_InspectImage3,Row1_3,Column1_3,Row2_3,Column2_3);

	inner_rectangle1(Domain_InspectImage4,&Row1_4,&Column1_4,&Row2_4,&Column2_4);
	gen_rectangle1(&Domain_InspectImage4,Row1_4,Column1_4,Row2_4,Column2_4);

	partition_rectangle(Domain_InspectImage1,&Partition_Region1,FitColumn/4,FitRow/4);
	partition_rectangle(Domain_InspectImage2,&Partition_Region2,FitColumn/4,FitRow/4);
	partition_rectangle(Domain_InspectImage3,&Partition_Region3,FitColumn/4,FitRow/4);
	partition_rectangle(Domain_InspectImage4,&Partition_Region4,FitColumn/4,FitRow/4);


	Domain_InspectImage1.Reset();
	Domain_InspectImage2.Reset();
	Domain_InspectImage3.Reset();
	Domain_InspectImage4.Reset();


	connection(Partition_Region1,&Partition_Region1);
	connection(Partition_Region2,&Partition_Region2);
	connection(Partition_Region3,&Partition_Region3);
	connection(Partition_Region4,&Partition_Region4);


	CString strFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
	CIniFileCS INIUniformity(strFolder+"\\LightUniformityData.txt");
	CString strSectionUniformity = "Mean";
	BOOL bLightUniformity = FALSE;
			
	int M;

	Hobject Partition_Region;
	HTuple Number;
	Partition_Region = Partition_Region1;
	count_obj(Partition_Region,&Number);
	opening_circle(Partition_Region,&Partition_Region,10);
	for(M=0;M<Number;M++)
	{
		Hobject Select_Region;
		HTuple Mean,Deviation;
		HTuple Area,Row,Column;
		select_obj(Partition_Region,&Select_Region,M+1);
		intensity(Select_Region,THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][0],&Mean,&Deviation);

		CString partname;
		partname.Format("Tab%d-Mean%d",1,M+1);
		strSectionUniformity = "Mean";
		long lMean = INIUniformity.Get_Long(strSectionUniformity,partname,100);

		strSectionUniformity = "AcceptRange";
		long lRange = INIUniformity.Get_Long(strSectionUniformity,"Range",5);

		int a = Mean[0].L();
		int b = lMean;
		int c = abs(Mean[0].L()-lMean);

		if((abs(Mean[0].L()-lMean)>lRange))
		{
			bLightUniformity = TRUE;
		}
	}

	Partition_Region = Partition_Region2;
	count_obj(Partition_Region,&Number);
	opening_circle(Partition_Region,&Partition_Region,10);
	for(M=0;M<Number;M++)
	{
		Hobject Select_Region;
		HTuple Mean,Deviation;
		HTuple Area,Row,Column;
		select_obj(Partition_Region,&Select_Region,M+1);
		intensity(Select_Region,THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][1],&Mean,&Deviation);

		CString partname;
		partname.Format("Tab%d-Mean%d",2,M+1);
		strSectionUniformity = "Mean";
		long lMean = INIUniformity.Get_Long(strSectionUniformity,partname,100);

		strSectionUniformity = "AcceptRange";
		long lRange = INIUniformity.Get_Long(strSectionUniformity,"Range",5);
		int a = Mean[0].L();
		int b = lMean;
		int c = abs(Mean[0].L()-lMean);
				
		if((abs(Mean[0].L()-lMean)>lRange))
		{
			bLightUniformity = TRUE;
		}
	}


	Partition_Region = Partition_Region3;
	count_obj(Partition_Region,&Number);
	opening_circle(Partition_Region,&Partition_Region,10);
	for(M=0;M<Number;M++)
	{
		Hobject Select_Region;
		HTuple Mean,Deviation;
		HTuple Area,Row,Column;
		select_obj(Partition_Region,&Select_Region,M+1);
		intensity(Select_Region,THEAPP.m_pInspectAdminViewDlg->pModuleGrabImage[0][2],&Mean,&Deviation);

		CString partname;
		partname.Format("Tab%d-Mean%d",3,M+1);
		strSectionUniformity = "Mean";
		long lMean = INIUniformity.Get_Long(strSectionUniformity,partname,100);

		strSectionUniformity = "AcceptRange";
		long lRange = INIUniformity.Get_Long(strSectionUniformity,"Range",5);
		int a = Mean[0].L();
		int b = lMean;
		int c = abs(Mean[0].L()-lMean);

		if((abs(Mean[0].L()-lMean)>lRange))
		{
			bLightUniformity = TRUE;
		}
	}
			

	if(bLightUniformity==TRUE)
	{
		THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[0] = DEFECT_TYPE_BARREL;
		THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[0] = DEFECT_TYPE_BARREL;
	}
	else
	{
		THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.OkNg[0] = DEFECT_TYPE_GOOD;
		THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.OkNg[0] = DEFECT_TYPE_GOOD;
	}
	THEAPP.m_pTrayAdminViewDlg->m_TrayImagePB.InvalidateRect(false);
	THEAPP.m_pTrayOperatorViewNowDlg->m_TrayImagePB.InvalidateRect(false);

#ifdef INLINE_MODE
	int StartT1 = GetTickCount();
	int iLineNo = 1;
	THEAPP.m_pHandlerService->Set_ScanComplete(iLineNo);  // 스캔 종료 -  트레이 언로딩 됨 - 언로딩 완료되면 스테이터스 업데이트 날라옴 ready
	
	THEAPP.m_pHandlerService->Set_InspectComplete(pInspectService->m_sLotID_H, pInspectService->m_iTrayNo_H, pInspectService->m_iLineNo_H, THEAPP.m_iModuleNumberOneLine, "", ""); // 검사 종료 신호 날림. PC 검사 ready 상태 set함.  //연산 쪽에 있어야함.

	while(TRUE)
	{
		if(THEAPP.m_pHandlerService->m_nInspectPCStatus!=2)
		{
			break;
		}
		////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
		if(THEAPP.m_pHandlerService->m_nInspectPCStatus != 2 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 4 && THEAPP.m_pHandlerService->m_nInspectPCStatus != 5)
		{
			THEAPP.m_pInspectService->QuitInspect();
			return 0;
		}
		Sleep(1);
	}
	int EndT1 = GetTickCount();
#endif

	THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = FALSE;
	THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = FALSE;
}

void CInspectService::InspectionMove(BOOL bRestart)
{
	// 조명 밝기 확인 모드
	if (THEAPP.m_pInspectSummary->m_bUniformityCheckMode == TRUE)
	{
		AfxBeginThread(UniformityCheckThread, this);
	}
	else
	{		
#ifdef INLINE_MODE

		for (int i = 0; i < THEAPP.m_iModuleNumberOneLine; i++)
			THEAPP.m_pInspectAdminViewDlg->m_iBarcodeGrabStatus[i] = BARCODE_GRAB_STATUS_NOT_READY;

		AfxBeginThread(BarcodeScanThread, this);
#endif
		for (int i = 0; i < THEAPP.m_iModuleNumberOneLine; i++)
			THEAPP.m_pInspectAdminViewDlg->m_iInspectGrabStatus[i] = INSPECT_GRAB_STATUS_NOT_READY;

		AfxBeginThread(InspectionThread, this);
		AfxBeginThread(ResultSaveThread, this);
	}
}

void CInspectService::SaveNgBarcodeInfo(CString sBarcodeID, CString sLotID, CString sTrayNo, CString sModuleCol, CString sModuleRow)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strFile;
	strFile.Format(THEAPP.m_FileBase.m_strCurrentLotFolder+"\\NGBarcodeInfo.txt");

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite)) 
		return;

	try {
		file.SeekToEnd();

		CString strSave;
		int idx = atoi((LPSTR)(LPCSTR)sModuleCol) + atoi((LPSTR)(LPCSTR)sModuleRow)*THEAPP.m_pModelDataManager->m_iTrayArrayX +1;
		strSave.Format("%s, %s, %s, %s, %s, %d\r\n", sBarcodeID, sLotID, sTrayNo, sModuleCol, sModuleRow, idx);

		file.Write(strSave, strSave.GetLength());
		file.Close();

	} catch (CFileException *pEx) {
		pEx->Delete();
	}
}

void CInspectService::OfflineInspection()
{
	
	for (int iLineNo=1; iLineNo<=THEAPP.m_iTrayLineNumber; iLineNo++)
	{
		THEAPP.m_pModelDataManager->ModelPreSet(iLineNo);

		THEAPP.m_pInspectSummary->m_iDxTrayNo = 1;
		THEAPP.m_pInspectSummary->m_iDxLineNo = iLineNo;
		THEAPP.m_pInspectSummary->UpdateData(FALSE);

		THEAPP.m_pInspectService->m_iTrayNo_H = 1;
		THEAPP.m_pInspectService->m_iLineNo_H = iLineNo;
		THEAPP.m_pInspectService->m_sLotID_H = THEAPP.m_pInspectResultDlg->CurrentLotID;

		if (iLineNo==1)
			THEAPP.m_pInspectSummary->Initailize_for_Inspection();

		THEAPP.m_pInspectSummary->Initialize_one_line();

		THEAPP.m_pInspectAdminViewDlg->m_bInspectRunDisplay = TRUE;
		THEAPP.m_pInspectAdminViewHideDlg->m_bInspectRunDisplay = TRUE;

		THEAPP.m_pInspectService->m_bLineInspectDone = FALSE;
		THEAPP.m_pInspectService->InspectionMove(FALSE);

		while (!m_bLineInspectDone)
		{
			Sleep(100);
		}
	}

}

//Multiple Defect Start
// about 400 lines - 251014, jhkim
UINT LotDetailDefectErrorThread(LPVOID lp)
{
	LOT_DEFECT_ALARM_THREAD_PARAM* pThreadItem = (LOT_DEFECT_ALARM_THREAD_PARAM *)lp;
	CInspectService *pInspectService = pThreadItem->pInspectService;
	int iThreadMzIdx = pThreadItem->iMzIdx;
	int iNoInspectModule = pThreadItem->iLotModuleAmt;
	CString sLotID = pThreadItem->sLotID;
	SAFE_DELETE(pThreadItem);
	
	// iNoInspectModule : 현재 로딩된 모듈 수
	// m_iLotModuleAmt_H : Handler 로 부터 UDP 통신을 통해 전달 받는 값.
	// int iNoInspectModule = pInspectService->m_iLotModuleAmt_H;

	// iMultipleDefectAlarmMinNum : 알람을 울리기 위한 최소 검사 모듈 수
	int iMultipleDefectAlarmMinNum = THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmMinimumNum;

#ifdef _DEBUG
	// Multi Defect test
	iNoInspectModule = iMultipleDefectAlarmMinNum + 1;
#endif

	// 최소 검사 모듈 수 보다 적으면 리턴
	if (iNoInspectModule < iMultipleDefectAlarmMinNum)
	{
		return 1;
	}

	try
	{
		THEAPP.DoubleLogOut("[LotDetailDefectErrorThread] Start - LotID : %s, MzIdx : %d, NoInspectModule : %d", (LPSTR)(LPCSTR)sLotID, iThreadMzIdx, iNoInspectModule);
		// 변수 초기화
		// MAX_MULTIPLE_DEFECT_NUMBER : Defect 종류, 총 25종 - 251013, jhkim
		Hobject HConcatMultipleDefecRgn[MAX_MULTIPLE_DEFECT_NUMBER];
		for (int i = 0; i < MAX_MULTIPLE_DEFECT_NUMBER; i++)
			gen_empty_obj(&HConcatMultipleDefecRgn[i]);

		// ValidHRegion 체크 후 다발 불량 영역을 하나로 합침
		for (int iDefectIdx = 0; iDefectIdx < MAX_MULTIPLE_DEFECT_NUMBER; ++iDefectIdx)
		{
			std::vector<Hobject> validDefectRegions;
			// MAX_TRAY_LOT: 최대 TRAY 수, 총 60개 - 251013, jhkim
			for (int iTrayIdx = 0; iTrayIdx < MAX_TRAY_LOT; ++iTrayIdx)
			{
				// MAX_MODULE_ONE_TRAY : Tray 당 최대 모듈 수 , 총 80개 - 251013, jhkim
				for (int iModuleIdx = 0; iModuleIdx < MAX_MODULE_ONE_TRAY; ++iModuleIdx)
				{
					// HObject != Hobject : 대소문자에 따라 다른 객체이므로 유의. - 251013, jhkim
					const Hobject& defectRegion = THEAPP.m_stMultipleDefectInfo[iThreadMzIdx][iDefectIdx].m_HMultipleDefectRgn[iTrayIdx][iModuleIdx];
					if (THEAPP.m_pGFunction->ValidHRegion(defectRegion))
						validDefectRegions.push_back(defectRegion);
				}
			}

			// Batch concat_obj outside the loops if there are valid regions
			if (!validDefectRegions.empty())
			{
				for (const auto& validateDefectRegion : validDefectRegions)
					concat_obj(HConcatMultipleDefecRgn[iDefectIdx], validateDefectRegion, &HConcatMultipleDefecRgn[iDefectIdx]);
			}
		}

		// iLotDefectAlarmDefectRatio : 알람을 울리기 위한 불량 비율 (%)
		int iLotDefectAlarmDefectRatio = THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmDefectRatio;
		// iMinDefectNumberForAlarm : 알람을 울리기 위한 최소 불량 개수
		int iMinDefectNumberForAlarm = (int)((double)iNoInspectModule * ((double)iLotDefectAlarmDefectRatio * PERCENTAGE_FACTOR));

		// iDefectNumberToAlarm : 각 불량 항목 별 검출 된 불량 개수
		int iDefectNumberToAlarm[MAX_MULTIPLE_DEFECT_NUMBER];
		Hobject HDefectDetailRgn[MAX_MULTIPLE_DEFECT_NUMBER];
		// bDefectToAlarm : 각 불량 별 알람 발생 여부
		BOOL bDefectToAlarm[MAX_MULTIPLE_DEFECT_NUMBER];

		// 변수 초기화
		for (int iDefectIdx = 0; iDefectIdx < MAX_MULTIPLE_DEFECT_NUMBER; iDefectIdx ++)
		{
			bDefectToAlarm[iDefectIdx] = FALSE;
			iDefectNumberToAlarm[iDefectIdx] = 0;
			gen_empty_obj(&HDefectDetailRgn[iDefectIdx]);

			// 불량 개수 카운트
			Hlong hlTempNum;
			// 아래 코드는 위쪽 ValidRefions 의 Length를 재면 될듯?
			count_obj(HConcatMultipleDefecRgn[iDefectIdx], &hlTempNum);
			iDefectNumberToAlarm[iDefectIdx] = hlTempNum;

			// 불량 개수가 알람 임계치 이상이면 알람 발생 대상으로 설정
			if (iDefectNumberToAlarm[iDefectIdx] >= iMinDefectNumberForAlarm)
			{
				bDefectToAlarm[iDefectIdx] = TRUE;
				HDefectDetailRgn[iDefectIdx] = HConcatMultipleDefecRgn[iDefectIdx];
			}
		}

		// 알람발생 대상을 구했으니, 아래 는 알람 발생 및 그 후 처리를 해야 한다.
		// 여기까지 불량 갯수, 불량 대상, 불량 Idx 를 알고 있음.
		/**
		 * 변수 정리
		 * iNoInspectModule : 현재 로딩된 모듈 수
		 * iMultipleDefectAlarmMinNum : 알람을 울리기 위한 최소 검사 모듈 수
		 * HConcatMultipleDefecRgn[MAX_MULTIPLE_DEFECT_NUMBER] : 각 불량 항목 별 합쳐진 불량 영역
		 * iLotDefectAlarmDefectRatio : 알람을 울리기 위한 불량 비율 (%)
		 * iMinDefectNumberForAlarm : 알람을 울리기 위한 최소 불량 개수
		 * iDefectNumberToAlarm[] : 각 불량 항목 별 검출 된 불량 개수
		 * bDefectToAlarm[] : 각 불량 별 알람 발생 여부
		 */

		BOOL bIsMultipleDefect = FALSE;
		
		// 아래 For 구문은 없어도 될듯?
		// 위에 For 에서 몇 번째 idx 가 True 인지 검사 했으므로, 해당 Idx 아래로 넘겨주면 됨.
		// (고려할 점) 근데 여러 불량 항목에 대해 True 라면?
		for (int iDefectIdx = 0; iDefectIdx < MAX_MULTIPLE_DEFECT_NUMBER; iDefectIdx++)
		{
			if (bIsMultipleDefect)
				break;

			// 가독성을 위해 FALSE 가 앞에 오도록 수정함. - 251013, jhkim
			if (bDefectToAlarm[iDefectIdx] == FALSE)
			{
				continue;
			}
			// 알람 발생 대상인 불량에 대해 다발 불량 검사 수행
			else
			{
				Hobject HCheckDefectRgn;
				Hobject HSamePosPassRgn, HSamePosPassRgn_EachBlob;
				Hlong hlNoCnt;
				long lDefectNumberCount, lDefectCnt;
				long lNoSamePosAreaBlob;
				int nFinalDefectCount = 0;
				// 면적유사도
				int iAreaTolerance = THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmAreaSimilarityTolerance;
				int iPosTolerance = THEAPP.Struct_PreferenceStruct.m_iMultipleDefectAlarmSamePosTolerance;

				for (int k = 0; k < iDefectNumberToAlarm[iDefectIdx]; k++)
				{
					if (bIsMultipleDefect)
						break;

					select_obj(HDefectDetailRgn[iDefectIdx], &HCheckDefectRgn, k + 1);

					if (THEAPP.m_pGFunction->ValidHRegion(HCheckDefectRgn) == FALSE)
						continue;

					/**
					 * @name select_shape_proto
					 * @brief Choose regions having a certain relation to each other.
					 * @param Regions : 		Regions to be examined.
					 * @param Pattern : 		Region compared to Regions.
					 * @param SelectedRegions : Regions fulfilling the condition.
					 * @param Feature : 		Shape features to be checked.
					 * @param Min : 			Lower border of feature.
					 * @param Max : 			Upper border of the feature.
					 *
					 * @details : The operator SelectShapeProto selects regions based on certain relations between the regions.
					 * 			  Every region from Regions is compared to the union of regions from Pattern.
					 * 			  The limits (Min and Max) are specified absolutely or in percent (0..100), depending on the feature
					 * 
					 * @details "distance_contour" (feature) : The minimum Euclidean distance from the edge of Pattern 
					 * 										   to the edge of every region from Regions is determined. (see DistanceRrMin).
					 *
					 * @details "covers" (feature) : It is examined how well the region Pattern fits into the regions from Regions.
					 * 								 If there is no shift so that Pattern is a subset of Regions the overlap is 0.
					 * 								 If Pattern corresponds to the region after a corresponding shift the overlap is 100.
					 * 								 Otherwise the area of the opening of Regions with Pattern is put into relation with the area of Regions (in percent).
					 *
					 * @details "overlaps_rel" (feature) : The area of the intersection of Pattern and every region in Regions is computed.
					 * 									   The relative overlap is the ratio of the area of the intersection 
					 * 									   and the are of the respective region in Regions (in percent).
					 */

					// TODO : 왜 distance_contour 인지 확인 필요. 퍼센트로 쓰려면 covers 혹은 overlaps_rel 을 써야 함.
					//		  그 외에 상황에 iTolerance 는 픽셀값(절대값)으로 계산됨.
					// 기존 코드는 아래 Max 값으로 iAreaTolerance 를 쓰고 있었음. iPosTolerance 로 수정함. - 251111, jhkim
					select_shape_proto(HDefectDetailRgn[iDefectIdx], HCheckDefectRgn, &HSamePosPassRgn, "distance_contour", 0, iPosTolerance);

					// HSamePosPassRgn : 동일 위치로 판단 된 blob Tuple
					count_obj(HSamePosPassRgn, &hlNoCnt);
					lDefectNumberCount = hlNoCnt;

					// 임계치 보다 적으면 continue
					if (lDefectNumberCount < iMinDefectNumberForAlarm)
						continue;

					for (int iDefectNumberIdx = 0; iDefectNumberIdx < lDefectNumberCount; iDefectNumberIdx++)
					{
						// why +1?
						select_obj(HSamePosPassRgn, &HCheckDefectRgn, (iDefectNumberIdx + 1));

						if (THEAPP.m_pGFunction->ValidHRegion(HCheckDefectRgn) == FALSE)
							continue;

						select_shape_proto(HSamePosPassRgn, HCheckDefectRgn, &HSamePosPassRgn_EachBlob, "distance_contour", 0, iPosTolerance);

						count_obj(HSamePosPassRgn_EachBlob, &hlNoCnt);
						lDefectCnt = hlNoCnt;

						if ((lDefectCnt < iMinDefectNumberForAlarm) || (lDefectCnt < 2))
							continue;

						CString sDefectNameToAlarm;
						// m_bUseMultipleDefectAlarmAreaSimilarity
						// (중간면적기준) 면적 유사도 범위(%) (On / Off)
						if (THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarmAreaSimilarity)
						{
							HTuple HArea;
							HTuple HCenterX, HCenterY, HMin, HMax;
							Hlong HMedian;
							int iAreaToleranceRange;
							int lNoSamePosAreaBlob = 0;

							area_center(HSamePosPassRgn_EachBlob, &HArea, &HCenterY, &HCenterX);
							tuple_median(HArea, &HMedian);

							iAreaToleranceRange = HMedian;
							iAreaToleranceRange = int((double)iAreaToleranceRange * (double)(iAreaTolerance * PERCENTAGE_FACTOR));
							HMin = HMedian - iAreaToleranceRange;
							HMax = HMedian + iAreaToleranceRange;
							
							if (HMin < 1)
								HMin = 1;

							for (int nTempIdx = 0; nTempIdx < lDefectCnt; nTempIdx++)
								if ((HArea[nTempIdx] >= HMin) && (HArea[nTempIdx] <= HMax))
									++lNoSamePosAreaBlob;

							// 다발 불량 임계치 이상이고, 2개 이상일 때 다발 불량으로 판정
							if ((lNoSamePosAreaBlob >= iMinDefectNumberForAlarm) && (lNoSamePosAreaBlob >= 2))
							{
								sDefectNameToAlarm = THEAPP.GetVisionAndDefectName(iDefectIdx);
								// 다발불량 발생 시 Handler 에 알람 요청
								// m_bUseMultipleDefectSendAlarm : 다발 불량 시 핸들러 알람 사용
								BOOL bUseAlarm = THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectSendAlarm;
								if (bUseAlarm)
								{
									THEAPP.m_pHandlerService->Set_AlarmRequest("CAM1", sLotID, HANDLER_ALARM_MULTIPLE_DEFECT, sDefectNameToAlarm, lNoSamePosAreaBlob);

									BOOL bSaveLasLog = THEAPP.Struct_PreferenceStruct.m_bSaveLasLog;
									if (bSaveLasLog)
										THEAPP.m_pSaveManager->SaveLasEvent(sLotID, THEAPP.GetLASTempDirectory(), THEAPP.GetLASEventDirectory(), FALSE);
								}

								// TODO : 이게 area similarity 인가? 확실히 확인. (면적유사도 의도대로 동작 하는 지 확인 필요함)
								select_shape(HSamePosPassRgn_EachBlob, &HSamePosPassRgn_EachBlob, "area", "and", HMin, HMax);
								nFinalDefectCount = (int)lNoSamePosAreaBlob;

								bIsMultipleDefect = TRUE;
							}
						}
						else
						{
							sDefectNameToAlarm = THEAPP.GetVisionAndDefectName(iDefectIdx);
							// 다발불량 발생 시 Handler 에 알람 요청 - 바로 위와는 유사하지만 lDefectCnt 내용이 다름.
							BOOL bUseAlarm = THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectSendAlarm;
							if (bUseAlarm)
							{
								THEAPP.m_pHandlerService->Set_AlarmRequest("CAM1", sLotID, HANDLER_ALARM_MULTIPLE_DEFECT, sDefectNameToAlarm, lDefectCnt);

								if (THEAPP.Struct_PreferenceStruct.m_bSaveLasLog)
									THEAPP.m_pSaveManager->SaveLasEvent(sLotID, THEAPP.GetLASTempDirectory(), THEAPP.GetLASEventDirectory(), FALSE);
							}

							nFinalDefectCount = lDefectCnt;

							bIsMultipleDefect = TRUE;
						}

						if (bIsMultipleDefect == FALSE)
							continue;

						Hobject HSelectedOneModuleBlobRgn;
						Hobject HUnionRgn;
						Hobject HOneModuleUnionRgn;
						Hobject HOneModuleConnRgn;

						// 아래 코드는 로그를 남기기 위한 과정 (추정)
						for (int nSelectRgnIdx = 0; nSelectRgnIdx < lDefectCnt; nSelectRgnIdx++)
						{
							select_obj(HSamePosPassRgn_EachBlob, &HSelectedOneModuleBlobRgn, nSelectRgnIdx + 1);

							for (int nTray = 0; nTray < MAX_TRAY_LOT; nTray++)
							{
								for (int nModule = 0; nModule < MAX_MODULE_ONE_TRAY; nModule++)
								{
									double dDummyValue;

									BOOL bIsVaild = THEAPP.m_pGFunction->ValidHRegion(THEAPP.m_stMultipleDefectInfo[iThreadMzIdx][iDefectIdx].m_HMultipleDefectRgn[nTray][nModule]);
									if (bIsVaild == FALSE)
										continue;

									Hlong hlUnionArea;
									union2(THEAPP.m_stMultipleDefectInfo[iThreadMzIdx][iDefectIdx].m_HMultipleDefectRgn[nTray][nModule], HSelectedOneModuleBlobRgn, &HUnionRgn);
									union1(HUnionRgn, &HUnionRgn);

									// 중심좌표는 필요없음. Temp 로 주소값만 넘겨주고 사용은 안함.
									area_center(HUnionRgn, &hlUnionArea, &dDummyValue, &dDummyValue);

									bIsVaild = THEAPP.m_pGFunction->ValidHRegion(HUnionRgn);
									if (bIsVaild == FALSE)
										continue;

									Hlong hlOneModuleArea;
									union1(THEAPP.m_stMultipleDefectInfo[iThreadMzIdx][iDefectIdx].m_HMultipleDefectRgn[nTray][nModule], &HOneModuleUnionRgn);
									
									// 중심좌표는 필요없음. Temp 로 주소값만 넘겨주고 사용은 안함.
									area_center(HOneModuleUnionRgn, &hlOneModuleArea, &dDummyValue, &dDummyValue);

									if (hlUnionArea == hlOneModuleArea)
									{
										Hobject HConnRgn, HDiffRgn;
										BOOL bFindModule = FALSE;
										
										connection(HUnionRgn, &HConnRgn);
										difference(HUnionRgn, HOneModuleUnionRgn, &HDiffRgn);
										
										// Hlong == 4 - 8byte integer
										BOOL bIsValid = THEAPP.m_pGFunction->ValidHRegion(HDiffRgn);
										if (bIsValid == FALSE)
											bFindModule = TRUE;
										else
										{
											Hlong hlConnCnt, hlDiffConnCnt;

											connection(HDiffRgn, &HDiffRgn);
											count_obj(HConnRgn, &hlConnCnt);
											count_obj(HDiffRgn, &hlDiffConnCnt);

											// What for?
											hlConnCnt = hlDiffConnCnt;

											// this statement cant be TRUE
											if (hlConnCnt > hlDiffConnCnt)
												bFindModule = TRUE;
										}

										if (bFindModule)
										{
											CString strMultipleDefectLog;
											int iTrayIdx = nTray + 1;
											int iModuleIdx = nModule + 1;

											// 다발불량 로그로 남김.
											// 로그 위치 : CMI_Results\[Date]\[LotID]\MultipleDefectList.txt
											// 로그 내용 중 Date 는 SaveMultipleDefectListLog 함수에서 처리
											strMultipleDefectLog.Format(
												"%s\t%s\t%s\t%d\t%d\t%s\t%s\t%d\t",								 
												THEAPP.Struct_PreferenceStruct.m_strEquipNo,					 // Station
												THEAPP.m_pModelDataManager->m_sModelName,						 // Model
												sLotID,															 // LOTNUM (ID)
												iTrayIdx,														 // Tray Number
												iModuleIdx,														 // Module Number
												THEAPP.m_strMultipleDefectBarcode[iThreadMzIdx][nTray][nModule], // SN
												/*g_strVisionName_Short[iVisionCamType],*/						 // Vision
												sDefectNameToAlarm,												 // Defect Name
												nFinalDefectCount												 // Defect Count
											);
											
											THEAPP.SaveMultipleDefectListLog(THEAPP.m_FileBase.m_strCurrentLotFolder, strMultipleDefectLog);


											gen_empty_obj(&THEAPP.m_stMultipleDefectInfo[iThreadMzIdx][iDefectIdx].m_HMultipleDefectRgn[nTray][nModule]);

											bIsMultipleDefect = TRUE;

											Sleep(10);

											break;
										}
									}
								}
							}
						}

						THEAPP.ResetMultipleDefectInfo(iThreadMzIdx);

						return 1;
					}
				}
			}
		}
		THEAPP.ResetMultipleDefectInfo(iThreadMzIdx);

		return 1;
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [InspectService::LotDetailDefectErrorThread] : %s", except.message);
		THEAPP.SaveLog(str);
		return 0;
	}
}

void CInspectService::LotDetailDefectError(int iVirtualMzNoIdx, CString sLotID)
{
	LOT_DEFECT_ALARM_THREAD_PARAM* pParam = new LOT_DEFECT_ALARM_THREAD_PARAM(iVirtualMzNoIdx, sLotID, m_iLotModuleAmt_H, this);

	// 다발 불량 쓰레드 시작
	AfxBeginThread(LotDetailDefectErrorThread, LPVOID(pParam));
}
//Multiple Defect end