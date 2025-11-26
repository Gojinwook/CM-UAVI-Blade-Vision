#include "stdafx.h"
#include "uScan.h"
#include "CameraManager.h"

CCameraManager* CCameraManager::m_pInstance = NULL;

CCameraManager* CCameraManager::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CCameraManager();
	}
	return m_pInstance;
}

void CCameraManager::DeleteInstance()
{
	for (int i=0; i<MAX_IMAGE_TAB; i++)
	{
		m_hoCallBackImage[i].Reset();
		gen_empty_obj(&(m_hoCallBackImage[i]));
		MbufFree(MilImageBuf[i]);
	}

	MdigFree(MilDigitizer);
	MsysFree(MilSystem);
	m_bGrabDone = TRUE;

	for (int i = 0; i < MAX_BARCODE_GRAB; i++)
	{
		m_hoCallBackImage_BC1[i].Reset();
		gen_empty_obj(&(m_hoCallBackImage_BC1[i]));
		MbufFree(MilImageBuf_BC1[i]);
	}
	MdigFree(MilDigitizer_BC1);
	m_bGrabDone_BC1 = TRUE;

	for (int i = 0; i < MAX_BARCODE_GRAB; i++)
	{
		m_hoCallBackImage_BC2[i].Reset();
		gen_empty_obj(&(m_hoCallBackImage_BC2[i]));
		MbufFree(MilImageBuf_BC2[i]);
	}
	MdigFree(MilDigitizer_BC2);
	m_bGrabDone_BC2 = TRUE;

	MsysFree(MilSystem_BC);

	MappFree(MilApplication);

	SAFE_DELETE(m_pInstance);
}

CCameraManager::CCameraManager(void)
{
	ImageGrabCount =0;
	m_bReGrab = FALSE;
	m_bGrabDone = TRUE;

	//0524
	m_bUseGrabErrFIltering = FALSE;
	m_iGrabErrDarkAreaGVLimit = 20;
	m_iGrabErrBrightAreaGVLimit = 255;

	switch (THEAPP.m_iMachineInspType)
	{
	case MACHINE_NORMAL:
		m_lCamImageWidth = 1600;
		m_lCamImageHeight = 1600;
		break;
	case MACHINE_SIDEFILL:
		m_lCamImageWidth = 3008;
		m_lCamImageHeight = 3008;
		break;
	case MACHINE_BRACKET:
		m_lCamImageWidth = 4096;
		m_lCamImageHeight = 3072;
		break;
	case MACHINE_WELDING:
		m_lCamImageWidth = 5120;
		m_lCamImageHeight = 3600;
		break;
	default:
		m_lCamImageWidth = 1600;
		m_lCamImageHeight = 1600;
		break;
	}

	for (int i = 0; i < MAX_BARCODE_GRAB; i++)
	{
		m_hoCallBackImage_BC1[i].Reset();
		gen_empty_obj(&(m_hoCallBackImage_BC1[i]));
	}
	m_bGrabDone_BC1 = TRUE;

	for (int i = 0; i < MAX_BARCODE_GRAB; i++)
	{
		m_hoCallBackImage_BC2[i].Reset();
		gen_empty_obj(&(m_hoCallBackImage_BC2[i]));
	}
	m_bGrabDone_BC2 = TRUE;
	
	m_lBarcodeCamImageWidth = 2048;
	m_lBarcodeCamImageHeight = 1088;

}


CCameraManager::~CCameraManager(void)
{
}

MIL_INT MPTYPE GrabStart(MIL_INT HookType, MIL_ID HookId, void MPTYPE *UserDataPtr)
{
	CCameraManager *pCameraManager = (CCameraManager *) UserDataPtr;
	
	try{

	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CCameraManager GrabStart] : %s", except.message);
		THEAPP.SaveLog(str);
	}
	return 0;
}

MIL_INT MPTYPE GrabEnd(MIL_INT HookType, MIL_ID HookId, void MPTYPE *UserDataPtr)
{
	/* Retrieve the MIL_ID of the grabbed buffer. */
	CCameraManager *pCameraManager = (CCameraManager *) UserDataPtr;

	THEAPP.SaveLog ("GrabEnd: ����");

	if((THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_ADMIN)||(THEAPP.m_iModeSwitch == MODE_INSPECT_VIEW_OPERATOR)) // �ڵ��˻�ÿ���
	{
		// 24.07.31 - v2656 - �׷� �Ϸ� ó�� �ݹ��Լ� �� �����̻��� �˰����� ����(����ó�� �ӵ� �������� �̹��� �׷� ���� ���� �� ���α׷� �ٿ�)

		if((THEAPP.m_pHandlerService->m_nInspectPCStatus==2) || (THEAPP.m_pHandlerService->m_nInspectPCStatus==4) || (THEAPP.m_pHandlerService->m_nInspectPCStatus==5)) // Inspect Run ���� �̸�....
		{
			pCameraManager->ImageGrabCount+=1;
		}

		pCameraManager->m_bGrabDone = TRUE;

		return 0;
	}

	try
	{
		THEAPP.SaveLog ("GrabEnd: ����");

		if(THEAPP.m_pGFunction->ValidHImage(pCameraManager->m_hoCallBackImage[0]))
		{
			Hobject HCropPartImage;
			Hobject HRectangleRgn, HThresRectangleRgn;

			if (THEAPP.Struct_PreferenceStruct.m_iCamFOVType==CAM_FOV_CROP)
			{
				crop_part (pCameraManager->m_hoCallBackImage[0], &HCropPartImage, GRAB_Y_OFFSET, GRAB_X_OFFSET, GRAB_Y_MAX, GRAB_X_MAX); 
				copy_image(HCropPartImage, &THEAPP.m_pInspectAdminViewDlg->m_pHImage); // ��ŷ�� �̹����� ���ε��÷��� �̹������ۿ� ����
			}
			else
			{
				copy_image(pCameraManager->m_hoCallBackImage[0], &THEAPP.m_pInspectAdminViewDlg->m_pHImage); // ��ŷ�� �̹����� ���ε��÷��� �̹������ۿ� ����
			}

			THEAPP.SaveLog ("Grab: ���� �׷� ����");

			if(THEAPP.m_iModeSwitch==MODE_ADMIN_TEACH_VIEW) // ƼĪȭ��....
			{
				THEAPP.m_pInspectAdminViewDlg->InvalidateRect(false);
				pCameraManager->TeachingImageGrabSave();  // ƼĪ �̹��� ����  => ƼĪȭ�鿡 �־�߸� �����.
			}
		}
		else
		{
			THEAPP.SaveLog("���� �׷� ���� - Invalid �ٽ� ����");
			pCameraManager->m_bReGrab = TRUE;
		}

		pCameraManager->m_bGrabDone = TRUE;

		return 0;
	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CCameraManager CallBack{GrabEnd}] : %s", except.message);
		THEAPP.SaveLog(str);
		THEAPP.SaveLog("���� �׷� ����");
		pCameraManager->m_bReGrab = TRUE;
		pCameraManager->m_bGrabDone = TRUE;
		return 0;
	}
}

MIL_INT MPTYPE GrabEnd_BC1(MIL_INT HookType, MIL_ID HookId, void MPTYPE *UserDataPtr)
{
	/* Retrieve the MIL_ID of the grabbed buffer. */
	CCameraManager *pCameraManager = (CCameraManager *)UserDataPtr;

	THEAPP.SaveLog("Barcode1 GrabEnd");

	pCameraManager->m_bGrabDone_BC1 = TRUE;

	return 0;
}

MIL_INT MPTYPE GrabEnd_BC2(MIL_INT HookType, MIL_ID HookId, void MPTYPE *UserDataPtr)
{
	/* Retrieve the MIL_ID of the grabbed buffer. */
	CCameraManager *pCameraManager = (CCameraManager *)UserDataPtr;

	THEAPP.SaveLog("Barcode2 GrabEnd");

	pCameraManager->m_bGrabDone_BC2 = TRUE;

	return 0;
}

bool CCameraManager::InitGrabInterface()
{
	MIL_INT SizeX = 0; 
	MIL_INT SizeY = 0;

	MappAlloc(M_DEFAULT, &MilApplication);

	MsysAlloc(M_DEFAULT, M_SYSTEM_RAPIXOCXP, M_DEV0, M_DEFAULT, &MilSystem);
	THEAPP.SaveLog("RapixoCXP Grabber Init");

	MdigAlloc(MilSystem, M_DEV0, THEAPP.GetWorkingDirectory()+"\\Data\\"+"Inspect.dcf", M_DEFAULT, &MilDigitizer);
	
	MdigInquire(MilDigitizer, M_SOURCE_SIZE_X, &SizeX);
	MdigInquire(MilDigitizer, M_SOURCE_SIZE_Y, &SizeY);

	m_lCamImageWidth = SizeX;
	m_lCamImageHeight = SizeY;
	
	INT64 imageSize;
	imageSize = SizeX * SizeY * 1; // *1 : Gray?
	
	int i;

	for (i=0; i<MAX_IMAGE_TAB; i++)
	{
		MbufAlloc2d(MilSystem, SizeX, SizeY, 8L+M_UNSIGNED, M_IMAGE+M_DISP+M_GRAB, &(MilImageBuf[i]));
		MbufClear(MilImageBuf[i], 0);
	}

	BYTE *addr;

	for (i=0; i<MAX_IMAGE_TAB; i++)
	{
		MbufInquire(MilImageBuf[i], M_HOST_ADDRESS, &addr);
		gen_image1_extern(&(m_hoCallBackImage[i]), "byte", m_lCamImageWidth, m_lCamImageHeight, (Hlong)addr, NULL);
	}
	
	MdigControl(MilDigitizer, M_GRAB_TIMEOUT, M_INFINITE);
	MdigControl(MilDigitizer, M_CAMERALINK_CC1_SOURCE, M_GRAB_EXPOSURE+M_TIMER1);
	
	MdigControl(MilDigitizer, M_GRAB_MODE, M_ASYNCHRONOUS);
	MdigControl(MilDigitizer, M_GRAB_TRIGGER_MODE, M_DEFAULT);

	MdigHookFunction(MilDigitizer, M_GRAB_START, GrabStart, this);
	MdigHookFunction(MilDigitizer, M_GRAB_END, GrabEnd, this);

	// Barcode
	MsysAlloc(M_SYSTEM_RADIENTEVCL, M_DEV0, M_DEFAULT, &MilSystem_BC);
	THEAPP.SaveLog("Radient EVCL Init");

	MdigAlloc(MilSystem_BC, M_DEV0, THEAPP.GetWorkingDirectory() + "\\Data\\" + "Barcode1.dcf", M_DEFAULT, &MilDigitizer_BC1);
	MdigAlloc(MilSystem_BC, M_DEV1, THEAPP.GetWorkingDirectory() + "\\Data\\" + "Barcode2.dcf", M_DEFAULT, &MilDigitizer_BC2);

	MdigInquire(MilDigitizer_BC1, M_SOURCE_SIZE_X, &SizeX);
	MdigInquire(MilDigitizer_BC1, M_SOURCE_SIZE_Y, &SizeY);

	m_lBarcodeCamImageWidth = SizeX;
	m_lBarcodeCamImageHeight = SizeY;

	for (i = 0; i < MAX_BARCODE_GRAB; i++)
	{
		MbufAlloc2d(MilSystem_BC, SizeX, SizeY, 8L + M_UNSIGNED, M_IMAGE + M_DISP + M_GRAB, &(MilImageBuf_BC1[i]));
		MbufClear(MilImageBuf_BC1[i], 0);

		MbufInquire(MilImageBuf_BC1[i], M_HOST_ADDRESS, &addr);
		gen_image1_extern(&(m_hoCallBackImage_BC1[i]), "byte", m_lBarcodeCamImageWidth, m_lBarcodeCamImageHeight, (Hlong)addr, NULL);

		MbufAlloc2d(MilSystem_BC, SizeX, SizeY, 8L + M_UNSIGNED, M_IMAGE + M_DISP + M_GRAB, &(MilImageBuf_BC2[i]));
		MbufClear(MilImageBuf_BC2[i], 0);

		MbufInquire(MilImageBuf_BC2[i], M_HOST_ADDRESS, &addr);
		gen_image1_extern(&(m_hoCallBackImage_BC2[i]), "byte", m_lBarcodeCamImageWidth, m_lBarcodeCamImageHeight, (Hlong)addr, NULL);
	}

	MdigControl(MilDigitizer_BC1, M_GRAB_TIMEOUT, M_INFINITE);
	MdigControl(MilDigitizer_BC1, M_GRAB_MODE, M_ASYNCHRONOUS);
	MdigControl(MilDigitizer_BC1, M_GRAB_TRIGGER_MODE, M_DEFAULT);
	MdigHookFunction(MilDigitizer_BC1, M_GRAB_END, GrabEnd_BC1, this);

	MdigControl(MilDigitizer_BC2, M_GRAB_TIMEOUT, M_INFINITE);
	MdigControl(MilDigitizer_BC2, M_GRAB_MODE, M_ASYNCHRONOUS);
	MdigControl(MilDigitizer_BC2, M_GRAB_TRIGGER_MODE, M_DEFAULT);
	MdigHookFunction(MilDigitizer_BC2, M_GRAB_END, GrabEnd_BC2, this);
	   
	return true;
}

BOOL CCameraManager::GrabBarcode(int iCamIdx, int iGrabIdx)
{
	DWORD dwGrabStart = 0, dwGrabEnd = 0;

	if (iCamIdx == 0)
	{
		m_bGrabDone_BC1 = FALSE;
		MdigGrab(MilDigitizer_BC1, MilImageBuf_BC1[iGrabIdx]);

		THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_BARCODE, 5);

		Sleep(20);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (m_bGrabDone_BC1)
				break;

			dwGrabEnd = GetTickCount();

			if ((dwGrabEnd - dwGrabStart) > 50)
			{
				MdigControl(MilDigitizer_BC1, M_GRAB_ABORT, M_DEFAULT);
				return FALSE;
			}

			Sleep(1);
		}
	}
	else if (iCamIdx == 1)
	{
		m_bGrabDone_BC2 = FALSE;
		MdigGrab(MilDigitizer_BC2, MilImageBuf_BC2[iGrabIdx]);

		THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_BARCODE, 5);

		Sleep(20);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (m_bGrabDone_BC2)
				break;

			dwGrabEnd = GetTickCount();

			if ((dwGrabEnd - dwGrabStart) > 50)
			{
				MdigControl(MilDigitizer_BC2, M_GRAB_ABORT, M_DEFAULT);
				return FALSE;
			}

			Sleep(1);
		}
	}

	return TRUE;
}

BOOL CCameraManager::GrabBarcodeImage(int iCamIdx, Hobject *pHBarcodeImage)
{
	DWORD dwGrabStart = 0, dwGrabEnd = 0;

	if (iCamIdx == 0)
	{
		m_bGrabDone_BC1 = FALSE;
		MdigGrab(MilDigitizer_BC1, MilImageBuf_BC1[0]);

		THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_BARCODE, 5);

		Sleep(20);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (m_bGrabDone_BC1)
				break;

			dwGrabEnd = GetTickCount();

			if ((dwGrabEnd - dwGrabStart) > 50)
			{
				MdigControl(MilDigitizer_BC1, M_GRAB_ABORT, M_DEFAULT);
				return FALSE;
			}

			Sleep(1);
		}
	}
	else if (iCamIdx == 1)
	{
		m_bGrabDone_BC2 = FALSE;
		MdigGrab(MilDigitizer_BC2, MilImageBuf_BC2[0]);

		THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_BARCODE, 5);

		Sleep(20);

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (m_bGrabDone_BC2)
				break;

			dwGrabEnd = GetTickCount();

			if ((dwGrabEnd - dwGrabStart) > 50)
			{
				MdigControl(MilDigitizer_BC2, M_GRAB_ABORT, M_DEFAULT);
				return FALSE;
			}

			Sleep(1);
		}
	}

	return TRUE;
}

void CCameraManager::CallHookFunction(int iGrabBufIdx)
{
	m_bGrabDone = FALSE;
	MbufClear(MilImageBuf[iGrabBufIdx],0);
	m_bReGrab = FALSE;

	MdigGrab(MilDigitizer, MilImageBuf[iGrabBufIdx]);
}

void CCameraManager::TeachingImageGrabSave()
{
	if(THEAPP.m_iModeSwitch==MODE_ADMIN_TEACH_VIEW)
	{
		if(THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_iTeachingGrab>=0)
		{
			if(THEAPP.m_pModelDataManager->m_sModelName == ".")
				return;

			CString FolderName;
			if(!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) //Ver2629
				FolderName =  THEAPP.GetWorkingDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName+"\\SW\\TeachImage\\";
			else
				FolderName =  THEAPP.GetWorkingEvmsDirectory()+"\\Model\\"+THEAPP.m_pModelDataManager->m_sModelName+"\\SW\\TeachImage\\";

			THEAPP.m_FileBase.CreatePath(FolderName,TRUE); //Ver2629

			CString FileName;

			switch(THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_iTeachingGrab)
			{
			case 0:
				{
					FileName = FolderName+"TeachingImage";
					break;
				}
			default:
				{
					FileName.Format("%sGrab_%s", FolderName, g_sImageName[THEAPP.m_iMachineInspType][THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_iTeachingGrab-1]);
					break;
				}
			}

			m_strSaveFileName = FileName;

			if(THEAPP.m_pGFunction->ValidHImage(m_hoCallBackImage[0]))
			{
				Hobject HImage;

				if (THEAPP.Struct_PreferenceStruct.m_iCamFOVType==CAM_FOV_CROP)
				{
					crop_part (m_hoCallBackImage[0], &HImage, GRAB_Y_OFFSET, GRAB_X_OFFSET, GRAB_Y_MAX, GRAB_X_MAX); 
					write_image(HImage, "bmp", 0, m_strSaveFileName);
				}
				else
					write_image(m_hoCallBackImage[0], "bmp", 0, m_strSaveFileName);
			}

			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_iTeachingGrab = -1;
		}
	}
}

void CCameraManager::GrabErrorPostProcess()
{
	MdigControl(MilDigitizer, M_GRAB_ABORT, M_DEFAULT);
	m_bReGrab = TRUE;
	m_bGrabDone = TRUE;
	Sleep(50);
}

// 24.07.31 - v2656 - RawImage ť, ������ �߰� - LeeGW START
BOOL CCameraManager::CopyRawImageQueue(int ImageCount, int nModuleNo, Hobject *pHImageOri, CString sBarcodeName)
{
	try{
		if (THEAPP.m_pGFunction->ValidHImage(*pHImageOri)==FALSE)
			return FALSE;

		CString ImageFileName;

		Hobject HSaveImage;
		gen_empty_obj(&HSaveImage);

		if(THEAPP.Struct_PreferenceStruct.m_bSaveRawImageMerge && ImageCount == -100)
		{
			copy_image(*pHImageOri, &HSaveImage);
			ImageFileName.Format("Module%d_Combine_%s", nModuleNo, sBarcodeName);

			if (THEAPP.Struct_PreferenceStruct.m_bSaveBMP == TRUE)
				THEAPP.m_pSaveManager->SaveImage(HSaveImage, "bmp", THEAPP.m_FileBase.m_strOriImageFolderPrev + "\\" + ImageFileName);
			else
				THEAPP.m_pSaveManager->SaveImage(HSaveImage, "jpg", THEAPP.m_FileBase.m_strOriImageFolderPrev + "\\" + ImageFileName);

			return TRUE;
		}

		double dResizeRatio;
		dResizeRatio = THEAPP.Struct_PreferenceStruct.m_dSaveRawImageResizeRatio;

		try 
		{
			if(THEAPP.Struct_PreferenceStruct.m_bSaveRawImageResize && ImageCount != 10)
			{
				if (dResizeRatio < 1)
					zoom_image_factor(*pHImageOri, &HSaveImage, dResizeRatio, dResizeRatio, "none");
				else
					copy_image(*pHImageOri, &HSaveImage);
			}
			else
			{
				copy_image(*pHImageOri, &HSaveImage);
			}
		}
		catch(HException &except)
		{
			CString str; str.Format("Halcon Exception [CopyRawImageQueue : zoom_image_factor] : %s", except.message); THEAPP.SaveLog(str);
			copy_image(*pHImageOri, &HSaveImage);
		}

		if (ImageCount == 10)
			ImageFileName.Format("Module%d_Barcode_%s", nModuleNo, sBarcodeName);
		else
			ImageFileName.Format("Module%d_%s_%s", nModuleNo, g_sImageName[THEAPP.m_iMachineInspType][ImageCount - 1], sBarcodeName);
	
		if (THEAPP.Struct_PreferenceStruct.m_bSaveBMP == TRUE)
			THEAPP.m_pSaveManager->SaveImage(HSaveImage, "bmp", THEAPP.m_FileBase.m_strOriImageFolderPrev + "\\" + ImageFileName);
		else
			THEAPP.m_pSaveManager->SaveImage(HSaveImage, "jpg", THEAPP.m_FileBase.m_strOriImageFolderPrev + "\\" + ImageFileName);

		return TRUE;
	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CCameraManager CopyRawImageQueue] : %s", except.message);
		THEAPP.SaveLog(str);

		return FALSE;
	}
}

void CCameraManager::InspectOriImageGrabSave(int ImageCount, int nModuleNo, Hobject HImageOri, CString sBarcodeName)
{
	try{

		SYSTEMTIME time;
		GetLocalTime(&time);
		CString strFileFullName;
		CString ImageFileName;

		Hobject HImage;
		copy_image(HImageOri, &HImage);

		if(THEAPP.m_pGFunction->ValidHImage(HImage)) 
		{
			switch (ImageCount)
			{
			case 10:
			{
				ImageFileName.Format("Module%d_Barcode_%s", nModuleNo, sBarcodeName);
				strFileFullName = THEAPP.m_FileBase.m_strOriImageFolderPrev + "\\" + ImageFileName;
				write_image(HImage, "jpg", 0, strFileFullName);
				break;
			}
			default:
			{
				ImageFileName.Format("Module%d_%s_%s", nModuleNo, g_sImageName[THEAPP.m_iMachineInspType][ImageCount - 1], sBarcodeName);
				strFileFullName = THEAPP.m_FileBase.m_strOriImageFolderPrev + "\\" + ImageFileName;
				if (THEAPP.Struct_PreferenceStruct.m_bSaveBMP == TRUE)
				{
					write_image(HImage, "bmp", 0, strFileFullName);
				}
				else
				{
					write_image(HImage, "jpg", 0, strFileFullName);
				}
				break;
			}
			}
		}
	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CCameraManager InspectOriImageGrabSave] : %s", except.message);
		THEAPP.SaveLog(str);
	}

}

BOOL CCameraManager::GrabErrorCheck(int igc)
{
try{
	THEAPP.SaveLog ("GrabErrorCheck");

	if(THEAPP.m_pGFunction->ValidHImage(m_hoCallBackImage[igc])==FALSE)
	{
		THEAPP.SaveLog ("Grab Error ReGrab");
		
		return TRUE;
	}

	char type[30];
	BYTE *pImageData;
	Hlong lImageWidth, lImageHeight;
	Hlong lCamHeight, lCamWidth;
	lCamHeight = THEAPP.m_pCameraManager->GetCamImageHeight();
	lCamWidth = THEAPP.m_pCameraManager->GetCamImageWidth();

	return FALSE;
}

catch(HException &except)
{
	CString str;
	str.Format("Halcon Exception [CCameraManager::GrabErrorCheck]: %s", except.message);
	THEAPP.SaveLog(str);

	return FALSE;
}
}