#pragma once

#define SURFACE_COUNT 30

class CCrevisCam
{
public:
	CCrevisCam(void);
	~CCrevisCam(void);

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

	INT32		m_hDevice;			 
	INT32		m_Width;			 
	INT32		m_Height;
	INT32		m_BufferSize;
	BOOL		m_IsOpened;	

protected:


};

