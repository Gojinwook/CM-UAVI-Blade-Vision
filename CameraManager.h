#pragma once

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

	long GetCamImageWidth() { return m_lCamImageWidth; }
	long GetCamImageHeight() { return m_lCamImageHeight; }

	bool InitGrabInterface();
	void TeachingImageGrabSave();
	BOOL CopyRawImageQueue(int ImageCount, int nModuleNo, Hobject *pHImage, CString sBarcodeName);
	void InspectOriImageGrabSave(int ImageCount, int nModuleNo, Hobject HImage, CString sBarcodeName);
	void CallHookFunction(int iGrabBufIdx=0);
	void GrabErrorPostProcess();
	BOOL GrabErrorCheck(int igc);	// 24.08.09 - v2658 - 잘린 영상 필터링 추가 - LeeGW

	//0524
	BOOL m_bUseGrabErrFIltering;
	int m_iGrabErrDarkAreaGVLimit;
	int m_iGrabErrBrightAreaGVLimit;

	// Barcode Camera
	MIL_ID MilSystem_BC;
	MIL_ID MilDigitizer_BC1;
	MIL_ID MilImageBuf_BC1[MAX_BARCODE_GRAB];
	Hobject m_hoCallBackImage_BC1[MAX_BARCODE_GRAB];
	BOOL m_bGrabDone_BC1;

	MIL_ID MilDigitizer_BC2;
	MIL_ID MilImageBuf_BC2[MAX_BARCODE_GRAB];
	Hobject m_hoCallBackImage_BC2[MAX_BARCODE_GRAB];
	BOOL m_bGrabDone_BC2;

	long GetBarcodeCamImageWidth() { return m_lBarcodeCamImageWidth; }
	long GetBarcodeCamImageHeight() { return m_lBarcodeCamImageHeight; }

	BOOL GrabBarcode(int iCamIdx, int iGrabIdx);
	BOOL GrabBarcodeImage(int iCamIdx, Hobject *pHBarcodeImage);
	
protected:
	unsigned int m_uiCamSerialNumber;

	long m_lCamImageWidth;
	long m_lCamImageHeight;

	long m_lBarcodeCamImageWidth;
	long m_lBarcodeCamImageHeight;
};

