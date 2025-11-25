// PreferenceDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "PreferenceDlg.h"
#include "afxdialogex.h"
#include "DefectPriorityDlg.h"

// CPreferenceDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CPreferenceDlg, CDialog)

// 폴더 선택 대화 상자 콜백 함수
// 대화 상자가 초기화될 때 원하는 경로로 포커스를 설정합니다.
// lpData에는 BrowseForFolder 함수에서 bi.lParam으로 넘긴 문자열 주소가 저장되어 있습니다.
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	// BFFM : Browse For Folder Message
	if (uMsg == BFFM_INITIALIZED)
	{
		// lpData (bi.lParam)에 저장된 초기 경로 문자열을 사용하여 포커스 설정
		// TRUE는 문자열 경로(Path)를 사용한다는 의미입니다.
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

CString BrowseFolderHelper(HWND hwnd, CString strInitialPath, CString strTitle)
{
	CString strReturn = _T("");
	// COM 라이브러리 초기화는 필수입니다.
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	LPTSTR pszDisplayName = NULL;
	BROWSEINFOA browseInfo;
	memset(&browseInfo, 0, sizeof(browseInfo));

	browseInfo.hwndOwner = hwnd;
	browseInfo.pszDisplayName = pszDisplayName;
	browseInfo.lpszTitle = strTitle;
	// **[핵심 변경]** 콜백 함수와 LPARAM을 설정합니다.
	browseInfo.lpfn = BrowseCallbackProc;

	DWORD attributes = GetFileAttributes(strInitialPath);
	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		// 폴더가 존재하지 않거나, 파일이거나, 접근할 수 없음.
		// -> bi.lParam에 NULL 또는 안전한 기본 경로를 전달
		// (예: 바탕 화면 경로를 찾아서 설정하거나, NULL 유지)
		AfxMessageBox(_T("경로가 유효하지 않아 기본 경로에서 시작합니다."));
		// bi.lParam = 0; 또는 bi.lParam = (LPARAM)(LPCTSTR)strFallbackPath;
		strInitialPath = _T("");
	}

	// CString을 LPCTSTR (char* in MBCS)로 캐스팅하여 콜백 함수로 전달합니다.
	// CString 은 (MBCS 환경에서는 내부적으로 char* 임)
	browseInfo.lParam = (LPARAM)(LPCTSTR)strInitialPath;
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	LPITEMIDLIST pIdSelected = NULL;
	// pIdSelected 포인터에 메모리 할당됨.
	// LPITEMIDLIST 로 변환된 주소값.
	pIdSelected = ::SHBrowseForFolder(&browseInfo);

	if (pIdSelected != NULL)
	{
		TCHAR szFolderPath[MAX_PATH] = {0};

		if (::SHGetPathFromIDList(pIdSelected, szFolderPath))
		{
			strReturn = CString(szFolderPath);
		}
		else
		{
			AfxMessageBox(_T("파일 시스템 경로로 변환할 수 없습니다."), MB_OK | MB_ICONWARNING);
		}

		// 5. 메모리 해제
		// SHBrowseForFolder 와 같은 Windows Shell API 는 COM 기반 함수.
		// COM 기반 함수는 CoTaskMemFree 로 메모리를 해제해야 한다.
		// 일반적인 c++ (delete()) 혹은 c 스타일 (free()) 와는 메모리 힙이 다르기때문에 에러.
		CoTaskMemFree(pIdSelected);
	}

	if (pszDisplayName != NULL)
		CoTaskMemFree(pszDisplayName);

	CoUninitialize();

	return strReturn;
}

CPreferenceDlg::CPreferenceDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CPreferenceDlg::IDD, pParent)
{
	m_strBasic_FolderPath = _T("D:\\");;
	m_strDebug_FolderPath = _T("");

	m_iTrayOK_B = 0;
	m_iTrayOK_G = 0;
	m_iTrayOK_R = 0;
	m_iTrayOKFont_B = 0;
	m_iTrayOKFont_G = 0;
	m_iTrayOKFont_R = 0;

	m_iTrayNG_B = 0;
	m_iTrayNG_G = 0;
	m_iTrayNG_R = 0;
	m_iTrayNGB_B = 0;
	m_iTrayNGB_G = 0;
	m_iTrayNGB_R = 0;
	m_iTrayNGL_B = 0;
	m_iTrayNGL_G = 0;
	m_iTrayNGL_R = 0;
	m_iTrayNGFont_B = 0;
	m_iTrayNGFont_G = 0;
	m_iTrayNGFont_R = 0;

	m_iResultFont_B = 0;
	m_iResultFont_G = 0;
	m_iResultFont_R = 0;

	m_strEquipNo = _T("CMI4000");

	m_iEditGrabDelayTime = 0;
	m_iEditGrabDoneWaitTime = 500;
	m_iDefectDispDist = 0;

	m_bCheckBeamProject = FALSE;
	m_bDxLogDetail = FALSE;
	m_bChkSaveDefectFeatureLog = FALSE; // 불량 Feature 정보 추가 - LeeGW
	m_bChkSaveDetectParamLog = FALSE;	// 검출 Param 로그 추가 - LeeGW

	m_iEditSaveLasDataZipWaitTime = 0;
	m_bCheckSaveLasDataZip = FALSE;
	m_bCheckSaveLasLog = FALSE;
	m_bCheckSaveLasImage = FALSE;
	m_bCheckSaveLasServerImage = FALSE;

	m_bCheckSaveRawImage = FALSE;
	m_bCheckSaveNGImage = FALSE;
	m_bCheckSaveReviewImage = FALSE; // Review 이미지 저장 추가 - LeeGW
	m_bCheckSaveFAIImage = FALSE;	 // Review 이미지 저장 추가 - LeeGW
	m_bCheckSaveResultImage = TRUE;	 // Result 이미지 저장 추가, Default TRUE - LeeGW
	m_bCheckSaveResultMerge = FALSE; // 24.07.08 - v2652 - ResultImage 통합 저장 기능 추가 - LeeGW

	m_iDiskMax = 0;
	m_iDiskLean = 0;
	m_iDiskTerm = 0;
	m_iDiskTimeH = 0;
	m_iDiskTimeM = 0;
	m_bDiskTermUse = FALSE;
	m_iBarcodeCamSerialNo = 0;

	m_iEditEmptyCircleRadius = 300;

	m_iEditEmptyMaxEdgePoint = 7000;

	m_iRadioBarcodeNoReadImageType = 0;
	m_sEditProductName = _T("");

	// 0524
	m_bCheckUseGrabErrFIltering = FALSE;
	m_iEditGrabErrDarkAreaGVLimit = 20;
	m_iEditGrabErrBrightAreaGVLimit = 255;
	m_iEditGrabErrRetryNo = 3;
	m_iEditTriggerSleepTime = 45;

	m_iEditLightErrorMinLimit = 20;
	m_iEditLightErrorMaxLimit = 255;

	m_iEditDefectWarningDefectCount = 5;
	m_iEditDefectWarningDefectDistance = 30;

	m_bIsUseAIInsp = FALSE;
	m_bIsApplyAIResult = FALSE;
	m_bIsApplyAISimulation = FALSE;
	m_iEditAICropImageWidth = 0;
	m_iEditAICropImageHeight = 0;
	m_iEditAIResizeImageWidth = 0;
	m_iEditAIResizeImageHeight = 0;
	m_bCheckUseAIResizeImageSave = FALSE;
	m_strADJIPAddress = _T("");
	m_iADJImageCropType = ADJ_INSP_TYPE_BLOB;
	m_iADJPortNo = 0;
	m_strADJModelName = _T("");
	m_iADJDelayTime = 10;

	m_bIsUseADJRunCheck = FALSE;
	m_strADJExePath = _T("");

	m_iEditLensResultImageX = 0;
	m_iEditLensResultImageY = 0;
	m_dEditLensResultImageZoom = 1.0;
	m_iEditExtraResultImageX = 0;
	m_iEditExtraResultImageY = 0;
	m_dEditExtraResultImageZoom = 1.0;

	m_iRadioPickerTrayDir = PICKER_TRAY_DIR_X;

	// Multiple Defect
	m_bCheckUseLotDefectAlarm = FALSE;
	m_bCheckUseLotDefectAlarmAreaSimilarity = FALSE;
	m_bCheckUseMultipleDefectSendAlarm = FALSE;
	m_iEditLotDefectAlarmSamePosTolerance = 10;
	m_iEditLotDefectAlarmAreaSimilarityTolerance = 30;
	m_iEditLotDefectAlarmDefectRatio = 10;
	m_iEditLotDefectAlarmMinimumNum = 60;
	// Multiple Defect

	m_bCheckChangeEvmsDirectory = FALSE;  // Ver2629
	m_bCheckUseAbsolutePathModel = FALSE; // Ver2629

	m_strEditRmsSaveFolderPath = _T("D:\\EVMS"); // RMS

	// Result Text
	m_iEditResultTextPosX = 0;
	m_iEditResultTextPosY = 0;
	m_iEditResultTextSize = 0;

	m_bCheckSpecialNGSort = FALSE; // 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

	m_bCheckSaveRawImageResize = FALSE;	  // 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	m_dEditSaveRawImageResizeRatio = 1.0; // 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	m_bCheckSaveRawImageMerge = FALSE;	  // 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW

	m_bCheckUseGrabErrFIltering2 = FALSE; // 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW
	m_iEditGrabErrSubGVLimit = 0;		  // 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW

	m_iRadioMachineInspType = 0; // 검사유형 통합 - LeeGW

	m_bCheckUseProcessRestart = FALSE;		  // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	m_dEditProcessRestartMemoryLimit = 100.0; // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	m_iEditProcessRestartTimeMin = 30;		  // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	m_iEditProcessRestartTimeMax = 1440;	  // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	m_iEditProcessRestartDelayTime = 0;

	m_bCheckUseMboMode = FALSE; // 25.05.13 - LeeGW
	m_iEditMboModeCount = 1;	// 25.05.13 - LeeGW

	m_bCheckUseHandlerRetry = FALSE; // 25.05.13 - LeeGW
	m_iEditHandlerRetryCount = 0;	 // 25.05.13 - LeeGW
	m_iEditHandlerRetryWaitTime = 0; // 25.05.13 - LeeGW
	m_iEditHandlerReplyWaitTime = 0; // 25.05.13 - LeeGW
}

CPreferenceDlg::~CPreferenceDlg()
{
}

void CPreferenceDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHK_BEAMPROJECT, m_bCheckBeamProject);
	// DDX_Check(pDX, IDC_CHK_SAVEBMP, m_bSaveBMP);
	// DDX_Check(pDX, IDC_CHK_SAVEJPG, m_bSaveJPG);
	DDX_Check(pDX, IDC_CHK_SAVE_DSF_LOG, m_bChkSaveDefectFeatureLog); // 불량 Feature 정보 추가 - LeeGW
	DDX_Check(pDX, IDC_CHK_SAVE_PARAM_LOG, m_bChkSaveDetectParamLog); // 검출 Param 로그 추가 - LeeGW
	DDX_Check(pDX, IDC_CHK_LOG_DETAIL, m_bDxLogDetail);
	DDX_Text(pDX, IDC_EDIT_SAVE_LAS_DATA_ZIP_WAIT_TIME, m_iEditSaveLasDataZipWaitTime);
	DDX_Check(pDX, IDC_CHECK_SAVE_LAS_DATA_ZIP, m_bCheckSaveLasDataZip);
	DDX_Check(pDX, IDC_CHECK_SAVE_LAS_LOG, m_bCheckSaveLasLog);
	DDX_Check(pDX, IDC_CHECK_SAVE_LAS_IMAGE, m_bCheckSaveLasImage);
	DDX_Check(pDX, IDC_CHECK_SAVE_LAS_SERVER_IMAGE, m_bCheckSaveLasServerImage);

	DDX_Text(pDX, IDC_EDIT_BASIC_FOLDERPATH, m_strBasic_FolderPath);
	DDX_Text(pDX, IDC_EDIT_DEBUG_FOLDERPATH, m_strDebug_FolderPath);

	DDX_Text(pDX, IDC_EDIT_TRAYOK_B, m_iTrayOK_B);
	DDX_Text(pDX, IDC_EDIT_TRAYOK_G, m_iTrayOK_G);
	DDX_Text(pDX, IDC_EDIT_TRAYOK_R, m_iTrayOK_R);

	DDX_Text(pDX, IDC_EDIT_TRAYOKFONT_B, m_iTrayOKFont_B);
	DDX_Text(pDX, IDC_EDIT_TRAYOKFONT_G, m_iTrayOKFont_G);
	DDX_Text(pDX, IDC_EDIT_TRAYOKFONT_R, m_iTrayOKFont_R);

	DDX_Text(pDX, IDC_EDIT_TRAYNG_B, m_iTrayNG_B);
	DDX_Text(pDX, IDC_EDIT_TRAYNG_G, m_iTrayNG_G);
	DDX_Text(pDX, IDC_EDIT_TRAYNG_R, m_iTrayNG_R);

	DDX_Text(pDX, IDC_EDIT_TRAYNGB_B, m_iTrayNGB_B);
	DDX_Text(pDX, IDC_EDIT_TRAYNGB_G, m_iTrayNGB_G);
	DDX_Text(pDX, IDC_EDIT_TRAYNGB_R, m_iTrayNGB_R);

	DDX_Text(pDX, IDC_EDIT_TRAYNGL_B, m_iTrayNGL_B);
	DDX_Text(pDX, IDC_EDIT_TRAYNGL_G, m_iTrayNGL_G);
	DDX_Text(pDX, IDC_EDIT_TRAYNGL_R, m_iTrayNGL_R);

	DDX_Text(pDX, IDC_EDIT_TRAYNGFONT_B, m_iTrayNGFont_B);
	DDX_Text(pDX, IDC_EDIT_TRAYNGFONT_G, m_iTrayNGFont_G);
	DDX_Text(pDX, IDC_EDIT_TRAYNGFONT_R, m_iTrayNGFont_R);

	DDX_Text(pDX, IDC_EDIT_RESULTFONT_B, m_iResultFont_B);
	DDX_Text(pDX, IDC_EDIT_RESULTFONT_G, m_iResultFont_G);
	DDX_Text(pDX, IDC_EDIT_RESULTFONT_R, m_iResultFont_R);
	DDX_Text(pDX, IDC_EDIT_EQUIPNO, m_strEquipNo);

	DDX_Control(pDX, IDC_PROGRESS_DISK_HDDC, m_ctrlProgressHddC);
	DDX_Control(pDX, IDC_PROGRESS_DISK_HDDD, m_ctrlProgressHddD);
	DDX_Text(pDX, IDC_EDIT_GRAB_DELAY_TIME, m_iEditGrabDelayTime);
	DDX_Text(pDX, IDC_EDIT_GRAB_DONE_WAIT_TIME, m_iEditGrabDoneWaitTime);
	DDX_Text(pDX, IDC_EDIT_DISP_SET_DIST, m_iDefectDispDist);
	DDX_Text(pDX, IDC_EDIT_DISK_MAX, m_iDiskMax);
	DDX_Text(pDX, IDC_EDIT_DISK_LEAN, m_iDiskLean);
	DDX_Text(pDX, IDC_EDIT_DISK_TERM, m_iDiskTerm);
	DDX_Text(pDX, IDC_EDIT_DISK_TIMEH, m_iDiskTimeH);
	DDX_Text(pDX, IDC_EDIT_DISK_TIMEM, m_iDiskTimeM);
	DDX_Check(pDX, IDC_CHK_DISK_TERM, m_bDiskTermUse);
	DDX_Text(pDX, IDC_EDIT_BARCODE_CAM_SERIAL, m_iBarcodeCamSerialNo);

	DDX_Text(pDX, IDC_EDIT_EM_CIRCLE_RADIUS, m_iEditEmptyCircleRadius);
	DDX_Text(pDX, IDC_EDIT_EM_MAX_EDGE_POINT, m_iEditEmptyMaxEdgePoint);
	DDX_Radio(pDX, IDC_RADIO_BARCODE_NOREAD_IMAGE_BMP, m_iRadioBarcodeNoReadImageType);

	DDX_Control(pDX, IDC_SPIN_DISK_MAX, m_SpinDiskMax);
	DDX_Control(pDX, IDC_SPIN_DISK_TIMEH, m_SpinDiskTimeH);
	DDX_Control(pDX, IDC_SPIN_DISK_TIMEM, m_SpinDiskTimeM);
	DDX_Control(pDX, IDC_SPIN_DISK_LEAN, m_SpinDiskLean);
	DDX_Control(pDX, IDC_SPIN_DISK_TERM, m_SpinDiskTerm);
	DDX_Check(pDX, IDC_CHECK_SAVE_RAW_IMAGE, m_bCheckSaveRawImage);
	DDX_Check(pDX, IDC_CHECK_SAVE_NG_IMAGE, m_bCheckSaveNGImage);
	DDX_Check(pDX, IDC_CHECK_SAVE_REVIEW_IMAGE, m_bCheckSaveReviewImage); // Review 이미지 저장 추가 - LeeGW
	DDX_Check(pDX, IDC_CHECK_SAVE_FAI_IMAGE, m_bCheckSaveFAIImage);		  // FAI 이미지 저장 추가 - LeeGW
	DDX_Check(pDX, IDC_CHECK_SAVE_RESULT_IMAGE, m_bCheckSaveResultImage); // Result 저장여부 추가 - LeeGW
	DDX_Check(pDX, IDC_CHECK_SAVE_RESULT_MERGE, m_bCheckSaveResultMerge); // 24.07.08 - v2652 - ResultImage 통합 저장 기능 추가 - LeeGW
	DDX_Text(pDX, IDC_EDIT_PRODUCT_NAME, m_sEditProductName);

	// 0524
	DDX_Check(pDX, IDC_CHECK_USE_GRAB_ERROR_FILTERING, m_bCheckUseGrabErrFIltering);
	DDX_Text(pDX, IDC_EDIT_GRAB_ERROR_BRIGHT_AREA_MIN_GV_LIMIT, m_iEditGrabErrBrightAreaGVLimit);
	DDX_Text(pDX, IDC_EDIT_GRAB_ERROR_DARK_AREA_MIN_GV_LIMIT, m_iEditGrabErrDarkAreaGVLimit);
	DDX_Text(pDX, IDC_EDIT_GRAB_ERROR_NO_RETRY, m_iEditGrabErrRetryNo);
	DDX_Text(pDX, IDC_EDIT_TRIGGER_SLEEP_TIME, m_iEditTriggerSleepTime);

	DDX_Text(pDX, IDC_EDIT_LIGHT_ERROR_MIN_LIMIT, m_iEditLightErrorMinLimit);
	DDX_Text(pDX, IDC_EDIT_LIGHT_ERROR_MAX_LIMIT, m_iEditLightErrorMaxLimit);

	DDX_Text(pDX, IDC_EDIT_DEFECT_WARNING_DEFECT_COUNT, m_iEditDefectWarningDefectCount);
	DDX_Text(pDX, IDC_EDIT_DEFECT_WARNING_DEFECT_DISTANCE, m_iEditDefectWarningDefectDistance);

	DDX_Radio(pDX, IDC_RADIO_CAMERA_FOV_CROP, m_iRadioCamFOVType);

	DDX_Check(pDX, IDC_CHECK_USE_AI_INSP, m_bIsUseAIInsp);
	DDX_Check(pDX, IDC_CHECK_APPLY_AI_RESULT, m_bIsApplyAIResult);
	DDX_Check(pDX, IDC_CHECK_APPLY_AI_SIMULATION, m_bIsApplyAISimulation);
	DDX_Radio(pDX, IDC_RADIO_CROP_TYPE_1, m_iADJImageCropType);
	DDX_Text(pDX, IDC_EDIT_AI_INSP_CROP_WIDTH, m_iEditAICropImageWidth);
	DDX_Text(pDX, IDC_EDIT_AI_INSP_CROP_HEIGHT, m_iEditAICropImageHeight);
	DDX_Text(pDX, IDC_EDIT_AI_INSP_RESIZE_WIDTH, m_iEditAIResizeImageWidth);
	DDX_Text(pDX, IDC_EDIT_AI_INSP_RESIZE_HEIGHT, m_iEditAIResizeImageHeight);
	DDX_Check(pDX, IDC_CHECK_USE_AI_RESIZE_SAVE, m_bCheckUseAIResizeImageSave);
	DDX_Text(pDX, IDC_EDIT_ADJ_IP_ADDRESS, m_strADJIPAddress);
	DDX_Text(pDX, IDC_EDIT_ADJ_PORT_NO, m_iADJPortNo);
	DDX_Text(pDX, IDC_EDIT_ADJ_MODEL_NAME, m_strADJModelName);
	DDX_Text(pDX, IDC_EDIT_ADJ_DELAY_TIME, m_iADJDelayTime);
	DDX_Check(pDX, IDC_CHECK_USE_ADJ_RUN_CHECK, m_bIsUseADJRunCheck);
	DDX_Text(pDX, IDC_EDIT_ADJ_EXE_PATH, m_strADJExePath);

	DDX_Text(pDX, IDC_EDIT_LENS_RESULT_IMAGE_X, m_iEditLensResultImageX);
	DDX_Text(pDX, IDC_EDIT_LENS_RESULT_IMAGE_Y, m_iEditLensResultImageY);
	DDX_Text(pDX, IDC_EDIT_LENS_RESULT_IMAGE_ZOOM, m_dEditLensResultImageZoom);
	DDX_Text(pDX, IDC_EDIT_EXTRA_RESULT_IMAGE_X, m_iEditExtraResultImageX);
	DDX_Text(pDX, IDC_EDIT_EXTRA_RESULT_IMAGE_Y, m_iEditExtraResultImageY);
	DDX_Text(pDX, IDC_EDIT_EXTRA_RESULT_IMAGE_ZOOM, m_dEditExtraResultImageZoom);

	DDX_Radio(pDX, IDC_RADIO_PICKER_TRAY_X, m_iRadioPickerTrayDir);

	// Multiple Defect Start
	DDX_Check(pDX, IDC_CHECK_USE_LOT_DEFECT_ALARM, m_bCheckUseLotDefectAlarm);
	DDX_Check(pDX, IDC_CHECK_USE_LOT_DEFECT_ALARM_AREA_SIMILARITY, m_bCheckUseLotDefectAlarmAreaSimilarity);
	DDX_Check(pDX, IDC_CHECK_USE_MULTIPLE_DEFECT_SEND_HANDLER_ALARM, m_bCheckUseMultipleDefectSendAlarm);
	DDX_Text(pDX, IDC_EDIT_LOT_DEFECT_ALARM_SAME_POS_TOLERANCE, m_iEditLotDefectAlarmSamePosTolerance);
	DDX_Text(pDX, IDC_EDIT_LOT_DEFECT_ALARM_AREA_SIMILARITY_TOLERANCE, m_iEditLotDefectAlarmAreaSimilarityTolerance);
	DDX_Text(pDX, IDC_EDIT_LOT_DEFECT_ALARM_DEFECT_RATIO, m_iEditLotDefectAlarmDefectRatio);
	DDX_Text(pDX, IDC_EDIT_LOT_DEFECT_ALARM_MINIMUM_NUM, m_iEditLotDefectAlarmMinimumNum);
	// Multiple Defect End

	DDX_Check(pDX, IDC_CHECK_CHANGE_EVMS_DIRECTORY, m_bCheckChangeEvmsDirectory);	 // Ver2629
	DDX_Check(pDX, IDC_CHECK_USE_ABSOLUTE_PATH_MODEL, m_bCheckUseAbsolutePathModel); // Ver2629
	DDX_Text(pDX, IDC_EDIT_RMS_SAVE_FOLDER_PATH, m_strEditRmsSaveFolderPath);		 // RMS
	DDX_Text(pDX, IDC_EDIT_RESULT_TEXT_POS_X, m_iEditResultTextPosX);				 // Result Text
	DDX_Text(pDX, IDC_EDIT_RESULT_TEXT_POS_Y, m_iEditResultTextPosY);				 // Result Text
	DDX_Text(pDX, IDC_EDIT_RESULT_TEXT_RATIO, m_iEditResultTextSize);				 // Result Text

	DDX_Check(pDX, IDC_CHECK_USE_SPECIAL_NG_SORT, m_bCheckSpecialNGSort); // 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

	DDX_Check(pDX, IDC_CHECK_USE_RAWIMAGE_RESIZE, m_bCheckSaveRawImageResize);		   // 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	DDX_Text(pDX, IDC_EDIT_RAWIMAGE_RESIZE_RATIO, m_dEditSaveRawImageResizeRatio);	   // 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	DDX_Check(pDX, IDC_CHECK_USE_RAWIMAGE_MERGE, m_bCheckSaveRawImageMerge);		   // 24.07.31 - v2656 - RAWIMAGE 축소, 합성 저장 기능 추가 - LeeGW
	DDX_Check(pDX, IDC_CHECK_USE_GRAB_ERROR_FILTERING2, m_bCheckUseGrabErrFIltering2); // 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW
	DDX_Text(pDX, IDC_EDIT_GRAB_ERROR_SUB_GV_MAX_LIMIT, m_iEditGrabErrSubGVLimit);	   // 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW

	DDX_Radio(pDX, IDC_RADIO_MACHINE_INSP_TYPE_1, m_iRadioMachineInspType);

	DDX_Check(pDX, IDC_CHECK_USE_PROCESS_RESTART, m_bCheckUseProcessRestart);				// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	DDX_Text(pDX, IDC_EDIT_PROCESS_RESTART_MEMORY_LIMIT, m_dEditProcessRestartMemoryLimit); // 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	DDX_Text(pDX, IDC_EDIT_PROCESS_RESTART_TIME_MIN, m_iEditProcessRestartTimeMin);			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	DDX_Text(pDX, IDC_EDIT_PROCESS_RESTART_TIME_MAX, m_iEditProcessRestartTimeMax);			// 24.12.02 - v3005 - 프로세스 초기화 추가 - LeeGW
	DDX_Text(pDX, IDC_EDIT_PROCESS_RESTART_DELAY_TIME, m_iEditProcessRestartDelayTime);		// 24.12.02 - v3006 - 프로세스 초기화 Delay 추가 - LeeGW

	DDX_Check(pDX, IDC_CHECK_USE_MBO_MODE, m_bCheckUseMboMode);	 // MBO 모드 - LEEGW
	DDX_Text(pDX, IDC_EDIT_MBO_MODE_COUNT, m_iEditMboModeCount); // MBO 모드 - LEEGW

	DDX_Check(pDX, IDC_CHECK_USE_HANDLER_RETRY, m_bCheckUseHandlerRetry);		 // 통신 재시도 - LEEGW
	DDX_Text(pDX, IDC_EDIT_HANDLER_RETRY_COUNT, m_iEditHandlerRetryCount);		 // 통신 재시도 - LEEGW
	DDX_Text(pDX, IDC_EDIT_HANDLER_RETRY_WAITTIME, m_iEditHandlerRetryWaitTime); // 통신 재시도 - LEEGW
	DDX_Text(pDX, IDC_EDIT_HANDLER_REPLY_WAITTIME, m_iEditHandlerReplyWaitTime); // 통신 재시도 - LEEGW

	// RMS Folder Set UI
	DDX_Control(pDX, IDC_BUTTON_FINDFOLDER_RMSDATA, m_ctrlButtonFindfolderRmsdata);
	DDX_Control(pDX, IDC_EDIT_RMS_SAVE_FOLDER_PATH, m_ctrlEditRmsSaveFolderPath);
}

BEGIN_MESSAGE_MAP(CPreferenceDlg, CDialog)
ON_BN_CLICKED(IDOK, &CPreferenceDlg::OnBnClickedOk)
ON_BN_CLICKED(IDCANCEL, &CPreferenceDlg::OnBnClickedCancel)
ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER, &CPreferenceDlg::OnBnClickedButtonFindfolder)
ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_DEBUG, &CPreferenceDlg::OnBnClickedButtonFindfolderDebug)
ON_BN_CLICKED(IDC_BUTTON_SET_TRAYOK_BGCOLOR, &CPreferenceDlg::OnBnClickedButtonSetTrayOkBgcolor)
ON_BN_CLICKED(IDC_BUTTON_SET_TRAYOK_FONTCOLOR, &CPreferenceDlg::OnBnClickedButtonSetTrayOkFontcolor)
ON_BN_CLICKED(IDC_BUTTON_SET_TRAYNG_BGCOLOR, &CPreferenceDlg::OnBnClickedButtonSetTrayNgBgcolor)
ON_BN_CLICKED(IDC_BUTTON_SET_TRAYNG_BGCOLOR_B, &CPreferenceDlg::OnBnClickedButtonSetTrayNgBgcolorB)
ON_BN_CLICKED(IDC_BUTTON_SET_TRAYNG_BGCOLOR_L, &CPreferenceDlg::OnBnClickedButtonSetTrayNgBgcolorL)
ON_BN_CLICKED(IDC_BUTTON_SET_TRAYNG_FONTCOLOR, &CPreferenceDlg::OnBnClickedButtonSetTrayNgFontcolor)
ON_BN_CLICKED(IDC_BUTTON_SET_RESULTFONT, &CPreferenceDlg::OnBnClickedButtonSetResultfont)
ON_BN_CLICKED(IDC_BUTTON_SETEQUIPNO, &CPreferenceDlg::OnBnClickedButtonSetequipno)
ON_BN_CLICKED(IDC_CHK_BEAMPROJECT, &CPreferenceDlg::OnBnClickedCheckBeamproject)
ON_BN_CLICKED(IDC_BUTTON_SET_DEFECT_PRIORITY, &CPreferenceDlg::OnBnClickedButtonSetDefectPriority)
ON_BN_CLICKED(IDC_BN_DISK_CLEAN, &CPreferenceDlg::OnBnClickedBnDiskClean)
ON_BN_CLICKED(IDC_CHK_DISK_TERM, &CPreferenceDlg::OnBnClickedChkDiskTerm)
ON_BN_CLICKED(IDC_BUTTON_SET_SERIAL, &CPreferenceDlg::OnBnClickedButtonSetSerial)
ON_BN_CLICKED(IDC_CHECK_USE_AI_INSP, &CPreferenceDlg::OnBnClickedCheckUseSuakit)
ON_BN_CLICKED(IDC_CHECK_CHANGE_EVMS_DIRECTORY, &CPreferenceDlg::OnBnClickedCheckChangeEvmsDirectory)
ON_BN_CLICKED(IDC_BUTTON_FIND_ADJ_FILE, &CPreferenceDlg::OnBnClickedButtonFindAdjFile)
ON_BN_CLICKED(IDC_BUTTON_FINDFOLDER_RMSDATA, &CPreferenceDlg::OnBnClickedButtonFindfolderRmsdata)
ON_BN_CLICKED(IDC_CHECK_USE_LOT_DEFECT_ALARM, &CPreferenceDlg::OnBnClickedCheckUseLotDefectAlarm)
ON_BN_CLICKED(IDC_CHECK_USE_LOT_DEFECT_ALARM_AREA_SIMILARITY, &CPreferenceDlg::OnBnClickedCheckUseLotDefectAlarmAreaSimilarity)
END_MESSAGE_MAP()

BOOL CPreferenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ShowDiskCapacity();

#ifdef INLINE_MODE
	GetDlgItem(IDC_CHK_BEAMPROJECT)->EnableWindow(FALSE);
#endif

	m_SpinDiskMax.SetRange(0, 100);
	m_SpinDiskLean.SetRange(0, 100);
	m_SpinDiskTerm.SetRange(1, 90);
	m_SpinDiskTimeH.SetRange(0, 23);
	m_SpinDiskTimeM.SetRange(0, 59);

	if (IsDlgButtonChecked(IDC_CHECK_USE_AI_INSP))
	{
		GetDlgItem(IDC_CHECK_APPLY_AI_RESULT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_APPLY_AI_SIMULATION)->EnableWindow(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_CHECK_APPLY_AI_RESULT))->SetCheck(FALSE);
		GetDlgItem(IDC_CHECK_APPLY_AI_RESULT)->EnableWindow(FALSE);

		((CButton *)GetDlgItem(IDC_CHECK_APPLY_AI_SIMULATION))->SetCheck(FALSE);
		GetDlgItem(IDC_CHECK_APPLY_AI_SIMULATION)->EnableWindow(FALSE);
	}

	// 다발불량 Alarm 이 Off 일 때 관련 Window 모두 Disable 되도록 변경하는 Sequence
	BOOL bIsMDAlarmOn = THEAPP.Struct_PreferenceStruct.m_bUseMultipleDefectAlarm;
	for (UINT id : MDGroupBoxIDs)
	{
		CWnd* pMDWnd = GetDlgItem(id);
		// Alarm과 같이 En/Disable
		pMDWnd->EnableWindow(bIsMDAlarmOn);
	}
	BOOL bUseAreaSim = m_bCheckUseLotDefectAlarmAreaSimilarity;
	CWnd* pMDUseAreaSimWnd = GetDlgItem(IDC_EDIT_LOT_DEFECT_ALARM_AREA_SIMILARITY_TOLERANCE);
	if (bIsMDAlarmOn)
		pMDUseAreaSimWnd->EnableWindow(bUseAreaSim);

	if (m_bCheckChangeEvmsDirectory == FALSE) // Ver2629
	{
		m_bCheckUseAbsolutePathModel = FALSE;
		GetDlgItem(IDC_CHECK_USE_ABSOLUTE_PATH_MODEL)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}

	// ADJ Disable - 251021, jhkim
	for (UINT id : ADJButtons)
	{
		CWnd* pWnd = GetDlgItem(id);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
	}

	return TRUE; // return TRUE unless you set the focus to a control
				 // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// CPreferenceDlg 메시지 처리기입니다.

void CPreferenceDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CDialog::OnOK();
}

void CPreferenceDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}

void CPreferenceDlg::ShowDiskCapacity()
{
	CString str;
	float TotalBytes, FreeBytes;
	float fTotal, fFree, fUsed;
	int iThres = THEAPP.Struct_PreferenceStruct.m_iDiskMax;

	ULARGE_INTEGER ulUserFree, ulTotal, ulRealFree;

	CString sDir = "C:\\";
	if (GetDiskFreeSpaceEx(sDir, &ulUserFree, &ulTotal, &ulRealFree))
	{
		TotalBytes = ulTotal.LowPart + (double)ulTotal.HighPart * (double)4294967296;
		fTotal = (float)(TotalBytes / 1024 / 1024 / 1024); // Convert (GB)
		FreeBytes = ulRealFree.LowPart + (double)ulRealFree.HighPart * (double)4294967296;
		fFree = (float)(FreeBytes / 1024 / 1024 / 1024); // Convert (GB)
		fUsed = fTotal - fFree;
	}
	int iPos = int(fUsed / fTotal * 100);
	if (iPos < 0)
		iPos = 0;
	if (iPos > 100)
		iPos = 100;
	SetDlgItemInt(IDC_STATIC_DISK_HDDC_PRO, iPos);
	SetDlgItemInt(IDC_STATIC_DISK_HDDC_TOTAL, (int)fTotal);
	SetDlgItemInt(IDC_STATIC_DISK_HDDC_USE, (int)fUsed);
	m_ctrlProgressHddC.SetRange(0, 100);
	m_ctrlProgressHddC.SetPos(iPos);
	if (iPos >= iThres)
		m_ctrlProgressHddC.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(255, 0, 0));
	else
		m_ctrlProgressHddC.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(0, 127, 255));

	sDir = "D:\\";
	if (GetDiskFreeSpaceEx(sDir, &ulUserFree, &ulTotal, &ulRealFree))
	{
		TotalBytes = ulTotal.LowPart + (double)ulTotal.HighPart * (double)4294967296;
		fTotal = (float)(TotalBytes / 1024 / 1024 / 1024); // Convert (GB)
		FreeBytes = ulRealFree.LowPart + (double)ulRealFree.HighPart * (double)4294967296;
		fFree = (float)(FreeBytes / 1024 / 1024 / 1024); // Convert (GB)
		fUsed = fTotal - fFree;
	}
	iPos = int(fUsed / fTotal * 100);
	if (iPos < 0)
		iPos = 0;
	if (iPos > 100)
		iPos = 100;
	SetDlgItemInt(IDC_STATIC_DISK_HDDD_PRO, iPos);
	SetDlgItemInt(IDC_STATIC_DISK_HDDD_TOTAL, (int)fTotal);
	SetDlgItemInt(IDC_STATIC_DISK_HDDD_USE, (int)fUsed);
	m_ctrlProgressHddD.SetRange(0, 100);
	m_ctrlProgressHddD.SetPos(iPos);
	if (iPos >= iThres)
		m_ctrlProgressHddD.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(255, 0, 0));
	else
		m_ctrlProgressHddD.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF)RGB(0, 127, 255));

	OnBnClickedChkDiskTerm();
}

void CPreferenceDlg::OnBnClickedBnDiskClean()
{
	UpdateData();

	CString str;
	str.Format("가장 오래된 저장 데이터로부터 디스크 용량이 %d(%%)에 도달되도록 결과 삭제합니다. 진행하시겠습니까?", m_iDiskLean);

	if (AfxMessageBox(str, MB_OKCANCEL | MB_SYSTEMMODAL) == IDCANCEL)
		return;

	THEAPP.CleanDiskbyCapacity(m_iDiskLean);

	////// InspectSummary 다이얼로그에 디스크 용량 표시 //////////////////////////
	THEAPP.m_pInspectSummary->ShowDiskCapacity();

	AfxMessageBox("디스크 정리 완료 !!.", MB_SYSTEMMODAL | MB_ICONINFORMATION);
}

void CPreferenceDlg::OnBnClickedChkDiskTerm()
{
	GetDlgItem(IDC_STATIC_DISK_TERM)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_DISK_TERMZ)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_DISK_TERM)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_DISK_TERM)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_DISK_TIMEH)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_DISK_TIMEH)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_DISK_TIMEM)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_DISK_TIMEM)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_DISK_TIME)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_DISK_TIMEHZ)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_DISK_TIMEMZ)->EnableWindow(TRUE);

	UpdateData();
	if (m_bDiskTermUse == FALSE)
	{
		GetDlgItem(IDC_STATIC_DISK_TERM)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_DISK_TERMZ)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_DISK_TERM)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_DISK_TERM)->EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT_DISK_TIMEH)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_DISK_TIMEH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_DISK_TIMEM)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_DISK_TIMEM)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_DISK_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_DISK_TIMEHZ)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_DISK_TIMEMZ)->EnableWindow(FALSE);
	}
}

// 검사결과 저장 폴더 설정
void CPreferenceDlg::OnBnClickedButtonFindfolder()
{
	CString strInitPath = m_strBasic_FolderPath;
	CString strFolderPath = _T("");

	strFolderPath = BrowseFolderHelper(m_hWnd, strInitPath, _T("검사결과 저장 폴더를 선택하세요"));
	if (strFolderPath.IsEmpty())
	{
		// AfxMessageBox(_T("폴더 경로를 찾지 못했습니다."), MB_OK | MB_ICONWARNING);
	}
	else
	{
		THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath = strFolderPath;
		m_strBasic_FolderPath = strFolderPath;
	}

	UpdateData(FALSE);
}

// 디버그용 폴더 설정
void CPreferenceDlg::OnBnClickedButtonFindfolderDebug()
{
	CString strInitPath = m_strDebug_FolderPath;
	CString strFolderPath = _T("");

	strFolderPath = BrowseFolderHelper(m_hWnd, strInitPath, _T("Debug Folder"));
	if (strFolderPath.IsEmpty())
	{
		// AfxMessageBox(_T("폴더 경로를 찾지 못했습니다."), MB_OK | MB_ICONWARNING);
	}
	else
	{
		if (strFolderPath.Find("RawImage") != -1)
		{
			m_strDebug_FolderPath = strFolderPath;
			UpdateData(FALSE);
		}
		else
		{
			MessageBox("RawImage 폴더를 선택해주세요");
		}
	}

	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedCheckSavebmp()
{
	UpdateData(TRUE);
	m_bSaveBMP = TRUE;
	m_bSaveJPG = FALSE;
	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedCheckSavejpg()
{
	UpdateData(TRUE);
	m_bSaveBMP = FALSE;
	m_bSaveJPG = TRUE;
	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedButtonSetTrayOkBgcolor()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		int color_R = 0, color_G = 0, color_B = 0;
		color_R = GetRValue(color);
		color_G = GetGValue(color);
		color_B = GetBValue(color);

		m_iTrayOK_R = color_R;
		m_iTrayOK_G = color_G;
		m_iTrayOK_B = color_B;

		UpdateData(FALSE);
	}
}

void CPreferenceDlg::OnBnClickedButtonSetTrayOkFontcolor()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		int color_R = 0, color_G = 0, color_B = 0;
		color_R = GetRValue(color);
		color_G = GetGValue(color);
		color_B = GetBValue(color);

		m_iTrayOKFont_R = color_R;
		m_iTrayOKFont_G = color_G;
		m_iTrayOKFont_B = color_B;

		UpdateData(FALSE);
	}
}

void CPreferenceDlg::OnBnClickedButtonSetTrayNgBgcolor()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		int color_R = 0, color_G = 0, color_B = 0;
		color_R = GetRValue(color);
		color_G = GetGValue(color);
		color_B = GetBValue(color);

		m_iTrayNG_R = color_R;
		m_iTrayNG_G = color_G;
		m_iTrayNG_B = color_B;

		UpdateData(FALSE);
	}
}

void CPreferenceDlg::OnBnClickedButtonSetTrayNgBgcolorB()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		int color_R = 0, color_G = 0, color_B = 0;
		color_R = GetRValue(color);
		color_G = GetGValue(color);
		color_B = GetBValue(color);

		m_iTrayNGB_R = color_R;
		m_iTrayNGB_G = color_G;
		m_iTrayNGB_B = color_B;

		UpdateData(FALSE);
	}
}

void CPreferenceDlg::OnBnClickedButtonSetTrayNgBgcolorL()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		int color_R = 0, color_G = 0, color_B = 0;
		color_R = GetRValue(color);
		color_G = GetGValue(color);
		color_B = GetBValue(color);

		m_iTrayNGL_R = color_R;
		m_iTrayNGL_G = color_G;
		m_iTrayNGL_B = color_B;

		UpdateData(FALSE);
	}
}

void CPreferenceDlg::OnBnClickedButtonSetTrayNgFontcolor()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		int color_R = 0, color_G = 0, color_B = 0;
		color_R = GetRValue(color);
		color_G = GetGValue(color);
		color_B = GetBValue(color);

		m_iTrayNGFont_R = color_R;
		m_iTrayNGFont_G = color_G;
		m_iTrayNGFont_B = color_B;

		UpdateData(FALSE);
	}
}

void CPreferenceDlg::OnBnClickedButtonSetResultfont()
{
	CColorDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		int color_R = 0, color_G = 0, color_B = 0;
		color_R = GetRValue(color);
		color_G = GetGValue(color);
		color_B = GetBValue(color);

		m_iResultFont_R = color_R;
		m_iResultFont_G = color_G;
		m_iResultFont_B = color_B;

		UpdateData(FALSE);
	}
}

void CPreferenceDlg::OnBnClickedButtonSetequipno()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 현재 control 에 있는 값을 변수에 넣어준다.
	UpdateData(TRUE);

	// ini 에 있는 값들을 바꾸어 준다.
	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI(strModelFolder + "\\Prefecrence.ini");

	CString strSection = "EQUIPMENT NO";
	INI.Set_String(strSection, "EquipNo", m_strEquipNo);
}

void CPreferenceDlg::OnBnClickedCheckBeamproject()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CPreferenceDlg::OnBnClickedButtonSetDefectPriority()
{
	CDefectPriorityDlg dlg;
	dlg.DoModal();
}

void CPreferenceDlg::OnBnClickedButtonSetSerial()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	// ini 에 있는 값들을 바꾸어 준다.
	CString strModelFolder = THEAPP.GetWorkingDirectory() + "\\Data\\";
	CIniFileCS INI(strModelFolder + "\\Prefecrence.ini");

	CString strSection = "BARCODE CAMERA";
	INI.Set_Integer(strSection, "SerialNumber", m_iBarcodeCamSerialNo);
}

void CPreferenceDlg::OnBnClickedCheckUseSuakit()
{
	UpdateData(TRUE);
	// IDC_CHECK_SUAKIT_APPLY_IN_MACHINE
	if (IsDlgButtonChecked(IDC_CHECK_USE_AI_INSP))
	{
		GetDlgItem(IDC_CHECK_APPLY_AI_RESULT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_APPLY_AI_SIMULATION)->EnableWindow(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_CHECK_APPLY_AI_RESULT))->SetCheck(FALSE);
		GetDlgItem(IDC_CHECK_APPLY_AI_RESULT)->EnableWindow(FALSE);

		((CButton *)GetDlgItem(IDC_CHECK_APPLY_AI_SIMULATION))->SetCheck(FALSE);
		GetDlgItem(IDC_CHECK_APPLY_AI_SIMULATION)->EnableWindow(FALSE);
	}
}

void CPreferenceDlg::OnBnClickedCheckChangeEvmsDirectory() // Ver2629
{
	UpdateData(TRUE);
	if (m_bCheckChangeEvmsDirectory == FALSE)
	{
		if (m_bCheckUseAbsolutePathModel == TRUE)
		{
			AfxMessageBox("체크된 EVMS 관련 기능이 모두 해제됩니다.");
		}
		m_bCheckUseAbsolutePathModel = FALSE;
		GetDlgItem(IDC_CHECK_USE_ABSOLUTE_PATH_MODEL)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_USE_ABSOLUTE_PATH_MODEL)->EnableWindow(TRUE);
	}
}

void CPreferenceDlg::OnBnClickedButtonFindAdjFile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString filter = _T("Executable Files (*.exe)|*.exe|All Files (*.*)|*.*||");

	CFileDialog fileDialog(TRUE, _T("exe"), NULL,
						   OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
						   filter, this);

	if (fileDialog.DoModal() == IDOK)
	{
		m_strADJExePath = fileDialog.GetPathName();
	}

	UpdateData(FALSE);
}

void CPreferenceDlg::OnBnClickedButtonFindfolderRmsdata()
{
	CString strInitPath = THEAPP.Struct_PreferenceStruct.m_strRmsSaveFolderPath;
	CString strFolderPath = _T("");

	strFolderPath = BrowseFolderHelper(m_hWnd, strInitPath, "RMS Data Folder");
	if (strFolderPath.IsEmpty())
	{
		// AfxMessageBox(_T("폴더 경로를 찾지 못했습니다."), MB_OK | MB_ICONWARNING);
	}
	else
	{
		// 폴더 경로 바꾸고 바로 적용 -> TCP Server 에서 RSM Save 폴더 경로를 사용하기 때문에.
		THEAPP.Struct_PreferenceStruct.m_strRmsSaveFolderPath = strFolderPath;
		m_strEditRmsSaveFolderPath = strFolderPath;
	}

	UpdateData(FALSE);
}


// 다발불량 Alarm 이 토글될 때 관련 Window 모두 함께 토글 되도록 변경하는 함수
void CPreferenceDlg::OnBnClickedCheckUseLotDefectAlarm()
{
	// TRUE 면 UI 에서 변수로 READ
	UpdateData(TRUE);
	BOOL bIsMDAlarmOn = m_bCheckUseLotDefectAlarm;
	for (UINT id : MDGroupBoxIDs)
	{
		CWnd* pMDWnd = GetDlgItem(id);
		
		// Alarm과 같이 En/Disable
		pMDWnd->EnableWindow(bIsMDAlarmOn);
	}
	// Area Similarity 토글 상태에 따라 Enable/Disable
	BOOL bUseAreaSim = m_bCheckUseLotDefectAlarmAreaSimilarity;
	CWnd* pMDEditAreaSimWnd = GetDlgItem(IDC_EDIT_LOT_DEFECT_ALARM_AREA_SIMILARITY_TOLERANCE);
	// 다발불량 알람 Off 면 무조건 Disable.
	if (bIsMDAlarmOn)
		pMDEditAreaSimWnd->EnableWindow(bUseAreaSim);
}


void CPreferenceDlg::OnBnClickedCheckUseLotDefectAlarmAreaSimilarity()
{
	// TRUE 면 UI 에서 변수로 READ
	UpdateData(TRUE);
	BOOL bUseAreaSim = m_bCheckUseLotDefectAlarmAreaSimilarity;
	CWnd* pMDUseAreaSimWnd = GetDlgItem(IDC_EDIT_LOT_DEFECT_ALARM_AREA_SIMILARITY_TOLERANCE);
	pMDUseAreaSimWnd->EnableWindow(bUseAreaSim);
}
