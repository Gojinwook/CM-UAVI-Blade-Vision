#pragma once

#ifdef BARCODE_CAM_POINTGRAY_USE
#include "PointGrayCam.h"
#endif

#ifdef BARCODE_CAM_CREVIS_USE
#include "CrevisCam.h"
#endif

class CCameraManager
{
public:

	static CCameraManager	*m_pInstance;
	static	CCameraManager* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	CCameraManager(void);
	~CCameraManager(void);

	int ImageGrabCount;
	CString m_strSaveFileName;

	MIL_ID MilApplication,
		MilSystem,
		MilDigitizer;

	MIL_ID MilImageBuf[MAX_IMAGE_TAB];

	Hobject m_hoCallBackImage[MAX_IMAGE_TAB];

	BOOL m_bGrabDone;
	BOOL m_bReGrab;

	bool InitGrabInterface();
	void CameraLive();
	void TeachingImageGrabSave();
	BOOL CopyRawImageQueue(int ImageCount, int nModuleNo, Hobject *pHImage, CString sBarcodeName);
	void InspectOriImageGrabSave(int ImageCount, int nModuleNo, Hobject HImage, CString sBarcodeName);
	void CallHookFunction(int iGrabBufIdx=0);
	void GrabErrorPostProcess();
	BOOL GrabErrorCheck(int igc);	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW

	// Barcode Camera
	BOOL InitPointGray();
	BOOL InitCrevisCam();
	unsigned int GetCamSerialNumber()								{ return m_uiCamSerialNumber; }
	void SetCamSerialNumber( unsigned int uiCameraSerialNumber )	{  m_uiCamSerialNumber = uiCameraSerialNumber; }
	void GrabBarcodeImage(Hobject *pHImage);
	BOOL ResetBarcodeCamera();

	//0524
	BOOL m_bUseGrabErrFIltering;
	int m_iGrabErrDarkAreaGVLimit;
	int m_iGrabErrBrightAreaGVLimit;

	long GetCamImageWidth() { return m_lCamImageWidth; }
	long GetCamImageHeight() { return m_lCamImageHeight; }

protected:
	unsigned int m_uiCamSerialNumber;

#ifdef BARCODE_CAM_POINTGRAY_USE
	CPointGrayCam m_PointGrayCam;
#endif

#ifdef BARCODE_CAM_CREVIS_USE
	CCrevisCam m_CrevisCam;
#endif

	long m_lCamImageWidth;
	long m_lCamImageHeight;

};

