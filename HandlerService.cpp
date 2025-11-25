// HandlerService.cpp : implementation file
//
#include "stdafx.h"
#include "uScan.h"
#include "HandlerService.h"
#include "IniFileCS.h"

// Cant understand why these are needed here - 250910, jhkim
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char * THIS_FILE = __FILE__;
#endif
/////////////////////////////////////////////////////////////

#define UDP_IP		"192.168.1.11"			// CMI3000 Full-Auto

#ifdef WELDING_CAM
	#define UDP_PORT	7001
#else
	#define UDP_PORT	20001
#endif

// TODO : 굳이 SendCommandCriticalSection 필요한 이유? (Handler 는 단일쓰레드 아닌가?)
//      : Header 에 선언 안하고 여기에, 전역변수로 선언한 이유? - 251028, jhkim
static CRITICAL_SECTION SendCommandCriticalSection;

/////////////////////////////////////////////////////////////////////////////
// CHandlerService
/* 
UINT CallUnLoadingThread(LPVOID lp)
{
	THEAPP.m_pHandlerService->Set_MoveToUnload();
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;

	while(!THEAPP.m_pHandlerService->m_bMotionMoveComplete) // 무브 컴플리트 기다림
	{
		// 4 = HS_PAUSE, 5 = HS_CYCLE_STOP, 6 = HS_STOP
		if (THEAPP.m_pHandlerService->m_nInspectPCStatus >= HS_PAUSE)
		{
			THEAPP.m_pHandlerService->m_bMotionMoveComplete = TRUE;
			return 0;
		}
		
		Sleep(1);
	}

	THEAPP.m_pHandlerService->Set_PositionRequest();
	THEAPP.m_pHandlerService->m_bMotionMoveComplete = FALSE;
	
	return 0;
}
 */
CHandlerService *CHandlerService::m_pInstance = NULL;

CHandlerService *CHandlerService::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new CHandlerService();

		// 메세지 전용 윈도우 생성
		if (!m_pInstance->m_hWnd) {
			CRect r = m_pInstance->GetPosition();
			m_pInstance->CreateEx(0, AfxRegisterWndClass(0), "CHandlerService", 0, r, NULL, 0, NULL);
		}
	}
	return m_pInstance;
}

void CHandlerService::DeleteInstance()
{
	if (m_pInstance->m_hWnd) m_pInstance->DestroyWindow();
	if (m_pInstance) delete m_pInstance;
	m_pInstance = NULL;
}

CHandlerService::CHandlerService()
{
	m_bUdpOpened = FALSE;
	m_bConnected = FALSE;

	m_bReplyReceived = FALSE;

	m_bScanDone = FALSE;

	m_sLotID = "";
	m_iTrayNo = 0;
	m_iLineNo = 1;
	m_bAMoveComplete = FALSE;
	m_bRMoveComplete = FALSE;
	m_bJMoveComplete = FALSE;
	m_bMotionMoveComplete = FALSE;
	m_nInspectPCStatus = VS_NOT_READY;
	m_bMoveCompleteFlag = FALSE;
	m_bBarcodeMoveCompleteFlag = FALSE;

	m_sInspectCompleteMsg = "";
}

CHandlerService::~CHandlerService()
{
}

BEGIN_MESSAGE_MAP(CHandlerService, CWnd)
	//{{AFX_MSG_MAP(CHandlerService)
	ON_MESSAGE(UM_UDP_RECEIVE, OnUdpReceive)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHandlerService message handlers

BOOL CHandlerService::Initialize_Handler()
{
	InitializeCriticalSection(&SendCommandCriticalSection);

#ifdef HANDLER_USE
	m_bUdpOpened = m_UdpHandler.Open_Socket(UDP_IP, UDP_PORT, m_hWnd);
#else
	// TODO : Make connectable in offline (for debugging)
	m_bUdpOpened = FALSE;
#endif
	if (m_bUdpOpened)
		Set_ConnectRequest();


	return m_bUdpOpened;
}

void CHandlerService::Terminate_Handler()
{
	Set_ConnectEnd();
	m_bConnected = FALSE;
	m_bUdpOpened = FALSE;
#ifdef HANDLER_USE
	m_UdpHandler.Close_Socket();
#endif

	DeleteCriticalSection(&SendCommandCriticalSection);
}

/////////////////////////////////////////////////////////////////////////////

void CHandlerService::Get_ConnectRequest()
{
	m_bConnected = TRUE;
	Set_ConnectReply();
}

void CHandlerService::Get_ConnectReply()
{
	//Set_ShowRequest(0,1); // home 화면 
	m_bReplyReceived = TRUE;
}

void CHandlerService::Get_ConnectEnd()
{
	m_bConnected = FALSE;
}

void CHandlerService::Get_StatusRequest()
{
	Set_StatusReply();
}
void CHandlerService::Get_StatusReply(CString sStatus)
{
	m_iHandlerStatus = atoi((LPSTR)(LPCSTR)sStatus);
}
void CHandlerService::Get_StatusUpdate(CString sStatus)
{
	UINT uiStatus = atoi((LPSTR)(LPCSTR)sStatus);

	auto StatusParser = [](UINT status) -> CString
	{
		switch (status)
		{
		case HS_NOT_READY:
			return "NOT READY";
			break;
		case HS_READY:
			return "READY";
			break;
		case HS_RUN:
			return "RUN";
			break;
		case HS_RESET:
			return "RESET";
			break;
		case HS_PAUSE:
			return "PAUSE";
			break;
		case HS_CYCLE_STOP:
			return "CYCLE STOP";
			break;
		default:
			if (status >= HS_STOP)
				return "STOP";
			break;
		}
	};

	CString sLog;
	sLog.Format("UDP_Received. Get_StatusUpdate. handler state : %s -> %s", StatusParser(m_iHandlerStatus), StatusParser(uiStatus));
	THEAPP.m_pLogDlg->WriteLog(sLog);
	THEAPP.SaveLog(sLog);

	m_iHandlerStatus = uiStatus;

	// AutoParam. Update 중에는 Status 시작하지 않음 - 251029, jhkim
	if (m_nInspectPCStatus == VS_NOT_READY)
	{
		// HS_NOT_READY 는 동기화 하지 않는다.
		sLog.Format("Vision not ready... Status update skipped : H(%s) V(%s)",  StatusParser(m_iHandlerStatus), StatusParser(m_nInspectPCStatus));
		THEAPP.m_pLogDlg->WriteLog(sLog);
		THEAPP.SaveLog(sLog);
		Set_StatusReply();
		return;
	}
	else
	{
		// Handler 와 Status 동기화 (비상시 탈출용)
		sLog.Format("Vision state Synced : H(%s) V(%s)",  StatusParser(m_iHandlerStatus), StatusParser(m_nInspectPCStatus));
		m_nInspectPCStatus = m_iHandlerStatus;
		THEAPP.m_pLogDlg->WriteLog(sLog);
		THEAPP.SaveLog(sLog);
	}

	switch(m_iHandlerStatus)
	{
	case HS_NOT_READY:
		THEAPP.SaveLog("핸들러 Not Ready");
		break;
		
	case HS_READY:
		THEAPP.SaveLog("핸들러 Ready");
		break;
		
	case HS_RUN:
		THEAPP.SaveLog("핸들러 Run");
		Set_StatusReply();
		break;
		
	case HS_RESET:
		// TODO : 왜 여기 포함 하단에서 START 버튼을 Enable 처리하는지? - 251028, jhkim
		//      : 강제로 Enable 시키는 역할?
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
		THEAPP.SaveLog("핸들러 Reset");
		break;
		
	case HS_PAUSE:		/////// CMI 3000 to be modified 스탑 베리에이션 확장해라
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
		THEAPP.SaveLog("핸들러 Pause");
		break;
		
	case HS_CYCLE_STOP:		/////// CMI 3000 to be modified 스탑 베리에이션 확장해라
		THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
		THEAPP.SaveLog("핸들러 Cycle Stop");
		break;
		
	default:
		if(m_iHandlerStatus >= HS_STOP)		/////// CMI 3000 to be modified 스탑 베리에이션 확장해라
		{
			THEAPP.m_pInspectSummary->GetDlgItem(IDC_MFCBUTTON_INSPECT_START)->EnableWindow(TRUE);
			THEAPP.SaveLog("핸들러 Stop");
		}
		break;
	}
}
void  CHandlerService::Get_ModeRequest()
{
	Set_ModeReply(m_iEquipmentMode);
}
void  CHandlerService::Get_ModeReply(int iMode)
{
	m_iEquipmentMode = iMode;
}

void  CHandlerService::Get_ModelReply(int iFlag)
{
	m_iModelCheck = iFlag;
}

void  CHandlerService::Get_LotStart(CString sLotID, CString sTrayAmt, CString sModuleAmt)
{
	Set_StatusUpdate(VS_READY);	// Lot Ready 송신

	m_sLotID = sLotID;
	THEAPP.m_pInspectService->m_sLotID_H = m_sLotID;
	THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_LOT_ID, m_sLotID);

	m_iLotTrayAmt = atoi ((LPSTR)(LPCSTR) sTrayAmt);
	m_iLotModuleAmt = atoi ((LPSTR)(LPCSTR) sModuleAmt);
	THEAPP.m_pInspectService->m_iLotTrayAmt_H = m_iLotTrayAmt;
	THEAPP.m_pInspectService->m_iLotModuleAmt_H = m_iLotModuleAmt;

	THEAPP.m_pInspectService->SetCycleStopStatus(TRUE);
	THEAPP.m_pInspectService->ReadyLot();

	THEAPP.m_pInspectService->m_iPrevTrayNo_H = -100;
}

////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
void  CHandlerService::Get_LoadComplete(CString sLotID, CString sTrayNo, CString sLineNo, CString sModelName, CString sIndexNo)
{
	THEAPP.SaveLog("Get_LoadComplete");

	// 24.07.23 SW 다운 확인 - LeeGW 
	try
	{
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);

		DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;  // 사용 중인 물리 메모리
		double physMemUsedMegaByte = (double)physMemUsed / (1024 * 1024);	// 현재 프로세스가 사용중인 메모리
		double physMemUsedPercent =((double)physMemUsed / (double)memInfo.ullTotalPhys) * 100;

		CString strMemLog;
		strMemLog.Format("Memory in Use : %.2fMB (%.2f/100.00)", physMemUsedMegaByte, physMemUsedPercent);
		THEAPP.SaveLog(strMemLog);
	}
	catch(HException &except)
	{
		THEAPP.SaveLog("Memory Check Failure!");
	}

	////////////////// 통신 파라미터 초기화 ///////////////////////////////////
	m_sLotID = sLotID;
	m_iTrayNo = atoi ((LPSTR)(LPCSTR) sTrayNo);
	m_iLineNo = atoi ((LPSTR)(LPCSTR) sLineNo);
	m_iIndexNo = atoi((LPSTR)(LPCSTR) sIndexNo);

	THEAPP.m_pInspectService->m_iTrayNo_H = atoi ((LPSTR)(LPCSTR) sTrayNo);
	THEAPP.m_pInspectService->m_iLineNo_H = atoi ((LPSTR)(LPCSTR) sLineNo);
	THEAPP.m_pInspectService->m_iIndexNo_H = atoi((LPSTR)(LPCSTR) sIndexNo);
	THEAPP.m_pInspectService->m_sLotID_H = m_sLotID;
	THEAPP.m_pInspectService->m_iMboCount_H = 0;

	THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_LOT_ID, m_sLotID);

	THEAPP.m_pInspectService->ReadyToStart();

	THEAPP.m_pInspectService->m_iPrevTrayNo_H = THEAPP.m_pInspectService->m_iTrayNo_H;

	THEAPP.m_pInspectService->SetCycleStopStatus(FALSE);

	if (THEAPP.Struct_PreferenceStruct.m_bUseMboMode == TRUE)
	{
		thmMboMode.Add(thread([=] {
			
			for (int i = 0; i < THEAPP.Struct_PreferenceStruct.m_iMboModeCount; i++)
			{
				THEAPP.m_pInspectService->m_iMboCount_H = i + 1;
				if (THEAPP.m_pInspectService->m_iMboCount_H == THEAPP.Struct_PreferenceStruct.m_iMboModeCount)
					THEAPP.m_pInspectService->m_sLotID_H = m_sLotID;
				else
					THEAPP.m_pInspectService->m_sLotID_H.Format("%s_%d", m_sLotID, i + 1);

				THEAPP.m_pInspectSummary->CheckPrevSaveFolder(THEAPP.m_pInspectService->m_sLotID_H, sTrayNo);
					
				THEAPP.m_pInspectService->InspectionMove(FALSE);				// 스캔과 검사 시작

				if (THEAPP.m_pInspectService->m_iMboCount_H != THEAPP.Struct_PreferenceStruct.m_iMboModeCount)
				{
					while (THEAPP.m_pInspectService->m_bInspectDone != TRUE)
					{
						Sleep(1);
					}

					THEAPP.m_pInspectService->m_bInspectDone = FALSE;
				}

			}
		
		}));
			
	}
	else
		THEAPP.m_pInspectService->InspectionMove(FALSE);				// 스캔과 검사 시작	
}

void CHandlerService::Get_BarcodeData(CString sBarcodeID, CString sLotID, CString sTrayNo, CString sModuleCol, CString sModuleRow)
{
	THEAPP.m_pInspectService->SaveNgBarcodeInfo(sBarcodeID, sLotID, sTrayNo, sModuleCol, sModuleRow);
}
////////////////// <==== changed for CMI3000 2000 ///////////////////////////////////////////////////


void  CHandlerService::Get_ScanReply()
{
	m_bReplyReceived = TRUE;
}

void  CHandlerService::Get_InspectReply()
{
	m_bReplyReceived = TRUE;
}

void  CHandlerService::Get_TriggerReply()
{
	m_bReplyReceived = TRUE;
}

void  CHandlerService::Get_AMoveReply(int iFlag)
{
	if(iFlag==1)
	{
		m_bAMoveComplete = TRUE;
	}
	else
	{
		m_bAMoveComplete = FALSE;
	}
	
}
void  CHandlerService::Get_RMoveReply(int iFlag)
{
	if(iFlag==1)
	{
		m_bRMoveComplete = TRUE;
	}
	else
	{
		m_bRMoveComplete = FALSE;
	}
}
void  CHandlerService::Get_JMoveReply(int iFlag)
{
	if(iFlag==1)
	{
		m_bJMoveComplete = TRUE;
	}
	else
	{
		m_bJMoveComplete = FALSE;
	}
}
void  CHandlerService::Get_MoveComplete(int iVisionType)
{
	m_bMotionMoveComplete = TRUE;

	if (iVisionType==VISION_TYPE_INSPECTION)
	{
		m_bMoveCompleteFlag = TRUE;	////// <==== removed for CMI3000 2000 //////  if(m_nInspectPCStatus==2) 쓰면 Full-Auto 죽음
	}
	else if (iVisionType==VISION_TYPE_BARCODE)
	{
		m_bBarcodeMoveCompleteFlag = TRUE;
	}
}

void  CHandlerService::Get_ShowRequest(int iMode,int iShowHideFlag)
{
	m_iHandlerProgramMode = iMode;
	m_iShowHideFlag = iShowHideFlag;
}
void  CHandlerService::Get_ShowReply()
{

}

void  CHandlerService::Get_FOBRequest() //FOB Mode Change
{
	Set_FOBReply( THEAPP.m_pInspectSummary->m_bDxFobTest );
}

void  CHandlerService::Get_APDRequest(CString sVisionType, CString sLotID, CString sMzNo) //APD
{
	Set_APDReply(sLotID);
}

/////////////////////////////////////////////////////////////////////
// UDP Socket Message
LRESULT CHandlerService::OnUdpReceive(WPARAM pstrIP, LPARAM pstrRecv)
{
	CString strIP = *(CString*)pstrIP;
	CString strRecv_ori = *(CString*)pstrRecv;
	
	if (strIP != UDP_IP) return 0;
	
	// TODO : nLoopMax = 무한루프 방지용? - 251028, jhkim
	int nLoopMax = 3;
	while ((!strRecv_ori.IsEmpty()) && (nLoopMax > 0)) 
	{
		int nStart = strRecv_ori.Find("@");
		int nEnd = strRecv_ori.Find("\n");

		if (nStart > nEnd) return 0;

		CString strRecv = strRecv_ori.Mid(nStart + 1, nEnd - nStart - 1);

		char *pszRecv = (LPTSTR)(LPCTSTR)strRecv;
		char *sep = ",";
		//////////// changed for C<I3000 2000 ====> ////////////////
		CString strCmd, strOp, sComOut = "", sComInit = "@";
		//////////// <==== changed for C<I3000 2000 ////////////////
		strCmd = strtok(pszRecv, sep);
	

		// Handler Log 추가 ///////////////////////////////////
		CString strLog;

		if (strCmd == "CONNECT") {
			strOp = strtok(NULL, sep);
			sComOut = sComInit + strCmd + "," + strOp;
			strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
			if (strOp == "REQUEST")	{Get_ConnectRequest();}
			else if (strOp == "REPLY") {Get_ConnectReply();}
			else if (strOp == "END") {Get_ConnectEnd();}		
		} 
		else if (strCmd == "STATUS") {
			strOp = strtok(NULL, sep);
			sComOut = sComInit + strCmd + "," + strOp;
			strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
			if (strOp == "REQUEST")	{Get_StatusRequest();}
			// Handler 의 상태를 받아서 업데이트.
			else if (strOp == "REPLY") {strOp = strtok(NULL, sep); Get_StatusReply(strOp);}
			else if (strOp == "UPDATE")	{strOp = strtok(NULL, sep); Get_StatusUpdate(strOp);}		
		} 
		else if (strCmd == "MODE") {
			strOp = strtok(NULL, sep);
			sComOut = sComInit + strCmd + "," + strOp;
			strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
			if (strOp == "REQUEST") {Get_ModeRequest();}
		}
		else if (strCmd == "MODEL") {
			strOp = strtok(NULL, sep);
			sComOut = sComInit + strCmd + "," + strOp;
			strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
			if (strOp == "REPLY")	{
				CString sTemp;
				sTemp = strtok(NULL, sep);
				int iFlag = atoi((LPSTR)(LPCSTR)sTemp);
				Get_ModelReply(iFlag);
			}
		}
		else if (strCmd == "LOT") {
			strOp = strtok(NULL, sep);
			if (strOp == "UPDATE")	{
				CString sTemp;
				sTemp = strtok(NULL, sep);
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
			}
			else if (strOp == "START")	{
				CString sTemp1, sTemp2, sTemp3;
				sTemp1 = strtok(NULL, sep);
				sTemp2 = strtok(NULL, sep);
				sTemp3 = strtok(NULL, sep);
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp1 + "," + sTemp2 + "," + sTemp3;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				// sLotID, sTrayAmt, sModuleAmt;
				Get_LotStart(sTemp1, sTemp2, sTemp3);
			}
		}
		////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
		else if (strCmd == "LOAD") {
			strOp = strtok(NULL, sep);
			if (strOp == "COMPLETE") {
				CString sTemp1, sTemp2, sTemp3, sTemp4, sTemp5;
				sTemp1 = strtok(NULL, sep);		// LOT_ID
				sTemp2 = strtok(NULL, sep);		// TrayNo(1-20)
				sTemp3 = strtok(NULL, sep);		// LineNo (1-6)
				sTemp4 = strtok(NULL, sep);		// Model Name
				sTemp5 = strtok(NULL, sep);		// IndexNo(1-8)
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp1 + "," + sTemp2 + "," + sTemp3 + "," + sTemp4 + "," + sTemp5;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				// sLotID, sTrayNo, sLineNo, sModelName, sIndexNo
				Get_LoadComplete(sTemp1, sTemp2, sTemp3, sTemp4, sTemp5);
			}
		}
		else if (strCmd == "BARCODE") {
			strOp = strtok(NULL, sep);
			if (strOp == "DATA") {
				CString sTemp1, sTemp2, sTemp3, sTemp4, sTemp5;
				sTemp1 = strtok(NULL, sep);
				sTemp2 = strtok(NULL, sep);
				sTemp3 = strtok(NULL, sep);
				sTemp4 = strtok(NULL, sep);
				sTemp5 = strtok(NULL, sep);
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp1 + "," + sTemp2 + "," + sTemp3 + "," + sTemp4 + "," + sTemp5;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				// sBarcodeID, sLotID, sTrayNo, sModuleCol, sModuleRow
				Get_BarcodeData(sTemp1, sTemp2, sTemp3, sTemp4, sTemp5);
			}
		}
		////////////////////////////////// <==== changed for CMI3000 2000 ///////////////////////////////////////////////////
		else if (strCmd == "SCAN") {
			strOp = strtok(NULL, sep);
			sComOut = sComInit + strCmd + "," + strOp;
			strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
			if (strOp == "REPLY")	{Get_ScanReply();}
		}
		else if (strCmd == "INSPECT") {
			strOp = strtok(NULL, sep);
			sComOut = sComInit + strCmd + "," + strOp;
			strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
			if (strOp == "REPLY") {Get_InspectReply();}
		}
		else if (strCmd == "AMOVE") {
			strOp = strtok(NULL, sep);
			if (strOp == "REPLY")	{
				CString sTemp1, sTemp2;
				sTemp1 = strtok(NULL, sep);
				sTemp2 = strtok(NULL, sep);
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp1 + "," + sTemp2;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				int AMoveFlag = atoi((LPSTR)(LPCSTR)sTemp1);
				Get_AMoveReply(AMoveFlag);
			}
		}
		else if (strCmd == "RMOVE") {
			strOp = strtok(NULL, sep);
			if (strOp == "REPLY")	{
				CString sTemp1, sTemp2;
				sTemp1 = strtok(NULL, sep);
				sTemp2 = strtok(NULL, sep);
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp1 + "," + sTemp2;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				int RMoveFlag = atoi((LPSTR)(LPCSTR)sTemp1);
				Get_RMoveReply(RMoveFlag);
			}
		}
		else if (strCmd == "JMOVE") {
			strOp = strtok(NULL, sep);
			if (strOp == "REPLY")	{
				CString sTemp = strtok(NULL, sep);
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				int JMoveFlag = atoi((LPSTR)(LPCSTR)sTemp);
				Get_JMoveReply(JMoveFlag);
			}
		}
		else if (strCmd == "MOVE") {
			strOp = strtok(NULL, sep);
			if (strOp == "COMPLETE") {
				CString sTemp = strtok(NULL, sep);
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);

				int iVisionType = -1;
				if (sTemp=="I")
					iVisionType = VISION_TYPE_INSPECTION;
				else if (sTemp=="B")
					iVisionType = VISION_TYPE_BARCODE;
				Get_MoveComplete(iVisionType);
			}
		}
		else if (strCmd == "TRIGGER") {
			strOp = strtok(NULL, sep);
			sComOut = sComInit + strCmd + "," + strOp;
			strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
			if (strOp == "REPLY") {Get_TriggerReply();}
		}
		else if (strCmd == "SHOW") {
			strOp = strtok(NULL, sep);
			if (strOp == "REQUEST") {
				CString sTemp1, sTemp2;
				sTemp1 = strtok(NULL, sep);
				sTemp2 = strtok(NULL, sep);
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp1 + "," + sTemp2;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				int iShowMode = atoi((LPSTR)(LPCSTR)sTemp1);
				int iShowHideFlag = atoi((LPSTR)(LPCSTR)sTemp2);
				Get_ShowRequest(iShowMode,iShowHideFlag);
			}
			if (strOp == "REPLY") {
				sComOut = sComInit + strCmd + "," + strOp;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				Get_ShowReply();
			}
		}
		else if (strCmd == "FOB") { //FOB Mode Change
			strOp = strtok(NULL, sep);
			if (strOp == "REQUEST") {
				sComOut = sComInit + strCmd + "," + strOp;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				Get_FOBRequest();
			}
		}
		else if (strCmd == "APD") {
			strOp = strtok(NULL, sep);
			if (strOp == "REQUEST")	{
				CString sTemp1, sTemp2, sTemp3;
				sTemp1 = strtok(NULL, sep);
				sTemp2 = strtok(NULL, sep);
				sTemp3 = strtok(NULL, sep);
				sComOut = sComInit + strCmd + "," + strOp + "," + sTemp1 + "," + sTemp2 + "," + sTemp3;
				strLog.Format("[Handler(V<-H)] : %s", sComOut);	THEAPP.SaveLog(strLog);
				Get_APDRequest(sTemp1, sTemp2, sTemp3); //APD
			}
		}

	strRecv_ori.Delete(0, nEnd + 1);
	nLoopMax--;
	}

	return 1;
}

/////////////////////////////////////////////////////////////////////////////

void CHandlerService::Set_ConnectRequest()
{
	CString	strSendMsg;
	strSendMsg = "@CONNECT,REQUEST\n";
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ConnectReply()
{
	CString	strSendMsg;
	strSendMsg = "@CONNECT,REPLY\n";
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ConnectEnd()
{
	CString	strSendMsg;
	strSendMsg = "@CONNECT,END\n";
	Send_Command(strSendMsg);
}

void CHandlerService::Set_StatusRequest()
{
	CString	strSendMsg;
	strSendMsg = "@STATUS,REQUEST\n";
	Send_Command(strSendMsg);
}

void CHandlerService::Set_StatusReply()
{
	CString	strSendMsg;
	THEAPP.SaveLog("Status Reply");
	strSendMsg.Format("@STATUS,REPLY,%d\n", m_nInspectPCStatus);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_StatusUpdate(int nStatus)
{
	CString	strSendMsg;
	m_nInspectPCStatus = nStatus;
	strSendMsg.Format("@STATUS,UPDATE,%d\n", nStatus);
	Send_Command(strSendMsg);
}
void CHandlerService::Set_ModeReply(int iMode)//Type(0:None, 1:Inspect, 2:Teach, 3:Demo, 4:Test, 5:Debug, 6:Step, 7:Home, 8:Manual)
{
	CString	strSendMsg;
	strSendMsg.Format("@MODE,REPLY,%d\n",iMode);
	Send_Command(strSendMsg);
}
void CHandlerService::Set_ModeUpdate(int iMode)//Type(0:None, 1:Inspect, 2:Teach, 3:Demo, 4:Test, 5:Debug, 6:Step, 7:Home, 8:Manual)
{
	CString	strSendMsg;
	strSendMsg.Format("@MODE,UPDATE,%d\n",iMode);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ModelCreate(CString sModelname)
{
	CString	strSendMsg;
	strSendMsg.Format("@MODEL,CREATE,%s\n",sModelname);
	Send_Command(strSendMsg);
}
void CHandlerService::Set_ModelUpdate(CString sModelname)
{
	CString	strSendMsg;
	strSendMsg.Format("@MODEL,UPDATE,%s\n",sModelname);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_LotReply()
{
	CString	strSendMsg;
	strSendMsg.Format("@LOT,REPLY\n");
	Send_Command(strSendMsg);
}

void CHandlerService::Set_LoadReply()
{
	CString	strSendMsg;
	strSendMsg.Format("@LOAD,REPLY\n");
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ScanComplete (int iLineNo, int iVisionType)	//검사영역을 모두 Scan 후 Complete 신호를 보낸다.
{
	CString	strSendMsg;
	if (iVisionType==VISION_TYPE_INSPECTION)
		strSendMsg.Format("@SCAN,COMPLETE,%s,%d,%d,I\n", m_sLotID, m_iTrayNo, iLineNo);
	else if (iVisionType==VISION_TYPE_BARCODE)
		strSendMsg.Format("@SCAN,COMPLETE,%s,%d,%d,B\n", m_sLotID, m_iTrayNo, iLineNo);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_InspectComplete(CString sLotID, int iTrayNo, int iLineNo, int iTrayModuleMax, CString sTrayResult, CString sBarcodeResult)//Inspection 완료 후 제어에 보낸다.
{
	CString	strSendMsg;
	strSendMsg.Format("@INSPECT,COMPLETE,%s,%d,%d,%d,%s,%s\n", sLotID, iTrayNo, iLineNo, iTrayModuleMax, sTrayResult, sBarcodeResult);	// New Type
	Send_Command(strSendMsg);

	m_sInspectCompleteMsg = strSendMsg;
}

void CHandlerService::Set_InspectComplete(CString sLotID, int iTrayNo, int iLineNo, int iTrayModuleMax, CString sTrayResult, CString sBarcodeResult, CString sNGCode, CString sBadName,
	CString s_dBlackCoatingOuterDiameter, CString s_dBlackCoatingPosX, CString s_dBlackCoatingPosY, CString s_dO1O2Distance, CString s_dDiameterMin)//Inspection 완료 후 제어에 보낸다.
{
	CString	strSendMsg;
	strSendMsg.Format("@INSPECT,COMPLETE,%s,%d,%d,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", sLotID, iTrayNo, iLineNo, iTrayModuleMax, sTrayResult, sBarcodeResult, sNGCode, sBadName,
		s_dBlackCoatingOuterDiameter, s_dBlackCoatingPosX, s_dBlackCoatingPosY, s_dO1O2Distance, s_dDiameterMin);	// New Type
	Send_Command(strSendMsg);

	m_sInspectCompleteMsg = strSendMsg;
}

void CHandlerService::Set_InspectComplete()//Inspection 완료 후 제어에 보낸다.
{
	if(m_sInspectCompleteMsg != "")
		Send_Command(m_sInspectCompleteMsg);
}

// NOTE : Somthing about Load, Unload prcoess? -251028, jhkim
void CHandlerService::Set_PositionRequest(int iVisionType)	//현재 모션 위치를 요구한다.
{
	CString	strSendMsg;
	if (iVisionType==VISION_TYPE_INSPECTION)
		strSendMsg.Format("@POSITION,REQUEST,I\n");
	else if (iVisionType==VISION_TYPE_BARCODE)
		strSendMsg.Format("@POSITION,REQUEST,B\n");
	
	Send_Command(strSendMsg);
}

////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////

void CHandlerService::Set_AMoveRequest(double dXposition, double dYposition, int iStageNo, int iVisionType)	//절대이동 구동 명령을 보낸다. (""이면 움직이지 않는다)
{
	CString	strSendMsg;

	if (iVisionType==VISION_TYPE_INSPECTION)
		strSendMsg.Format("@AMOVE,REQUEST,%.3lf,%.3lf,%d,I\n", dXposition, dYposition, 0);
	else if (iVisionType==VISION_TYPE_BARCODE)
		strSendMsg.Format("@AMOVE,REQUEST,%.3lf,%.3lf,%d,B\n", dXposition, dYposition, 0);

	Send_Command(strSendMsg);
}

void CHandlerService::Set_RMoveRequest(double dXposition, double dYposition, int iStageNo, int iVisionType)//상대이동 구동 명령을 보낸다. (""이면 움직이지 않는다)
{
	CString	strSendMsg;
	if(dXposition>=0) {
		if (iVisionType==VISION_TYPE_INSPECTION)
			strSendMsg.Format("@RMOVE,REQUEST,%.3lf,%.3lf,%3d,I\n", dXposition, dYposition, 0);
		else if (iVisionType==VISION_TYPE_BARCODE)
			strSendMsg.Format("@RMOVE,REQUEST,%.3lf,%.3lf,%3d,B\n", dXposition, dYposition, 0);
		Send_Command(strSendMsg);
	}
}

void CHandlerService::Set_JMoveRequest(int XDirection, int YDirection, int iStageNo)//조그이동 (1 : +이동, -1 : -이동, 0: 정지)(""이면 움직이지 않는다)
{
	CString	strSendMsg;
	strSendMsg.Format("@JMOVE,REQUEST,%d,%d,%d\n", XDirection, YDirection, iStageNo);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ZMoveRequest(double dZposition)	// Z축 절대이동 구동 명령을 보낸다.
{
	CString	strSendMsg;

	strSendMsg.Format("@ZMOVE,REQUEST,%.3lf\n", dZposition);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_BarcodeReply()
{
	CString	strSendMsg;
	strSendMsg.Format("@BARCODE,REPLY\n");
	Send_Command(strSendMsg);
}
////////////////////////////////// <==== changed for CMI3000 2000 ///////////////////////////////////////////////////

void CHandlerService::Set_MoveReply()
{
	CString	strSendMsg;
	strSendMsg.Format("@MOVE,REPLY\n");
	Send_Command(strSendMsg);
}

void CHandlerService::Set_ShowRequest(int iMode,int iShowHideFlag) //Type(0:HomeDlg, 1:ManualOPDlg, 2:MonitorDlg, 3:SetupDlg)
{
	CString	strSendMsg;
	strSendMsg.Format("@SHOW,REQUEST,%d,%d\n",iMode,iShowHideFlag);
	Send_Command(strSendMsg);
}
void CHandlerService::Set_ShowReply()
{
	CString	strSendMsg;
	strSendMsg.Format("@SHOW,REPLY\n");
	Send_Command(strSendMsg);
}

void CHandlerService::Set_TriggerRequest(int iSurface_P1, int iEdge_P1, int iLens1_P1, int iLens2_P1, int iSurface_P2, int iEdge_P2, int iLens1_P2, int iLens2_P2)
{
	CString	strSendMsg;
	strSendMsg.Format("@TRIGGER,REQUEST,I,%d,%d,%d,%d,%d,%d,%d,%d\n", iSurface_P1, iEdge_P1, iLens1_P1, iLens2_P1, iSurface_P2, iEdge_P2, iLens1_P2, iLens2_P2);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_BarcodeTriggerRequest()
{
	CString	strSendMsg;
	strSendMsg.Format("@TRIGGER,REQUEST,B,1,0,0,0,0,0,0,0\n");
	Send_Command(strSendMsg);
}

void CHandlerService::Set_LightUpdate(int Top_Dom, int Top_Ring, int Mid_55, int MID_65, int MID_75,int Spare) //Type(0:Top_Dom, 1:Top_Ring, 2:Mid_55, 3:MID_65, 4:MID_75,Spare) 1: On, 0:Off
{
	CString	strSendMsg;
	strSendMsg.Format("@LIGHT,UPDATE,%d,%d,%d,%d,%d,%d\n", Top_Dom,  Top_Ring,  Mid_55,  MID_65,  MID_75, Spare);
	Send_Command(strSendMsg);
}
void CHandlerService::Set_LightRequest() 
{
	CString	strSendMsg;
	strSendMsg.Format("@LIGHT,REQUEST\n");
	Send_Command(strSendMsg);
}
void CHandlerService::Set_MoveToLoad()
{
	CString	strSendMsg;
	strSendMsg.Format("@MOVETO,REQUEST,%d\n", 1);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_MoveToZero()
{
	CString	strSendMsg;
	strSendMsg.Format("@MOVETO,REQUEST,%d\n", 0);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_MoveToUnload()
{
	CString	strSendMsg;
	strSendMsg.Format("@MOVETO,REQUEST,%d\n", 2);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_FOBUpdate(int nFOBStatus) //FOB Mode Change
{
	CString	strSendMsg;
	strSendMsg.Format("@FOB,UPDATE,%d\n", nFOBStatus);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_FOBReply(int nFOBStatus) //FOB Mode Change
{
	CString	strSendMsg;
	strSendMsg.Format("@FOB,REPLY,%d\n", nFOBStatus);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_APDReply(CString sLotID) //APD
{
	CString strSendMsg;

	//APD
	double dEachImageAvgGV[MAX_IMAGE_TAB];
	int n = THEAPP.m_nTotalModuleCnt;
	int tt = THEAPP.m_dEachImgAvgGV[0];

	for(int nImgNo = 0; nImgNo < MAX_IMAGE_TAB; nImgNo++ )
	{
		if (nImgNo >= THEAPP.m_iMaxInspImageNo)
			break;

		dEachImageAvgGV[nImgNo]= THEAPP.m_dEachImgAvgGV[nImgNo] / THEAPP.m_nTotalModuleCnt;
	}

	CString strApdResult,strTemp;
	strApdResult.Format("%d", MAX_IMAGE_TAB );

	for(int nImgNo = 0; nImgNo < MAX_IMAGE_TAB; nImgNo++ )
	{
		if (nImgNo >= THEAPP.m_iMaxInspImageNo)
			break;

		strTemp.Format(",%.2lf",dEachImageAvgGV[nImgNo]);
		strApdResult = strApdResult + strTemp;
	}

	THEAPP.ResetEachImgAvgGV();

	strSendMsg.Format("@APD,REPLY,I,%s,0,%s\n",sLotID , strApdResult);
	Send_Command(strSendMsg);
	//APD END
}

/////////////////////////////////////////////////////////////////////
// UDP Socket Send Message

void CHandlerService::Send_Command(CString strSend)
{
	EnterCriticalSection(&SendCommandCriticalSection);

#ifdef HANDLER_USE
	m_UdpHandler.Write_String(strSend);
#endif

	// Handler Log 추가 ///////////////////////////////////
	CString strLog;
	strSend.TrimRight(_T("\n"));
	strLog.Format("[Handler(V->H)] : %s", strSend);
	THEAPP.SaveLog(strLog);
	///////////////////////////////////////////////////////

	LeaveCriticalSection(&SendCommandCriticalSection);
}
/////////////////////////////////////////////////////////////////////


#include <winnetwk.h>
#pragma comment(lib, "Mpr")

BOOL CHandlerService::Connect_NetworkDrive(CString strDirve, CString strPath)
{
	TCHAR szRemoteName[128];
	DWORD dwLen = sizeof(szRemoteName); 

	DWORD dwReturn = WNetGetConnection(strDirve, szRemoteName, &dwLen);
	if (dwReturn == NO_ERROR) return TRUE;	// Already Connected

	NETRESOURCE NetR;
	NetR.dwType = RESOURCETYPE_DISK;				// 공유 디스크
	NetR.lpLocalName = (LPSTR)(LPCSTR)strDirve;		// 로컬 드라이브
	NetR.lpRemoteName = (LPSTR)(LPCSTR)strPath;	// 경로
	NetR.lpProvider = NULL;

	DWORD dwFlag = CONNECT_REDIRECT;	// CONNECT_UPDATE_PROFILE;
	dwReturn = WNetAddConnection2(&NetR, "mirero", "Administrator", dwFlag);
	if (dwReturn == NO_ERROR) return TRUE;	// Success Connect
	else return FALSE;						// Fail Connect
}

//Multiple Defect
void CHandlerService::Set_AlarmRequest(CString sVisionType, CString sLotID, /*int iMzNo,*/ int iAlarmNo, CString sDefectName, int iDefectCount)	
{
	CString	strSendMsg;

	strSendMsg.Format("@ALARM,REQUEST,%s,%s,%d,%s,%d\n", sVisionType, sLotID,/* iMzNo,*/ iAlarmNo, sDefectName, iDefectCount);
	Send_Command(strSendMsg);
}

void CHandlerService::Set_BarcodeAlarmRequest(CString sVisionType, CString sLotID, /*int iMzNo,*/ int iAlarmNo, double dOffsetX, double dOffsetY, double dOffsetDegree) //Barcode Rotation Inspection (Barcode Shift NG)
{
	CString	strSendMsg;

	strSendMsg.Format("@ALARM,REQUEST,%s,%s,%d,%.2lf,%.2lf,%.2lf\n", sVisionType, sLotID,/* iMzNo,*/ iAlarmNo, dOffsetX, dOffsetY,dOffsetDegree);
	Send_Command(strSendMsg);
}