#pragma once
#include "afxcmn.h"


// CPreferenceDlg 대화 상자입니다.

class CPreferenceDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreferenceDlg)

public:
	CPreferenceDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPreferenceDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PREFERENCE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


private:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonFindfolder();
	afx_msg void OnBnClickedButtonFindfolderDebug();
	afx_msg void OnBnClickedCheckSavebmp();
	afx_msg void OnBnClickedCheckSavejpg();

	afx_msg void OnBnClickedButtonSetTrayOkBgcolor();
	afx_msg void OnBnClickedButtonSetTrayOkFontcolor();
	afx_msg void OnBnClickedButtonSetTrayNgBgcolor();
	afx_msg void OnBnClickedButtonSetTrayNgBgcolorB();
	afx_msg void OnBnClickedButtonSetTrayNgBgcolorL();
	afx_msg void OnBnClickedButtonSetTrayNgFontcolor();
	afx_msg void OnBnClickedButtonSetResultfont();
	
	afx_msg void OnBnClickedButtonSetequipno();
	afx_msg void OnBnClickedCheckBeamproject();
	afx_msg void OnBnClickedButtonSetDefectPriority();

	afx_msg void OnBnClickedBnDiskClean();
	afx_msg void OnBnClickedChkDiskTerm();


	CProgressCtrl m_ctrlProgressHddC;
	CProgressCtrl m_ctrlProgressHddD;


public:
	void ShowDiskCapacity();

	BOOL m_bSaveBMP, m_bSaveJPG;
	BOOL m_bCheckBeamProject;
	BOOL m_bDxLogDetail;
	BOOL m_bChkSaveDefectFeatureLog;	// 불량 Feature 정보 추가 - LeeGW
	BOOL m_bChkSaveDetectParamLog;		// 검출 Param 로그 추가 - LeeGW

	int m_iEditSaveLasDataZipWaitTime;
	BOOL m_bCheckSaveLasDataZip;
	BOOL m_bCheckSaveLasLog;
	BOOL m_bCheckSaveLasImage;
	BOOL m_bCheckSaveLasServerImage;

	CString m_strBasic_FolderPath;
	CString m_strDebug_FolderPath;
	
	CString m_strEquipNo;
	
	int m_iTrayOK_B, m_iTrayOK_G, m_iTrayOK_R;
	int m_iTrayOKFont_B, m_iTrayOKFont_G, m_iTrayOKFont_R;
	int m_iTrayNG_B, m_iTrayNG_G, m_iTrayNG_R;
	int m_iTrayNGB_B, m_iTrayNGB_G, m_iTrayNGB_R;
	int m_iTrayNGL_B, m_iTrayNGL_G, m_iTrayNGL_R;
	int m_iTrayNGFont_B, m_iTrayNGFont_G, m_iTrayNGFont_R;
	int m_iResultFont_B, m_iResultFont_G, m_iResultFont_R;
		
	int m_iEditGrabDelayTime;
	int m_iEditGrabDoneWaitTime;
	int m_iDefectDispDist; // Display 시 겹치지 않는 거리 설정. 

	int m_iDiskMax;
	int m_iDiskLean;
	int m_iDiskTerm;
	int m_iDiskTimeH;
	int m_iDiskTimeM;
	BOOL m_bDiskTermUse;
	int m_iBarcodeCamSerialNo;

	int m_iEditEmptyCircleRadius;
	int m_iEditEmptyMaxEdgePoint;

	afx_msg void OnBnClickedButtonSetSerial();
	int m_iRadioBarcodeNoReadImageType;
	CSpinButtonCtrl m_SpinDiskMax;
	CSpinButtonCtrl m_SpinDiskTimeH;
	CSpinButtonCtrl m_SpinDiskTimeM;
	CSpinButtonCtrl m_SpinDiskLean;
	CSpinButtonCtrl m_SpinDiskTerm;
	BOOL m_bCheckSaveRawImage;
	BOOL m_bCheckSaveNGImage;
	BOOL m_bCheckSaveReviewImage;	// Review 이미지 저장 추가 - LeeGW
	BOOL m_bCheckSaveFAIImage;		// FAI 이미지 저장 추가 - LeeGW
	BOOL m_bCheckSaveResultImage;	// Result 저장여부 추가 - LeeGW
	BOOL m_bCheckSaveResultMerge;	// 24.07.22 - v2808 - ResultImage 통합 저장 기능 추가 - LeeGW
	CString m_sEditProductName;

	//0524
	BOOL m_bCheckUseGrabErrFIltering;
	int m_iEditGrabErrBrightAreaGVLimit;
	int m_iEditGrabErrDarkAreaGVLimit;
	int m_iEditGrabErrRetryNo;
	int m_iEditTriggerSleepTime;

	int m_iEditLightErrorMinLimit;
	int m_iEditLightErrorMaxLimit;

	int m_iEditDefectWarningDefectCount;
	int m_iEditDefectWarningDefectDistance;

	int m_iRadioCamFOVType;
	int m_iRadioPickerTrayDir;

	BOOL m_bIsUseAIInsp;
	BOOL m_bIsApplyAIResult;
	BOOL m_bIsApplyAISimulation;
	BOOL m_bCheckUseAIResizeImageSave;
	int m_iEditAICropImageWidth;
	int m_iEditAICropImageHeight;
	int m_iEditAIResizeImageWidth;
	int m_iEditAIResizeImageHeight;

	BOOL m_bIsUseADJRunCheck;
	CString m_strADJIPAddress;
	CString m_strADJModelName;
	CString m_strADJExePath;
	int m_iADJImageCropType;
	int m_iADJPortNo;
	int m_iADJDelayTime;
	
	afx_msg void OnBnClickedCheckUseSuakit();
	afx_msg void OnBnClickedButtonFindAdjFile();

	// ADJ Disable - 251021, jhkim
	const UINT ADJButtons[20]={
		IDC_CHECK_USE_AI_INSP,
		IDC_CHECK_USE_ADJ_RUN_CHECK,
		IDC_IPADDRESS_ADJ_IP,
		IDC_EDIT_ADJ_IP_ADDRESS,
		IDC_BUTTON_ADJConnect,
		IDC_ADJ_MODEL_NUM,
		IDC_EDIT_ADJ_MODEL_NAME,
		IDC_ADJ_DELAY_TIME,
		IDC_EDIT_ADJ_DELAY_TIME,
		IDC_EDIT_ADJ_EXE_PATH,
		IDC_BUTTON_FIND_ADJ_FILE,
		IDC_IPADDRESS_ADJ_PORT,
		IDC_EDIT_ADJ_PORT_NO,
		IDC_RADIO_ADJ_TYPE_1,
		IDC_RADIO_ADJ_TYPE_2,
		IDC_EDIT_AI_INSP_RESIZE_WIDTH,
		IDC_EDIT_AI_INSP_RESIZE_HEIGHT,
		IDC_EDIT_AI_INSP_CROP_WIDTH,
		IDC_EDIT_AI_INSP_CROP_HEIGHT,
		IDC_CHECK_USE_AI_RESIZE_SAVE
	};

	int m_iEditLensResultImageX;
	int m_iEditLensResultImageY;
	double m_dEditLensResultImageZoom;
	int m_iEditExtraResultImageX;
	int m_iEditExtraResultImageY;
	double m_dEditExtraResultImageZoom;

	//Multiple Defect Start
	BOOL m_bCheckUseLotDefectAlarm;			           //사용 유무                           
	BOOL m_bCheckUseLotDefectAlarmAreaSimilarity;	   //동일 불량 체크 면적유사도 사용 유무 
	BOOL m_bCheckUseMultipleDefectSendAlarm;           //핸들러로 알람 전송 여부             
	int m_iEditLotDefectAlarmSamePosTolerance;		   //동일 불량 체크 거리                 
	int m_iEditLotDefectAlarmAreaSimilarityTolerance;  //동일 불량 체크면적유사도 비율       
	int m_iEditLotDefectAlarmDefectRatio;              //다발 불량 비율                      
	int m_iEditLotDefectAlarmMinimumNum;               //최소 검사 수량(전체 검사 수)        
	//Multiple Defect End

	BOOL m_bCheckChangeEvmsDirectory; //Ver2629
	BOOL m_bCheckUseAbsolutePathModel; //Ver2629 //ABSOLUTE_PATH_MODEL
	afx_msg void OnBnClickedCheckChangeEvmsDirectory();
	
	CString m_strEditRmsSaveFolderPath; //RMS
	int m_iEditResultTextPosX; //Result Text 
	int m_iEditResultTextPosY; //Result Text
	int m_iEditResultTextSize; //Result Text

	BOOL m_bCheckSpecialNGSort;	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

	BOOL m_bCheckSaveRawImageResize;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	double m_dEditSaveRawImageResizeRatio;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	BOOL m_bCheckSaveRawImageMerge;	// 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

	BOOL m_bCheckUseGrabErrFIltering2;	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW
	int m_iEditGrabErrSubGVLimit;	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW

	int m_iRadioMachineInspType;

	BOOL m_bCheckUseProcessRestart;			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	double m_dEditProcessRestartMemoryLimit;	// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	int m_iEditProcessRestartTimeMin;		// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	int m_iEditProcessRestartTimeMax;		// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	int m_iEditProcessRestartDelayTime;

	BOOL m_bCheckUseMboMode;	// 25.05.13 - LeeGW
	int	m_iEditMboModeCount;	// 25.05.13 - LeeGW

	BOOL m_bCheckUseHandlerRetry;	// 25.06.23 - LeeGW
	int	m_iEditHandlerRetryCount;	// 25.06.23 - LeeGW
	int	m_iEditHandlerRetryWaitTime;	// 25.06.23 - LeeGW
	int	m_iEditHandlerReplyWaitTime;	// 25.06.23 - LeeGW

	// RSM Data Folder Set UI
	CButton m_ctrlButtonFindfolderRmsdata;
	CEdit m_ctrlEditRmsSaveFolderPath;
	
	// AutoParam. 251014, jhkim
	afx_msg void OnBnClickedButtonFindfolderRmsdata();

	// 다발불량 관련 UI 들
	// 현재까지는 IDC_CHECK_USE_LOT_DEFECT_ALARM 를 조작할 일이 없어서 주석 - 251023, jhkim
	const UINT MDGroupBoxIDs[6] =	{
		// IDC_CHECK_USE_LOT_DEFECT_ALARM,
		IDC_CHECK_USE_LOT_DEFECT_ALARM_AREA_SIMILARITY,
		IDC_CHECK_USE_MULTIPLE_DEFECT_SEND_HANDLER_ALARM,
		IDC_EDIT_LOT_DEFECT_ALARM_MINIMUM_NUM,
		IDC_EDIT_LOT_DEFECT_ALARM_SAME_POS_TOLERANCE,
		IDC_EDIT_LOT_DEFECT_ALARM_AREA_SIMILARITY_TOLERANCE,
		IDC_EDIT_LOT_DEFECT_ALARM_DEFECT_RATIO
	};
	
	afx_msg void OnBnClickedCheckUseLotDefectAlarm();
	afx_msg void OnBnClickedCheckUseLotDefectAlarmAreaSimilarity();
};
