#include "stdafx.h"
#include "AutoCalService.h"

#include "uScan.h"

UINT AutoCalLightThread(LPVOID lp);

CAutoCalService *CAutoCalService::m_pInstance = NULL;

CAutoCalService *CAutoCalService::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new CAutoCalService();
	}
	return m_pInstance;
}

void CAutoCalService::DeleteInstance()
{
	if (m_pInstance) delete m_pInstance;
	m_pInstance = NULL;
}

CAutoCalService::CAutoCalService(void)
{
	m_pInspectAlgorithm = new Algorithm[MAX_IMAGE_TAB];

	InitAutoCalResult();
}


CAutoCalService::~CAutoCalService(void)
{
	if (m_pInspectAlgorithm)
	{
		delete[] m_pInspectAlgorithm;
		m_pInspectAlgorithm = NULL;
	}
}

void CAutoCalService::InitAutoCalResult()
{
	for(int i = 0; i < MAX_LIGHT_CHANNEL; i++)
	{
		m_bAutoCal_Done[i] = false;

		m_iTeachCH_LV[i] = 0;
		m_iTeachCH_GV[i] = 0;
		m_iInspectCH_LV[i] = 0;
		m_iInspectCH_GV[i] = 0;
		m_bJudgeCH[i] = true;	
	}

	m_iTeachTOTAL_LV = 0;
	m_iTeachTOTAL_GV = 0;
	m_iInspectTOTAL_LV = 0;
	m_iInspectTOTAL_GV = 0;
	m_bJudgeTOTAL = true;


	m_bLightAutoCalResult = true;
	m_iLightAutoCalProgressPercent = 0;
}


void CAutoCalService::AutoCalLightStart(int iTeachingStep)
{
	m_iTeachingStep = iTeachingStep;

	THEAPP.m_pInspectAdminViewDlg->StopLive();
	Sleep(100);

	AfxBeginThread(AutoCalLightThread, this);
}

#include "LightAutoCalResultDlg.h"

UINT AutoCalLightThread(LPVOID lp)
{

	CAutoCalService* pAutoCalService = (CAutoCalService*)lp;

	try
	{
		CString sInspectLog;
		sInspectLog.Format("**** 조명 Auto Cal 시작 ****"); 
		THEAPP.SaveLog(sInspectLog);

		double dTimeStart = 0, dTimeEnd = 0;
		dTimeStart = GetTickCount();

		DWORD dwGrabStart = 0, dwGrabEnd = 0;
		BOOL bGrabFail = FALSE;

		////////////////////////////// 스캔 시작 ////////////////////////////////////

#ifdef INLINE_MODE

		int iInspectCH_LV[MAX_LIGHT_CHANNEL];
		int iInspectCH_GV[MAX_LIGHT_CHANNEL];

		int iInspectTotol_GV;

		for(int i = 0; i < MAX_LIGHT_CHANNEL; i++)
		{
			iInspectCH_LV[i] = 999;
			iInspectCH_GV[i] = 255;
		}
		iInspectTotol_GV =255;

		THEAPP.m_pTabControlDlg->m_pLightAutoCalResultDlg->m_ctrlProgressAutoCal.SetPos(THEAPP.m_pAutoCalService->m_iLightAutoCalProgressPercent);
		pAutoCalService->m_iLightAutoCalProgressPercent = 15;

		CString sGrabTime, sGrabTime_total;
		double tGrabStart = 0, tGrabEnd = 0;
		double tGrabStart_total = 0, tGrabEnd_total = 0;
		CString strLog;

		int iLVChannel1 = THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_1];
		int iLVChannel2 = THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_2];
		int iLVChannel3 = THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_3];
		int iLVChannel4 = THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_4];
		int iLVChannel5 = THEAPP.m_pModelDataManager->m_iInspLightBright[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_5];

		int iLVChannel1Start = iLVChannel1 - THEAPP.m_pModelDataManager->m_iLightValueStart[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
		int iLVChannel1End = iLVChannel1 + THEAPP.m_pModelDataManager->m_iLightValueEnd[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];

		int iLVChannel2Start = iLVChannel2 - THEAPP.m_pModelDataManager->m_iLightValueStart[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
		int iLVChannel2End = iLVChannel2 + THEAPP.m_pModelDataManager->m_iLightValueEnd[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];

		int iLVChannel3Start = iLVChannel3 - THEAPP.m_pModelDataManager->m_iLightValueStart[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
		int iLVChannel3End = iLVChannel3 + THEAPP.m_pModelDataManager->m_iLightValueEnd[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];

		int iLVChannel4Start = iLVChannel4 - THEAPP.m_pModelDataManager->m_iLightValueStart[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
		int iLVChannel4End = iLVChannel4 + THEAPP.m_pModelDataManager->m_iLightValueEnd[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];

		int iLVChannel5Start = iLVChannel5 - THEAPP.m_pModelDataManager->m_iLightValueStart[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
		int iLVChannel5End = iLVChannel5 + THEAPP.m_pModelDataManager->m_iLightValueEnd[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];

		int iLVChannelInterval = THEAPP.m_pModelDataManager->m_iLightValueInterval[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];

		int iPageIdx = THEAPP.m_pTabControlDlg->m_iCurrentTab - 1;

		if (iPageIdx >= LIGHTCTRL_PAGE_COUNT && iPageIdx < 0)
			iPageIdx = 0;

		if(iLVChannel1 > 0)
		{
			for(int iLV = iLVChannel1Start; iLV <= iLVChannel1End; iLV += iLVChannelInterval)
			{
				if(THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_ComPort.OpenPort( 1, 19200 ))
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[iPageIdx].uiChannel1 = iLV;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[iPageIdx].uiChannel2 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[iPageIdx].uiChannel3 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[iPageIdx].uiChannel4 = 0;

					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].SetIllumination(iPageIdx);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_ComPort.ClosePort();

					Sleep(100);
				}


				if(THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
				{
					THEAPP.m_pCameraManager->m_bGrabDone = FALSE;
					THEAPP.m_pCameraManager->CallHookFunction();

					if (THEAPP.m_pTabControlDlg->m_iCurrentTab - 1 < MAX_TRIGGER_TYPE)
						THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pTabControlDlg->m_iCurrentTab-1);

					Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);
				}

				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
						break;

					dwGrabEnd = GetTickCount();

					if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
					{
						THEAPP.m_pCameraManager->GrabErrorPostProcess();
						break;
					}
				}

				///////////////////////////////////////////////////////////////////////////
				double dAvgValue = 255;
				double dStd = 0;
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_pHImage))
				{
					Hobject HDomainRegion;
					get_domain(THEAPP.m_pInspectAdminViewDlg->m_pHImage, &HDomainRegion);

					intensity(HDomainRegion, THEAPP.m_pInspectAdminViewDlg->m_pHImage, &dAvgValue, &dStd);
				}


				int iLightAverageValue = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_1];
				if(abs((int)dAvgValue - iLightAverageValue) < iInspectCH_LV[LIGHT_CHANNEL_1])
				{
					iInspectCH_LV[LIGHT_CHANNEL_1] = iLV;
					iInspectCH_GV[LIGHT_CHANNEL_1] = dAvgValue;
				}
			}

			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_1] = iLVChannel1;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_1] = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_1];
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_1] = iInspectCH_LV[LIGHT_CHANNEL_1];
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_1] = iInspectCH_GV[LIGHT_CHANNEL_1];

			if(abs(THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_1] - THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_1]) > THEAPP.m_pModelDataManager->m_iLightValueInTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1])
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_1] = false;
			}
			else
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_1] = true;
			}
		}
		else
		{
			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_1] = 0;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_1] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_1] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_1] = 0;

			THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_1] = true;
		}

		THEAPP.m_pAutoCalService->m_bAutoCal_Done[LIGHT_CHANNEL_1] = true;


		THEAPP.m_pTabControlDlg->m_pLightAutoCalResultDlg->m_ctrlProgressAutoCal.SetPos(THEAPP.m_pAutoCalService->m_iLightAutoCalProgressPercent);
		pAutoCalService->m_iLightAutoCalProgressPercent = 30;

		if(iLVChannel2 > 0)
		{
			for(int iLV = iLVChannel2Start; iLV <= iLVChannel2End; iLV += iLVChannelInterval)
			{
				if(THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(2, 19200 ))
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel1 = iLV;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel2 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel3 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel4 = 0;

					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination(iPageIdx);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();

					Sleep(100);
				}


				if(THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
				{
					THEAPP.m_pCameraManager->m_bGrabDone = FALSE;
					THEAPP.m_pCameraManager->CallHookFunction();

					if (THEAPP.m_pTabControlDlg->m_iCurrentTab - 1 < MAX_TRIGGER_TYPE)
						THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pTabControlDlg->m_iCurrentTab - 1);

					Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);
				}


				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
						break;

					dwGrabEnd = GetTickCount();

					if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
					{
						THEAPP.m_pCameraManager->GrabErrorPostProcess();
						break;
					}
				}

				///////////////////////////////////////////////////////////////////////////
				double dAvgValue = 255;
				double dStd = 0;
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_pHImage))
				{
					Hobject HDomainRegion;
					get_domain(THEAPP.m_pInspectAdminViewDlg->m_pHImage, &HDomainRegion);

					intensity(HDomainRegion, THEAPP.m_pInspectAdminViewDlg->m_pHImage, &dAvgValue, &dStd);
				}


				int iLightAverageValue = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][LIGHT_CHANNEL_2];
				if(abs((int)dAvgValue - iLightAverageValue) < iInspectCH_LV[LIGHT_CHANNEL_2])
				{
					iInspectCH_LV[LIGHT_CHANNEL_2] = iLV;
					iInspectCH_GV[LIGHT_CHANNEL_2] = dAvgValue;
				}
			}

			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_2] = iLVChannel2;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_2] = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][LIGHT_CHANNEL_2];
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_2] = iInspectCH_LV[LIGHT_CHANNEL_2];
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_2] = iInspectCH_GV[LIGHT_CHANNEL_2];

			if(abs(THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_2] - THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_2]) > THEAPP.m_pModelDataManager->m_iLightValueInTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1])
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_2] = false;
			}
			else
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_2] = true;
			}
		}
		else
		{
			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_2] = 0;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_2] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_2] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_2] = 0;

			THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_2] = true;
		}

		THEAPP.m_pAutoCalService->m_bAutoCal_Done[LIGHT_CHANNEL_2] = true;

		THEAPP.m_pTabControlDlg->m_pLightAutoCalResultDlg->m_ctrlProgressAutoCal.SetPos(THEAPP.m_pAutoCalService->m_iLightAutoCalProgressPercent);
		pAutoCalService->m_iLightAutoCalProgressPercent = 45;

		if(iLVChannel3 > 0)
		{
			for(int iLV = iLVChannel3Start; iLV <= iLVChannel3End; iLV += iLVChannelInterval)
			{
				if(THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(2, 19200 ))
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel1 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel2 = iLV;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel3 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel4 = 0;

					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination(iPageIdx);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();

					Sleep(100);
				}


				if(THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
				{
					THEAPP.m_pCameraManager->m_bGrabDone = FALSE;
					THEAPP.m_pCameraManager->CallHookFunction();
					
					if (THEAPP.m_pTabControlDlg->m_iCurrentTab - 1 < MAX_TRIGGER_TYPE)
						THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pTabControlDlg->m_iCurrentTab - 1);

					Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);
				}


				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
						break;

					dwGrabEnd = GetTickCount();

					if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
					{
						THEAPP.m_pCameraManager->GrabErrorPostProcess();
						break;
					}
				}

				///////////////////////////////////////////////////////////////////////////
				double dAvgValue = 255;
				double dStd = 0;
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_pHImage))
				{
					Hobject HDomainRegion;
					get_domain(THEAPP.m_pInspectAdminViewDlg->m_pHImage, &HDomainRegion);

					intensity(HDomainRegion, THEAPP.m_pInspectAdminViewDlg->m_pHImage, &dAvgValue, &dStd);
				}


				int iLightAverageValue = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][LIGHT_CHANNEL_3];
				if(abs((int)dAvgValue - iLightAverageValue) < iInspectCH_LV[LIGHT_CHANNEL_3])
				{
					iInspectCH_LV[LIGHT_CHANNEL_3] = iLV;
					iInspectCH_GV[LIGHT_CHANNEL_3] = dAvgValue;
				}
			}

			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_3] = iLVChannel3;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_3] = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_3];
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_3] = iInspectCH_LV[LIGHT_CHANNEL_3];
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_3] = iInspectCH_GV[LIGHT_CHANNEL_3];

			if(abs(THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_3] - THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_3]) > THEAPP.m_pModelDataManager->m_iLightValueInTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1])
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_3] = false;
			}
			else
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_3] = true;
			}
		}
		else
		{
			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_3] = 0;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_3] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_3] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_3] = 0;

			THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_3] = true;
		}

		THEAPP.m_pAutoCalService->m_bAutoCal_Done[LIGHT_CHANNEL_3] = true;

		THEAPP.m_pTabControlDlg->m_pLightAutoCalResultDlg->m_ctrlProgressAutoCal.SetPos(THEAPP.m_pAutoCalService->m_iLightAutoCalProgressPercent);
		pAutoCalService->m_iLightAutoCalProgressPercent = 60;

		if(iLVChannel4 > 0)
		{
			for(int iLV = iLVChannel4Start; iLV <= iLVChannel4End; iLV += iLVChannelInterval)
			{
				if(THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(2, 19200 ))
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel1 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel2 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel3 = iLV;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel4 = 0;

					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination(iPageIdx);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();

					Sleep(100);
				}

				if(THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
				{
					THEAPP.m_pCameraManager->m_bGrabDone = FALSE;
					THEAPP.m_pCameraManager->CallHookFunction();

					if (THEAPP.m_pTabControlDlg->m_iCurrentTab - 1 < MAX_TRIGGER_TYPE)
						THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pTabControlDlg->m_iCurrentTab - 1);

					Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

				}


				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
						break;

					dwGrabEnd = GetTickCount();

					if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
					{
						THEAPP.m_pCameraManager->GrabErrorPostProcess();
						break;
					}
				}

				///////////////////////////////////////////////////////////////////////////
				double dAvgValue = 255;
				double dStd = 0;
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_pHImage))
				{
					Hobject HDomainRegion;
					get_domain(THEAPP.m_pInspectAdminViewDlg->m_pHImage, &HDomainRegion);

					intensity(HDomainRegion, THEAPP.m_pInspectAdminViewDlg->m_pHImage, &dAvgValue, &dStd);
				}


				int iLightAverageValue = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][LIGHT_CHANNEL_4];
				if(abs((int)dAvgValue - iLightAverageValue) < iInspectCH_LV[LIGHT_CHANNEL_4])
				{
					iInspectCH_LV[LIGHT_CHANNEL_4] = iLV;
					iInspectCH_GV[LIGHT_CHANNEL_4] = dAvgValue;
				}
			}

			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_4] = iLVChannel4;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_4] = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_4];
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_4] = iInspectCH_LV[LIGHT_CHANNEL_4];
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_4] = iInspectCH_GV[LIGHT_CHANNEL_4];

			if(abs(THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_4] - THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_4]) > THEAPP.m_pModelDataManager->m_iLightValueInTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1])
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_4] = false;
			}
			else
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_4] = true;
			}
		}
		else
		{
			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_4] = 0;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_4] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_4] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_4] = 0;

			THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_4] = true;
		}

		THEAPP.m_pAutoCalService->m_bAutoCal_Done[LIGHT_CHANNEL_4] = true;

		THEAPP.m_pTabControlDlg->m_pLightAutoCalResultDlg->m_ctrlProgressAutoCal.SetPos(THEAPP.m_pAutoCalService->m_iLightAutoCalProgressPercent);
		pAutoCalService->m_iLightAutoCalProgressPercent = 75;

		if(iLVChannel5 > 0)
		{
			for(int iLV = iLVChannel5Start; iLV <= iLVChannel5End; iLV += iLVChannelInterval)
			{
				if(THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(2, 19200 ))
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel1 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel2 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel3 = 0;
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel4 = iLV;

					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination(iPageIdx);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();

					Sleep(100);
				}

				if(THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
				{
					THEAPP.m_pCameraManager->m_bGrabDone = FALSE;
					THEAPP.m_pCameraManager->CallHookFunction();

					if (THEAPP.m_pTabControlDlg->m_iCurrentTab - 1 < MAX_TRIGGER_TYPE)
						THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pTabControlDlg->m_iCurrentTab - 1);

					Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

				}


				dwGrabStart = GetTickCount();
				while (1)
				{
					if (THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
						break;

					dwGrabEnd = GetTickCount();

					if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
					{
						THEAPP.m_pCameraManager->GrabErrorPostProcess();
						break;
					}
				}

				///////////////////////////////////////////////////////////////////////////
				double dAvgValue = 255;
				double dStd = 0;
				if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_pHImage))
				{
					Hobject HDomainRegion;
					get_domain(THEAPP.m_pInspectAdminViewDlg->m_pHImage, &HDomainRegion);

					intensity(HDomainRegion, THEAPP.m_pInspectAdminViewDlg->m_pHImage, &dAvgValue, &dStd);
				}


				int iLightAverageValue = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab - 1][LIGHT_CHANNEL_5];
				if(abs((int)dAvgValue - iLightAverageValue) < iInspectCH_LV[LIGHT_CHANNEL_5])
				{
					iInspectCH_LV[LIGHT_CHANNEL_5] = iLV;
					iInspectCH_GV[LIGHT_CHANNEL_5] = dAvgValue;
				}
			}

			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_5] = iLVChannel5;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_5] = THEAPP.m_pModelDataManager->m_iLightAverageValue[THEAPP.m_pTabControlDlg->m_iCurrentTab-1][LIGHT_CHANNEL_5];
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_5] = iInspectCH_LV[LIGHT_CHANNEL_5];
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_5] = iInspectCH_GV[LIGHT_CHANNEL_5];

			if(abs(THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_5] - THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_5]) > THEAPP.m_pModelDataManager->m_iLightValueInTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1])
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_5] = false;
			}
			else
			{
				THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_5] = true;
			}
		}
		else
		{
			THEAPP.m_pAutoCalService->m_iTeachCH_LV[LIGHT_CHANNEL_5] = 0;
			THEAPP.m_pAutoCalService->m_iTeachCH_GV[LIGHT_CHANNEL_5] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_5] = 0;
			THEAPP.m_pAutoCalService->m_iInspectCH_GV[LIGHT_CHANNEL_5] = 0;

			THEAPP.m_pAutoCalService->m_bJudgeCH[LIGHT_CHANNEL_5] = true;
		}

		THEAPP.m_pAutoCalService->m_bAutoCal_Done[LIGHT_CHANNEL_5] = true;

		THEAPP.m_pTabControlDlg->m_pLightAutoCalResultDlg->m_ctrlProgressAutoCal.SetPos(THEAPP.m_pAutoCalService->m_iLightAutoCalProgressPercent);
		pAutoCalService->m_iLightAutoCalProgressPercent = 90;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if(THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(1, 19200 ))
		{
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel1 = THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_1];
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel2 = 0;
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel3 = 0;
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel4 = 0;

			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination(iPageIdx);
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();

			Sleep(100);
		}

		if(THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(2, 19200 ))
		{
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel1 = THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_2];
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel2 = THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_3];
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel3 = THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_4];
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[iPageIdx].uiChannel4 = THEAPP.m_pAutoCalService->m_iInspectCH_LV[LIGHT_CHANNEL_5];

			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination(iPageIdx);
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();

			Sleep(100);
		}

		if(THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
		{
			THEAPP.m_pCameraManager->m_bGrabDone = FALSE;
			THEAPP.m_pCameraManager->CallHookFunction();

			if (THEAPP.m_pTabControlDlg->m_iCurrentTab - 1 < MAX_TRIGGER_TYPE)
				THEAPP.m_pTriggerManager->FireTrigger(VISION_TYPE_INSPECTION, THEAPP.m_pTabControlDlg->m_iCurrentTab - 1);

			Sleep(THEAPP.Struct_PreferenceStruct.m_iTriggerSleepTime);

		}

		dwGrabStart = GetTickCount();
		while (1)
		{
			if (THEAPP.m_pCameraManager->m_bGrabDone == TRUE)
				break;

			dwGrabEnd = GetTickCount();

			if((dwGrabEnd-dwGrabStart) > MAX_GRAB_TIMEOUT)
			{
				THEAPP.m_pCameraManager->GrabErrorPostProcess();
				break;
			}
		}

		///////////////////////////////////////////////////////////////////////////
		double dAvgValue = 255;
		double dStd = 0;
		if (THEAPP.m_pGFunction->ValidHImage(THEAPP.m_pInspectAdminViewDlg->m_pHImage))
		{
			Hobject HDomainRegion;
			get_domain(THEAPP.m_pInspectAdminViewDlg->m_pHImage, &HDomainRegion);

			intensity(HDomainRegion, THEAPP.m_pInspectAdminViewDlg->m_pHImage, &dAvgValue, &dStd);
		}

		iInspectTotol_GV = dAvgValue;

		THEAPP.m_pAutoCalService->m_iTeachTOTAL_GV = THEAPP.m_pModelDataManager->m_iLightAverageValueTotal[THEAPP.m_pTabControlDlg->m_iCurrentTab-1];
		THEAPP.m_pAutoCalService->m_iInspectTOTAL_GV = iInspectTotol_GV;

		if(abs(THEAPP.m_pAutoCalService->m_iInspectTOTAL_GV - THEAPP.m_pAutoCalService->m_iTeachTOTAL_GV) > THEAPP.m_pModelDataManager->m_iTotalImageValueTol[THEAPP.m_pTabControlDlg->m_iCurrentTab-1])
		{
			THEAPP.m_pAutoCalService->m_bJudgeTOTAL = false;
		}
		else
		{
			THEAPP.m_pAutoCalService->m_bJudgeTOTAL = true;
		}

#endif


#ifdef INLINE_MODE

		THEAPP.m_pTabControlDlg->m_pLightAutoCalResultDlg->m_ctrlProgressAutoCal.SetPos(100);

		CString sTimeCheck;
		dTimeEnd = GetTickCount();
		sTimeCheck.Format("********** Top_1 스캔 시간: %.4lf s", (dTimeEnd - dTimeStart)/1000);
		THEAPP.SaveLog(sTimeCheck);

		THEAPP.m_pTabControlDlg->m_pLightAutoCalResultDlg->UpdateResultView();

#endif
		pAutoCalService->m_iLightAutoCalProgressPercent = 100;
		pAutoCalService->m_bLightAutoCalResult = false;

		return 0;
	}
	catch(HException &except)
	{
		
		CString str; 
		str.Format("Halcon Exception [AutoCalLightThread] : %s", except.message);
		THEAPP.SaveLog(str);
		
		pAutoCalService->m_bLightAutoCalResult = false;
		return 0;
	}
}

