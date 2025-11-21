#include "stdafx.h"
#include "PointGrayCam.h"
#include "uScan.h"


CPointGrayCam::CPointGrayCam(void)
{

#ifdef BARCODE_CAM_CALLBACK_USE
	mbGogingRefresh = FALSE;

	m_iGrabNumber = 0;

	for(int i = 0 ; i < SURFACE_COUNT ; i++)	
	{
		mpImageBuf[i]= new BYTE[PGCAM_IMAGE_WIDTH * PGCAM_IMAGE_HEIGHT];
	}
#endif

}

CPointGrayCam::~CPointGrayCam(void)
{
#ifdef BARCODE_CAM_CALLBACK_USE
	for(int i = 0 ; i < SURFACE_COUNT ; i++)	
	{
		delete[] mpImageBuf[i];
	}
#endif
}

#if defined BARCODE_CAM_CALLBACK_USE && defined BARCODE_CAM_POINTGRAY_USE
void OnImageGrabbed(Image* pImage, const void* pCallbackData)
{
	CPointGrayCam* pPointGrayCam= (CPointGrayCam*)pCallbackData;

	// Start capturing images
	int iCameraWidth = pImage->GetCols();
	int iCameraHeight = pImage->GetRows();
	BYTE *pCdata = pImage->GetData();

	long lWidth, lHeight;
	unsigned char *cpImage;
	char caType[100];

	CString strMsg;

	if(pPointGrayCam->m_iGrabNumber < SURFACE_COUNT)
	{
		memcpy(pPointGrayCam->mpImageBuf[pPointGrayCam->m_iGrabNumber], pCdata, iCameraWidth * iCameraHeight);
		strMsg.Format("[PointGray Callback] Grab Number: %d", pPointGrayCam->m_iGrabNumber);
		THEAPP.SaveLog(strMsg);
		pPointGrayCam->m_iGrabNumber++; 
	}
	else
	{
		pPointGrayCam->m_iGrabNumber = 0;

		memcpy(pPointGrayCam->mpImageBuf[pPointGrayCam->m_iGrabNumber], pCdata, iCameraWidth * iCameraHeight);
		strMsg.Format("[PointGray Callback] Grab Number Initialized: %d", pPointGrayCam->m_iGrabNumber);
		THEAPP.SaveLog(strMsg);
		pPointGrayCam->m_iGrabNumber++; 
	}

	pPointGrayCam->mbGogingRefresh = TRUE;
}
#endif

BOOL CPointGrayCam::Initialize()
{
#ifdef BARCODE_CAM_POINTGRAY_USE

	FlyCapture2::Error error;

	PGRGuid guid;
	BusManager busMgr;
	busMgr.GetCameraFromSerialNumber(THEAPP.m_pCameraManager->GetCamSerialNumber(), &guid);

	error = m_cam.Connect(&guid);
	if (error != PGRERROR_OK)
		return FALSE;

	error = m_cam.GetCameraInfo(&m_camInfo);
	if (error != PGRERROR_OK)
		return FALSE;

	TriggerModeInfo triggerModeInfo;
	error = m_cam.GetTriggerModeInfo(&triggerModeInfo);
	if (error != PGRERROR_OK)
		return FALSE;

	if ( triggerModeInfo.present != true )
		return FALSE;

	TriggerMode triggerMode;
	error = m_cam.GetTriggerMode(&triggerMode);
	if (error != PGRERROR_OK)
		return FALSE;

	triggerMode.onOff = true;
	triggerMode.source = 0;

	error = m_cam.SetTriggerMode(&triggerMode);
	if (error != PGRERROR_OK)
		return FALSE;

	FC2Config config;
	error = m_cam.GetConfiguration(&config);
	if (error != PGRERROR_OK)
		return FALSE;

	#ifdef BARCODE_CAM_CALLBACK_USE

		// Set the grab timeout to 1 seconds
		// TODO : actually this set to 300 msec. not 1 sec. - 251031, jhkim
		config.numBuffers = 30;						//사용할 버퍼의 개수를 지정.
		config.grabMode = BUFFER_FRAMES;			//버퍼 프레임 모드로 사용
		config.grabTimeout = 300;					// (msec)
		error = m_cam.SetConfiguration(&config);
		if (error != PGRERROR_OK)
			return FALSE;

	#else

		// Set the grab timeout to 300 mseconds
		config.grabTimeout = 300;
		error = m_cam.SetConfiguration(&config);
		if (error != PGRERROR_OK)
			return FALSE;

	#endif

#endif

	return TRUE;
}

void CPointGrayCam::UnInitialize()
{
#ifdef BARCODE_CAM_POINTGRAY_USE

	m_cam.Disconnect();

#endif
}

BOOL CPointGrayCam::StartGrab()
{
#ifdef BARCODE_CAM_POINTGRAY_USE

	FlyCapture2::Error error;

	#ifdef BARCODE_CAM_CALLBACK_USE
		error = m_cam.StartCapture(OnImageGrabbed, this);
		if(error != PGRERROR_OK)
		{
			return FALSE;
		}
	#else
		error = m_cam.StartCapture();
		if (error != PGRERROR_OK)
			return FALSE;
	#endif

#endif

	return TRUE;
}


#ifdef BARCODE_CAM_CALLBACK_USE

BOOL CPointGrayCam::GetGrabImage(Hobject *pHImage)
{
	try
	{

#ifndef BARCODE_CAM_POINTGRAY_USE
		gen_image1(pHImage, "byte", PGCAM_IMAGE_WIDTH, PGCAM_IMAGE_HEIGHT, 0);
		return TRUE;
#else

		FlyCapture2::Error error;

		int iGrabCount = 0;
		
		while(1)
		{
			Sleep(10);
		
			if(mbGogingRefresh)
				break;
		
			if(iGrabCount > 30)
				break;
		
			iGrabCount++;
		}

		if(!mbGogingRefresh)
		{
			pHImage->Reset();

			CString sErrMsg;
			sErrMsg.Format("###바코드### [GRAB ERROR] Callback 함수에 영상 트리거 안 들어옴");
			THEAPP.SaveLog(sErrMsg);
		
			return FALSE;
		}

		BYTE *BpImage;
		char caType[256];
		Hlong lWidth, lHeight;

		if (THEAPP.m_pGFunction->ValidHImage(*pHImage))
			get_image_pointer1(*pHImage, (Hlong*)&BpImage, caType, &lWidth, &lHeight);
		else
		{
			pHImage->Reset();
			gen_image1(pHImage, "byte", PGCAM_IMAGE_WIDTH, PGCAM_IMAGE_HEIGHT, 0);
			get_image_pointer1(*pHImage, (Hlong*)&BpImage, caType, &lWidth, &lHeight);
		}

		memcpy(BpImage, mpImageBuf[m_iGrabNumber - 1], PGCAM_IMAGE_WIDTH*PGCAM_IMAGE_HEIGHT);
#endif

		return TRUE;
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CPointGrayCam::GetGrabImage] : %s", except.message);
		THEAPP.SaveLog(str);
		return FALSE;
	}
}

#else

BOOL CPointGrayCam::GetGrabImage(Hobject *pHImage)
{
	try
	{

#ifndef BARCODE_CAM_POINTGRAY_USE
		gen_image1(pHImage, "byte", PGCAM_IMAGE_WIDTH, PGCAM_IMAGE_HEIGHT, 0);
		return TRUE;
#else

		FlyCapture2::Error error;

		BYTE *BpImage;
		char caType[256];
		Hlong lWidth, lHeight;

		if (THEAPP.m_pGFunction->ValidHImage(*pHImage))
			get_image_pointer1(*pHImage, (Hlong*)&BpImage, caType, &lWidth, &lHeight);
		else
		{
			pHImage->Reset();
			gen_image1(pHImage, "byte", PGCAM_IMAGE_WIDTH, PGCAM_IMAGE_HEIGHT, 0);
			get_image_pointer1(*pHImage, (Hlong*)&BpImage, caType, &lWidth, &lHeight);
		}


		error = m_cam.RetrieveBuffer(&m_rawImage);
		if (error != PGRERROR_OK)
		{
			pHImage->Reset();

			CString sErrMsg;
			sErrMsg.Format("###바코드### [GRAB ERROR] RetrieveBuffer ErrorNumber-%s", error.GetDescription());
			THEAPP.SaveLog(sErrMsg);
			return FALSE;
		}

		BYTE *pCdata = m_rawImage.GetData();
		memcpy(BpImage, pCdata, PGCAM_IMAGE_WIDTH*PGCAM_IMAGE_HEIGHT);

#endif

		return TRUE;
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CPointGrayCam::GetGrabImage] : %s", except.message);
		THEAPP.SaveLog(str);
		return FALSE;
	}
}

#endif

BOOL CPointGrayCam::EndGrab()
{
#ifdef BARCODE_CAM_POINTGRAY_USE

	FlyCapture2::Error error;

	error = m_cam.StopCapture();
	if (error != PGRERROR_OK)
		return FALSE;

#endif

	return TRUE;
}

BOOL CPointGrayCam::ResetCamera()
{
	CString strMsg;

	BOOL bRet = TRUE;

	bRet = EndGrab();
	//if (bRet==FALSE)
	//{
	//	strMsg.Format("[ResetCamera] EndGrab() Error");
	//	THEAPP.SaveLog(strMsg);

	//	return FALSE;
	//}

	UnInitialize();

	Sleep(500);

	mbGogingRefresh = FALSE;
	m_iGrabNumber = 0;

	bRet = Initialize();
	if (bRet==FALSE)
	{
		strMsg.Format("[ResetCamera] Initialize() Error");
		THEAPP.SaveLog(strMsg);

		return FALSE;
	}

	bRet = StartGrab();
	if (bRet==FALSE)
	{
		strMsg.Format("[ResetCamera] StartGrab() Error");
		THEAPP.SaveLog(strMsg);

		return FALSE;
	}

	return TRUE;
}