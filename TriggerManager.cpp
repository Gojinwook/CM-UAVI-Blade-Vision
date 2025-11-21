#include "stdafx.h"
#include "uScan.h"
#include "TriggerManager.h"

CTriggerManager* CTriggerManager::m_pInstance = NULL;

CTriggerManager* CTriggerManager::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new CTriggerManager();
	}
	return m_pInstance;
}

void CTriggerManager::DeleteInstance()
{
	SAFE_DELETE(m_pInstance);
}

CTriggerManager::CTriggerManager(void)
{

}


CTriggerManager::~CTriggerManager(void)
{
	m_AJinAXL.Terminate();
}

BOOL CTriggerManager::Initialize()
{
	m_AJinAXL.Initialize();

	m_iSleepTime = 30;

	return TRUE;
}

// 검사유형 통합(ROI기준 알고리즘 설정) - LeeGW
void CTriggerManager::FireTrigger(int iVisionType, int iTriggerPageIndex)
{
#ifdef VISION_TRIGGER
	BOOL bPageOnOff[11];
	for (int i = 0; i < 11; i++)
		bPageOnOff[i] = FALSE;

	int iVisionOrder = 0;
	int iPageOffset = 1;
	BOOL bSharedPage = FALSE;

	if (iVisionType == VISION_TYPE_INSPECTION)
	{
		if (THEAPP.m_iMachineInspType == MACHINE_SIDEFILL ||
			THEAPP.m_iMachineInspType == MACHINE_BRACKET )
		{
			switch (iTriggerPageIndex)
			{
			case TRIGGER_SURFACE:
			case TRIGGER_EDGE:
				bPageOnOff[iTriggerPageIndex] = TRUE;
				bPageOnOff[iTriggerPageIndex + 2] = TRUE;
				break;

			default:
				bPageOnOff[iTriggerPageIndex + 5] = TRUE;
				break;
			}
		}
		else if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
		{
			iPageOffset = 8;
			bPageOnOff[iTriggerPageIndex] = TRUE;
		}
		else // 나머지 조명 페이지 확인필요 v3000
		{
			bPageOnOff[iTriggerPageIndex] = TRUE;
			bPageOnOff[iTriggerPageIndex + 4] = TRUE;
		}
	}
	else if (iVisionType==VISION_TYPE_BARCODE)
	{
		if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
		{
			iPageOffset = 8;
			bSharedPage = TRUE;
		}
			
		iVisionOrder = 1;
		bPageOnOff[iTriggerPageIndex] = TRUE;
	}

	m_AJinAXL.Set_Trigger(iVisionOrder, bPageOnOff, m_iSleepTime, iPageOffset, bSharedPage);	// FALSE: No Usage
#else // VISION_TRIGGER
	if (iVisionType == VISION_TYPE_INSPECTION)
	{
		int iDomPageMax;
		if (THEAPP.Struct_PreferenceStruct.m_iMachineInspType == MACHINE_NORMAL)
			iDomPageMax = 4;
		else
			iDomPageMax = 2;

		BOOL bPageOnOff[8];
		for (int i = 0; i < 8; i++)
			bPageOnOff[i] = FALSE;

		if (THEAPP.m_pModelDataManager->m_iLightPageNo[iTriggerPageIndex][0] >= 0)
			bPageOnOff[THEAPP.m_pModelDataManager->m_iLightPageNo[iTriggerPageIndex][0]] = TRUE;
		if (THEAPP.m_pModelDataManager->m_iLightPageNo[iTriggerPageIndex][1] >= 0)
			bPageOnOff[THEAPP.m_pModelDataManager->m_iLightPageNo[iTriggerPageIndex][1] + iDomPageMax] = TRUE;

		THEAPP.m_pHandlerService->Set_TriggerRequest(bPageOnOff[0], bPageOnOff[1], bPageOnOff[2], bPageOnOff[3], bPageOnOff[4], bPageOnOff[5], bPageOnOff[6], bPageOnOff[7]);

	}
	else if (iVisionType == VISION_TYPE_BARCODE)
	{
		THEAPP.m_pHandlerService->Set_BarcodeTriggerRequest();
	}

#endif // VISION_TRIGGER
}
// 검사유형 통합(ROI기준 알고리즘 설정) - LeeGW END

void CTriggerManager::Delay(int msec)
{
	m_AJinAXL.Delay(msec);	// Delay
}