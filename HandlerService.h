// HandlerService.h : header file
//
#pragma once

#include "UdpSocketCS.h"
#include "ThreadManager.hpp"

class CHandlerService : public CWnd
{
private:
	static CHandlerService *m_pInstance;
	CRect m_ScreenRect;
	
	CRect GetPosition() { return m_ScreenRect; }
	void SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

public:
	static CHandlerService *GetInstance();
	void DeleteInstance();
	BOOL Connect_NetworkDrive(CString strDirve, CString strPath);
	
// Construction
public:
	CHandlerService();
	virtual ~CHandlerService();

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHandlerService)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CHandlerService)
	afx_msg LRESULT OnUdpReceive(WPARAM pstrIP, LPARAM pstrRecv);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	mutex mtx;

	// UDP Socket 
	CUdpSocketCS m_UdpHandler;
	BOOL m_bUdpOpened;

	BOOL m_bConnected;	   // 제어PC와 연결상태
	BOOL m_bReplyReceived; // 핸들러 응답 체크

	BOOL m_bScanDone;
	BOOL m_bMoveDone;
	BOOL m_bPosRead;

	CString m_sLotID;
	CString m_sModelID;
	int m_iTrayNo;		 // added for CMI 3000 2000
	int m_iLineNo;		 // added for New Type
	int m_iIndexNo;		 // added for New Type
	int m_iLotTrayAmt;	 // added for CMI 3000 2000
	int m_iLotModuleAmt; // added for CMI 3000 2000

	double m_dCurrentXpos;
	double m_dCurrentYpos;

	int m_iHandlerStatus;
	int m_iEquipmentMode;
	int m_iModelCheck;
	double m_dXposition,m_dYposition;
	BOOL m_bAMoveComplete,m_bRMoveComplete,m_bJMoveComplete;
	
	int m_iHandlerProgramMode;
	int m_iShowHideFlag;
	
	CString m_sInspectCompleteMsg;

	CThreadManager thmMboMode;

	void Get_ConnectRequest();	//프로그램 실행 시 연결요청 한다.
	void Get_ConnectReply();
	void Get_ConnectEnd();		//프로그램 종료 시 보낸다. (응답 필요없음)

	void Get_StatusRequest();	//상태정보를 요청한다.
	void Get_StatusReply(CString sStatus);	//Status(0:No Ready, 1:Ready, 2:Run, 3:Reset, 4~6:Stop, 7:Estop, 8:Warning, 9:FMError)
	void Get_StatusUpdate(CString sStatus);	//Status(0:No Ready, 1:Ready, 2:Run, 3:Reset, 4~6:Stop, 7:Estop, 8:Warning, 9:FMError)

	void Get_ModeRequest();
	void Get_ModeReply(int iMode);

	void Get_ModelReply(int iFlag);

	void Get_LotStart (CString sLotID, CString sTrCnt, CString sNoduleCnt);

	////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
	void Get_LoadComplete (CString sLotID, CString sTrayNo, CString sLineNo, CString sModelName, CString sIndexNo);
	void Get_BarcodeData (CString sBarcodeID, CString sLotID, CString sTrayNo, CString sModuleCol, CString sModuleRow);
	////////////////// <==== changed for CMI3000 2000 ///////////////////////////////////////////////////

	void Get_ScanReply();
	void Get_InspectReply();
	void Get_PositionReply(double dXposition, double dZposition, int iVisionType);
	void Get_TriggerReply();

	void Get_AMoveReply(int iFlag);
	void Get_RMoveReply(int iFlag);
	void Get_JMoveReply(int iFlag);
	void Get_MoveComplete(int iVisionType);
	
	void Get_ShowRequest(int iMode,int iShowHideFlag);
	void Get_ShowReply();

	void Get_FOBRequest(); //FOB Mode Change add
	void Get_APDRequest(CString sVisionType, CString sLotID, CString sMzNo); //APD

	void Send_Command(CString strSend);
	
public:
	BOOL Initialize_Handler();
	void Terminate_Handler();
	BOOL m_bMotionMoveComplete;

	void ResetReplyReceived() { m_bReplyReceived = FALSE; }
	BOOL Is_ReplyReceived() { return m_bReplyReceived; }
	BOOL Is_Connected() { return m_bConnected; }
	BOOL Is_ScanDone() { return m_bScanDone; }
	BOOL Is_MoveDone() { return m_bMoveDone; }
	BOOL Is_PosRead() {	return m_bPosRead; }
	
	BOOL m_bMoveCompleteFlag;
	BOOL m_bBarcodeMoveCompleteFlag;
	int m_nInspectPCStatus;	// 검사PC 상태 (0:Not Ready, 1:Ready)

	void Set_ConnectRequest();
	void Set_ConnectReply();
	void Set_ConnectEnd();

	void Set_StatusRequest();
	void Set_StatusReply();
	void Set_StatusUpdate(int nStatus);
	
	void Set_ModeReply(int iMode);//Type(0:None, 1:Inspect, 2:Teach, 3:Demo, 4:Test, 5:Debug, 6:Step, 7:Home, 8:Manual)
	void Set_ModeUpdate(int iMode);//Type(0:None, 1:Inspect, 2:Teach, 3:Demo, 4:Test, 5:Debug, 6:Step, 7:Home, 8:Manual)

	void Set_ModelCreate(CString sModelname);
	void Set_ModelUpdate(CString sModelname);

	void Set_LotReply();

	void Set_LoadReply();

	void Set_ScanComplete (int iLineNo, int iVisionType=VISION_TYPE_INSPECTION); //검사영역을 모두 Scan 후 Complete 신호를 보낸다. (NewType)
	void Set_InspectComplete();
	void Set_InspectComplete(CString sLotID, int iTrayNo, int iLineNo, int iTrayModuleMax, CString sTrayResult, CString sBarcodeResult); //Inspection 완료 후 제어에 보낸다.
	void Set_InspectComplete(CString sLotID, int iTrayNo, int iLineNo, int iTrayModuleMax, CString sTrayResult, CString sBarcodeResult, CString sNGCode, CString sBadName,
		CString s_dBlackCoatingOuterDiameter, CString s_dBlackCoatingPosX, CString s_dBlackCoatingPosY, CString s_dO1O2Distance, CString s_dDiameterMin); //Inspection 완료 후 제어에 보낸다.
	void Set_PositionRequest(int iVisionType=VISION_TYPE_INSPECTION);//현재 모션 위치를 요구한다.

	////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
//	void Set_AMoveRequest(double dXposition, double dYposition, int iStageNo);	// 절대이동 구동 명령을 보낸다. (""이면 움직이지 않는다)
	void Set_AMoveRequest(double dXposition, double dYposition, int iStageNo, int iVisionType=VISION_TYPE_INSPECTION);	//절대이동 구동 명령을 보낸다. (""이면 움직이지 않는다)
//	void Set_RMoveRequest(double dXposition, double dYposition, int iStageNo);	// 상대이동 구동 명령을 보낸다. (""이면 움직이지 않는다)
	void Set_RMoveRequest(double dXposition, double dYposition, int iStageNo, int iVisionType=VISION_TYPE_INSPECTION);	//상대이동 구동 명령을 보낸다. (""이면 움직이지 않는다)
	void Set_JMoveRequest(int XDirection, int YDirection, int iStageNo);		// 조그이동 (1 : +이동, -1 : -이동, 0: 정지)(""이면 움직이지 않는다)
	void Set_ZMoveRequest(double dZposition);	// Z축 절대이동 구동 명령을 보낸다.

	void CHandlerService::Set_BarcodeReply();
	////////////////////////////////// <==== changed for CMI3000 2000 ///////////////////////////////////////////////////

	void Set_MoveReply();

	void Set_ShowRequest(int iMode,int iShowHideFlag); //Type(0:HomeDlg, 1:ManualOPDlg, 2:MonitorDlg, 3:SetupDlg)
	void Set_ShowReply();

	void Set_TriggerRequest(int iSurface_P1, int iEdge_P1, int iLens1_P1, int iLens2_P1, int iSurface_P2=0, int iEdge_P2=0, int iLens1_P2=0, int iLens2_P2=0);
	void Set_BarcodeTriggerRequest();

	void Set_LightUpdate(int Top_Dom, int Top_Ring, int Mid_55, int MID_65, int MID_75,int Spare); //Type(0:Top_Dom, 1:Top_Ring, 2:Mid_55, 3:MID_65, 4:MID_75,Spare) 1: On, 0:Off
	void Set_LightRequest(); 
	void Set_MoveToLoad();
	void Set_MoveToZero();
	void Set_MoveToUnload();

	void Set_FOBUpdate( int nFOBStatus ); //FOB Mode Change
	void Set_FOBReply( int nFOBStatus ); //FOB Mode Change

	void Set_APDReply(CString sLotID); //APD

	CString GetLotId() { return m_sLotID; }

	void SetLotId(CString strLotID) { m_sLotID = strLotID; }
	void SetLineNumber(int iLineNo) { m_iLineNo = iLineNo; } 
	void SetTrayNumber(int iTrayNo) { m_iTrayNo = iTrayNo; } 
	
	double GetCurentPosX() { return m_dCurrentXpos; }
	double GetCurentPosY() { return m_dCurrentYpos; }

	void Set_AlarmRequest(CString sVisionType, CString sLotID, /*int iMzNo,*/ int iAlarmNo, CString sDefectName, int iDefectCount); //Multiple Defect
	void Set_BarcodeAlarmRequest(CString sVisionType, CString sLotID, /*int iMzNo,*/ int iAlarmNo, double dOffsetX, double dOffsetY, double dOffsetDegree); //Barcode Rotation Inspection (Barcode Shift NG)

	CString Get_InpectCompleteMsg() { return m_sInspectCompleteMsg; };	//프로그램 실행 시 연결요청 한다.
};

/////////////////////////////////////////////////////////////////////////////
