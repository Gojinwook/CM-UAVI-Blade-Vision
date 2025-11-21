#pragma once

#include "Algorithm.h"

constexpr double PERCENTAGE_FACTOR = 0.01;

typedef struct _RAW_IMAGE_SAVE_PARAM
{
	CString sSavePath;
	Hobject HSaveImage;
} RAW_IMAGE_SAVE_PARAM;

class CInspectService
{
public:
	CInspectService(void);
	~CInspectService(void);

	static CInspectService *m_pInstance;

public:
	static CInspectService *GetInstance();
	void DeleteInstance();
	void ReadyLot();
	void ReadyToStart(BOOL bErrorStatus = FALSE);
	void QuitInspect();
	void InspectionMove(BOOL bRestart);
	void SaveNgBarcodeInfo(CString sBarcodeID, CString sLotID, CString sTrayNo, CString sModuleCol, CString sModuleRow);

	void OfflineInspection();

	CString m_sLotID_H;
	int m_iLotTrayAmt_H;   // for Handler data, added for CMI 3000 2000
	int m_iLotModuleAmt_H; // for Handler data, added for CMI 3000 2000
	int m_iPrevTrayNo_H;
	int m_iTrayNo_H;	   // for Handler data, added for CMI 3000 2000
	int m_iLineNo_H;	   // for Handler data, added for CMI 3000 2000
	int m_iIndexNo_H;	   // LeeGW
	int m_iMboCount_H;	   // LeeGW
	BOOL m_bInspectDone;   // added for CMI 3000 2000
	BOOL m_bInspectRepeat; // added for CMI 3000 2000
	BOOL m_bInspectRun;	   // added for CMI 3000 2000

	BOOL m_bLineInspectDone; // Offline

	int m_iFobTestG;
	int m_iFobTestN;
	int m_iFobTestE;

	Algorithm *m_pInspectAlgorithm;

	void LotDetailDefectError(int iVirtualMzNoIdx, CString sLotID); // Multiple Defect

	BOOL GetCycleStopStatus() { return m_bCycleStopSignaled; }				  // LOT 시작 명령 수신 시 핸들러 처리를 대기하는 부분 모두 Clear 하도록 코드 추가
	void SetCycleStopStatus(BOOL bStatus) { m_bCycleStopSignaled = bStatus; } // LOT 시작 명령 수신 시 핸들러 처리를 대기하는 부분 모두 Clear 하도록 코드 추가

protected:
	deque<RAW_IMAGE_SAVE_PARAM *> m_listRawImageSaveParam; // 24.07.11 - v2654 - RawImage 저장 Thread 검사 Thread에서 분리 - LeeGW

	BOOL m_bCycleStopSignaled; // LOT 시작 명령 수신 시 핸들러 처리를 대기하는 부분 모두 Clear 하도록 코드 추가
};

// Multiple Defect
typedef struct _LOT_DEFECT_ALARM_THREAD_PARAM
{
	_LOT_DEFECT_ALARM_THREAD_PARAM(int iMzIdx, CString sLotID, int iLotModuleAmt, CInspectService *ptr)
	{
		this->iMzIdx = iMzIdx;
		pInspectService = ptr;
		this->sLotID = sLotID;
		this->iLotModuleAmt = iLotModuleAmt;
	}
	CInspectService *pInspectService;
	int iMzIdx;
	int iLotModuleAmt;
	CString sLotID;

} LOT_DEFECT_ALARM_THREAD_PARAM;