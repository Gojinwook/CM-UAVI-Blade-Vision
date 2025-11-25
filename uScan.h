// uScan.h : main header file for the uScan application
//

#if !defined(AFX_uScan_H__7AC743F0_B386_4D5B_9E6B_09F72F50F422__INCLUDED_)
#define AFX_uScan_H__7AC743F0_B386_4D5B_9E6B_09F72F50F422__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "InspectAdminViewDlg.h"
#include "InspectAdminViewHideDlg.h"
#include "InspectSummary.h"
#include "InspectResultDlg.h"
#include "CalDataService.h"
#include "DefectListDlg.h"
#include "TrayAdminViewDlg.h"
#include "TrayBeamProjectViewDlg.h"
#include "TrayOperatorViewNowDlg.h"
#include "TrayOperatorViewPrevDlg.h"
#include "LogDlg.h"
#include "InspectViewBarrelSurfaceDlg.h"
#include "InspectViewBarrelEdgeDlg.h"
#include "InspectViewLensInnerDlg.h"
#include "InspectViewLensOuterDlg.h"
#include "InspectViewExtra1Dlg.h"
#include "InspectViewExtra2Dlg.h"
#include "InspectViewOverayImageDlg.h"
#include "TabControlDlg.h"

#include "FileBase.h"
#include "ModelDataManager.h"

#include "Algorithm.h"

#include "HandlerService.h"
#include "CameraManager.h"
#include "TriggerManager.h"

#include "InspectService.h"
#include "AutoCalService.h"	
#include "MIInterface.h"
#include "IniFileCS.h"

#include "zip.h"

#include "SaveManager.h"	//LeeGW


// CuScanApp:
// See uScan.cpp for the implementation of this class
//
#include <afxadv.h>

class CRecentFileList;

// LeeGW START
static CRITICAL_SECTION CS_DSF_LOG;	
static CRITICAL_SECTION CS_LOT_RESULT_LOG;	
static CRITICAL_SECTION CS_LOT_SUMMARY_LOG;
// LeeGW END

// 바코드 특수 상태 값 - 251001, jhkim
constexpr const TCHAR* BARCODE_STATUS_DEFAULT = _T("NOREAD");
constexpr const TCHAR* BARCODE_STATUS_NOGRAB = _T("NOGRAB");

// 2024.09.24 - v2660 - LAS ZIP 추가 - LeeGW
typedef struct _stBarcodeVirtualMagazine
{
	CString m_sBarcode[BARCODE_MAX_NUM];
	int m_nCurMagazineNo;
	//Initial시 전체 랏 ID 빈공간으로 초기화 및 인덱스 0
	_stBarcodeVirtualMagazine()
	{
		for (int i = 0; i < BARCODE_MAX_NUM; i++)
		{
			m_sBarcode[i] = "";
		}
		m_nCurMagazineNo = 0;
	}
	int GetVirtualMagazineNo(CString sBarcode)
	{
		//동일한 Lot ID가 있는 경우  return 인덱스
		for (int i = 0; i < BARCODE_MAX_NUM; i++) //현재 저장된 Lot ID와 동일한 Lot ID가 들어올 경우
		{
			if (this->m_sBarcode[i] == sBarcode)
				return i;
		}
		//동일한 Lot ID가 없을 경우 초기화 된 값과 같으면 return 인덱스 및 Lot ID 인덱스에 저장
		for (int i = 0; i < BARCODE_MAX_NUM; i++) //초기 시작해서 저장된 Lot ID가 없는 경우
		{
			if (this->m_sBarcode[i] == "")
			{
				this->m_sBarcode[i] = sBarcode;
				m_nCurMagazineNo = i;
				return i;
			}
		}
		//(위의 두가지 경우를 제외하면)현재 저장된 Lot ID와 다른 Lot ID가 들어올 경우
		m_nCurMagazineNo = (m_nCurMagazineNo + 1) % BARCODE_MAX_NUM;
		this->m_sBarcode[m_nCurMagazineNo] = sBarcode;
		return m_nCurMagazineNo;
	}
}stBarcodeVirtualMagazine;
// 2024.09.24 - v2660 - LAS ZIP 추가 - LeeGW

// 불량 파라미터 정보 저장
typedef struct _stDetectParamInfoStruct
{
	int iImageIdx;
	int iTabIdx;
	BOOL bBarrelLens;
	CAlgorithmParam AlgorithmParam;
	Hobject HRegion;
} DetectParamInfoStruct;

class CuScanApp : public CWinApp
{
public:
	CuScanApp();

	BOOL IsRun();
	void DoEvents();
	void SetInspectionReady();
	void SetExitProgram();

	enum eAppState { eNull = 0, eInspect, eTeaching, eReview, eSPC, eManualOP };

	CString GetWorkingDirectory() { return m_szWorkingDir; }
	CString GetWorkingEvmsDirectory() { return m_szWorkingEvmsDir; } //Ver2629
	int GetVersion() { return m_iAppVersion; }
	CString GetOperator() { return m_sOperator; }
	CString GetLoginMode() { return m_sLoginMode; }


	CString GetLASDataDirectory() { return m_szLASDataFolder; }
	CString GetLASImageDirectory() { return m_szLASImageFolder; }
	CString GetLASTempDirectory() { return m_szLASTempFolder; }
	CString GetLASSectionDirectory() { return m_szLASSectionFolder; }
	CString GetLASEventDirectory() { return m_szLASEventFolder; }

	CString GetPCID() { return m_szPCID; }

	void	SaveLog ( CString logMsg );
	void	DoubleLogOut(const char *format, ...);
	void	SaveDetectLog (CString logMsg);
	void	SaveLotSummaryLog(BOOL bSaveLas, CString sStartTime, CString sEndTime, CString sLotID, double dInspectTime,
							int iTotalInsp, int iTotalOK, double dOKRatio, int iTotalNG, double dNGRatio,
							int iTotalBarrelDefect, int iTotalLensDefect, int iTotalBarrelLensDefect, int iTotalBarrelDirt, int iTotalBarrelScratch, int iTotalBarrelStain,
							int iTotalBarrelDent, int iTotalBarrelChipping, int iTotalBarrelEpoxy, int iTotalBarrelOutsideCT, int iTotalBarrelPeelOff, int iTotalBarrelWhiteDot,
							int iTotalLensContamination, int iTotalLensScratch, int iTotalLensDirt, int iTotalLensWhiteDot, int iTotalEpoxyHole, int iTotalSpringNG,
							int iTotalFAING, int iTotalBlackCoatingNG, int iTotalModuleMix, int iTotalBarcodeError, int iTotalBarcodeShift, int iTotalMatchingError, int iTotalLightError);
	void	SaveLotResultLog(BOOL bSaveLas, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode, double dInspectTime, CString strResult,
						CString strBarrelOKNG, CString strLensOKNG, CString sBLInspType, CString sBLDefectType, double dBLDefectArea, int iBLCenterX, int iBLCenterY,
						CString strExtraResult,  CString sExInspType, CString sExDefectType, double dExDefectArea, int iExCenterX, int iExCenterY,
						CString strFAIResult, CString sBarcodeShiftResult, CString sBarcodeRotationResult, double dBarcodePosOffsetX, double dBarcodePosOffsetY, double dBarcodePosOffsetRotation,
						CString sBlackCoatingResult, double dBlackCoatingDiameter, double dBlackCoatingPosX, double dBlackCoatingPosY, double dO1O2Distance, double dDiameterMin,
						CString sModuleMixResult, CString sBarcodeErrorResult, CString sMatchingErrorResult, CString sLightErrorResult, int iDefectCode);
	void SaveDefectFeatureLog(BOOL bSaveLas, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode, int iImageIdx, int iTabIdx, int iDefectCode, Hobject HDefectImg, Hobject HDefectRgn);	// LeeGW
	void SaveDefectFeatureLog(BOOL bSaveLas, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode, CString sDefectCode, Hobject HDefectImg, Hobject HDefectRgn);
	void SaveDefectParamLog(BOOL bSaveLas, BOOL bBarrelLens, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode, int iImageIdx, int iTabIdx, CAlgorithmParam AlgorithmParam);
	void SaveFAIResultLog(BOOL bSaveLas, CString strLotID, int iTrayNo, int iModuleNo, CString sBarcode);
	// void SaveADJLotResultLog(CString sPath, CString strResult, CString strAdditionalTitle, BOOL bUseAdditionalLog, int iPCType, int iPcVisionNo, int iMzNo);
	void SaveADJDaySummaryINI(CString sSectionModel, BOOL bResultNG, BOOL bResultSkip);	// LeeGW
	void SaveOnlyADJLotResultLog(BOOL bSaveLas, CString sLotID, int iTrayNo, CString sBarcode, int iModuleNo, CString sDeepModelName, CString sVisionResult, CString sADJResult, CString sAddStr);	//LeeGW
	void SavePCStatusLog(BOOL bSaveLas);

	int	 m_nADJResult[DEEP_MODEL_NUM][MAX_MODULE_NUM];	//ADJ Result

	void	SetViewStatusText(int idx, CString szMsg);
	void    UpdateCurMode();

	int	CheckProcessExist();
	CString	GetProgramFileName();

	CCalDataService* GetCalDataService(int nCamIdx);	// cskim

	void RestartProcess(); 	// 프로세스 초기화 Restart - LeeGW
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CuScanApp)
	public:
	virtual BOOL InitInstance();
	void ExitProcess(CString strTargetProcName, DWORD dwExceptProcId);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	
	// Implementation
	void init_valiable();
	int    m_iModeSwitch;

	// SW Version
	CString m_strVerInfo;

	CGFunction*			m_pGFunction;

	//////////////////////////////////////////////////////////////////////다이알로그 클래스 선언부
	CInspectAdminViewDlg*		m_pInspectAdminViewDlg;
	CInspectAdminViewHideDlg*  m_pInspectAdminViewHideDlg;

	CCalDataService*	m_pCalDataService;

	CTrayAdminViewDlg* m_pTrayAdminViewDlg;
	
	CInspectSummary*  m_pInspectSummary;
	CInspectResultDlg* m_pInspectResultDlg;
	CDefectListDlg* m_pDefectListDlg;
	CLogDlg* m_pLogDlg;
	CTrayBeamProjectViewDlg* m_pTrayBeamProjectViewDlg;
	CTrayOperatorViewPrevDlg* m_pTrayOperatorViewPrevDlg;
	CTrayOperatorViewNowDlg* m_pTrayOperatorViewNowDlg;
	CInspectViewBarrelSurfaceDlg* m_pInspectViewBarrelSurfaceDlg;
	CInspectViewBarrelEdgeDlg*  m_pInspectViewBarrelEdgeDlg;
	CInspectViewLensInnerDlg* m_pInspectViewLensInnerDlg;
	CInspectViewLensOuterDlg* m_pInspectViewLensOuterDlg;
	CInspectViewExtra1Dlg* m_pInspectViewExtra1Dlg;
	CInspectViewExtra2Dlg* m_pInspectViewExtra2Dlg;

	CInspectViewOverayImageDlg* m_pInspectViewOverayImageDlg;
	CTabControlDlg* m_pTabControlDlg;

	//////////////////////////////////////////////////////////////////////다이알로그 클래스 선언부

	typedef struct _stPreferenceStruct
	{
		_stPreferenceStruct()
		{
			m_strBasic_FolderPath = _T("D:\\");
			m_strRmsSaveFolderPath = _T("D:\\EVMS");
		};

		BOOL m_bSaveBMP, m_bSaveJPG;
		BOOL m_bCheckBeamProject;
		BOOL m_bDxLogDetail;
		BOOL m_bSaveDefectFeatureLog;	// 불량 Feature 정보 추가 - LeeGW
		BOOL m_bSaveDetectParamLog;		// 검출 Param 로그 추가 - LeeGW

		int m_iSaveLasDataZipWaitTime;
		BOOL m_bSaveLasDataZip;
		BOOL m_bSaveLasLog;
		BOOL m_bSaveLasImage;
		BOOL m_bSaveLasServerImage;

		BOOL m_bSaveRawImage;
		BOOL m_bSaveNGImage;
		BOOL m_bSaveReviewImage;	// ReviewImage 저장 추가 - LeeGW
		BOOL m_bSaveFAIImage;		// FAIImage 저장 추가 - LeeGW
		BOOL m_bSaveResultImage;	// ResultImage 저장 추가 - LeeGW
		BOOL m_bSaveResultMerge;	// 24.07.08 - v2652 - ResultImage 통합 저장 기능 추가 - LeeGW

		CString m_strBasic_FolderPath;
		CString m_strDebug_FolderPath;

		CString m_strEquipNo;

		int m_iBarcodeCamSerial;


		int m_iTrayOK_B, m_iTrayOK_G, m_iTrayOK_R;
		int m_iTrayOKFont_B, m_iTrayOKFont_G, m_iTrayOKFont_R;

		int m_iTrayNG_B, m_iTrayNG_G, m_iTrayNG_R;
		int m_iTrayNGB_B, m_iTrayNGB_G, m_iTrayNGB_R;
		int m_iTrayNGL_B, m_iTrayNGL_G, m_iTrayNGL_R;
		int m_iTrayNGFont_B, m_iTrayNGFont_G, m_iTrayNGFont_R;
		int m_iResultFont_B, m_iResultFont_G, m_iResultFont_R;

		int m_iGrabDelayTime;
		int m_iGrabDoneWaitTime;
		int m_iDefectDispDist;

		long lNGColor[MAX_DEFECT_NUMBER];
		int	iDefectPriority[MAX_DEFECT_NUMBER];
		
		int m_iDiskMax;
		int m_iDiskLean;
		int m_iDiskTerm;
		int m_iDiskTimeH;
		int m_iDiskTimeM;
		BOOL m_bDiskTermUse;

		int m_iEmptyCircleRadius;
		int m_iEmptyMaxEdgePoint;

		int m_iBarcodeNoReadImageType;

		CString m_sProductName;

		//0524
		BOOL m_bUseGrabErrFIltering;
		int m_iGrabErrDarkAreaGVLimit;
		int m_iGrabErrBrightAreaGVLimit;
		int m_iGrabErrRetryNo;
		int m_iTriggerSleepTime;

		int m_iLightErrorMinLimit;
		int m_iLightErrorMaxLimit;

		int m_iDefectWarningDefectCount;
		int m_iDefectWarningDefectDistance;

		int m_iCamFOVType;
		int m_iPickerTrayDir;

		int m_iLensResultImageX;
		int	m_iLensResultImageY;
		double m_dLensResultImageZoom; 
		int	m_iExtraResultImageX;
		int	m_iExtraResultImageY;
		double m_dExtraResultImageZoom;

		BOOL m_bIsUseAIInsp;
		BOOL m_bIsApplyAIResult;
		BOOL m_bIsApplyAISimulation;
		BOOL m_bUseAIResizeImageSave;
		int m_iAICropImageWidth;
		int m_iAICropImageHeight;
		int m_iAIResizeImageWidth;
		int m_iAIResizeImageHeight;

		CString m_strCurrentADJ_IP;
		CString m_strADJIPAddress;
		int	m_iADJPortNo;
		CString	m_strADJModelName;
		int	m_iADJDelayTime;
		int m_iADJImageCropType;

		BOOL m_bIsUseADJRunCheck;
		CString m_strADJExePath;

		// Multiple Defect Start
		BOOL m_bUseMultipleDefectAlarm;						// 1Lot 기준 불량 다발 발생 시 알람 기능 사용 (On / Off)
		BOOL m_bUseMultipleDefectAlarmAreaSimilarity;		// (중간면적기준) 면적 유사도 범위(%) (On / Off)
		BOOL m_bUseMultipleDefectSendAlarm;				// 핸들러로 알람 전송 (On / Off)
		int m_iMultipleDefectAlarmMinimumNum;				// 알람 발생을 위한 최소 검사 모듈 수량
		int m_iMultipleDefectAlarmSamePosTolerance;			// 동일 위치로 판단 할 최소 거리 (픽셀)
		int m_iMultipleDefectAlarmAreaSimilarityTolerance;	// (중간면적기준) 면적 유사도 범위(%) - 진짜 0 ~ 100% 만 받으려나?
		int m_iMultipleDefectAlarmDefectRatio;				// 알람 발생(1Lot 기준 다발 불량 비율(%)
		// Multiple Defect End

		BOOL m_bChangeEvmsDirectory; //Ver2629
		BOOL m_bUseAbsolutePathModel; //Ver2629

		CString m_strRmsSaveFolderPath; //RMS

		int m_iResultTextPosX; //Result Text
		int m_iResultTextPosY; //Result Text
		int m_iResultTextSize; //Result Text

		BOOL m_bSpecialNGSort;	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

		BOOL m_bSaveRawImageResize;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
		double m_dSaveRawImageResizeRatio;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
		BOOL m_bSaveRawImageMerge;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

		BOOL m_bUseGrabErrFIltering2;	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW
		int m_iGrabErrSubGVLimit;	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW
		
		int m_iMachineInspType;

		BOOL m_bUseProcessRestart;			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
		double m_dProcessRestartMemoryLimit;	// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
		int m_iProcessRestartTimeMin;		// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
		int	m_iProcessRestartTimeMax;		// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
		int m_iProcessRestartDelayTime;			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW	
		
		BOOL	m_bUseMboMode;
		int m_iMboModeCount;
		
		BOOL m_bUseHandlerRetry;
		int m_iHandlerRetryCount;
		int m_iHandlerRetryWaitTime;
		int m_iHandlerReplyWaitTime;

	}PreferenceStruct;
	PreferenceStruct Struct_PreferenceStruct;

	typedef struct _stFaiReviewInfoStruct
	{
		BOOL	m_bSaveReview;								// FAI Review 저장 여부
		CString	m_sFaiName;
		int		m_iImageNo;									// 각 Review 영상 생성 번호
		int		m_iStartX;									// 시작 위치
		int		m_iStartY;
		float	m_fZoomRatio;
		int		m_iMergeCnt;
		CString	m_sColor;

		void Reset()
		{
			m_sFaiName = "FAI-0";
			m_bSaveReview = FALSE;
			m_iImageNo = 0;
			m_iStartX = 0;
			m_iStartY = 0;
			m_fZoomRatio = 0.0;
			m_iMergeCnt = 0;
			m_sColor = "green";

		};
	}FaiReviewInfoStruct;
	FaiReviewInfoStruct Struct_FAI_ReviewInfo[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];

	void ReadPreferenceINI();
	void ReadFaiReviewInfo();

	CModelDataManager* m_pModelDataManager;

	//////////////////////////////////////////////////////////////////////////
	// MachineType Info
	int m_iTrayLineNumber;
	int m_iModuleNumberOneLine;
	int m_iMaxModuleOneTray;
	int m_iMaxModuleOneLot;
	//////////////////////////////////////////////////////////////////////////
	// LeeGW//////////////////////////////////////////////////////////////////
	HZIP hz[BARCODE_MAX_NUM];	
	ZRESULT zr[BARCODE_MAX_NUM];	
	stBarcodeVirtualMagazine	Struct_BarcodeVirtualMagazine;

	void CreateLasZip(CString strLotID, CString sBarcode);
	void AddLasZip(CString strLotID, CString sFilePath, CString sLasFileName);
	void CloseLasZip(CString sBarcode);
	void MoveLasZip(CString strLotID, CString sBarcode);

	//쓰레드 단위로 초기화 된 MFC함수의 Send가 엇갈리는 것을 막기 위한 cs
	CRITICAL_SECTION		m_csSendADJ;									
	CRITICAL_SECTION		m_csADJResultLog;
	CRITICAL_SECTION		m_csADJDaySummaryLog;
	//////////////////////////////////////////////////////////////////////

	CRITICAL_SECTION		m_csMULTIPLEDEFECTLOG; //Multiple Defect

	//////////////////////////////////////////////////////////////////////파일 폴더 클래스 선언부
	CFileBase m_FileBase;
	//////////////////////////////////////////////////////////////////////파일 폴더 클래스 선언부

	SYSTEMTIME m_InspectStartTime[MAX_MODULE_ONE_TRAY];		// One module inspection start time
	double m_InspectTime[MAX_MODULE_ONE_TRAY];				// One module inspection inspection time

	double m_dLotStartTickCount;							// Lot start time

	int	m_iBarcodeShiftPos[MAX_TRAY_LOT][MAX_MODULE_ONE_TRAY];

	//////////////////////////////////////////////////////////////////////불량 Warning
	int iModuleCountOneLot;
	int m_iModuleDefectCenterX[MAX_MODULE_ONE_LOT];
	int m_iModuleDefectCenterY[MAX_MODULE_ONE_LOT];
	CString m_sModuleDefectName[MAX_MODULE_ONE_LOT];
	//////////////////////////////////////////////////////////////////////불량 Warning

	//////////////////////////////////////////////////////////////////////////	Disk Cleaner
	void DeletePath(CString strPath);
	void CleanDiskbyCapacity(int iCapacity);
	void CleanDiskbyDay();
	double GetDiskCapacityPercent();
	BOOL CheckFolderHasItem(CString strPath);
	SYSTEMTIME FindingYesterDay(SYSTEMTIME Todaytime);
	//////////////////////////////////////////////////////////////////////////

	Algorithm* m_pAlgorithm;
	CHandlerService*  m_pHandlerService;
	CInspectService* m_pInspectService;
	CAutoCalService* m_pAutoCalService;
	CCameraManager* m_pCameraManager;
	CTriggerManager* m_pTriggerManager;

	CSaveManager* m_pSaveManager;	// LeeGW

	CMIInterface m_MIInterface;
	void InitProgress(int range, CString sText);

	double m_dEachImgAvgGV[MAX_IMAGE_TAB]; // APD
	int m_nTotalModuleCnt;				   // APD
	void ResetEachImgAvgGV()			   // APD
	{
		m_nTotalModuleCnt = 0;
		for (int i = 0; i < MAX_IMAGE_TAB; i++)
			m_dEachImgAvgGV[i] = 0;
	}

	void LoadResultLotINI();
	void LoadResultDayINI();
	void GetLastDate(int *Year, int *Month, int *Day);

	BOOL m_bShowReviewWindow;

	// 불량 파라미터 정보 저장
	vector<DetectParamInfoStruct> m_vDetectParamInfo[MAX_TRAY_LOT][MAX_MODULE_ONE_TRAY][MAX_DEFECT_NAME];

	#pragma region Multiple Defect
	typedef struct _stMultipleDefectInfo
	{
		_stMultipleDefectInfo()
		{
			Reset();
		}

		Hobject m_HMultipleDefectRgn[MAX_TRAY_LOT][MAX_MODULE_ONE_TRAY];

		void Reset()
		{
			for (int i = 0; i < MAX_TRAY_LOT; i++)
				for (int j = 0; j < MAX_MODULE_ONE_TRAY; j++)
					gen_empty_obj(&m_HMultipleDefectRgn[i][j]);
		};
	} MultipleDefectInfo;
	MultipleDefectInfo m_stMultipleDefectInfo[MAX_VIRTUAL_MAGAZINE_NO][MAX_MULTIPLE_DEFECT_NUMBER];

	typedef struct _stMultipleDefectVirtualMagazine
	{
		CString m_strLotID[MAX_VIRTUAL_MAGAZINE_NO];
		int m_nCurMagazineNo;

		_stMultipleDefectVirtualMagazine()
		{
			for (int i = 0; i < MAX_VIRTUAL_MAGAZINE_NO; i++)
			{
				m_strLotID[i] = "";
			}
			m_nCurMagazineNo = 0;
		}
		
		// (Maybe) 이전과 현재 Lot ID가 같은 지 확인 후, 같으면 해당 인덱스 반환
		int GetVirtualMagazineNo( CString strLot )
		{
			for (int i = 0; i < MAX_VIRTUAL_MAGAZINE_NO; i++) //현재 저장된 Lot ID와 동일한 Lot ID가 들어올 경우
			{
				if( this->m_strLotID[i] == strLot )
					return i;
			}

			for (int i = 0; i < MAX_VIRTUAL_MAGAZINE_NO; i++) //초기 시작해서 저장된 Lot ID가 없는 경우
			{
				if( this->m_strLotID[i] == "" )
				{
					this->m_strLotID[i] = strLot;
					m_nCurMagazineNo = i;
					return i;
				}
			}

			//(위의 두가지 경우를 제외하면)현재 저장된 Lot ID와 다른 Lot ID가 들어올 경우

			m_nCurMagazineNo = (m_nCurMagazineNo + 1) % MAX_VIRTUAL_MAGAZINE_NO;

			this->m_strLotID[m_nCurMagazineNo] = strLot;
			return m_nCurMagazineNo;
		}

	} MultipleDefectVirtualMagazine;
	MultipleDefectVirtualMagazine m_stVirtualMagazineNo;
	
	CString m_strMultipleDefectBarcode[MAX_VIRTUAL_MAGAZINE_NO][MAX_TRAY_LOT][MAX_MODULE_ONE_TRAY];
	CString GetVisionAndDefectName( int nDefectCode );
	void SaveMultipleDefectListLog(CString sPath, CString strLog);
	void ResetMultipleDefectInfo(int iModuleIdx);
	#pragma endregion Multiple Defect
	
	// FAI Measurement ParkSW 20240626 #6 Side
	typedef struct _stCenterlineMeasureStruct
	{
		// Common
		double m_dInnerCircleCenterX[MAX_FAI_ITEM], m_dInnerCircleCenterY[MAX_FAI_ITEM]; // FAI-51
		double m_dOuterCircleCenterX[MAX_FAI_ITEM], m_dOuterCircleCenterY[MAX_FAI_ITEM];
		double m_dDatumCircleCenterX[MAX_FAI_ITEM], m_dDatumCircleCenterY[MAX_FAI_ITEM];
		double m_dDatumEllipseCenterX[MAX_FAI_ITEM], m_dDatumEllipseCenterY[MAX_FAI_ITEM];

		double m_dInnerChamferCircleX[MAX_FAI_ITEM][MAX_FAI_CIRCLE_FIT_POINT], m_dInnerChamferCircleY[MAX_FAI_ITEM][MAX_FAI_CIRCLE_FIT_POINT], m_dInnerChamferCircleES[MAX_FAI_ITEM][MAX_FAI_CIRCLE_FIT_POINT];
		double m_dOuterPrismCircleX[MAX_FAI_ITEM][MAX_FAI_CIRCLE_FIT_POINT], m_dOuterPrismCircleY[MAX_FAI_ITEM][MAX_FAI_CIRCLE_FIT_POINT], m_dOuterPrismCircleES[MAX_FAI_ITEM][MAX_FAI_CIRCLE_FIT_POINT];
		double m_dDatumCircleX[MAX_FAI_ITEM][MAX_FAI_ELLIPSE_FIT_POINT], m_dDatumCircleY[MAX_FAI_ITEM][MAX_FAI_ELLIPSE_FIT_POINT], m_dDatumCircleES[MAX_FAI_ITEM][MAX_FAI_CIRCLE_FIT_POINT];
		double m_dDatumEllipseX[MAX_FAI_ITEM][MAX_FAI_ELLIPSE_FIT_POINT], m_dDatumEllipseY[MAX_FAI_ITEM][MAX_FAI_ELLIPSE_FIT_POINT], m_dDatumEllipseES[MAX_FAI_ITEM][MAX_FAI_CIRCLE_FIT_POINT];
		double m_dBracketLineStartX[MAX_FAI_ITEM][MAX_FAI_ROTATION_BRACKET_ROI], m_dBracketLineStartY[MAX_FAI_ITEM][MAX_FAI_ROTATION_BRACKET_ROI], m_dBracketLineEndX[MAX_FAI_ITEM][MAX_FAI_ROTATION_BRACKET_ROI], m_dBracketLineEndY[MAX_FAI_ITEM][MAX_FAI_ROTATION_BRACKET_ROI];

		// Measurement Value/Count/NG
		double m_dFAIMeasureValue[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];
		double m_dFAICriterionValue[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];

		// Inspection Result
		int m_iNGFAICode;
		BOOL m_bInspResultNG;
		BOOL m_bFAI_ResultNG[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];
		CString m_strFAI_OKNG[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];

		void ResetFAI()
		{
			m_bInspResultNG = FALSE;
			m_iNGFAICode = -1;

			for (int iii = 0; iii < MAX_FAI_ITEM; iii++)
			{
				for (int i = 0; i < MAX_FAI_CIRCLE_FIT_POINT; i++)
				{
					m_dInnerChamferCircleX[iii][i] = m_dInnerChamferCircleY[iii][i] = m_dInnerChamferCircleES[iii][i] = -1;
					m_dOuterPrismCircleX[iii][i] = m_dOuterPrismCircleY[iii][i] = m_dOuterPrismCircleES[iii][i] = -1;
					m_dDatumCircleX[iii][i] = m_dDatumCircleY[iii][i] = m_dDatumCircleES[iii][i] = -1;
					m_dDatumEllipseX[iii][i] = m_dDatumEllipseY[iii][i] = m_dDatumEllipseES[iii][i] = -1;
				}

				m_dInnerCircleCenterX[iii] = m_dInnerCircleCenterY[iii] = -1;
				m_dOuterCircleCenterX[iii] = m_dOuterCircleCenterY[iii] = -1;
				m_dDatumCircleCenterX[iii] = m_dDatumCircleCenterY[iii] = -1;
				m_dDatumEllipseCenterX[iii] = m_dDatumEllipseCenterY[iii] = -1;

				for (int i = 0; i < MAX_FAI_ROTATION_BRACKET_ROI; i++)
					m_dBracketLineStartX[iii][i] = m_dBracketLineStartY[iii][i] = m_dBracketLineEndX[iii][i] = m_dBracketLineEndY[iii][i] = -1;

			}

			for (int i = 0; i < MAX_FAI_ITEM; i++)
			{
				for (int j = 0; j < MAX_ONE_FAI_MEASURE_VALUE; j++)
				{
					m_dFAIMeasureValue[i][j] = FAI_MEASURE_ERROR_NUMBER;
					m_bFAI_ResultNG[i][j] = FALSE;
					m_strFAI_OKNG[i][j] = _T("OK");
				}

			}
		};
	} CenterlineMeasureStruct;
	CenterlineMeasureStruct m_StructFaiMeasure[MAX_TRAY_LOT][MAX_MODULE_ONE_TRAY]; // FAI END ParkSW 20240626

	//FOB Mode Change add Start
	int       m_nFOBModeUser;
	CString   m_strFOBModeOperatorID;
	//FOB Mode Change add End

	BOOL m_bUseEvms; //Ver2629

	int m_iMachineInspType;
	int m_iMaxInspImageNo;
	BOOL m_bUseFAI[MAX_FAI_ITEM] = { FALSE };

protected:
	CString     m_szWorkingEvmsDir; //Ver2629
	CString		m_szWorkingDir;
	// MEMO : EVMS Folder ("D:\\EVMS\\TP\\Log") - 250912, jhkim
	CString		m_szLASDataFolder;
	// MEMO : Image Folder ("D:\\EVMS\\TP\\Logs") - 250912, jhkim
	CString		m_szLASImageFolder;
	// MEMO : TEMP Folder ("D:\\EVMS\\TEMP") - 250912, jhkim
	CString		m_szLASTempFolder;
	// MEMO : TDM Folder ("D:\\EVMS\\TDM") - 250912, jhkim
	CString		m_szLASTdmFolder;
	// MEMO : SECTION Folder ("D:\\EVMS\\TDM\\SECTION") - 250912, jhkim
	CString		m_szLASSectionFolder;
	// MEMO : EVEMT Folder ("D:\\EVMS\\TDM\\EVENT") - 250912, jhkim
	CString		m_szLASEventFolder;
	CString		m_szPCID;

	CString		m_sLoginMode;
	CString		m_sOperator;

	BOOL		m_bRun;
	BOOL		m_bInitComplete;

	int			m_iAppVersion;
	
	void		SetVersion();
	//{{AFX_MSG(CuScanApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHelp();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_uScan_H__7AC743F0_B386_4D5B_9E6B_09F72F50F422__INCLUDED_)
