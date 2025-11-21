#pragma once

#define SURFACE_COUNT 30

class CPointGrayCam
{
public:
	CPointGrayCam(void);
	~CPointGrayCam(void);

	BOOL Initialize();
	void UnInitialize();	

	BOOL StartGrab();
	BOOL GetGrabImage(Hobject *pHImage);
	BOOL EndGrab();

	BOOL ResetCamera();

#ifdef BARCODE_CAM_CALLBACK_USE
	BOOL mbGogingRefresh;
	int m_iGrabNumber;
	BYTE* mpImageBuf[SURFACE_COUNT];
#endif

protected:

#ifdef BARCODE_CAM_POINTGRAY_USE
	Camera m_cam;
	CameraInfo m_camInfo;
	Image m_rawImage;
#endif

};

