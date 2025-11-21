#include "stdafx.h"
#include "CrevisCam.h"
#include "uScan.h"


#if defined BARCODE_CAM_CALLBACK_USE && defined BARCODE_CAM_CREVIS_USE
INT32 GrabFunction(INT32 event, void* pImage, void* pUserDefine)
{
	CCrevisCam* pCrevisCam = (CCrevisCam*)pUserDefine;
	CString strMsg;

	switch(event)
	{
	case EVENT_NEW_IMAGE:

		if(pCrevisCam->m_iGrabNumber < SURFACE_COUNT)
		{
			memcpy(pCrevisCam->mpImageBuf[pCrevisCam->m_iGrabNumber], pImage, pCrevisCam->m_BufferSize);
			pCrevisCam->m_iGrabNumber++; 
		}
		else
		{
			pCrevisCam->m_iGrabNumber = 0;

			memcpy(pCrevisCam->mpImageBuf[pCrevisCam->m_iGrabNumber], pImage, pCrevisCam->m_BufferSize);
			pCrevisCam->m_iGrabNumber++; 
		}

		pCrevisCam->mbGogingRefresh = TRUE;

		break;

	case EVENT_GRAB_ERROR:		
		break;
	}	
	return 0;
}
#endif

CCrevisCam::CCrevisCam(void)
{

	m_hDevice = -1;			 
	m_Width = PGCAM_IMAGE_WIDTH;			 
	m_Height = PGCAM_IMAGE_HEIGHT;
	m_BufferSize = m_Width * m_Height;
	m_IsOpened = FALSE;;

#ifdef BARCODE_CAM_CALLBACK_USE
	mbGogingRefresh = FALSE;
	m_iGrabNumber = 0;
	for(int i = 0 ; i < SURFACE_COUNT ; i++)	
	{
		mpImageBuf[i] = NULL;;
	}
#endif
}


CCrevisCam::~CCrevisCam(void)
{
}



BOOL CCrevisCam::Initialize()
{
#ifdef BARCODE_CAM_CREVIS_USE

	INT32 status = MCAM_ERR_SUCCESS;
	UINT32 camNum;
	CString strErr;

	//Initialze System
	status = ST_InitSystem();
	if(status != MCAM_ERR_SUCCESS)
	{	
		strErr.Format(_T("Initialze system failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

	// Update Device List
	status = ST_UpdateDevice();
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Update device list failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

	// Get Available Camera number
	ST_GetAvailableCameraNum(&camNum);
	if(camNum <= 0) 
	{
		strErr.Format(_T("No available camera."));
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

	// camera open
	status = ST_OpenDevice(THEAPP.Struct_PreferenceStruct.m_iBarcodeCamSerial, &m_hDevice);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Open device failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}	

	// Set PixelFormat : Mono8
	status = ST_SetEnumReg(m_hDevice, MCAM_PIXEL_FORMAT, PIXEL_FORMAT_MONO8);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Write Register failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

	//Get Width
	status = ST_GetIntReg(m_hDevice, MCAM_WIDTH, &m_Width);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Read Register failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}	

	//Get Height
	status = ST_GetIntReg(m_hDevice, MCAM_HEIGHT, &m_Height);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Read Register failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

	m_BufferSize = m_Width * m_Height;

	mbGogingRefresh = FALSE;
	m_iGrabNumber = 0;

	for(int i = 0 ; i < SURFACE_COUNT ; i++)	
	{
		mpImageBuf[i]= new BYTE[m_BufferSize];
	}

#ifdef BARCODE_CAM_CALLBACK_USE

	ST_SetCallbackFunction(m_hDevice, EVENT_NEW_IMAGE, GrabFunction, this);

	// Set Trigger Mode : On
	status = ST_SetEnumReg(m_hDevice, MCAM_TRIGGER_MODE, TRIGGER_MODE_ON);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Write Register failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}
	//TriggerSource => Line1
	status = ST_SetEnumReg(m_hDevice, MCAM_TRIGGER_SOURCE, TRIGGER_SOURCE_LINE1);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Write Register failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

	//TriggerActivation => Falling Edge
	status = ST_SetEnumReg(m_hDevice, MCAM_TRIGGER_ACTIVATION, TRIGGER_ACTIVATION_FALLING_EDGE);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Write Register failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

	//TriggerDelay => 1us
	status = ST_SetFloatReg(m_hDevice, MCAM_TRIGGER_DELAY, 1.0f);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Write Register failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

	//GrabTimeout => 5s
	status = ST_SetGrabTimeout(m_hDevice, 5000);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Write Register failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

#else

	// Set Trigger Mode : OFF
	status = ST_SetEnumReg(m_hDevice, MCAM_TRIGGER_SOURCE, TRIGGER_MODE_OFF);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Write Register failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

#endif

	m_IsOpened = TRUE;

#endif

	return TRUE;
}

void CCrevisCam::UnInitialize()
{
#ifdef BARCODE_CAM_CREVIS_USE

	INT32 status = MCAM_ERR_SUCCESS;
	CString strErr;

	if(m_IsOpened == FALSE)
		return;	

	ST_CloseDevice(m_hDevice);	

	ST_FreeSystem();

	for(int i = 0 ; i < SURFACE_COUNT ; i++)	
	{
		if(mpImageBuf[i])
			delete[] mpImageBuf[i];
	}

	m_IsOpened = FALSE;

#endif
}

BOOL CCrevisCam::StartGrab()
{
#ifdef BARCODE_CAM_CREVIS_USE

	INT32 status = MCAM_ERR_SUCCESS;
	CString strErr;

	// Acquistion Start
	status = ST_AcqStart(m_hDevice);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Acquisition start failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}

#endif

	return TRUE;
}

#ifdef BARCODE_CAM_CALLBACK_USE

BOOL CCrevisCam::GetGrabImage(Hobject *pHImage)
{
	try
	{

#ifndef BARCODE_CAM_CREVIS_USE
		gen_image1(pHImage, "byte", m_Width, m_Height, 0);
		return TRUE;
#else
		INT32 status = MCAM_ERR_SUCCESS;
		CString strErr;
		CString sErrMsg;

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

			sErrMsg.Format("###바코드### [GRAB ERROR] Callback 함수에 영상 트리거 안 들어옴");
			THEAPP.SaveLog(sErrMsg);

			return FALSE;
		}

		BYTE *BpImage;
		char caType[256];
		Hlong lWidth, lHeight;

		if (THEAPP.m_pGFunction->ValidHImage(*pHImage))
		{
			get_image_pointer1(*pHImage, (Hlong*)&BpImage, caType, &lWidth, &lHeight);
		}
		else
		{
			pHImage->Reset();
			gen_image1(pHImage, "byte", m_Width, m_Height, 0);
			get_image_pointer1(*pHImage, (Hlong*)&BpImage, caType, &lWidth, &lHeight);
		}

		memcpy(BpImage, mpImageBuf[m_iGrabNumber - 1], lWidth * lHeight);

#endif
		return TRUE;
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CCrevisCam::GetGrabImage] : %s", except.message);
		THEAPP.SaveLog(str);
		return FALSE;
	}
}

#else

BOOL CCrevisCam::GetGrabImage(Hobject *pHImage)
{
	try
	{

#ifndef BARCODE_CAM_CREVIS_USE
		gen_image1(pHImage, "byte", m_Width, m_Height, 0);
		return TRUE;
#else

		INT32 status = MCAM_ERR_SUCCESS;
		CString strErr;

		BYTE *BpImage;
		char caType[256];
		Hlong lWidth, lHeight;

		if (THEAPP.m_pGFunction->ValidHImage(*pHImage))
			get_image_pointer1(*pHImage, (Hlong*)&BpImage, caType, &lWidth, &lHeight);
		else
		{
			pHImage->Reset();
			gen_image1(pHImage, "byte", m_Width, m_Height, 0);
			get_image_pointer1(*pHImage, (Hlong*)&BpImage, caType, &lWidth, &lHeight);
		}


		// Grab Function
		void *pCdata;
		status = ST_GrabImage(m_hDevice, pCdata, m_BufferSize);	
		if(status != MCAM_ERR_SUCCESS)
		{
			pHImage->Reset();

			CString sErrMsg;
			sErrMsg.Format("###바코드### [GRAB ERROR] ST_GrabImage ErrorNumber-%s", error.GetDescription());
			THEAPP.SaveLog(sErrMsg);
			return FALSE;
		}

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

BOOL CCrevisCam::EndGrab()
{
#ifdef BARCODE_CAM_CREVIS_USE

	INT32 status = MCAM_ERR_SUCCESS;
	CString strErr;

	// Acquistion Stop
	status = ST_AcqStop(m_hDevice);
	if(status != MCAM_ERR_SUCCESS)
	{
		strErr.Format(_T("Acquisition stop failed : %d"), status);
		THEAPP.SaveLog(strErr);
		return FALSE;
	}
#endif

	return TRUE;
}

BOOL CCrevisCam::ResetCamera()
{
	CString strMsg;

	BOOL bRet = TRUE;

	bRet = EndGrab();

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
