#include "stdafx.h"
#include "ModelDataManager.h"
#include "uScan.h"
#include "IniFileCS.h"

CModelDataManager *CModelDataManager::m_pInstance = NULL;

CModelDataManager *CModelDataManager::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance)
	{
		m_pInstance = new CModelDataManager();
		m_pInstance->InitModelData();
	}
	return m_pInstance;
}

void CModelDataManager::DeleteInstance()
{
	SAFE_DELETE(m_pInstance);
}

CModelDataManager::CModelDataManager(void)
{
	SetFilePtr(&m_File);

	m_sModelName = ".";
	m_iModelVersion = 1000;
	m_iTrayArrayX = 0;
	m_iTrayArrayY = 0;
	m_dModulePitchX = 0;
	m_dModulePitchY = 0;
	m_iTrayHESizeX = 0;
	m_iTrayHESizeY = 0;
	m_iTrayModuleAmt = 0;
	m_iTrayModuleMax = 0;
	m_bOneBarrel = FALSE;
	m_bBlackCoating = FALSE;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_bUseLocalAlignMatching[i] = FALSE;
		m_lLAlignModelID[i] = -1;
	}

	m_strLogin_Mode = "";
	m_strOperator = "";
	m_strPwd = "";

	m_iRefContCol = 570;
	m_iRefContRow = 640;
	m_iRefContRad1 = 410;
	m_iRefContRad2 = 360;
	m_iRefContRad3 = 325;
	m_iRefContRad4 = 190;
	m_dContRatio23 = 0.91;
	m_dContRatio45 = 0.83;

	m_iBlackMatchingImageType = BLACK_MATCHING_IMAGE_TYPE_LENS1;
	m_bUseMatchingAngle = FALSE;
	m_iMatchingAngleRange = 10;
	m_bUseMatchingScale = TRUE;
	m_iMatchingScaleMax = 98;
	m_iMatchingScaleMin = 102;

	m_dOutmostContourCenterX = -1;
	m_dOutmostContourCenterY = -1;

	m_iMatchingContrastC1 = 15;
	m_iMatchingContrastC2 = 10;
	m_iMatchingContrastC3 = 10;
	m_iMatchingContrastC4 = 10;

	m_iBarcodeLTPointX = PGCAM_IMAGE_WIDTH / 2 - PGCAM_IMAGE_WIDTH / 4;
	m_iBarcodeLTPointY = PGCAM_IMAGE_HEIGHT / 2 - PGCAM_IMAGE_HEIGHT / 4;
	m_iBarcodeRBPointX = PGCAM_IMAGE_WIDTH / 2 + PGCAM_IMAGE_WIDTH / 4;
	m_iBarcodeRBPointY = PGCAM_IMAGE_HEIGHT / 2 + PGCAM_IMAGE_HEIGHT / 4;

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		ContoursOri[cont].Reset();
	}

	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		m_mdInspPosZ[iImageIdx] = 0.0;
	}

	m_pInspectionArea = NULL;
	m_pInspectionArea = new GTRegion;
	m_pInspectionArea->SetVisible(FALSE, FALSE);
	m_pInspectionArea->SetSelectable(FALSE, FALSE);
	m_pInspectionArea->SetMovable(FALSE, FALSE);
	m_pInspectionArea->SetSizable(FALSE, FALSE);

	// 24.06.28 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
	for (int img = 0; img < BARREL_LENS_IMAGE_TAB; img++)
	{
		m_bUseBLROIAlign[img] = FALSE;

		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			m_bUseBLROIAlignContour[img][cont] = FALSE;
		}

		m_iBLROIAlignInnerMargin[img] = 0;
		m_iBLROIAlignOuterMargin[img] = 0;
	}

	m_dBLROIAlignMatchingScore = 0.1;
	m_iBLROIAlignSearchingMargin = 0;
	// 24.06.28 - v2651 - ROI Templet Matching Align 추가 - LeeGW END
}

CModelDataManager::~CModelDataManager(void)
{
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (m_lLAlignModelID[i] >= 0)
		{
			if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_TEMPLATE)
				clear_ncc_model(m_lLAlignModelID[i]);
			else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_SHAPE)
				clear_shape_model(m_lLAlignModelID[i]);

			m_lLAlignModelID[i] = -1;
		}
	}

	for (int img = 0; img < BARREL_LENS_IMAGE_TAB; img++)
	{
		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			if (m_iBLROIAlignMatchingModel[img][cont] >= 0)
			{
				clear_ncc_model(m_iBLROIAlignMatchingModel[img][cont]);
			}
		}
	}

	SAFE_DELETE(m_pInspectionArea);
}

void CModelDataManager::InitModelData()
{
	m_sModelName = ".";
	m_iModelVersion = 1000;
	m_iTrayArrayX = 0;
	m_iTrayArrayY = 0;
	m_dModulePitchX = 0;
	m_dModulePitchY = 0;
	m_iTrayHESizeX = 0;
	m_iTrayHESizeY = 0;
	m_iTrayModuleAmt = 0;
	m_iTrayModuleMax = 0;
	m_bOneBarrel = FALSE;
	m_bBlackCoating = FALSE;

	//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_bUseLocalAlignMatching[i] = FALSE;
		m_iLocalMatchingTeachingRectLTX[i] = 0;
		m_iLocalMatchingTeachingRectLTY[i] = 0;
		m_iLocalMatchingTeachingRectRBX[i] = 0;
		m_iLocalMatchingTeachingRectRBY[i] = 0;

		m_iLocalMatchingPyramidLevel[i] = 0;
		m_iLocalMatchingAngleRange[i] = 5.0;
		m_dLocalMatchingScaleMin[i] = 0.98;
		m_dLocalMatchingScaleMax[i] = 1.02;
		m_iLocalMatchingTeachingContrast[i] = 40;
		m_iLocalMatchingTeachingContrastLow[i] = 40;
		m_iLocalMatchingTeachingContrastMinSize[i] = 1;
		m_dLocalMatchingScore[i] = 0.3;
		m_iLocalMatchingInspectionMinContrast[i] = 15;
		m_iLocalMatchingSearchMarginX[i] = 300;
		m_iLocalMatchingSearchMarginY[i] = 300;
		m_dLocalTeachAlignRefX[i] = -1;
		m_dLocalTeachAlignRefY[i] = -1;
		m_bUseMatchingAffineConstant[i] = FALSE;

		for (int j = 0; j < MAX_IMAGE_TAB; j++)
			m_bLocalAlignImage[i][j] = FALSE;

		if (m_lLAlignModelID[i] >= 0)
		{
			if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_TEMPLATE)
				clear_ncc_model(m_lLAlignModelID[i]);
			else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_SHAPE)
				clear_shape_model(m_lLAlignModelID[i]);

			m_lLAlignModelID[i] = -1;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	m_iRefContCol = 570;
	m_iRefContRow = 640;
	m_iRefContRad1 = 410;
	m_iRefContRad2 = 360;
	m_iRefContRad3 = 325;
	m_iRefContRad4 = 190;
	m_dContRatio23 = 0.91;
	m_dContRatio45 = 0.83;

	for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
	{
		ContoursOri[cont].Reset();
	}

	m_iBlackMatchingImageType = BLACK_MATCHING_IMAGE_TYPE_LENS1;
	m_bUseMatchingAngle = FALSE;
	m_iMatchingAngleRange = 10;
	m_bUseMatchingScale = TRUE;
	m_iMatchingScaleMax = 98;
	m_iMatchingScaleMin = 102;

	m_iMatchingContrastC1 = 15;
	m_iMatchingContrastC2 = 10;
	m_iMatchingContrastC3 = 10;
	m_iMatchingContrastC4 = 10;

	m_iBarcodeLTPointX = PGCAM_IMAGE_WIDTH / 2 - PGCAM_IMAGE_WIDTH / 4;
	m_iBarcodeLTPointY = PGCAM_IMAGE_HEIGHT / 2 - PGCAM_IMAGE_HEIGHT / 4;
	m_iBarcodeRBPointX = PGCAM_IMAGE_WIDTH / 2 + PGCAM_IMAGE_WIDTH / 4;
	m_iBarcodeRBPointY = PGCAM_IMAGE_HEIGHT / 2 + PGCAM_IMAGE_HEIGHT / 4;

	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		m_mdInspPosZ[iImageIdx] = 0.0;
	}

	// 24.06.28 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
	for (int img = 0; img < BARREL_LENS_IMAGE_TAB; img++)
	{
		m_bUseBLROIAlign[img] = FALSE;

		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			m_bUseBLROIAlignContour[img][cont] = FALSE;
		}

		m_iBLROIAlignInnerMargin[img] = 0;
		m_iBLROIAlignOuterMargin[img] = 0;
	}

	m_dBLROIAlignMatchingScore = 0.1;
	m_iBLROIAlignSearchingMargin = 0;

	for (int img = 0; img < BARREL_LENS_IMAGE_TAB; img++)
	{
		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			if (m_iBLROIAlignMatchingModel[img][cont] >= 0)
			{
				clear_ncc_model(m_iBLROIAlignMatchingModel[img][cont]);
			}
		}
	}
	// 24.06.28 - v2651 - ROI Templet Matching Align 추가 - LeeGW END
}

// 0531
void CModelDataManager::INI_CompareNRecord_BOOL(BOOL bNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile *file)
{
	BOOL bPrev;
	bPrev = INI.Get_Bool(sSection, sKey, FALSE);

	CString sPrev, sNow;

	if (bPrev != bNow)
	{
		if (bPrev == TRUE)
			sPrev = "사용";
		else
			sPrev = "미사용";
		if (bNow == TRUE)
			sNow = "사용";
		else
			sNow = "미사용";

		CString sFullPath;
		sFullPath.Format("%s\t%s\t%s\t%s\r\n", sFixVal, sParam, sPrev, sNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::INI_CompareNRecord(int iNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile *file)
{
	int iPrev;
	iPrev = INI.Get_Integer(sSection, sKey, 0);

	if (iPrev != iNow)
	{
		CString sFullPath;
		sFullPath.Format("%s\t%s\t%d\t%d\r\n", sFixVal, sParam, iPrev, iNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::INI_CompareNRecord(double dNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile *file)
{
	double dPrev;
	dPrev = INI.Get_Double(sSection, sKey, 0.0);

	if (dPrev != dNow)
	{
		CString sFullPath;
		sFullPath.Format("%s\t%s\t%.3lf\t%.3lf\r\n", sFixVal, sParam, dPrev, dNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::CompareNRecord(BOOL bPrev, BOOL bNow, CString sParam, CString sFixVal, CFile *file)
{
	CString sPrev, sNow;

	if (bPrev != bNow)
	{
		if (bPrev == TRUE)
			sPrev = "TRUE";
		else
			sPrev = "FALSE";
		if (bNow == TRUE)
			sNow = "TRUE";
		else
			sNow = "FALSE";

		DoubleLogOut("[LoadRmsParamData] Parameter Changed : %s, %s -> %s", sParam, sPrev, sNow);
		CString sFullPath;
		sFullPath.Format("%s,%s,%s,%s\r\n", sFixVal, sParam, sPrev, sNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::CompareNRecord(UINT iPrev, UINT iNow, CString sParam, CString sFixVal, CFile *file)
{
	if (iPrev != iNow)
	{
		DoubleLogOut("[LoadRmsParamData] Parameter Changed : %s, %u -> %u", sParam, iPrev, iNow);
		CString sFullPath;
		sFullPath.Format("%s,%s,%u,%u\r\n", sFixVal, sParam, iPrev, iNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

void CModelDataManager::CompareNRecord(double dPrev, double dNow, CString sParam, CString sFixVal, CFile *file)
{
	if (dPrev != dNow)
	{
		DoubleLogOut("[LoadRmsParamData] Parameter Changed : %s, %.4lf -> %.4lf", sParam, dPrev, dNow);
		CString sFullPath;
		sFullPath.Format("%s,%s,%.4lf,%.4lf\r\n", sFixVal, sParam, dPrev, dNow);
		file->Write(sFullPath, sFullPath.GetLength());
	}
}

// 0531
void CModelDataManager::UpdateParamChange()
{
	if (m_sModelName == ".")
		return;

	////////////////////////////// Update parameter-variables //////////////////////////////
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		THEAPP.m_pTabControlDlg->m_pTeachParamDlg[i]->GetInspectParam();
	}

	////////////////////////////// Update inspectCondition-variables ///////////////////////////////
	CString sTemp;

	// Top
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(2, 0, &sTemp);
	m_dDirtMinArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(3, 0, &sTemp);
	m_dDirtMidArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(4, 0, &sTemp);
	m_dDirtMaxArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(5, 0, &sTemp);
	m_iDirtMinNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(6, 0, &sTemp);
	m_iDirtMidNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(7, 0, &sTemp);
	m_iDirtMaxNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(2, 1, &sTemp);
	m_dScratchMinArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(3, 1, &sTemp);
	m_dScratchMidArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(4, 1, &sTemp);
	m_dScratchMaxArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(5, 1, &sTemp);
	m_iScratchMinNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(6, 1, &sTemp);
	m_iScratchMidNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(7, 1, &sTemp);
	m_iScratchMaxNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(2, 2, &sTemp);
	m_dStainMinArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(3, 2, &sTemp);
	m_dStainMidArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(4, 2, &sTemp);
	m_dStainMaxArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(5, 2, &sTemp);
	m_iStainMinNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(6, 2, &sTemp);
	m_iStainMidNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(7, 2, &sTemp);
	m_iStainMaxNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(2, 3, &sTemp);
	m_dDentMinArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(3, 3, &sTemp);
	m_dDentMidArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(4, 3, &sTemp);
	m_dDentMaxArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(5, 3, &sTemp);
	m_iDentMinNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(6, 3, &sTemp);
	m_iDentMidNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(7, 3, &sTemp);
	m_iDentMaxNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(2, 4, &sTemp);
	m_dChipMinArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(3, 4, &sTemp);
	m_dChipMidArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(4, 4, &sTemp);
	m_dChipMaxArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(5, 4, &sTemp);
	m_iChipMinNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(6, 4, &sTemp);
	m_iChipMidNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(7, 4, &sTemp);
	m_iChipMaxNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(2, 5, &sTemp);
	m_dEpoxyMinArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(3, 5, &sTemp);
	m_dEpoxyMidArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(4, 5, &sTemp);
	m_dEpoxyMaxArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(5, 5, &sTemp);
	m_iEpoxyMinNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(6, 5, &sTemp);
	m_iEpoxyMidNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(7, 5, &sTemp);
	m_iEpoxyMaxNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(2, 6, &sTemp);
	m_dRes1MinArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(3, 6, &sTemp);
	m_dRes1MidArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(4, 6, &sTemp);
	m_dRes1MaxArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(5, 6, &sTemp);
	m_iRes1MinNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(6, 6, &sTemp);
	m_iRes1MidNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(7, 6, &sTemp);
	m_iRes1MaxNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(2, 7, &sTemp);
	m_dRes2MinArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(3, 7, &sTemp);
	m_dRes2MidArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(4, 7, &sTemp);
	m_dRes2MaxArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(5, 7, &sTemp);
	m_iRes2MinNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(6, 7, &sTemp);
	m_iRes2MidNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(7, 7, &sTemp);
	m_iRes2MaxNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);

	// white dot - LeeGW
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(2, 8, &sTemp);
	m_dWhiteDotMinArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(3, 8, &sTemp);
	m_dWhiteDotMidArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(4, 8, &sTemp);
	m_dWhiteDotMaxArea_Top = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(5, 8, &sTemp);
	m_iWhiteDotMinNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(6, 8, &sTemp);
	m_iWhiteDotMidNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionTopBarrelDlg->m_Grid.QuickGetText(7, 8, &sTemp);
	m_iWhiteDotMaxNumAccept_Top = atoi((LPSTR)(LPCSTR)sTemp);

	// Bottom
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(2, 0, &sTemp);
	m_dDirtMinArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(3, 0, &sTemp);
	m_dDirtMidArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(4, 0, &sTemp);
	m_dDirtMaxArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(5, 0, &sTemp);
	m_iDirtMinNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(6, 0, &sTemp);
	m_iDirtMidNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(7, 0, &sTemp);
	m_iDirtMaxNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(2, 1, &sTemp);
	m_dScratchMinArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(3, 1, &sTemp);
	m_dScratchMidArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(4, 1, &sTemp);
	m_dScratchMaxArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(5, 1, &sTemp);
	m_iScratchMinNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(6, 1, &sTemp);
	m_iScratchMidNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(7, 1, &sTemp);
	m_iScratchMaxNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(2, 2, &sTemp);
	m_dStainMinArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(3, 2, &sTemp);
	m_dStainMidArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(4, 2, &sTemp);
	m_dStainMaxArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(5, 2, &sTemp);
	m_iStainMinNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(6, 2, &sTemp);
	m_iStainMidNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(7, 2, &sTemp);
	m_iStainMaxNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(2, 3, &sTemp);
	m_dDentMinArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(3, 3, &sTemp);
	m_dDentMidArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(4, 3, &sTemp);
	m_dDentMaxArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(5, 3, &sTemp);
	m_iDentMinNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(6, 3, &sTemp);
	m_iDentMidNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(7, 3, &sTemp);
	m_iDentMaxNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(2, 4, &sTemp);
	m_dChipMinArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(3, 4, &sTemp);
	m_dChipMidArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(4, 4, &sTemp);
	m_dChipMaxArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(5, 4, &sTemp);
	m_iChipMinNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(6, 4, &sTemp);
	m_iChipMidNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(7, 4, &sTemp);
	m_iChipMaxNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(2, 5, &sTemp);
	m_dEpoxyMinArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(3, 5, &sTemp);
	m_dEpoxyMidArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(4, 5, &sTemp);
	m_dEpoxyMaxArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(5, 5, &sTemp);
	m_iEpoxyMinNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(6, 5, &sTemp);
	m_iEpoxyMidNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(7, 5, &sTemp);
	m_iEpoxyMaxNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(2, 6, &sTemp);
	m_dRes1MinArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(3, 6, &sTemp);
	m_dRes1MidArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(4, 6, &sTemp);
	m_dRes1MaxArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(5, 6, &sTemp);
	m_iRes1MinNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(6, 6, &sTemp);
	m_iRes1MidNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(7, 6, &sTemp);
	m_iRes1MaxNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(2, 7, &sTemp);
	m_dRes2MinArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(3, 7, &sTemp);
	m_dRes2MidArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(4, 7, &sTemp);
	m_dRes2MaxArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(5, 7, &sTemp);
	m_iRes2MinNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(6, 7, &sTemp);
	m_iRes2MidNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(7, 7, &sTemp);
	m_iRes2MaxNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);

	// white dot - LeeGW
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(2, 8, &sTemp);
	m_dWhiteDotMinArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(3, 8, &sTemp);
	m_dWhiteDotMidArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(4, 8, &sTemp);
	m_dWhiteDotMaxArea_Btm = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(5, 8, &sTemp);
	m_iWhiteDotMinNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(6, 8, &sTemp);
	m_iWhiteDotMidNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionBottomBarrelDlg->m_Grid.QuickGetText(7, 8, &sTemp);
	m_iWhiteDotMaxNumAccept_Btm = atoi((LPSTR)(LPCSTR)sTemp);

	// Inner
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(2, 0, &sTemp);
	m_dDirtMinArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(3, 0, &sTemp);
	m_dDirtMidArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(4, 0, &sTemp);
	m_dDirtMaxArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(5, 0, &sTemp);
	m_iDirtMinNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(6, 0, &sTemp);
	m_iDirtMidNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(7, 0, &sTemp);
	m_iDirtMaxNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(2, 1, &sTemp);
	m_dScratchMinArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(3, 1, &sTemp);
	m_dScratchMidArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(4, 1, &sTemp);
	m_dScratchMaxArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(5, 1, &sTemp);
	m_iScratchMinNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(6, 1, &sTemp);
	m_iScratchMidNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(7, 1, &sTemp);
	m_iScratchMaxNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(2, 2, &sTemp);
	m_dStainMinArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(3, 2, &sTemp);
	m_dStainMidArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(4, 2, &sTemp);
	m_dStainMaxArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(5, 2, &sTemp);
	m_iStainMinNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(6, 2, &sTemp);
	m_iStainMidNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(7, 2, &sTemp);
	m_iStainMaxNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(2, 3, &sTemp);
	m_dDentMinArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(3, 3, &sTemp);
	m_dDentMidArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(4, 3, &sTemp);
	m_dDentMaxArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(5, 3, &sTemp);
	m_iDentMinNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(6, 3, &sTemp);
	m_iDentMidNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(7, 3, &sTemp);
	m_iDentMaxNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(2, 4, &sTemp);
	m_dChipMinArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(3, 4, &sTemp);
	m_dChipMidArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(4, 4, &sTemp);
	m_dChipMaxArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(5, 4, &sTemp);
	m_iChipMinNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(6, 4, &sTemp);
	m_iChipMidNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(7, 4, &sTemp);
	m_iChipMaxNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(2, 5, &sTemp);
	m_dEpoxyMinArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(3, 5, &sTemp);
	m_dEpoxyMidArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(4, 5, &sTemp);
	m_dEpoxyMaxArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(5, 5, &sTemp);
	m_iEpoxyMinNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(6, 5, &sTemp);
	m_iEpoxyMidNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(7, 5, &sTemp);
	m_iEpoxyMaxNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(2, 6, &sTemp);
	m_dRes1MinArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(3, 6, &sTemp);
	m_dRes1MidArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(4, 6, &sTemp);
	m_dRes1MaxArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(5, 6, &sTemp);
	m_iRes1MinNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(6, 6, &sTemp);
	m_iRes1MidNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(7, 6, &sTemp);
	m_iRes1MaxNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(2, 7, &sTemp);
	m_dRes2MinArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(3, 7, &sTemp);
	m_dRes2MidArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(4, 7, &sTemp);
	m_dRes2MaxArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(5, 7, &sTemp);
	m_iRes2MinNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(6, 7, &sTemp);
	m_iRes2MidNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(7, 7, &sTemp);
	m_iRes2MaxNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);

	// white dot - LeeGW
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(2, 8, &sTemp);
	m_dWhiteDotMinArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(3, 8, &sTemp);
	m_dWhiteDotMidArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(4, 8, &sTemp);
	m_dWhiteDotMaxArea_Inner = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(5, 8, &sTemp);
	m_iWhiteDotMinNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(6, 8, &sTemp);
	m_iWhiteDotMidNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionInnerBarrelDlg->m_Grid.QuickGetText(7, 8, &sTemp);
	m_iWhiteDotMaxNumAccept_Inner = atoi((LPSTR)(LPCSTR)sTemp);

	// Lens
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(2, 0, &sTemp);
	m_dLensMinArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(3, 0, &sTemp);
	m_dLensMidArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(4, 0, &sTemp);
	m_dLensMaxArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(5, 0, &sTemp);
	m_iLensMinNumAccept = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(6, 0, &sTemp);
	m_iLensMidNumAccept = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(7, 0, &sTemp);
	m_iLensMaxNumAccept = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(2, 1, &sTemp);
	m_dLensScratchMinArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(3, 1, &sTemp);
	m_dLensScratchMidArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(4, 1, &sTemp);
	m_dLensScratchMaxArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(5, 1, &sTemp);
	m_iLensScratchMinNumAccept = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(6, 1, &sTemp);
	m_iLensScratchMidNumAccept = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(7, 1, &sTemp);
	m_iLensScratchMaxNumAccept = atoi((LPSTR)(LPCSTR)sTemp);

	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(2, 2, &sTemp);
	m_dLensDirtMinArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(3, 2, &sTemp);
	m_dLensDirtMidArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(4, 2, &sTemp);
	m_dLensDirtMaxArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(5, 2, &sTemp);
	m_iLensDirtMinNumAccept = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(6, 2, &sTemp);
	m_iLensDirtMidNumAccept = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(7, 2, &sTemp);
	m_iLensDirtMaxNumAccept = atoi((LPSTR)(LPCSTR)sTemp);

	// white dot - LeeGW
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(2, 3, &sTemp);
	m_dLensWhiteDotMinArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(3, 3, &sTemp);
	m_dLensWhiteDotMidArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(4, 3, &sTemp);
	m_dLensWhiteDotMaxArea = atof((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(5, 3, &sTemp);
	m_iLensWhiteDotMinNumAccept = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(6, 3, &sTemp);
	m_iLensWhiteDotMidNumAccept = atoi((LPSTR)(LPCSTR)sTemp);
	THEAPP.m_pTabControlDlg->m_pInspectionConditionTabDlg->m_pConditionLensDlg->m_Grid.QuickGetText(7, 3, &sTemp);
	m_iLensWhiteDotMaxNumAccept = atoi((LPSTR)(LPCSTR)sTemp);
}

// 0531
void CModelDataManager::SaveModelData()
{
	if (m_sModelName == ".")
		return;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		THEAPP.m_pTabControlDlg->m_pTeachParamDlg[i]->GetInspectParam();
		THEAPP.DoEvents();
	}

	////////////////////////////////////////////////////////////////////////////////////////////Inspect parameter INI File 로 저장
	CString strModelFolder;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

	CIniFileCS INICondition(strModelFolder + "\\SW\\InspectCondition.ini");

	CString strSection;
#pragma region Lens & Barrel
	strSection = "Lens_Contamination"; // 24.05.09 - v2646 - 불량명 변경 - LeeGW
	INICondition.Set_Double(strSection, "Min Area", m_dLensMinArea);
	INICondition.Set_Double(strSection, "Mid Area", m_dLensMidArea);
	INICondition.Set_Double(strSection, "Max Area", m_dLensMaxArea);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iLensMinNumAccept);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iLensMidNumAccept);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iLensMaxNumAccept);

	strSection = "Lens_Scratch";
	INICondition.Set_Double(strSection, "Min Area", m_dLensScratchMinArea);
	INICondition.Set_Double(strSection, "Mid Area", m_dLensScratchMidArea);
	INICondition.Set_Double(strSection, "Max Area", m_dLensScratchMaxArea);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iLensScratchMinNumAccept);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iLensScratchMidNumAccept);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iLensScratchMaxNumAccept);

	strSection = "Lens_Dirt"; // 24.05.09 - v2646 - 불량명 변경 - LeeGW
	INICondition.Set_Double(strSection, "Min Area", m_dLensDirtMinArea);
	INICondition.Set_Double(strSection, "Mid Area", m_dLensDirtMidArea);
	INICondition.Set_Double(strSection, "Max Area", m_dLensDirtMaxArea);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iLensDirtMinNumAccept);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iLensDirtMidNumAccept);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iLensDirtMaxNumAccept);

	strSection = "Lens_WhiteDot"; // white dot - LeeGW
	INICondition.Set_Double(strSection, "Min Area", m_dLensWhiteDotMinArea);
	INICondition.Set_Double(strSection, "Mid Area", m_dLensWhiteDotMidArea);
	INICondition.Set_Double(strSection, "Max Area", m_dLensWhiteDotMaxArea);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iLensWhiteDotMinNumAccept);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iLensWhiteDotMidNumAccept);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iLensWhiteDotMaxNumAccept);

	//////////////////////// New ////////////////////////////////
	strSection = "Top_Dirt";
	INICondition.Set_Double(strSection, "Min Area", m_dDirtMinArea_Top);
	INICondition.Set_Double(strSection, "Mid Area", m_dDirtMidArea_Top);
	INICondition.Set_Double(strSection, "Max Area", m_dDirtMaxArea_Top);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iDirtMinNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iDirtMidNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iDirtMaxNumAccept_Top);

	strSection = "Top_Scratch";
	INICondition.Set_Double(strSection, "Min Area", m_dScratchMinArea_Top);
	INICondition.Set_Double(strSection, "Mid Area", m_dScratchMidArea_Top);
	INICondition.Set_Double(strSection, "Max Area", m_dScratchMaxArea_Top);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iScratchMinNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iScratchMidNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iScratchMaxNumAccept_Top);

	strSection = "Top_Stain";
	INICondition.Set_Double(strSection, "Min Area", m_dStainMinArea_Top);
	INICondition.Set_Double(strSection, "Mid Area", m_dStainMidArea_Top);
	INICondition.Set_Double(strSection, "Max Area", m_dStainMaxArea_Top);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iStainMinNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iStainMidNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iStainMaxNumAccept_Top);

	strSection = "Top_Dent";
	INICondition.Set_Double(strSection, "Min Area", m_dDentMinArea_Top);
	INICondition.Set_Double(strSection, "Mid Area", m_dDentMidArea_Top);
	INICondition.Set_Double(strSection, "Max Area", m_dDentMaxArea_Top);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iDentMinNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iDentMidNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iDentMaxNumAccept_Top);

	strSection = "Top_Chipping";
	INICondition.Set_Double(strSection, "Min Area", m_dChipMinArea_Top);
	INICondition.Set_Double(strSection, "Mid Area", m_dChipMidArea_Top);
	INICondition.Set_Double(strSection, "Max Area", m_dChipMaxArea_Top);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iChipMinNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iChipMidNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iChipMaxNumAccept_Top);

	strSection = "Top_Epoxy";
	INICondition.Set_Double(strSection, "Min Area", m_dEpoxyMinArea_Top);
	INICondition.Set_Double(strSection, "Mid Area", m_dEpoxyMidArea_Top);
	INICondition.Set_Double(strSection, "Max Area", m_dEpoxyMaxArea_Top);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iEpoxyMinNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iEpoxyMidNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iEpoxyMaxNumAccept_Top);

	strSection = "Top_Outside_CT";
	INICondition.Set_Double(strSection, "Min Area", m_dRes1MinArea_Top);
	INICondition.Set_Double(strSection, "Mid Area", m_dRes1MidArea_Top);
	INICondition.Set_Double(strSection, "Max Area", m_dRes1MaxArea_Top);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iRes1MinNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iRes1MidNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iRes1MaxNumAccept_Top);

	strSection = "Top_Peel_Off";
	INICondition.Set_Double(strSection, "Min Area", m_dRes2MinArea_Top);
	INICondition.Set_Double(strSection, "Mid Area", m_dRes2MidArea_Top);
	INICondition.Set_Double(strSection, "Max Area", m_dRes2MaxArea_Top);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iRes2MinNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iRes2MidNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iRes2MaxNumAccept_Top);

	strSection = "Top_WhiteDot";
	INICondition.Set_Double(strSection, "Min Area", m_dWhiteDotMinArea_Top);
	INICondition.Set_Double(strSection, "Mid Area", m_dWhiteDotMidArea_Top);
	INICondition.Set_Double(strSection, "Max Area", m_dWhiteDotMaxArea_Top);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iWhiteDotMinNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iWhiteDotMidNumAccept_Top);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iWhiteDotMaxNumAccept_Top);
#pragma endregion
	//////////////////////////////////////////////////////////////////////////
#pragma region Inner
	strSection = "Inner_Dirt";
	INICondition.Set_Double(strSection, "Min Area", m_dDirtMinArea_Inner);
	INICondition.Set_Double(strSection, "Mid Area", m_dDirtMidArea_Inner);
	INICondition.Set_Double(strSection, "Max Area", m_dDirtMaxArea_Inner);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iDirtMinNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iDirtMidNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iDirtMaxNumAccept_Inner);

	strSection = "Inner_Scratch";
	INICondition.Set_Double(strSection, "Min Area", m_dScratchMinArea_Inner);
	INICondition.Set_Double(strSection, "Mid Area", m_dScratchMidArea_Inner);
	INICondition.Set_Double(strSection, "Max Area", m_dScratchMaxArea_Inner);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iScratchMinNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iScratchMidNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iScratchMaxNumAccept_Inner);

	strSection = "Inner_Stain";
	INICondition.Set_Double(strSection, "Min Area", m_dStainMinArea_Inner);
	INICondition.Set_Double(strSection, "Mid Area", m_dStainMidArea_Inner);
	INICondition.Set_Double(strSection, "Max Area", m_dStainMaxArea_Inner);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iStainMinNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iStainMidNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iStainMaxNumAccept_Inner);

	strSection = "Inner_Dent";
	INICondition.Set_Double(strSection, "Min Area", m_dDentMinArea_Inner);
	INICondition.Set_Double(strSection, "Mid Area", m_dDentMidArea_Inner);
	INICondition.Set_Double(strSection, "Max Area", m_dDentMaxArea_Inner);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iDentMinNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iDentMidNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iDentMaxNumAccept_Inner);

	strSection = "Inner_Chipping";
	INICondition.Set_Double(strSection, "Min Area", m_dChipMinArea_Inner);
	INICondition.Set_Double(strSection, "Mid Area", m_dChipMidArea_Inner);
	INICondition.Set_Double(strSection, "Max Area", m_dChipMaxArea_Inner);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iChipMinNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iChipMidNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iChipMaxNumAccept_Inner);

	strSection = "Inner_Epoxy";
	INICondition.Set_Double(strSection, "Min Area", m_dEpoxyMinArea_Inner);
	INICondition.Set_Double(strSection, "Mid Area", m_dEpoxyMidArea_Inner);
	INICondition.Set_Double(strSection, "Max Area", m_dEpoxyMaxArea_Inner);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iEpoxyMinNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iEpoxyMidNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iEpoxyMaxNumAccept_Inner);

	strSection = "Inner_Outside_CT";
	INICondition.Set_Double(strSection, "Min Area", m_dRes1MinArea_Inner);
	INICondition.Set_Double(strSection, "Mid Area", m_dRes1MidArea_Inner);
	INICondition.Set_Double(strSection, "Max Area", m_dRes1MaxArea_Inner);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iRes1MinNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iRes1MidNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iRes1MaxNumAccept_Inner);

	strSection = "Inner_Peel_Off";
	INICondition.Set_Double(strSection, "Min Area", m_dRes2MinArea_Inner);
	INICondition.Set_Double(strSection, "Mid Area", m_dRes2MidArea_Inner);
	INICondition.Set_Double(strSection, "Max Area", m_dRes2MaxArea_Inner);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iRes2MinNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iRes2MidNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iRes2MaxNumAccept_Inner);

	strSection = "Inner_WhiteDot";
	INICondition.Set_Double(strSection, "Min Area", m_dWhiteDotMinArea_Inner);
	INICondition.Set_Double(strSection, "Mid Area", m_dWhiteDotMidArea_Inner);
	INICondition.Set_Double(strSection, "Max Area", m_dWhiteDotMaxArea_Inner);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iWhiteDotMinNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iWhiteDotMidNumAccept_Inner);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iWhiteDotMaxNumAccept_Inner);
#pragma endregion
	//////////////////////////////////////////////////////////////////////////
#pragma region Bottom
	strSection = "Bottom_Dirt";
	INICondition.Set_Double(strSection, "Min Area", m_dDirtMinArea_Btm);
	INICondition.Set_Double(strSection, "Mid Area", m_dDirtMidArea_Btm);
	INICondition.Set_Double(strSection, "Max Area", m_dDirtMaxArea_Btm);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iDirtMinNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iDirtMidNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iDirtMaxNumAccept_Btm);

	strSection = "Bottom_Scratch";
	INICondition.Set_Double(strSection, "Min Area", m_dScratchMinArea_Btm);
	INICondition.Set_Double(strSection, "Mid Area", m_dScratchMidArea_Btm);
	INICondition.Set_Double(strSection, "Max Area", m_dScratchMaxArea_Btm);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iScratchMinNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iScratchMidNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iScratchMaxNumAccept_Btm);

	strSection = "Bottom_Stain";
	INICondition.Set_Double(strSection, "Min Area", m_dStainMinArea_Btm);
	INICondition.Set_Double(strSection, "Mid Area", m_dStainMidArea_Btm);
	INICondition.Set_Double(strSection, "Max Area", m_dStainMaxArea_Btm);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iStainMinNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iStainMidNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iStainMaxNumAccept_Btm);

	strSection = "Bottom_Dent";
	INICondition.Set_Double(strSection, "Min Area", m_dDentMinArea_Btm);
	INICondition.Set_Double(strSection, "Mid Area", m_dDentMidArea_Btm);
	INICondition.Set_Double(strSection, "Max Area", m_dDentMaxArea_Btm);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iDentMinNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iDentMidNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iDentMaxNumAccept_Btm);

	strSection = "Bottom_Chipping";
	INICondition.Set_Double(strSection, "Min Area", m_dChipMinArea_Btm);
	INICondition.Set_Double(strSection, "Mid Area", m_dChipMidArea_Btm);
	INICondition.Set_Double(strSection, "Max Area", m_dChipMaxArea_Btm);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iChipMinNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iChipMidNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iChipMaxNumAccept_Btm);

	strSection = "Bottom_Epoxy";
	INICondition.Set_Double(strSection, "Min Area", m_dEpoxyMinArea_Btm);
	INICondition.Set_Double(strSection, "Mid Area", m_dEpoxyMidArea_Btm);
	INICondition.Set_Double(strSection, "Max Area", m_dEpoxyMaxArea_Btm);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iEpoxyMinNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iEpoxyMidNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iEpoxyMaxNumAccept_Btm);

	strSection = "Bottom_Outside_CT";
	INICondition.Set_Double(strSection, "Min Area", m_dRes1MinArea_Btm);
	INICondition.Set_Double(strSection, "Mid Area", m_dRes1MidArea_Btm);
	INICondition.Set_Double(strSection, "Max Area", m_dRes1MaxArea_Btm);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iRes1MinNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iRes1MidNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iRes1MaxNumAccept_Btm);

	strSection = "Bottom_Peel_Off";
	INICondition.Set_Double(strSection, "Min Area", m_dRes2MinArea_Btm);
	INICondition.Set_Double(strSection, "Mid Area", m_dRes2MidArea_Btm);
	INICondition.Set_Double(strSection, "Max Area", m_dRes2MaxArea_Btm);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iRes2MinNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iRes2MidNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iRes2MaxNumAccept_Btm);

	strSection = "Bottom_WhiteDot";
	INICondition.Set_Double(strSection, "Min Area", m_dWhiteDotMinArea_Btm);
	INICondition.Set_Double(strSection, "Mid Area", m_dWhiteDotMidArea_Btm);
	INICondition.Set_Double(strSection, "Max Area", m_dWhiteDotMaxArea_Btm);
	INICondition.Set_Integer(strSection, "Accept Min Number", m_iWhiteDotMinNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Mid Number", m_iWhiteDotMidNumAccept_Btm);
	INICondition.Set_Integer(strSection, "Accept Max Number", m_iWhiteDotMaxNumAccept_Btm);
#pragma endregion
	//////////////////////// New ////////////////////////////////

	CString sFileName = strModelFolder + "\\SW\\InspectParam.ini";
	CIniFileCS INI(sFileName);

	// Go (2016/06/02)
	CString sCommonName;

	for (int imgIdx = 0; imgIdx < MAX_IMAGE_TAB; imgIdx++)
	{
		if (imgIdx >= THEAPP.m_iMaxInspImageNo)
			break;

		strSection.Format("%s", g_sImageName[THEAPP.m_iMachineInspType][imgIdx]);
		for (int tab = 0; tab < MAX_TEACHING_TAB; tab++)
		{
			sCommonName.Format("%s_Tab%d", strSection, tab + 1);
			m_AlgorithmParam[imgIdx][tab].Save(INI, strSection, sCommonName);
		}
	}

#pragma region Lens Defect Parameter
	INI.Set_Bool(strSection, "m_bLensDirtLensInspect", m_bLensDirtLensInspect);
	INI.Set_Integer(strSection, "m_iLensDirtLensFThresLow", m_iLensDirtLensFThresLow);
	INI.Set_Integer(strSection, "m_iLensDirtLensFThresHigh", m_iLensDirtLensFThresHigh);
	INI.Set_Integer(strSection, "m_iLensDirtLensRoiOuterMargin", m_iLensDirtLensRoiOuterMargin);
	INI.Set_Bool(strSection, "m_bLensDirtLensUseConnection", m_bLensDirtLensUseConnection);
	INI.Set_Integer(strSection, "m_iLensDirtLensConnectionLength", m_iLensDirtLensConnectionLength);
	INI.Set_Integer(strSection, "m_iLensDirtLensDefectMinSize", m_iLensDirtLensDefectMinSize);

	INI.Set_Bool(strSection, "m_bLensScratchLensInspect", m_bLensScratchLensInspect);
	INI.Set_Integer(strSection, "m_iLensScratchLensDThresMediF", m_iLensScratchLensDThresMediF);
	INI.Set_Integer(strSection, "m_iLensScratchLensDThresMeanF", m_iLensScratchLensDThresMeanF);
	INI.Set_Integer(strSection, "m_iLensScratchLensDThresVal", m_iLensScratchLensDThresVal);
	INI.Set_Integer(strSection, "m_iLensScratchLensRoiOuterMargin", m_iLensScratchLensRoiOuterMargin);
	INI.Set_Bool(strSection, "m_bLensScratchLensUseConnection", m_bLensScratchLensUseConnection);
	INI.Set_Integer(strSection, "m_iLensScratchLensConnectionLength", m_iLensScratchLensConnectionLength);
	INI.Set_Integer(strSection, "m_iLensScratchLensDefectMinSize", m_iLensScratchLensDefectMinSize);
	INI.Set_Integer(strSection, "m_iLensScratchLensScratchMinLength", m_iLensScratchLensScratchMinLength);

	INI.Set_Bool(strSection, "m_bLensStainLensInspect", m_bLensStainLensInspect);
	INI.Set_Bool(strSection, "m_bLensStainLensUseDThres", m_bLensStainLensUseDThres);
	INI.Set_Integer(strSection, "m_iLensStainScaleFactor", m_iLensStainScaleFactor);
	INI.Set_Integer(strSection, "m_iLensStainLensDThresMediF", m_iLensStainLensDThresMediF);
	INI.Set_Integer(strSection, "m_iLensStainLensDThresMeanF", m_iLensStainLensDThresMeanF);
	INI.Set_Integer(strSection, "m_iLensStainLensDThresVal", m_iLensStainLensDThresVal);
	INI.Set_Bool(strSection, "m_bLensStainLensUseFThres", m_bLensStainLensUseFThres);
	INI.Set_Integer(strSection, "m_iLensStainLensFThresLow", m_iLensStainLensFThresLow);
	INI.Set_Integer(strSection, "m_iLensStainLensFThresHigh", m_iLensStainLensFThresHigh);
	INI.Set_Integer(strSection, "m_iLensStainLensRoiOuterMargin", m_iLensStainLensRoiOuterMargin);
	INI.Set_Integer(strSection, "m_iLensStainLensRoiEnforceErosion", m_iLensStainLensRoiEnforceErosion);
	INI.Set_Integer(strSection, "m_iLensStainLensRoiCastThres", m_iLensStainLensRoiCastThres);
	INI.Set_Integer(strSection, "m_iLensStainLensRoiCastMargin", m_iLensStainLensRoiCastMargin);
	INI.Set_Integer(strSection, "m_iLensStainLensDefectMinSize", m_iLensStainLensDefectMinSize);
	INI.Set_Bool(strSection, "m_bLensStainUseNoScale", m_bLensStainUseNoScale);
	INI.Set_Integer(strSection, "m_iLensStainNoScaleThres", m_iLensStainNoScaleThres);
	INI.Set_Bool(strSection, "m_bLensStainUseLength", m_bLensStainUseLength);
	INI.Set_Integer(strSection, "m_iLensStainDefectMinLength", m_iLensStainDefectMinLength);
	INI.Set_Integer(strSection, "m_iLensStainDefectMaxLength", m_iLensStainDefectMaxLength);
#pragma endregion
	///////////////////////////// Top Berrel Inner for Inspection Condition ///////////
	strSection = "Barrel Inner Condition";
	INI.Set_Integer(strSection, "m_iDecisonTopBarrelInner_RoiOuterMargin", m_iDecisonTopBarrelInner_RoiOuterMargin);
	INI.Set_Integer(strSection, "m_iDecisonTopBarrelInner_RoiInnerMargin", m_iDecisonTopBarrelInner_RoiInnerMargin);

	// ROI
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

	SetFolderFile(strModelFolder, "SW\\InspectParam.dat");

	if (!m_bOpened)
		OpenDataFile(CFile::modeCreate | CFile::modeWrite);

	if (m_bOpened)
	{
		m_pInspectionArea->Save((HANDLE)m_File.m_hFile);

		CloseDataFile();
	}

	// Model SW Version 저장
	CIniFileCS INI_MODEL(strModelFolder + "\\SW\\ModelBase.ini");
	strSection = "Model Base";

	INI_MODEL.Set_Integer(strSection, "VERSION", THEAPP.GetVersion());
	m_iModelVersion = THEAPP.GetVersion();
}

void CModelDataManager::InitLightInfo()
{
	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		for (int iLightIdx = 0; iLightIdx < MAX_LIGHT_CHANNEL; iLightIdx++)
		{
			m_iInspLightBright[iImageIdx][iLightIdx] = 0;
		}

		for (int iCtrlPageIdx = 0; iCtrlPageIdx < MAX_LIGHT_PAGE; iCtrlPageIdx++)
		{
			m_iLightPageNo[iImageIdx][iCtrlPageIdx] = 0;
		}
	}

	m_iBarcodeLightBright = 100;
}

int CModelDataManager::LoadLightInfo(CString sLinfoPath)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString sSection;

	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
			break;

		sSection = g_sImageName[THEAPP.m_iMachineInspType][iImageIdx];

		CString key;
		for (int ch = 0; ch < MAX_LIGHT_CHANNEL; ch++)
		{

			key.Format("Bright%d", ch + 1);
			m_iInspLightBright[iImageIdx][ch] = INIInspectionLight.Get_Integer(sSection, key, 500);
		}

		for (int iCtrlPageIdx = 0; iCtrlPageIdx < MAX_LIGHT_PAGE; iCtrlPageIdx++)
		{
			key.Format("Page%d", iCtrlPageIdx + 1);
			m_iLightPageNo[iImageIdx][iCtrlPageIdx] = INIInspectionLight.Get_Integer(sSection, key, 1);
		}
	}

	sSection = "Barcode";
	m_iBarcodeLightBright = INIInspectionLight.Get_Integer(sSection, "Bright1", 500);

	THEAPP.SaveLog("Light Setting Loaded");

	return 1;
}

void CModelDataManager::SaveLightInfo(CString sLinfoPath)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString sSection;

	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
			break;

		sSection.Format("%s", g_sImageName[THEAPP.m_iMachineInspType][iImageIdx]);
		CString key;
		for (int ch = 0; ch < MAX_LIGHT_CHANNEL; ch++)
		{
			key.Format("Bright%d", ch + 1);
			INIInspectionLight.Set_Integer(sSection, key, m_iInspLightBright[iImageIdx][ch]);
		}

		// 조명 페이지 설정 추가 - LeeGW
		for (int iCtrlPageIdx = 0; iCtrlPageIdx < MAX_LIGHT_PAGE; iCtrlPageIdx++)
		{
			key.Format("Page%d", iCtrlPageIdx + 1);
			INIInspectionLight.Set_Integer(sSection, key, m_iLightPageNo[iImageIdx][iCtrlPageIdx]);
		}
	}

	sSection = "Barcode";
	INIInspectionLight.Set_Integer(sSection, "Bright1", m_iBarcodeLightBright);

	// RMS add Start
	CString sData = "";
	CString sSectionTemp;
	CString sCommonString = _T("1-Side_L");

	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
			break;

		sSectionTemp.Format("%s_Image%d", sCommonString, iImageIdx + 1);

		for (int ch = 0; ch < MAX_LIGHT_CHANNEL; ch++)
		{
			sSection.Format("%s_Bright%d", sSectionTemp, ch + 1);
			sData.Format("%d", m_iInspLightBright[iImageIdx][ch]);
			THEAPP.SaveRmsData(THEAPP.Struct_PreferenceStruct.m_strRmsSaveFolderPath, m_sModelName, sSection, sData);
		}

		for (int iCtrlPageIdx = 0; iCtrlPageIdx < MAX_LIGHT_PAGE; iCtrlPageIdx++)
		{
			sSection.Format("%s_Page%d", sSectionTemp, iCtrlPageIdx + 1);
			sData.Format("%d", m_iLightPageNo[iImageIdx][iCtrlPageIdx]);
			THEAPP.SaveRmsData(THEAPP.Struct_PreferenceStruct.m_strRmsSaveFolderPath, m_sModelName, sSection, sData);
		}
	}
	sSection.Format("%s_Barcode_Bright1", sCommonString);
	sData.Format("%d", m_iBarcodeLightBright);

	THEAPP.SaveRmsData(THEAPP.Struct_PreferenceStruct.m_strRmsSaveFolderPath, m_sModelName, sSection, sData);
}

void CModelDataManager::InitLightAverageValue()
{

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		for (int j = 0; j < MAX_LIGHT_CHANNEL; j++)
		{
			m_iLightAverageValue[i][j] = 0;
		}

		m_iLightAverageValueTotal[i] = 0;
	}
}

int CModelDataManager::LoadLightAverageValue(CString sLinfoPath)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString sSection, strItem;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		sSection.Format("Image_%d", i + 1);

		for (int j = 0; j < MAX_LIGHT_CHANNEL; j++)
		{
			strItem.Format("Average_Value_%d", j + 1);
			m_iLightAverageValue[i][j] = INIInspectionLight.Get_Integer(sSection, strItem, 0);
		}

		strItem.Format("Average_Value_Total");
		m_iLightAverageValueTotal[i] = INIInspectionLight.Get_Integer(sSection, strItem, 0);
	}

	return 1;
}

void CModelDataManager::SaveLightAverageValue(CString sLinfoPath)
{
	CIniFileCS INIInspectionLight(sLinfoPath);

	CString sSection, strItem;

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		sSection.Format("Image_%d", i + 1);

		for (int j = 0; j < MAX_LIGHT_CHANNEL; j++)
		{
			strItem.Format("Average_Value_%d", j + 1);
			INIInspectionLight.Set_Integer(sSection, strItem, m_iLightAverageValue[i][j]);
		}

		strItem.Format("Average_Value_Total");
		INIInspectionLight.Set_Integer(sSection, strItem, m_iLightAverageValueTotal[i]);
	}
}

void CModelDataManager::ModelPreSet()
{
	m_viModuleNo.resize(m_iTrayModuleMax);
	m_viPosInspNo.resize(m_iTrayModuleMax);
	for (int ii = 0; ii < m_iTrayModuleMax; ii++)
	{
		m_viModuleNo[ii] = ii + 1;
		m_viPosInspNo[ii] = ii;
	}

	int ii;
	CString strModuleNumber;

	for (ii = 0; ii < m_iTrayModuleMax; ii++)
	{
		m_viModuleNo[ii] = -1;
		m_viPosInspNo[ii] = -1;
	}

	int iPosReadCnt = 0;

	for (ii = 0; ii < m_iTrayArrayY; ii++)
	{
		for (int j = 0; j < m_iTrayArrayX; j++)
		{
			m_viModuleNo[iPosReadCnt] = iPosReadCnt + 1; // 검사 시퀀스에 포켓 번호 할당
			m_viPosInspNo[iPosReadCnt] = iPosReadCnt;	 // 포켓 배열에 검사시퀀스 할당

			iPosReadCnt++;
		}
	}

	// Motion Position Loading

	m_mdInspectPosXY.resize(THEAPP.m_iModuleNumberOneLine);
	m_mdBarcodeScanPosXY.resize(THEAPP.m_iModuleNumberOneLine);

	for (ii = 0; ii < THEAPP.m_iModuleNumberOneLine; ii++)
	{
		m_mdInspectPosXY[ii].resize(2);
		m_mdInspectPosXY[ii].assign(2, -1);
		m_mdBarcodeScanPosXY[ii].resize(2);
		m_mdBarcodeScanPosXY[ii].assign(2, -1);
	}

	CString FolderName = ".";
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		FolderName = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	else
		FolderName = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

	CString PosFileName_1 = FolderName + "\\HW\\MotionMovingPosition1.ini";
	CIniFileCS PosINI_1(PosFileName_1);

	//////////////////////////////////////////////////////////////////////////
	// Insp Z Position
	double dInspPosZ;
	CString strSection, strKey, strTemp;
	strSection = "Insp Z Position";
	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		strKey.Format("Image%d", iImageIdx + 1);
		dInspPosZ = PosINI_1.Get_Double(strSection, strKey, 0.0);
		m_mdInspPosZ[iImageIdx] = dInspPosZ;

		strTemp.Format("Insp Z Position = %.3f", m_mdInspPosZ[iImageIdx]);
		THEAPP.SaveLog(strTemp);
	}

	// SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z1, m_mdInspPosZ[0]);
	// SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z2, m_mdInspPosZ[1]);
	// SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z3, m_mdInspPosZ[2]);
	// SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z4, m_mdInspPosZ[3]);
	// SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z5, m_mdInspPosZ[4]);
	// SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z6, m_mdInspPosZ[5]);
	// SetDlgItemText(IDC_EDIT_SET_INSP_POSITION_Z7, m_mdInspPosZ[6]);
	//////////////////////////////////////////////////////////////////////////

	CString PosFileName_Barcode = FolderName + "\\HW\\BarcodeMotionMovingPosition.ini";
	CIniFileCS PosINI_Barcode(PosFileName_Barcode);

	CString sSectionPosXY = "Moving Position";

	THEAPP.SaveLog("Inspection Position is loaded");
	THEAPP.SaveLog(" Nr Pos Mod  (   x   ,    y   )");

	iPosReadCnt = 0;
	ii = 0;
	int iModuleNumber;

	for (int j = 0; j < THEAPP.m_iModuleNumberOneLine; j++)
	{
		strModuleNumber.Format("X-%d", iPosReadCnt);
		double AMoveX = PosINI_1.Get_Double(sSectionPosXY, strModuleNumber, 0.0);
		strModuleNumber.Format("Y-%d", iPosReadCnt);
		double AMoveY = PosINI_1.Get_Double(sSectionPosXY, strModuleNumber, 0.0);

		m_mdInspectPosXY[iPosReadCnt][0] = AMoveX;
		m_mdInspectPosXY[iPosReadCnt][1] = AMoveY;

		CString str;
		str.Format("%3d %3d (%7.3f, %7.3f)", ii, iPosReadCnt, m_mdInspectPosXY[iPosReadCnt][0], m_mdInspectPosXY[iPosReadCnt][1]);
		THEAPP.SaveLog(str);

		// Barcode
		strModuleNumber.Format("X-%d", iPosReadCnt);
		AMoveX = PosINI_Barcode.Get_Double(sSectionPosXY, strModuleNumber, 0.0);

		m_mdBarcodeScanPosXY[iPosReadCnt][0] = AMoveX;
		m_mdBarcodeScanPosXY[iPosReadCnt][1] = 0;

		str.Format("%3d %3d (%7.3f, %7.3f)", ii, iPosReadCnt, m_mdBarcodeScanPosXY[iPosReadCnt][0], m_mdBarcodeScanPosXY[iPosReadCnt][1]);
		THEAPP.SaveLog(str);

		iPosReadCnt++;
	}
}

void CModelDataManager::ModelPreSet(int iLineNo)
{
	m_viInspectModuleNo.resize(THEAPP.m_iModuleNumberOneLine);

	CString strModuleNumber;

	for (int ii = 0; ii < THEAPP.m_iModuleNumberOneLine; ii++)
	{
		m_viInspectModuleNo[ii] = -1;
	}

	int iPosReadCnt = 0;
	int iModuleNumber;
	CString sErrMsg;

	if (iLineNo < 1 || iLineNo > THEAPP.m_iTrayLineNumber)
	{
		sErrMsg.Format("핸들러로부터 잘못된 라인번호가 전송되었습니다. 라인번호 %d", iLineNo);
		AfxMessageBox(sErrMsg, MB_ICONERROR | MB_SYSTEMMODAL);

		iLineNo = 1;
	}

	if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_X)
	{
		for (int j = 0; j < THEAPP.m_iModuleNumberOneLine; j++)
		{
			iModuleNumber = iLineNo + THEAPP.m_iTrayLineNumber * j;
			m_viInspectModuleNo[iPosReadCnt] = iModuleNumber; // 검사 시퀀스에 포켓 번호 할당

			iPosReadCnt++;
		}
	}
	else
	{
		for (int j = 0; j < THEAPP.m_iModuleNumberOneLine; j++)
		{
			iModuleNumber = (THEAPP.m_iTrayLineNumber - iLineNo) * THEAPP.m_iModuleNumberOneLine + j + 1;
			// iModuleNumber = (iLineNo - 1) * THEAPP.m_iModuleNumberOneLine + j + 1;
			m_viInspectModuleNo[iPosReadCnt] = iModuleNumber; // 검사 시퀀스에 포켓 번호 할당

			iPosReadCnt++;
		}
	}
}

void CModelDataManager::LoadModel()
{
	// TODO : Add evenet handler code here
	LoadModelBaseData();
	// need to optimize - 251105, jhkim
	LoadModelData();
	LoadModelContourData();
}

void CModelDataManager::LoadModelBaseData()
{
	if (m_sModelName == ".")
		return;

	CString strModelFolder;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

	if (GetFileAttributes(strModelFolder) == -1)
	{
		m_sModelName = "default";
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
			strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
		else
			strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;
	}

	CIniFileCS INI(strModelFolder + "\\SW\\ModelBase.ini");
	CString strSection = "Model Base";

	m_iModelVersion = INI.Get_Integer(strSection, "VERSION", 1000);

	THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_MODEL_NAME, m_sModelName);
	CString sModelName = INI.Get_String(strSection, "Model Name", "default");
	if (sModelName != m_sModelName)
	{
		DoubleLogOut("Model Name mismatch. %s(INI) != %s()", sModelName, m_sModelName);
		THEAPP.m_pInspectResultDlg->SetDlgItemText(IDC_EDIT_MODEL_NAME, "FALSE");
	}

	m_iTrayArrayX = INI.Get_Integer(strSection, "TrayArrayX", 6);
	m_iTrayArrayY = INI.Get_Integer(strSection, "TrayArrayY", 6);

	//////////////////////////////////////////////////////////////////////////

	if (THEAPP.Struct_PreferenceStruct.m_iPickerTrayDir == PICKER_TRAY_DIR_X)
	{
		THEAPP.m_iTrayLineNumber = m_iTrayArrayY;
		THEAPP.m_iModuleNumberOneLine = m_iTrayArrayX;
	}
	else
	{
		THEAPP.m_iTrayLineNumber = m_iTrayArrayX;
		THEAPP.m_iModuleNumberOneLine = m_iTrayArrayY;
	}

	THEAPP.m_iMaxModuleOneTray = THEAPP.m_iTrayLineNumber * THEAPP.m_iModuleNumberOneLine + 1;
	THEAPP.m_iMaxModuleOneLot = THEAPP.m_iTrayLineNumber * THEAPP.m_iModuleNumberOneLine * MAX_TRAY_LOT;

	//////////////////////////////////////////////////////////////////////////

	m_iTrayModuleMax = m_iTrayArrayX * m_iTrayArrayY;
	m_iTrayModuleAmt = INI.Get_Integer(strSection, "TrayLotModuleAmt", m_iTrayModuleMax);

	if (m_iTrayModuleAmt > m_iTrayModuleMax)
		m_iTrayModuleAmt = m_iTrayModuleMax; // 모듈 개수는 트레이의 용량보다 많을 수 없음

	m_iTrayModuleAmt = m_iTrayModuleMax; // Add

	// Matching Info
	m_iRefContRow = INI.Get_Integer(strSection, "MatchingContourCenterY", 640);
	m_iRefContCol = INI.Get_Integer(strSection, "MatchingContourCenterX", 570);
	m_iRefContRad1 = INI.Get_Integer(strSection, "MatchingContour1Radius", 410);
	m_iRefContRad2 = INI.Get_Integer(strSection, "MatchingContour2Radius", 360);
	m_iRefContRad3 = INI.Get_Integer(strSection, "MatchingContour3Radius", 325);
	m_iRefContRad4 = INI.Get_Integer(strSection, "MatchingContour4Radius", 190);
	m_dContRatio23 = INI.Get_Double(strSection, "ContRatio23", 0.91);
	m_dContRatio45 = INI.Get_Double(strSection, "ContRatio45", 0.83);
	m_iBlackMatchingImageType = INI.Get_Integer(strSection, "BlackBarrelMatchingImage", 1);
	m_bUseMatchingAngle = INI.Get_Bool(strSection, "UseContour1MatchingAngle", FALSE);
	m_iMatchingAngleRange = INI.Get_Integer(strSection, "Contour1MatchingAngleRange", 10);
	m_bUseMatchingScale = INI.Get_Bool(strSection, "UseScaleMatching", TRUE);
	m_iMatchingScaleMin = INI.Get_Integer(strSection, "m_iMatchingScaleMin", 98);
	m_iMatchingScaleMax = INI.Get_Integer(strSection, "m_iMatchingScaleMax", 102);

	m_iMatchingContrastC1 = INI.Get_Integer(strSection, "MatchingMinContrastContour1", 15);
	m_iMatchingContrastC2 = INI.Get_Integer(strSection, "MatchingMinContrastContour2", 10);
	m_iMatchingContrastC3 = INI.Get_Integer(strSection, "MatchingMinContrastContour3", 10);
	m_iMatchingContrastC4 = INI.Get_Integer(strSection, "MatchingMinContrastContour4", 10);

	m_iBarcodeLTPointX = INI.Get_Integer(strSection, "m_iBarcodeLTPointX", PGCAM_IMAGE_WIDTH / 2 - PGCAM_IMAGE_WIDTH / 4);
	m_iBarcodeLTPointY = INI.Get_Integer(strSection, "m_iBarcodeLTPointY", PGCAM_IMAGE_HEIGHT / 2 - PGCAM_IMAGE_HEIGHT / 4);
	m_iBarcodeRBPointX = INI.Get_Integer(strSection, "m_iBarcodeRBPointX", PGCAM_IMAGE_WIDTH / 2 + PGCAM_IMAGE_WIDTH / 4);
	m_iBarcodeRBPointY = INI.Get_Integer(strSection, "m_iBarcodeRBPointY", PGCAM_IMAGE_HEIGHT / 2 + PGCAM_IMAGE_HEIGHT / 4);

	// TODO : Need to optimize - 251104, jhkim
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iRefContRow = m_iRefContRow;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iRefContCol = m_iRefContCol;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iRefContRad1 = m_iRefContRad1;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iRefContRad2 = m_iRefContRad2;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iRefContRad3 = m_iRefContRad3;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iRefContRad4 = m_iRefContRad4;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_dContRatio23 = m_dContRatio23;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_dContRatio45 = m_dContRatio45;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iRadioBlackMatchingImageType = m_iBlackMatchingImageType;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_bCheckUseMatchingAngle = m_bUseMatchingAngle;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iEditMatchingAngleRange = m_iMatchingAngleRange;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_bCheckUseMatchingScale = m_bUseMatchingScale;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iEditMatchingScaleMin = m_iMatchingScaleMin;
	THEAPP.m_pTabControlDlg->m_pMatchingDlg->m_iEditMatchingScaleMax = m_iMatchingScaleMax;

	THEAPP.m_pTabControlDlg->m_pMatchingDlg->UpdateData(FALSE);

	m_dModulePitchX = INI.Get_Double(strSection, "ModulePitchX", 1.0);
	m_dModulePitchY = INI.Get_Double(strSection, "ModulePitchY", 1.0);
	THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_Edit_SetModulePitchX = m_dModulePitchX;
	THEAPP.m_pTabControlDlg->m_pJogSetDlg->m_Edit_SetModulePitchY = m_dModulePitchY;
	THEAPP.m_pTabControlDlg->m_pJogSetDlg->UpdateData(FALSE);

	m_iTrayHESizeX = INI.Get_Integer(strSection, "TrayHESizeX", 600);
	m_iTrayHESizeY = INI.Get_Integer(strSection, "TrayHESizeY", 712);

	m_bOneBarrel = INI.Get_Bool(strSection, "OneBarrel", FALSE);

	m_bBlackCoating = INI.Get_Bool(strSection, "Black Coating", FALSE);

	//////////////////////////////////////////////////////////////////////////

	CString szTemplateFolder, sTemplateFolder;

	szTemplateFolder = strModelFolder + "\\SW\\MatchingModel";

	strSection = "ROI Align";

	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
	for (int img = 0; img < BARREL_LENS_IMAGE_TAB; img++)
	{
		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			if (m_iBLROIAlignMatchingModel[img][cont] >= 0)
			{
				clear_ncc_model(m_iBLROIAlignMatchingModel[img][cont]);
			}
		}
	}

	CString strKey;
	for (int img = 0; img < BARREL_LENS_IMAGE_TAB; img++)
	{
		strKey.Format("m_bUseBLROIAlign_%d", img + 1);
		m_bUseBLROIAlign[img] = INI.Get_Bool(strSection, strKey, FALSE);

		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			strKey.Format("m_bUseBLROIAlignContour_%d_%d", img + 1, cont + 1);
			m_bUseBLROIAlignContour[img][cont] = INI.Get_Bool(strSection, strKey, FALSE);
		}

		strKey.Format("m_iBLROIAlignInnerMargin_%d", img + 1);
		m_iBLROIAlignInnerMargin[img] = INI.Get_Integer(strSection, strKey, 0);

		strKey.Format("m_iBLROIAlignOuterMargin_%d", img + 1);
		m_iBLROIAlignOuterMargin[img] = INI.Get_Integer(strSection, strKey, 0);
	}

	m_dBLROIAlignMatchingScore = INI.Get_Double(strSection, "m_dBLROIAlignMatchingScore", 0.1);
	m_iBLROIAlignSearchingMargin = INI.Get_Integer(strSection, "m_iBLROIAlignSearchingMargin", 0);

	for (int img = 0; img < BARREL_LENS_IMAGE_TAB; img++)
	{
		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			sTemplateFolder.Format("%s\\BLROIAlignModel_%d_%d", szTemplateFolder, img + 1, cont + 1);

			CFileFind finder;
			if (finder.FindFile((LPCTSTR)sTemplateFolder))
			{
				read_ncc_model(HTuple(sTemplateFolder), &(m_iBLROIAlignMatchingModel[img][cont]));
			}
		}
	}
	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW END

	//////////////////////////////////////////////////////////////////////////

	strSection = "Local Align";

	// Reset Local Align Model
	int i, j;
	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (m_lLAlignModelID[i] >= 0)
		{
			if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_TEMPLATE)
				clear_ncc_model(m_lLAlignModelID[i]);
			else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_SHAPE)
				clear_shape_model(m_lLAlignModelID[i]);

			m_lLAlignModelID[i] = -1;
		}
	}

	CString sTemp;
	BOOL bTemp;

	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		sTemp.Format("m_bUseLocalAlignMatching_%d", i + 1);
		m_bUseLocalAlignMatching[i] = INI.Get_Bool(strSection, sTemp, FALSE);

		sTemp.Format("m_iLocalMatchingMethod_%d", i + 1);
		m_iLocalMatchingMethod[i] = INI.Get_Integer(strSection, sTemp, ALIGN_MATCHING_SHAPE);

		sTemp.Format("m_iLocalMatchingTeachingRectLTX_%d", i + 1);
		m_iLocalMatchingTeachingRectLTX[i] = INI.Get_Integer(strSection, sTemp, 0);
		sTemp.Format("m_iLocalMatchingTeachingRectLTY_%d", i + 1);
		m_iLocalMatchingTeachingRectLTY[i] = INI.Get_Integer(strSection, sTemp, 0);
		sTemp.Format("m_iLocalMatchingTeachingRectRBX_%d", i + 1);
		m_iLocalMatchingTeachingRectRBX[i] = INI.Get_Integer(strSection, sTemp, 0);
		sTemp.Format("m_iLocalMatchingTeachingRectRBY_%d", i + 1);
		m_iLocalMatchingTeachingRectRBY[i] = INI.Get_Integer(strSection, sTemp, 0);

		sTemp.Format("m_iLocalMatchingPyramidLevel_%d", i + 1);
		m_iLocalMatchingPyramidLevel[i] = INI.Get_Integer(strSection, sTemp, 0);
		sTemp.Format("m_iLocalMatchingAngleRange_%d", i + 1);
		m_iLocalMatchingAngleRange[i] = INI.Get_Double(strSection, sTemp, 5.0);
		sTemp.Format("m_dLocalMatchingScaleMin_%d", i + 1);
		m_dLocalMatchingScaleMin[i] = INI.Get_Double(strSection, sTemp, 0.98);
		sTemp.Format("m_dLocalMatchingScaleMax_%d", i + 1);
		m_dLocalMatchingScaleMax[i] = INI.Get_Double(strSection, sTemp, 1.02);

		sTemp.Format("m_iLocalMatchingTeachingContrast_%d", i + 1);
		m_iLocalMatchingTeachingContrast[i] = INI.Get_Integer(strSection, sTemp, 40);

		sTemp.Format("m_iLocalMatchingTeachingContrastLow_%d", i + 1);
		m_iLocalMatchingTeachingContrastLow[i] = INI.Get_Integer(strSection, sTemp, 40);

		sTemp.Format("m_iLocalMatchingTeachingContrastMinSize_%d", i + 1);
		m_iLocalMatchingTeachingContrastMinSize[i] = INI.Get_Integer(strSection, sTemp, 1);

		sTemp.Format("m_dLocalMatchingScore_%d", i + 1);
		m_dLocalMatchingScore[i] = INI.Get_Double(strSection, sTemp, 0.3);
		sTemp.Format("m_iLocalMatchingInspectionMinContrast_%d", i + 1);
		m_iLocalMatchingInspectionMinContrast[i] = INI.Get_Integer(strSection, sTemp, 15);
		sTemp.Format("m_iLocalMatchingSearchMarginX_%d", i + 1);
		m_iLocalMatchingSearchMarginX[i] = INI.Get_Integer(strSection, sTemp, 300);
		sTemp.Format("m_iLocalMatchingSearchMarginY_%d", i + 1);
		m_iLocalMatchingSearchMarginY[i] = INI.Get_Integer(strSection, sTemp, 300);
		sTemp.Format("m_dLocalTeachAlignRefX_%d", i + 1);
		m_dLocalTeachAlignRefX[i] = INI.Get_Double(strSection, sTemp, -1);
		sTemp.Format("m_dLocalTeachAlignRefY_%d", i + 1);
		m_dLocalTeachAlignRefY[i] = INI.Get_Double(strSection, sTemp, -1);
		sTemp.Format("m_bUseMatchingAffineConstant_%d", i + 1);
		m_bUseMatchingAffineConstant[i] = INI.Get_Bool(strSection, sTemp, FALSE);

		for (j = 0; j < MAX_IMAGE_TAB; j++)
		{
			if (j >= THEAPP.m_iMaxInspImageNo)
				break;

			sTemp.Format("m_bLocalAlignImage_%d_%d", i + 1, j + 1);
			m_bLocalAlignImage[i][j] = INI.Get_Bool(strSection, sTemp, FALSE);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Load Local Align Model

	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		sTemplateFolder.Format("%s\\LocalAlignModel_%d", szTemplateFolder, i + 1);

		CFileFind finder;

		if (finder.FindFile((LPCTSTR)sTemplateFolder))
		{
			if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_TEMPLATE)
				read_ncc_model(HTuple(sTemplateFolder), &(m_lLAlignModelID[i]));
			else if (m_iLocalMatchingMethod[i] == ALIGN_MATCHING_SHAPE)
				read_shape_model(HTuple(sTemplateFolder), &(m_lLAlignModelID[i]));
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Load Shift Parameter

	strSection = "Barcode Shift";
	m_bInspectBarcodeShift = INI.Get_Bool(strSection, "m_bInspectBarcodeShift", TRUE);
	m_dBarcodeShiftRefX = INI.Get_Double(strSection, "m_dBarcodeShiftRefX", 0.0);
	m_dBarcodeShiftRefY = INI.Get_Double(strSection, "m_dBarcodeShiftRefY", 0.0);
	m_dBarcodeShiftToleranceX = INI.Get_Double(strSection, "m_dBarcodeShiftToleranceX", 10.0);
	m_dBarcodeShiftToleranceY = INI.Get_Double(strSection, "m_dBarcodeShiftToleranceY", 10.0);

	// Load //Barcode Rotation Inspection
	strSection = "Barcode Rotation";
	m_bInspectBarcodeRotation = INI.Get_Bool(strSection, "m_bInspectBarcodeRotation", FALSE);
	m_dBarcodeShiftToleranceRotation = INI.Get_Double(strSection, "m_dBarcodeShiftToleranceRotation", 3.0);
	m_dBarcodeShiftRefRotation = INI.Get_Double(strSection, "m_dBarcodeShiftRefRotation", 0.0);

	// Barcode Hole Center Start
	strSection = "Barcode Hole Center";
	m_bUseBarcodeShiftHoleCenter = INI.Get_Bool(strSection, "m_bUseBarcodeShiftHoleCenter", FALSE);
	m_iBarcodeShiftHoleCenterGvMin = INI.Get_Integer(strSection, "m_iBarcodeShiftHoleCenterGvMin", 80);
	m_iBarcodeShiftHoleCenterGvMax = INI.Get_Integer(strSection, "m_iBarcodeShiftHoleCenterGvMax", 255);
	m_iBarcodeShiftHoleCenterGvClosing = INI.Get_Integer(strSection, "m_iBarcodeShiftHoleCenterGvClosing", 10);
	m_iBarcodeShiftHoleCenterDilation = INI.Get_Integer(strSection, "m_iBarcodeShiftHoleCenterDilation", 100);
	// Barcode Hole Center End

	strSection = "Circle Diameter";
	m_bInspectBlackCoatingDiameter = INI.Get_Bool(strSection, "m_bInspectBlackCoatingDiameter", TRUE);
	m_dBlackCoatingRef = INI.Get_Double(strSection, "m_dBlackCoatingRef", 0.0);
	m_dBlackCoatingTolerance = INI.Get_Double(strSection, "m_dBlackCoatingTolerance", 0.0);
	m_dBlackCoatingOffset = INI.Get_Double(strSection, "m_dBlackCoatingOffset", 0.0);

	m_iBlackCoatingImage = INI.Get_Integer(strSection, "m_iBlackCoatingImage", 0);
	m_iBlackCoatingContour = INI.Get_Integer(strSection, "m_iBlackCoatingContour", 2);
	m_iBlackCoatingOuterMargin = INI.Get_Integer(strSection, "m_iBlackCoatingOuterMargin", 10);
	m_iBlackCoatingInnerMargin = INI.Get_Integer(strSection, "m_iBlackCoatingInnerMargin", -10);
	m_iBlackCoatingMinGray = INI.Get_Integer(strSection, "m_iBlackCoatingMinGray", 0);
	m_iBlackCoatingMaxGray = INI.Get_Integer(strSection, "m_iBlackCoatingMaxGray", 90);

	m_iDatumImage = INI.Get_Integer(strSection, "m_iDatumImage", 0);
	m_iDatumContour = INI.Get_Integer(strSection, "m_iDatumContour", 3);
	m_iDatumOuterMargin = INI.Get_Integer(strSection, "m_iDatumOuterMargin", 10);
	m_iDatumInnerMargin = INI.Get_Integer(strSection, "m_iDatumInnerMargin", -10);
	m_iDatumMinGray = INI.Get_Integer(strSection, "m_iDatumMinGray", 0);
	m_iDatumMaxGray = INI.Get_Integer(strSection, "m_iDatumMaxGray", 90);

	m_bUseImageScaling_BC = INI.Get_Bool(strSection, "m_bUseImageScaling_BC", FALSE);
	m_bUseImageScaling_DT = INI.Get_Bool(strSection, "m_bUseImageScaling_DT", FALSE);
	m_iImageScaleMethod_BC = INI.Get_Integer(strSection, "m_iImageScaleMethod_BC", 0);
	m_iImageScaleMethod_DT = INI.Get_Integer(strSection, "m_iImageScaleMethod_DT", 0);
	m_iImageScaleMax_BC = INI.Get_Integer(strSection, "m_iImageScaleMax_BC", 100);
	m_iImageScaleMax_DT = INI.Get_Integer(strSection, "m_iImageScaleMax_DT", 100);
	m_iImageScaleMin_BC = INI.Get_Integer(strSection, "m_iImageScaleMin_BC", 0);
	m_iImageScaleMin_DT = INI.Get_Integer(strSection, "m_iImageScaleMin_DT", 0);

	m_iInspectionType = INI.Get_Integer(strSection, "m_iInspectionType", 0);

	m_iBlackCoatingWidth = INI.Get_Integer(strSection, "m_iBlackCoatingWidth", 5);
	m_iBlackCoatingHeight = INI.Get_Integer(strSection, "m_iBlackCoatingHeight", 20);
	m_iBlackCoatingDiffGray = INI.Get_Integer(strSection, "m_iBlackCoatingDiffGray", 5);
	m_iBlackCoatingTransition = INI.Get_Integer(strSection, "m_iBlackCoatingTransition", 0);
	m_iDatumWidth = INI.Get_Integer(strSection, "m_iDatumWidth", 5);
	m_iDatumHeight = INI.Get_Integer(strSection, "m_iDatumHeight", 20);
	m_iDatumDiffGray = INI.Get_Integer(strSection, "m_iDatumDiffGray", 5);
	m_iDatumTransition = INI.Get_Integer(strSection, "m_iDatumTransition", 0);
	//////////////////////////////////////////////////////////////////////////

	CIniFileCS AutoCalINI(strModelFolder + "\\HW\\LightAutoCal.ini");
	strSection = "Auto Cal Setting";

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		sTemp.Format("m_iLightValueStart_%d", i);
		m_iLightValueStart[i] = AutoCalINI.Get_Integer(strSection, sTemp, 500);

		sTemp.Format("m_iLightValueEnd_%d", i);
		m_iLightValueEnd[i] = AutoCalINI.Get_Integer(strSection, sTemp, 800);

		sTemp.Format("m_iLightValueInterval_%d", i);
		m_iLightValueInterval[i] = AutoCalINI.Get_Integer(strSection, sTemp, 5);

		sTemp.Format("m_iLightValueInTol_%d", i);
		m_iLightValueInTol[i] = AutoCalINI.Get_Integer(strSection, sTemp, 800);

		sTemp.Format("m_iTotalImageValueTol_%d", i);
		m_iTotalImageValueTol[i] = AutoCalINI.Get_Integer(strSection, sTemp, 80);
	}

	ModelPreSet();
}

void CModelDataManager::LoadModelData()
{
	if (m_sModelName == ".")
		return;

	CString strModelFolder;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

	CString strSection, sCommonName;

	// FAI 치수 측정 - LeeGW
	CString sFAIParamFile = strModelFolder + "\\SW\\InspectParam_FAI.ini";
	CIniFileCS INIFAIParam(sFAIParamFile);

	strSection = "FAI_SET_OPTION";
	CString strValue;
	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{

		strValue.Format("MULTIPLE_FAI_%d", i);
		m_dFAIMultiple[i] = INIFAIParam.Get_Double(strSection, strValue, 1.0);

		strValue.Format("OFFSET_FAI_%d", i);
		m_dFAIOffset[i] = INIFAIParam.Get_Double(strSection, strValue, 0.0);

		strValue.Format("SPEC_MIN_FAI_%d", i);
		m_dFAISpecMin[i] = INIFAIParam.Get_Double(strSection, strValue, 0.0);

		strValue.Format("SPEC_MAX_FAI_%d", i);
		m_dFAISpecMax[i] = INIFAIParam.Get_Double(strSection, strValue, 1.0);

		strValue.Format("USE_NG_FAI_%d", i);
		m_bFAINGUse[i] = INIFAIParam.Get_Bool(strSection, strValue, FALSE);

		strValue.Format("USE_SPECIAL_NG_FAI_%d", i);
		m_bFAISpecialNGUse[i] = INIFAIParam.Get_Bool(strSection, strValue, FALSE);

		strValue.Format("USE_UNIQUE_OFFSET_FAI_%d", i);
		m_bFAIUniqueOffsetUse[i] = INIFAIParam.Get_Bool(strSection, strValue, FALSE);

		for (int ii = 0; ii < MAX_CIRCULAR_TABLE_POCKET; ii++)
		{
			if (ii < (THEAPP.m_iModuleNumberOneLine * MAX_CIRCULAR_TABLE_INDEX))
			{
				strValue.Format("MULTIPLE_FAI_%d_%d", i, ii + 1);
				m_dFAIUniqueMultiple[i][ii] = INIFAIParam.Get_Double(strSection, strValue, 1.0);

				strValue.Format("OFFSET_FAI_%d_%d", i, ii + 1);
				m_dFAIUniqueOffset[i][ii] = INIFAIParam.Get_Double(strSection, strValue, 0.0);
			}
			else
			{
				m_dFAIUniqueMultiple[i][ii] = 1.0;
				m_dFAIUniqueOffset[i][ii] = 0.0;
			}
		}
	}

	strValue.Format("FAI_INNER_CIRCLE_TYPE");
	m_iFAIInnerCircleFitType = INIFAIParam.Get_Integer(strSection, strValue, 0);

	strValue.Format("FAI_OUTER_CIRCLE_TYPE");
	m_iFAIOuterCircleFitType = INIFAIParam.Get_Integer(strSection, strValue, 0);

	strValue.Format("FAI_DATUM_CIRCLE_TYPE");
	m_iFAIDatumCircleFitType = INIFAIParam.Get_Integer(strSection, strValue, 0);

	strValue.Format("FAI_SIMUL_POCKET_NUM");
	m_iFAISimulPocketNum = INIFAIParam.Get_Integer(strSection, strValue, 0);
	

	// Inspect Param
	CString sFileName = strModelFolder + "\\SW\\InspectParam.ini";
	CIniFileCS INI(sFileName);

	for (int imgIdx = 0; imgIdx < MAX_IMAGE_TAB; imgIdx++)
	{
		if (imgIdx >= THEAPP.m_iMaxInspImageNo)
			break;

		strSection.Format("%s", g_sImageName[THEAPP.m_iMachineInspType][imgIdx]);
		for (int tab = 0; tab < MAX_TEACHING_TAB; tab++)
		{
			sCommonName.Format("%s_Tab%d", strSection, tab + 1);
			m_AlgorithmParam[imgIdx][tab].Load(INI, strSection, sCommonName);
		}
	}

	m_bLensDirtLensInspect = INI.Get_Bool(strSection, "m_bLensDirtLensInspect", TRUE);
	m_iLensDirtLensFThresLow = INI.Get_Integer(strSection, "m_iLensDirtLensFThresLow", 50);
	m_iLensDirtLensFThresHigh = INI.Get_Integer(strSection, "m_iLensDirtLensFThresHigh", 255);
	m_iLensDirtLensRoiOuterMargin = INI.Get_Integer(strSection, "m_iLensDirtLensRoiOuterMargin", -2);
	m_bLensDirtLensUseConnection = INI.Get_Bool(strSection, "m_bLensDirtLensUseConnection", FALSE);
	m_iLensDirtLensConnectionLength = INI.Get_Integer(strSection, "m_iLensDirtLensConnectionLength", 1);
	m_iLensDirtLensDefectMinSize = INI.Get_Integer(strSection, "m_iLensDirtLensDefectMinSize", 20);

	m_bLensScratchLensInspect = INI.Get_Bool(strSection, "m_bLensScratchLensInspect", TRUE);
	m_iLensScratchLensDThresMediF = INI.Get_Integer(strSection, "m_iLensScratchLensDThresMediF", 3);
	m_iLensScratchLensDThresMeanF = INI.Get_Integer(strSection, "m_iLensScratchLensDThresMeanF", 9);
	m_iLensScratchLensDThresVal = INI.Get_Integer(strSection, "m_iLensScratchLensDThresVal", 5);
	m_iLensScratchLensRoiOuterMargin = INI.Get_Integer(strSection, "m_iLensScratchLensRoiOuterMargin", -3);
	m_bLensScratchLensUseConnection = INI.Get_Bool(strSection, "m_bLensScratchLensUseConnection", FALSE);
	m_iLensScratchLensConnectionLength = INI.Get_Integer(strSection, "m_iLensScratchLensConnectionLength", 1);
	m_iLensScratchLensDefectMinSize = INI.Get_Integer(strSection, "m_iLensScratchLensDefectMinSize", 30);
	m_iLensScratchLensScratchMinLength = INI.Get_Integer(strSection, "m_iLensScratchLensScratchMinLength", 15);

	m_bLensStainLensInspect = INI.Get_Bool(strSection, "m_bLensStainLensInspect", TRUE);
	m_bLensStainLensUseDThres = INI.Get_Bool(strSection, "m_bLensStainLensUseDThres", TRUE);
	m_iLensStainScaleFactor = INI.Get_Integer(strSection, "m_iLensStainScaleFactor", 1);
	m_iLensStainLensDThresMediF = INI.Get_Integer(strSection, "m_iLensStainLensDThresMediF", 5);
	m_iLensStainLensDThresMeanF = INI.Get_Integer(strSection, "m_iLensStainLensDThresMeanF", 9);
	m_iLensStainLensDThresVal = INI.Get_Integer(strSection, "m_iLensStainLensDThresVal", 3);
	m_bLensStainLensUseFThres = INI.Get_Bool(strSection, "m_bLensStainLensUseFThres", FALSE);
	m_iLensStainLensFThresLow = INI.Get_Integer(strSection, "m_iLensStainLensFThresLow", 25);
	m_iLensStainLensFThresHigh = INI.Get_Integer(strSection, "m_iLensStainLensFThresHigh", 255);
	m_iLensStainLensRoiOuterMargin = INI.Get_Integer(strSection, "m_iLensStainLensRoiOuterMargin", -4);
	m_iLensStainLensRoiEnforceErosion = INI.Get_Integer(strSection, "m_iLensStainLensRoiEnforceErosion", 25);
	m_iLensStainLensRoiCastThres = INI.Get_Integer(strSection, "m_iLensStainLensRoiCastThres", 40);
	m_iLensStainLensRoiCastMargin = INI.Get_Integer(strSection, "m_iLensStainLensRoiCastMargin", 5);
	m_iLensStainLensDefectMinSize = INI.Get_Integer(strSection, "m_iLensStainLensDefectMinSize", 50);
	m_bLensStainUseNoScale = INI.Get_Bool(strSection, "m_bLensStainUseNoScale", TRUE);
	m_iLensStainNoScaleThres = INI.Get_Integer(strSection, "m_iLensStainNoScaleThres", 170);
	m_bLensStainUseLength = INI.Get_Bool(strSection, "m_bLensStainUseLength", TRUE);
	m_iLensStainDefectMinLength = INI.Get_Integer(strSection, "m_iLensStainDefectMinLength", 40);
	m_iLensStainDefectMaxLength = INI.Get_Integer(strSection, "m_iLensStainDefectMaxLength", 100);

	///////////////////////////// Top Berrel Inner for Inspection Condition ///////////
	strSection = "Barrel Inner Condition";
	m_iDecisonTopBarrelInner_RoiOuterMargin = INI.Get_Integer(strSection, "m_iDecisonTopBarrelInner_RoiOuterMargin", 60);
	m_iDecisonTopBarrelInner_RoiInnerMargin = INI.Get_Integer(strSection, "m_iDecisonTopBarrelInner_RoiInnerMargin", -30);

	CString sCondition = strModelFolder + "\\SW\\InspectCondition.ini";
	CIniFileCS INICondition(sCondition);

	strSection = "Lens_Contamination"; // 24.05.09 - v2646 - 불량명 변경 - LeeGW
	m_dLensMinArea = INICondition.Get_Double(strSection, "Min Area", 0.0050);
	m_dLensMidArea = INICondition.Get_Double(strSection, "Mid Area", 0.0050);
	m_dLensMaxArea = INICondition.Get_Double(strSection, "Max Area", 0.0080);
	m_iLensMinNumAccept = INICondition.Get_Integer(strSection, "Accept Min Number", 10000);
	m_iLensMidNumAccept = INICondition.Get_Integer(strSection, "Accept Mid Number", 100);
	m_iLensMaxNumAccept = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Lens_Scratch";
	m_dLensScratchMinArea = INICondition.Get_Double(strSection, "Min Area", 0.0020);
	m_dLensScratchMidArea = INICondition.Get_Double(strSection, "Mid Area", 0.0070);
	m_dLensScratchMaxArea = INICondition.Get_Double(strSection, "Max Area", 0.0090);
	m_iLensScratchMinNumAccept = INICondition.Get_Integer(strSection, "Accept Min Number", 10000);
	m_iLensScratchMidNumAccept = INICondition.Get_Integer(strSection, "Accept Mid Number", 10000);
	m_iLensScratchMaxNumAccept = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Lens_Dirt";
	m_dLensDirtMinArea = INICondition.Get_Double(strSection, "Min Area", 0.0010);
	m_dLensDirtMidArea = INICondition.Get_Double(strSection, "Mid Area", 0.0020);
	m_dLensDirtMaxArea = INICondition.Get_Double(strSection, "Max Area", 0.0050);
	m_iLensDirtMinNumAccept = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iLensDirtMidNumAccept = INICondition.Get_Integer(strSection, "Accept Mid Number", 15);
	m_iLensDirtMaxNumAccept = INICondition.Get_Integer(strSection, "Accept Max Number", 5);

	strSection = "Lens_WhiteDot";
	m_dLensWhiteDotMinArea = INICondition.Get_Double(strSection, "Min Area", 0.0010);
	m_dLensWhiteDotMidArea = INICondition.Get_Double(strSection, "Mid Area", 0.0020);
	m_dLensWhiteDotMaxArea = INICondition.Get_Double(strSection, "Max Area", 0.0050);
	m_iLensWhiteDotMinNumAccept = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iLensWhiteDotMidNumAccept = INICondition.Get_Integer(strSection, "Accept Mid Number", 15);
	m_iLensWhiteDotMaxNumAccept = INICondition.Get_Integer(strSection, "Accept Max Number", 5);

	//////////////////////// added for CMI3000 2000 ====>////////////////////////////////
	strSection = "Top_Dirt";
	m_dDirtMinArea_Top = INICondition.Get_Double(strSection, "Min Area", 0.0100);
	m_dDirtMidArea_Top = INICondition.Get_Double(strSection, "Mid Area", 0.0150);
	m_dDirtMaxArea_Top = INICondition.Get_Double(strSection, "Max Area", 0.0200);
	m_iDirtMinNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iDirtMidNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iDirtMaxNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Top_Scratch";
	m_dScratchMinArea_Top = INICondition.Get_Double(strSection, "Min Area", 0.0100);
	m_dScratchMidArea_Top = INICondition.Get_Double(strSection, "Mid Area", 0.0200);
	m_dScratchMaxArea_Top = INICondition.Get_Double(strSection, "Max Area", 0.0400);
	m_iScratchMinNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iScratchMidNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iScratchMaxNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Top_Stain";
	m_dStainMinArea_Top = INICondition.Get_Double(strSection, "Min Area", 0.0080);
	m_dStainMidArea_Top = INICondition.Get_Double(strSection, "Mid Area", 0.0150);
	m_dStainMaxArea_Top = INICondition.Get_Double(strSection, "Max Area", 0.0170);
	m_iStainMinNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iStainMidNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iStainMaxNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Top_Dent";
	m_dDentMinArea_Top = INICondition.Get_Double(strSection, "Min Area", 0.0080);
	m_dDentMidArea_Top = INICondition.Get_Double(strSection, "Mid Area", 0.0100);
	m_dDentMaxArea_Top = INICondition.Get_Double(strSection, "Max Area", 0.0150);
	m_iDentMinNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iDentMidNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iDentMaxNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Top_Chipping";
	m_dChipMinArea_Top = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dChipMidArea_Top = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dChipMaxArea_Top = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iChipMinNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iChipMidNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iChipMaxNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Top_Epoxy";
	m_dEpoxyMinArea_Top = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dEpoxyMidArea_Top = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dEpoxyMaxArea_Top = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iEpoxyMinNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iEpoxyMidNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iEpoxyMaxNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Top_Outside_CT";
	m_dRes1MinArea_Top = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dRes1MidArea_Top = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dRes1MaxArea_Top = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iRes1MinNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iRes1MidNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iRes1MaxNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Top_Peel_Off";
	m_dRes2MinArea_Top = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dRes2MidArea_Top = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dRes2MaxArea_Top = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iRes2MinNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iRes2MidNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iRes2MaxNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Top_WhiteDot";
	m_dWhiteDotMinArea_Top = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dWhiteDotMidArea_Top = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dWhiteDotMaxArea_Top = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iWhiteDotMinNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iWhiteDotMidNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iWhiteDotMaxNumAccept_Top = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	//////////////////////////////////////////////////////////////////////////

	strSection = "Inner_Dirt";
	m_dDirtMinArea_Inner = INICondition.Get_Double(strSection, "Min Area", 0.0100);
	m_dDirtMidArea_Inner = INICondition.Get_Double(strSection, "Mid Area", 0.0150);
	m_dDirtMaxArea_Inner = INICondition.Get_Double(strSection, "Max Area", 0.0200);
	m_iDirtMinNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iDirtMidNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iDirtMaxNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Inner_Scratch";
	m_dScratchMinArea_Inner = INICondition.Get_Double(strSection, "Min Area", 0.0100);
	m_dScratchMidArea_Inner = INICondition.Get_Double(strSection, "Mid Area", 0.0200);
	m_dScratchMaxArea_Inner = INICondition.Get_Double(strSection, "Max Area", 0.0400);
	m_iScratchMinNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iScratchMidNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iScratchMaxNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Inner_Stain";
	m_dStainMinArea_Inner = INICondition.Get_Double(strSection, "Min Area", 0.0080);
	m_dStainMidArea_Inner = INICondition.Get_Double(strSection, "Mid Area", 0.0150);
	m_dStainMaxArea_Inner = INICondition.Get_Double(strSection, "Max Area", 0.0170);
	m_iStainMinNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iStainMidNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iStainMaxNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Inner_Dent";
	m_dDentMinArea_Inner = INICondition.Get_Double(strSection, "Min Area", 0.0080);
	m_dDentMidArea_Inner = INICondition.Get_Double(strSection, "Mid Area", 0.0100);
	m_dDentMaxArea_Inner = INICondition.Get_Double(strSection, "Max Area", 0.0150);
	m_iDentMinNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iDentMidNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iDentMaxNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Inner_Chipping";
	m_dChipMinArea_Inner = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dChipMidArea_Inner = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dChipMaxArea_Inner = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iChipMinNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iChipMidNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iChipMaxNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Inner_Epoxy";
	m_dEpoxyMinArea_Inner = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dEpoxyMidArea_Inner = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dEpoxyMaxArea_Inner = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iEpoxyMinNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iEpoxyMidNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iEpoxyMaxNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Inner_Outside_CT";
	m_dRes1MinArea_Inner = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dRes1MidArea_Inner = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dRes1MaxArea_Inner = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iRes1MinNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iRes1MidNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iRes1MaxNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Inner_Peel_Off";
	m_dRes2MinArea_Inner = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dRes2MidArea_Inner = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dRes2MaxArea_Inner = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iRes2MinNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iRes2MidNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iRes2MaxNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Inner_WhiteDot";
	m_dWhiteDotMinArea_Inner = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dWhiteDotMidArea_Inner = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dWhiteDotMaxArea_Inner = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iWhiteDotMinNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iWhiteDotMidNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iWhiteDotMaxNumAccept_Inner = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	//////////////////////////////////////////////////////////////////////////

	strSection = "Bottom_Dirt";
	m_dDirtMinArea_Btm = INICondition.Get_Double(strSection, "Min Area", 0.0100);
	m_dDirtMidArea_Btm = INICondition.Get_Double(strSection, "Mid Area", 0.0200);
	m_dDirtMaxArea_Btm = INICondition.Get_Double(strSection, "Max Area", 0.0300);
	m_iDirtMinNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iDirtMidNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iDirtMaxNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Bottom_Scratch";
	m_dScratchMinArea_Btm = INICondition.Get_Double(strSection, "Min Area", 0.0100);
	m_dScratchMidArea_Btm = INICondition.Get_Double(strSection, "Mid Area", 0.0200);
	m_dScratchMaxArea_Btm = INICondition.Get_Double(strSection, "Max Area", 0.0400);
	m_iScratchMinNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iScratchMidNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iScratchMaxNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Bottom_Stain";
	m_dStainMinArea_Btm = INICondition.Get_Double(strSection, "Min Area", 0.0080);
	m_dStainMidArea_Btm = INICondition.Get_Double(strSection, "Mid Area", 0.0150);
	m_dStainMaxArea_Btm = INICondition.Get_Double(strSection, "Max Area", 0.0170);
	m_iStainMinNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iStainMidNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iStainMaxNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Bottom_Dent";
	m_dDentMinArea_Btm = INICondition.Get_Double(strSection, "Min Area", 0.0080);
	m_dDentMidArea_Btm = INICondition.Get_Double(strSection, "Mid Area", 0.0100);
	m_dDentMaxArea_Btm = INICondition.Get_Double(strSection, "Max Area", 0.0150);
	m_iDentMinNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iDentMidNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iDentMaxNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Bottom_Chipping";
	m_dChipMinArea_Btm = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dChipMidArea_Btm = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dChipMaxArea_Btm = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iChipMinNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iChipMidNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iChipMaxNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Bottom_Epoxy";
	m_dEpoxyMinArea_Btm = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dEpoxyMidArea_Btm = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dEpoxyMaxArea_Btm = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iEpoxyMinNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iEpoxyMidNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iEpoxyMaxNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Bottom_Outside_CT";
	m_dRes1MinArea_Btm = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dRes1MidArea_Btm = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dRes1MaxArea_Btm = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iRes1MinNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iRes1MidNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iRes1MaxNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Bottom_Peel_Off";
	m_dRes2MinArea_Btm = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dRes2MidArea_Btm = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dRes2MaxArea_Btm = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iRes2MinNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iRes2MidNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iRes2MaxNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	strSection = "Bottom_WhiteDot";
	m_dWhiteDotMinArea_Btm = INICondition.Get_Double(strSection, "Min Area", 0.0200);
	m_dWhiteDotMidArea_Btm = INICondition.Get_Double(strSection, "Mid Area", 0.0500);
	m_dWhiteDotMaxArea_Btm = INICondition.Get_Double(strSection, "Max Area", 0.0700);
	m_iWhiteDotMinNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Min Number", 100);
	m_iWhiteDotMidNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Mid Number", 2);
	m_iWhiteDotMaxNumAccept_Btm = INICondition.Get_Integer(strSection, "Accept Max Number", 1);

	//////////////////////// <==== added for CMI3000 2000 ////////////////////////////////

	// ROI
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

	SetFolderFile(strModelFolder, "SW\\InspectParam.dat");

	if (!m_bOpened)
		OpenDataFile();

	if (m_bOpened)
	{
		SAFE_DELETE(m_pInspectionArea);
		m_pInspectionArea = new GTRegion;

		m_pInspectionArea->Load((HANDLE)m_File.m_hFile, m_iModelVersion);

		CloseDataFile();
	}

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		THEAPP.m_pTabControlDlg->m_pTeachParamDlg[i]->SetInspectParam();
	}
	/////////////////////*** Load 조명 Parameters ***////////////////////////////////////////////////////////////////////
	////////////////////////////////// Changed for CMI3000 2000 ====> ///////////////////////////////////////////////////

	//**********  조명 값을 파일에서 읽어옴.
	CString sLinfoPath = strModelFolder + "\\HW\\InspectLightInfo.ini";
	LoadLightInfo(sLinfoPath);

	sLinfoPath = strModelFolder + "\\HW\\LightAverageValue.ini";
	THEAPP.m_pModelDataManager->LoadLightAverageValue(sLinfoPath);

	for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
			break;

		for (int i = 0; i < MAX_LIGHT_CHANNEL; i++)
		{
			THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_SliderLight[i].SetPos(m_iInspLightBright[iImageIdx][i]);
		}
	}

#ifdef INLINE_MODE
	// New Light Controller

	if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
	{
		// Inspection
		for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
		{
			if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
				break;

			// Light 1~5
			if (m_iLightPageNo[iImageIdx][1] >= 0)
			{
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel1 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_1];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel2 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_2];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel3 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_3];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel4 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_4];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel5 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_5];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel6 = 0;
				if (THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(1, 19200)) // Com1
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination_6CH(m_iLightPageNo[iImageIdx][1]);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();
				}
			}
		}

		// Barcode

		CLightControl BarcodeLightControl;
		int iBarcodePageIdx = WELDING_CAM_BARCODE_PAGE_IDX;

		// Light 1
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel1 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel2 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel3 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel4 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel5 = m_iBarcodeLightBright;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel6 = 0;
		if (BarcodeLightControl.m_ComPort.OpenPort(1, 19200)) // Com1
		{
			BarcodeLightControl.SetIllumination_6CH(iBarcodePageIdx);
			BarcodeLightControl.m_ComPort.ClosePort();
		}
	}
	else
	{
		// Inspection
		for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
		{
			if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
				break;

			// Light 1
			if (m_iLightPageNo[iImageIdx][0] >= 0)
			{
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[m_iLightPageNo[iImageIdx][0]].uiChannel1 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_1];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[m_iLightPageNo[iImageIdx][0]].uiChannel2 = 0;
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[m_iLightPageNo[iImageIdx][0]].uiChannel3 = 0;
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[m_iLightPageNo[iImageIdx][0]].uiChannel4 = 0;
				if (THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_ComPort.OpenPort(1, 19200)) // Com1
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].SetIllumination(m_iLightPageNo[iImageIdx][0]);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_ComPort.ClosePort();
				}
			}

			// Light 2~5
			if (m_iLightPageNo[iImageIdx][1] >= 0)
			{
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel1 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_2];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel2 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_3];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel3 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_4];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel4 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_5];
				if (THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(2, 19200)) // Com2
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination(m_iLightPageNo[iImageIdx][1]);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();
				}
			}
		}

		// Barcode

		CLightControl BarcodeLightControl;
		int iBarcodePageIdx = 0;

		// Light 1
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel1 = m_iBarcodeLightBright;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel2 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel3 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel4 = 0;
		if (BarcodeLightControl.m_ComPort.OpenPort(4, 19200)) // Com4
		{
			BarcodeLightControl.SetIllumination(iBarcodePageIdx);
			BarcodeLightControl.m_ComPort.ClosePort();
		}
	}

	sLinfoPath = strModelFolder + "\\HW\\LightAverageValue.ini";
	LoadLightAverageValue(sLinfoPath);

#endif
	////////////////////////////////// <==== Changed for CMI3000 2000 ///////////////////////////////////////////////////
}

void CModelDataManager::LoadModelContourData()
{
	try
	{
		if (m_sModelName == ".")
			return;

		CString strModelFolder;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
			strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
		else
			strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

		CString strContourFolder = strModelFolder + "\\SW\\MatchingModel\\";

		if (m_bOneBarrel == FALSE)
		{
			read_contour_xld_arc_info(&ContoursOri[MATCHING_CONTOUR_1], strContourFolder + "MatModel_1.xld");
			read_contour_xld_arc_info(&ContoursOri[MATCHING_CONTOUR_2], strContourFolder + "MatModel_2.xld");
		}
		else
		{
			ContoursOri[MATCHING_CONTOUR_1].Reset();
			ContoursOri[MATCHING_CONTOUR_2].Reset();
		}

		read_contour_xld_arc_info(&ContoursOri[MATCHING_CONTOUR_3], strContourFolder + "MatModel_3.xld");
		read_contour_xld_arc_info(&ContoursOri[MATCHING_CONTOUR_4], strContourFolder + "MatModel_4.xld");
		gen_empty_obj(&ContoursOri[MATCHING_CONTOUR_5]);

		try
		{
			read_contour_xld_arc_info(&ContoursAffinTrans1_Uniformity_Ori, strContourFolder + "UniformityMatModel_1.xld");
			create_aniso_shape_model_xld(ContoursAffinTrans1_Uniformity_Ori, "auto", 0, 0, "auto", 0.9, 1.1, "auto", 0.9, 1.1, "auto", "auto", "ignore_local_polarity", 15, &ModelID1_Uniformity);
		}
		catch (HException &except)
		{
			CString str;
			str.Format("Halcon Exception [CModelDataManager LoadModelContourData: Uniformity] : %s", except.message);
			THEAPP.SaveLog(str);
		}

		THEAPP.SaveLog("매칭 모델만들기 시작"); // 모델 불러올때 매칭모델 만듬.

		double dAngleRangeRad;
		tuple_rad((double)THEAPP.m_pModelDataManager->m_iMatchingAngleRange, &dAngleRangeRad);

		if (m_bOneBarrel == FALSE)
		{
			if (m_bUseMatchingScale)
			{
				double dScaleMin = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMin) * 0.01;
				double dScaleMax = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMax) * 0.01;

				if (m_bUseMatchingAngle)
					create_scaled_shape_model_xld(ContoursOri[MATCHING_CONTOUR_1], "auto", -dAngleRangeRad, dAngleRangeRad * 2.0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC1, &ModelID[MATCHING_MODEL_1]);
				else
					create_scaled_shape_model_xld(ContoursOri[MATCHING_CONTOUR_1], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC1, &ModelID[MATCHING_MODEL_1]);

				create_scaled_shape_model_xld(ContoursOri[MATCHING_CONTOUR_2], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC2, &ModelID[MATCHING_MODEL_2]);

				create_scaled_shape_model_xld(ContoursOri[MATCHING_CONTOUR_3], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC3, &ModelID[MATCHING_MODEL_3]);
				create_scaled_shape_model_xld(ContoursOri[MATCHING_CONTOUR_4], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC4, &ModelID[MATCHING_MODEL_4]);
			}
			else
			{
				if (m_bUseMatchingAngle)
					create_shape_model_xld(ContoursOri[MATCHING_CONTOUR_1], "auto", -dAngleRangeRad, dAngleRangeRad * 2.0, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC1, &ModelID[MATCHING_MODEL_1]);
				else
					create_shape_model_xld(ContoursOri[MATCHING_CONTOUR_1], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC1, &ModelID[MATCHING_MODEL_1]);

				create_shape_model_xld(ContoursOri[MATCHING_CONTOUR_2], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC2, &ModelID[MATCHING_MODEL_2]);

				create_shape_model_xld(ContoursOri[MATCHING_CONTOUR_3], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC3, &ModelID[MATCHING_MODEL_3]);
				create_shape_model_xld(ContoursOri[MATCHING_CONTOUR_4], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC4, &ModelID[MATCHING_MODEL_4]);
			}
		}
		else
		{
			if (m_bUseMatchingScale)
			{
				double dScaleMin = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMin) * 0.01;
				double dScaleMax = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMax) * 0.01;

				if (m_bUseMatchingAngle)
					create_scaled_shape_model_xld(ContoursOri[MATCHING_CONTOUR_3], "auto", -dAngleRangeRad, dAngleRangeRad * 2.0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC3, &ModelID[MATCHING_MODEL_3]);
				else
					create_scaled_shape_model_xld(ContoursOri[MATCHING_CONTOUR_3], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC3, &ModelID[MATCHING_MODEL_3]);

				create_scaled_shape_model_xld(ContoursOri[MATCHING_CONTOUR_4], "auto", 0, 0, "auto", dScaleMin, dScaleMax, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC4, &ModelID[MATCHING_MODEL_4]);
			}
			else
			{
				if (m_bUseMatchingAngle)
					create_shape_model_xld(ContoursOri[MATCHING_CONTOUR_3], "auto", -dAngleRangeRad, dAngleRangeRad * 2.0, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC3, &ModelID[MATCHING_MODEL_3]);
				else
					create_shape_model_xld(ContoursOri[MATCHING_CONTOUR_3], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC3, &ModelID[MATCHING_MODEL_3]);

				create_shape_model_xld(ContoursOri[MATCHING_CONTOUR_4], "auto", 0, 0, "auto", "auto", "ignore_local_polarity", m_iMatchingContrastC4, &ModelID[MATCHING_MODEL_4]);
			}
		}

		THEAPP.SaveLog("매칭 모델만들기 끝");
	}
	catch (HException &except)
	{
		CString str;
		str.Format("Halcon Exception [CModelDataManager LoadModelContourData] : %s", except.message);
		THEAPP.SaveLog(str);
	}
}

// 24.02.29 Local Align 추가 - LeeGW Start
int CModelDataManager::GetLastLocalAlignID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iLastLocalAlignID = 0;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
		{
			if (pInspectROIRgn->m_iLocalAlignID > iLastLocalAlignID)
			{
				iLastLocalAlignID = pInspectROIRgn->m_iLocalAlignID;
			}
		}
	}

	return iLastLocalAlignID;
}

BOOL CModelDataManager::CheckLocalAlignID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_LOCAL_ALIGN)
		{
			if (pInspectROIRgn->m_iLocalAlignID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
// 24.02.29 Local Align 추가 - LeeGW End

// FAI 추가
int CModelDataManager::GetLastFAIOuterCircleID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iFAIOuterCircleID = 0;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
		{
			if (pInspectROIRgn->m_iFAIOuterCircleID > iFAIOuterCircleID)
			{
				iFAIOuterCircleID = pInspectROIRgn->m_iFAIOuterCircleID;
			}
		}
	}

	return iFAIOuterCircleID;
}

BOOL CModelDataManager::CheckFAIOuterCircleID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
		{
			if (pInspectROIRgn->m_iFAIOuterCircleID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CModelDataManager::GetLastFAIInnerCircleID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iFAIInnerCircleID = 0;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
		{
			if (pInspectROIRgn->m_iFAIInnerCircleID > iFAIInnerCircleID)
			{
				iFAIInnerCircleID = pInspectROIRgn->m_iFAIInnerCircleID;
			}
		}
	}

	return iFAIInnerCircleID;
}

BOOL CModelDataManager::CheckFAIInnerCircleID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
		{
			if (pInspectROIRgn->m_iFAIInnerCircleID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CModelDataManager::GetLastFAIDatumCircleID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iFAIDatumCircleID = 0;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
		{
			if (pInspectROIRgn->m_iFAIDatumCircleID > iFAIDatumCircleID)
			{
				iFAIDatumCircleID = pInspectROIRgn->m_iFAIDatumCircleID;
			}
		}
	}

	return iFAIDatumCircleID;
}

BOOL CModelDataManager::CheckFAIDatumCircleID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
		{
			if (pInspectROIRgn->m_iFAIDatumCircleID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CModelDataManager::GetLastFAIDatumEllipseID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iFAIDatumEllipseID = 0;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
		{
			if (pInspectROIRgn->m_iFAIDatumEllipseID > iFAIDatumEllipseID)
			{
				iFAIDatumEllipseID = pInspectROIRgn->m_iFAIDatumEllipseID;
			}
		}
	}

	return iFAIDatumEllipseID;
}

BOOL CModelDataManager::CheckFAIDatumEllipseID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
		{
			if (pInspectROIRgn->m_iFAIDatumEllipseID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CModelDataManager::GetLastFAIWeldingSpotID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iFAIWeldingSpotID = 0;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
		{
			if (pInspectROIRgn->m_iFAIWeldingSpotID > iFAIWeldingSpotID)
			{
				iFAIWeldingSpotID = pInspectROIRgn->m_iFAIWeldingSpotID;
			}
		}
	}

	return iFAIWeldingSpotID;
}

BOOL CModelDataManager::CheckFAIWeldingSpotID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
		{
			if (pInspectROIRgn->m_iFAIWeldingSpotID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CModelDataManager::GetLastFAIWeldingPocketID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iFAIWeldingPocketID = 0;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
		{
			if (pInspectROIRgn->m_iFAIWeldingPocketID > iFAIWeldingPocketID)
			{
				iFAIWeldingPocketID = pInspectROIRgn->m_iFAIWeldingPocketID;
			}
		}
	}

	return iFAIWeldingPocketID;
}

int CModelDataManager::GetLastFAIBracketID(int iTeachingImageIdx)
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iFAIBracketID = 0;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
		{
			if (pInspectROIRgn->m_iFAIBracketID > iFAIBracketID)
			{
				iFAIBracketID = pInspectROIRgn->m_iFAIBracketID;
			}
		}
	}

	return iFAIBracketID;
}

BOOL CModelDataManager::CheckFAIBracketID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
		{
			if (pInspectROIRgn->m_iFAIBracketID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CModelDataManager::CheckFAIWeldingPocketID(int iTeachingImageIdx, int iCheckID)
{
	if (m_pInspectionArea == NULL)
		return FALSE;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iTeachImageIndex != iTeachingImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
		{
			if (pInspectROIRgn->m_iFAIWeldingPocketID == iCheckID)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// Auto param.
// Current_Recipe.txt 파일에서 파라미터를 읽어와서 적용
//  3. TCP로 파라미터 받을 때.
//  			(ThmRecipeUpdater) -> (LoadRmsParamData) -> (SaveChangeParamHistory)
// 			-> (CompareParameter) -> (SaveLasEvent/SaveManager)
// @param bCompareFileToCurrent : if true, compare file parameters to current and log changes; if false, compare current to file and log changes.
// 250918, jhkim
void CModelDataManager::LoadRmsParamData(CString sPath)
{
	if (sPath.IsEmpty())
	{
		DoubleLogOut("[LoadRmsParamData] Path is empty.\n");
		return;
	}

	if (m_sModelName == _T("."))
	{
		DoubleLogOut("[LoadRmsParamData] Model name is invalid.\n");
		return;
	}

	try
	{
		// Auto Param Model Load Start
		// 모델 load 완료 될 때까지 Handler 에게 Not Ready 상태 전달
		THEAPP.m_pHandlerService->Set_StatusUpdate(VS_NOT_READY);
		
		DoubleLogOut("[LoadRmsParamData] Recipe load Started");
		THEAPP.m_pLogDlg->WriteLog(_T("[LoadRmsParamData] 레시피 로드 시작"));
		THEAPP.m_pInspectSummary->SetBtnEnable(FALSE);


		CString strFile = sPath;
		DoubleLogOut("[LoadRmsParamData] Recipe path : '%s'", strFile.GetString());

		// Lambda Function
		auto EnsureDir = [](const CString &dir) -> BOOL
		{
			if (dir.IsEmpty())
				return FALSE;
			CString path = dir;
			path.TrimRight(_T("\\/"));
			int pos = 0;
			while ((pos = path.Find(_T("\\"), pos + (pos > 0))) != -1)
			{
				CString sub = path.Left(pos);
				if (!sub.IsEmpty() && sub.Right(1) != _T(":"))
				{
					::CreateDirectory(sub, nullptr);
				}
			}
			::CreateDirectory(path, nullptr);
			return TRUE;
		};

		// call lambda function
		EnsureDir(sPath);

		WIN32_FILE_ATTRIBUTE_DATA fad{};
		if (!GetFileAttributesEx(strFile, GetFileExInfoStandard, &fad))
		{
			DWORD gle = GetLastError();
			DoubleLogOut("[LoadRmsParamData] File does not exist or cannot access. GLE=%lu\n", gle);
			return;
		}

		CFile fileRead;
		CFileException ex;
		UINT openFlags = CFile::modeRead | CFile::shareDenyNone;

		if (!fileRead.Open(strFile, openFlags, &ex))
		{
			TCHAR buf[512];
			ex.GetErrorMessage(buf, 512);
			DoubleLogOut("[LoadRmsParamData] Open failed. code=%u, msg=%s\n", ex.m_cause, buf);
			return;
		}

		ULONGLONG fileSize = fileRead.GetLength();
		CStringA bufferA;
		bufferA.GetBufferSetLength((int)fileSize);

		UINT nRead = fileRead.Read(bufferA.GetBuffer(), (UINT)fileSize);
		bufferA.ReleaseBuffer(nRead);

		fileRead.Close();

		// Key-Value 파싱
		std::map<CString, CString> vParedRecvParam;
		int start = 0;
		while (start < bufferA.GetLength())
		{
			int end = bufferA.Find('\n', start);
			if (end == -1)
				end = bufferA.GetLength();

			CStringA lineA = bufferA.Mid(start, end - start);
			lineA.Trim("\r\n\t "); // CRLF, 탭, 공백 제거

			if (!lineA.IsEmpty())
			{
				CString line(lineA); // ANSI → CString
				int pos = line.Find(_T('='));
				if (pos > 0)
				{
					CString k = line.Left(pos);
					k.Trim();
					CString v = line.Mid(pos + 1);
					v.Trim();
					vParedRecvParam[k] = v;
				}
			}
			start = end + 1;
		}

		// Parser
		// Bool, Int, Double Text 를 각각에 맞게 파싱하기
		// fallback : 기존에 모델이 갖고 있던 값
		// (아마) 찾는 값이 없으면 기존에 있는 값을 그대로 쓰는듯
		auto ReadBool = [&](const CString &key, BOOL fallback) -> BOOL
		{
			// it : 새로 전달받은 값 (TCP 로 받은 값)
			auto it = vParedRecvParam.find(key);
			auto that = vParedRecvParam.end();
			if (it == that)
			{
				auto ret = fallback;
				return ret;
			}
			else
			{
				// second : the second stored value
				CString s = it->second;
				s.Trim();
				s.MakeUpper();
				if (s == _T("TRUE") || s == _T("1"))
					return TRUE;
				if (s == _T("FALSE") || s == _T("0"))
					return FALSE;

				auto ret = fallback;
				return ret;
			}
		};
		auto ReadInt = [&](const CString &key, int fallback) -> int
		{
			auto it = vParedRecvParam.find(key);
			auto that = vParedRecvParam.end();
			if (it == that)
			{
				auto ret = fallback;
				return ret;
			}
			{
				// second : the second stored value
				return _tstoi(it->second);
			}
		};
		auto ReadDouble = [&](const CString &key, double fallback) -> double
		{
			auto it = vParedRecvParam.find(key);
			auto that = vParedRecvParam.end();
			if (it == that)
			{
				auto ret = fallback;
				return ret;
			}
			{
				// second : the second stored value
				return _tstof(it->second);
			}
		};

		// 문자열 합치기
		// prepix + tail
		// ex) 1-Side_V_Image1_Tab1_m_bUseDefectConditionArea
		auto K = [&](const CString &prefix, LPCTSTR tail) -> CString
		{
			CString k;
			k.Format(_T("%s_%s"), prefix.GetString(), tail);
			return k;
		};

		// Update Param
		CString strModelFolder;
		if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel)
			strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
		else
			strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

		CString sFileName = strModelFolder + "\\SW\\InspectParam.ini";

		// 현재 모델 폴더에저장된 파라미터 정보 불러옴 : INI 파일
		// ex : ~ \Run\Model\BWI25 v1009\SW\InspectParam.ini
		CIniFileCS INI(sFileName);
		CString strSection, strKey;

		BOOL bParamChanged = FALSE;
		THEAPP.m_pLogDlg->WriteLog(_T("[LoadRmsParamData] 모델 파라미터 읽기 시작"));
		for (int imgIdx = 0; imgIdx < BARREL_LENS_IMAGE_TAB; ++imgIdx)
		{
			if (imgIdx >= THEAPP.m_iMaxInspImageNo)
				break;

			strSection.Format("%s", g_sImageName[THEAPP.m_iMachineInspType][imgIdx]);

			// TCP 로 받은 INI 파일에서 Tab 별로 읽어오기
			for (int tabIdx = 0; tabIdx < MAX_TEACHING_TAB; ++tabIdx)
			{
				CString strRMSParamName;
				strRMSParamName.Format(_T("1-Side_V_Image%d_Tab%d"), imgIdx + 1, tabIdx + 1);

				CAlgorithmParam CurrentParam = m_AlgorithmParam[imgIdx][tabIdx];

				// ---------- BOOL ----------
				CurrentParam.m_bUseDefectConditionArea = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionArea")), CurrentParam.m_bUseDefectConditionArea);
				CurrentParam.m_bUseDefectConditionLength = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionLength")), CurrentParam.m_bUseDefectConditionLength);
				CurrentParam.m_bUseDefectConditionWidth = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionWidth")), CurrentParam.m_bUseDefectConditionWidth);
				CurrentParam.m_bUseDefectConditionMean = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionMean")), CurrentParam.m_bUseDefectConditionMean);
				CurrentParam.m_bUseDefectConditionStdev = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionStdev")), CurrentParam.m_bUseDefectConditionStdev);
				CurrentParam.m_bUseDefectConditionAnisometry = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionAnisometry")), CurrentParam.m_bUseDefectConditionAnisometry);
				CurrentParam.m_bUseDefectConditionCircularity = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionCircularity")), CurrentParam.m_bUseDefectConditionCircularity);
				CurrentParam.m_bUseDefectConditionConvexity = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionConvexity")), CurrentParam.m_bUseDefectConditionConvexity);
				CurrentParam.m_bUseDefectConditionEllipseRatio = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionEllipseRatio")), CurrentParam.m_bUseDefectConditionEllipseRatio);
				CurrentParam.m_bDefectConditionXLength = ReadBool(K(strRMSParamName, _T("m_bDefectConditionXLength")), CurrentParam.m_bDefectConditionXLength);
				CurrentParam.m_bDefectConditionYLength = ReadBool(K(strRMSParamName, _T("m_bDefectConditionYLength")), CurrentParam.m_bDefectConditionYLength);

				// ---------- MIN ----------
				CurrentParam.m_iDefectConditionAreaMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionArea")), CurrentParam.m_iDefectConditionAreaMin);
				CurrentParam.m_iDefectConditionLengthMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionLength")), CurrentParam.m_iDefectConditionLengthMin);
				CurrentParam.m_iDefectConditionWidthMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionWidth")), CurrentParam.m_iDefectConditionWidthMin);
				CurrentParam.m_iDefectConditionMeanMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionMean")), CurrentParam.m_iDefectConditionMeanMin);
				CurrentParam.m_dDefectConditionStdevMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionStdev")), CurrentParam.m_dDefectConditionStdevMin);
				CurrentParam.m_dDefectConditionAnisometryMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionAnisometry")), CurrentParam.m_dDefectConditionAnisometryMin);
				CurrentParam.m_dDefectConditionCircularityMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionCircularity")), CurrentParam.m_dDefectConditionCircularityMin);
				CurrentParam.m_dDefectConditionConvexityMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionConvexity")), CurrentParam.m_dDefectConditionConvexityMin);
				CurrentParam.m_dDefectConditionEllipseRatioMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionEllipseRatio")), CurrentParam.m_dDefectConditionEllipseRatioMin);
				CurrentParam.m_iDefectConditionXLengthMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionXLength")), CurrentParam.m_iDefectConditionXLengthMin);
				CurrentParam.m_iDefectConditionYLengthMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionYLength")), CurrentParam.m_iDefectConditionYLengthMin);

				// ---------- MAX ----------
				CurrentParam.m_iDefectConditionAreaMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionAreaMax")), CurrentParam.m_iDefectConditionAreaMax);
				CurrentParam.m_iDefectConditionLengthMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionLengthMax")), CurrentParam.m_iDefectConditionLengthMax);
				CurrentParam.m_iDefectConditionWidthMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionWidthMax")), CurrentParam.m_iDefectConditionWidthMax);
				CurrentParam.m_iDefectConditionMeanMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionMeanMax")), CurrentParam.m_iDefectConditionMeanMax);
				CurrentParam.m_dDefectConditionStdevMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionStdevMax")), CurrentParam.m_dDefectConditionStdevMax);
				CurrentParam.m_dDefectConditionAnisometryMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionAnisometryMax")), CurrentParam.m_dDefectConditionAnisometryMax);
				CurrentParam.m_dDefectConditionCircularityMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionCircularityMax")), CurrentParam.m_dDefectConditionCircularityMax);
				CurrentParam.m_dDefectConditionConvexityMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionConvexityMax")), CurrentParam.m_dDefectConditionConvexityMax);
				CurrentParam.m_dDefectConditionEllipseRatioMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionEllipseRatioMax")), CurrentParam.m_dDefectConditionEllipseRatioMax);
				CurrentParam.m_iDefectConditionXLengthMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionXLengthMax")), CurrentParam.m_iDefectConditionXLengthMax);
				CurrentParam.m_iDefectConditionYLengthMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionYLengthMax")), CurrentParam.m_iDefectConditionYLengthMax);

				// 변경된 파라미터가 있는지 확인
				if ((CurrentParam == m_AlgorithmParam[imgIdx][tabIdx]))
				{
					// DoubleLogOut("[LoadRmsParamData] %dth Img, %dth Tab is unchanged.", imgIdx, tabIdx);
				}
				else
				{
					if (!bParamChanged)
						bParamChanged = TRUE;

					CString logMsg;
					logMsg.Format(_T("[LoadRmsParamData] %d번 이미지, %d번 탭의 파라미터 변경됨.\n"), (imgIdx + 1), (tabIdx + 1));
					logMsg.Append(m_AlgorithmParam[imgIdx][tabIdx].GetDifferentVariables(CurrentParam));
					DoubleLogOut(logMsg);
					THEAPP.m_pLogDlg->WriteLog(logMsg);
					
					// TCP 로 전달받은 파라미터 정보 InspectParam.ini 파일에 저장
					// UI 로 보이는 값은 해당 InspectParam.ini 파일에서 읽어오는 것 같다.
					// 근데 중요한건 현재 \EVENT 로그에는 안남음.
					strKey.Format("%s_Tab%d", strSection, (tabIdx + 1));
					CurrentParam.Save(INI, strSection, strKey);
				}
			}
		}
		
		if (bParamChanged)
		{
			// 변경된 게 있다면 업데이트
			// SaveChangeParamHistory : EVENT 로그 남기기.
			BOOL bCompareFileToCurrent = FALSE;
			SaveChangeParamHistory(bCompareFileToCurrent);
		}

#pragma region SaveModelData
		for (int imgIdx = 0; imgIdx < BARREL_LENS_IMAGE_TAB; ++imgIdx)
		{
			if (imgIdx >= THEAPP.m_iMaxInspImageNo)
				break;

			strSection.Format("%s", g_sImageName[THEAPP.m_iMachineInspType][imgIdx]);

			// TCP 로 받은 INI 파일에서 Tab 별로 읽어오기
			for (int tabIdx = 0; tabIdx < MAX_TEACHING_TAB; ++tabIdx)
			{

				CString strRMSParamName;
				strRMSParamName.Format(_T("1-Side_V_Image%d_Tab%d"), imgIdx + 1, tabIdx + 1);

				CAlgorithmParam *CurrentParam = &m_AlgorithmParam[imgIdx][tabIdx];

				// ---------- BOOL ----------
				CurrentParam->m_bUseDefectConditionArea = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionArea")), CurrentParam->m_bUseDefectConditionArea);
				CurrentParam->m_bUseDefectConditionLength = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionLength")), CurrentParam->m_bUseDefectConditionLength);
				CurrentParam->m_bUseDefectConditionWidth = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionWidth")), CurrentParam->m_bUseDefectConditionWidth);
				CurrentParam->m_bUseDefectConditionMean = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionMean")), CurrentParam->m_bUseDefectConditionMean);
				CurrentParam->m_bUseDefectConditionStdev = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionStdev")), CurrentParam->m_bUseDefectConditionStdev);
				CurrentParam->m_bUseDefectConditionAnisometry = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionAnisometry")), CurrentParam->m_bUseDefectConditionAnisometry);
				CurrentParam->m_bUseDefectConditionCircularity = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionCircularity")), CurrentParam->m_bUseDefectConditionCircularity);
				CurrentParam->m_bUseDefectConditionConvexity = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionConvexity")), CurrentParam->m_bUseDefectConditionConvexity);
				CurrentParam->m_bUseDefectConditionEllipseRatio = ReadBool(K(strRMSParamName, _T("m_bUseDefectConditionEllipseRatio")), CurrentParam->m_bUseDefectConditionEllipseRatio);
				CurrentParam->m_bDefectConditionXLength = ReadBool(K(strRMSParamName, _T("m_bDefectConditionXLength")), CurrentParam->m_bDefectConditionXLength);
				CurrentParam->m_bDefectConditionYLength = ReadBool(K(strRMSParamName, _T("m_bDefectConditionYLength")), CurrentParam->m_bDefectConditionYLength);

				// ---------- MIN ----------
				CurrentParam->m_iDefectConditionAreaMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionArea")), CurrentParam->m_iDefectConditionAreaMin);
				CurrentParam->m_iDefectConditionLengthMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionLength")), CurrentParam->m_iDefectConditionLengthMin);
				CurrentParam->m_iDefectConditionWidthMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionWidth")), CurrentParam->m_iDefectConditionWidthMin);
				CurrentParam->m_iDefectConditionMeanMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionMean")), CurrentParam->m_iDefectConditionMeanMin);
				CurrentParam->m_dDefectConditionStdevMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionStdev")), CurrentParam->m_dDefectConditionStdevMin);
				CurrentParam->m_dDefectConditionAnisometryMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionAnisometry")), CurrentParam->m_dDefectConditionAnisometryMin);
				CurrentParam->m_dDefectConditionCircularityMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionCircularity")), CurrentParam->m_dDefectConditionCircularityMin);
				CurrentParam->m_dDefectConditionConvexityMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionConvexity")), CurrentParam->m_dDefectConditionConvexityMin);
				CurrentParam->m_dDefectConditionEllipseRatioMin = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionEllipseRatio")), CurrentParam->m_dDefectConditionEllipseRatioMin);
				CurrentParam->m_iDefectConditionXLengthMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionXLength")), CurrentParam->m_iDefectConditionXLengthMin);
				CurrentParam->m_iDefectConditionYLengthMin = ReadInt(K(strRMSParamName, _T("m_iDefectConditionYLength")), CurrentParam->m_iDefectConditionYLengthMin);

				// ---------- MAX ----------
				CurrentParam->m_iDefectConditionAreaMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionAreaMax")), CurrentParam->m_iDefectConditionAreaMax);
				CurrentParam->m_iDefectConditionLengthMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionLengthMax")), CurrentParam->m_iDefectConditionLengthMax);
				CurrentParam->m_iDefectConditionWidthMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionWidthMax")), CurrentParam->m_iDefectConditionWidthMax);
				CurrentParam->m_iDefectConditionMeanMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionMeanMax")), CurrentParam->m_iDefectConditionMeanMax);
				CurrentParam->m_dDefectConditionStdevMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionStdevMax")), CurrentParam->m_dDefectConditionStdevMax);
				CurrentParam->m_dDefectConditionAnisometryMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionAnisometryMax")), CurrentParam->m_dDefectConditionAnisometryMax);
				CurrentParam->m_dDefectConditionCircularityMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionCircularityMax")), CurrentParam->m_dDefectConditionCircularityMax);
				CurrentParam->m_dDefectConditionConvexityMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionConvexityMax")), CurrentParam->m_dDefectConditionConvexityMax);
				CurrentParam->m_dDefectConditionEllipseRatioMax = ReadDouble(K(strRMSParamName, _T("m_dDefectConditionEllipseRatioMax")), CurrentParam->m_dDefectConditionEllipseRatioMax);
				CurrentParam->m_iDefectConditionXLengthMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionXLengthMax")), CurrentParam->m_iDefectConditionXLengthMax);
				CurrentParam->m_iDefectConditionYLengthMax = ReadInt(K(strRMSParamName, _T("m_iDefectConditionYLengthMax")), CurrentParam->m_iDefectConditionYLengthMax);

				strKey.Format("%s_Tab%d", strSection, tabIdx + 1);

				// TCP 로 전달받은 파라미터 정보 InspectParam.ini 파일에 저장
				// UI 로 보이는 값은 해당 InspectParam.ini 파일에서 읽어오는 것 같다.
				// 중요한건 EVENT 로그에 안남음.
				CurrentParam->Save(INI, strSection, strKey);
			}
		}
#pragma endregion

		if (bParamChanged)
		{
			CString msg = _T("[LoadRmsParamData] 파라미터 변경 및 업데이트 완료");
			DoubleLogOut(msg);
			THEAPP.m_pLogDlg->WriteLog(msg);
			
		}
		else
		{
			CString msg = _T("[LoadRmsParamData] 파라미터 변경 없음. 업데이트 취소");
			DoubleLogOut(msg);
			THEAPP.m_pLogDlg->WriteLog(msg);
		}
		
		// 버튼 활성화
		THEAPP.m_pInspectSummary->SetBtnEnable(TRUE);
		// Auto Param Model Load End
		// 모델 load 완료 되었으니 Handler 에게 Ready 상태 전달
		THEAPP.m_pHandlerService->Set_StatusUpdate(VS_READY);

		// strFile 경로의 파일이 존재하면 삭제
		DWORD attr = GetFileAttributes(strFile);
		if (attr != INVALID_FILE_ATTRIBUTES)
		{
			if (!DeleteFile(strFile))
			{
				DWORD gle = GetLastError();
				DoubleLogOut("[LoadRmsParamData] Failed to delete file '%s'. GLE=%lu", strFile.GetString(), gle);
			}
			else
			{
				DoubleLogOut("[LoadRmsParamData] File deleted: '%s'", strFile.GetString());
			}
		}
		else
		{
			DoubleLogOut("[LoadRmsParamData] File not found, skip delete: '%s'", strFile.GetString());
		}
	}
	catch (CException *e)
	{
		TCHAR szErr[512] = {0};
		e->GetErrorMessage(szErr, 512);
		DoubleLogOut(_T("[LoadRmsParamData] Exception: %s\n"), szErr);
		e->Delete();

		THEAPP.m_pInspectSummary->SetBtnEnable(TRUE);
		// TODO : 에러났을때 Ready 상태로 바꿔도 되나? 아니면 에러났다는 걸 알려야 하나?
		THEAPP.m_pHandlerService->Set_StatusUpdate(VS_READY);
	}
}

//  현재 SaveChangeParamHistory 호출하는 곳 :
//  1. 로드 버튼 누를 때
//  2. Save 버튼 누를 때
//  3. TCP로 파라미터 받을 때.
//  			(ThmRecipeUpdater) -> (LoadRmsParamData) -> (SaveChangeParamHistory)
// 			-> (CompareParameter) -> (SaveLasEvent/SaveManager)
// @param bCompareFileToCurrent : if true, compare file parameters to current and log changes; if false, compare current to file and log changes.
void CModelDataManager::SaveChangeParamHistory(BOOL bCompareFileToCurrent)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strDate;
	strDate.Format("%04d%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour);

	CString strLotID;
	strLotID = THEAPP.m_pInspectResultDlg->CurrentLotID;
	if (strLotID == "")
		strLotID = _T("LOTID0000");

	CString sFileName;
	sFileName.Format("%s_%s_AVI_MFG_SPEC_AVIMONITOR_RESULT.csv", strLotID, strDate);

	CString sFolderName;
	sFolderName = THEAPP.GetLASTempDirectory();

	CString strFile;
	strFile.Format("%s\\%s", sFolderName, sFileName);

	CFile file;
	if (!file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		// TODO : Sender가 파일을 마구마구 보내면 아래 메세지가 마구마구 뜨게 됨. - 250919, jhkim
		DWORD iErr = GetLastError();
		if (iErr == ERROR_SHARING_VIOLATION)
		{
			CString errMsg;
			errMsg.Format(_T("%s 폴더의 %s 파일이 다른 프로그램에서 사용 중입니다."), sFolderName, sFileName);
			AfxMessageBox(errMsg, MB_ICONERROR | MB_SYSTEMMODAL);
			return;
		}
		else if (iErr == ERROR_ACCESS_DENIED)
		{
			CString errMsg;
			errMsg.Format(_T("%s 폴더의 %s 파일에 대한 접근이 거부되었습니다."), sFolderName, sFileName);
			AfxMessageBox(errMsg, MB_ICONERROR | MB_SYSTEMMODAL);
			return;
		}
		else
		{
			CString errMsg;
			errMsg.Format(_T("%s 폴더의 %s 파일이 생성되지 않았습니다. Code : %d"), sFolderName, sFileName, iErr);
			AfxMessageBox(errMsg, MB_ICONERROR | MB_SYSTEMMODAL);
			return;

		}
	}

	try
	{
		CString strSave;

		// 헤더쓰기
		file.SeekToEnd();
		if (file.GetPosition() == 0)
		{
			// 1. Prepare the header line
			for (size_t i = 0; i < m_vHeaders.size(); ++i)
			{
				strSave += m_vHeaders[i];

				if (i < m_vHeaders.size() - 1)
					strSave += _T(","); // Add a comma as a delimiter
			}

			// 2. Add a newline character at the end of the header line
			strSave += _T("\r\n");

			file.Write(strSave, strSave.GetLength());
		}

		// 바뀐 parameter TEMP 폴더에 csv 파일로 쓰기
		// TODO : need to optimize - 251105, jhkim
		CompareParameter(&file, time, bCompareFileToCurrent);

		/************* 파일 닫기 **************/
		file.Close();

		// TEMP 폴더 -> LAS Event 폴더에 복사
		THEAPP.m_pSaveManager->SaveLasEvent(strLotID, THEAPP.GetLASTempDirectory(), THEAPP.GetLASEventDirectory(), FALSE);
	}
	catch (CFileException *pEx)
	{
		pEx->Delete();
	}
}

// 0531
//  only for change param history (EVENT - SPEC)
// compareFileToCurrent: if true, compare file parameters to current and log changes; if false, compare current to file and log changes.
void CModelDataManager::CompareParameter(CFile *file, SYSTEMTIME time, BOOL compareFileToCurrent)
{
	if (m_sModelName == ".")
		return;

	//////////////////////////////////////////////////////////////////////////
	// Get Current Parameter
	//////////////////////////////////////////////////////////////////////////

	CString strModelFolder;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

	CString sFileName = strModelFolder + "\\SW\\InspectParam.ini";
	CIniFileCS INI(sFileName);
	CAlgorithmParam FileAlgorithmParam;
	CString sCommonName;
	CString strSection;
	CString sFixString = _T("");
	CString strBaseDefectInfo = _T("");

	CString strTime = "";
	CString strBaseStr = "";
	CString strDelimeter = ",";

	// Date
	strTime.Format("\'%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
	strBaseStr += strTime + strDelimeter;

	// Time
	strTime.Format("\'%04d-%02d-%02d %02d:%02d:%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	strBaseStr += strTime + strDelimeter;

	// Station
	strBaseStr += THEAPP.GetPCID() + strDelimeter;

	// Machine No(==Code)
	strBaseStr += THEAPP.Struct_PreferenceStruct.m_strEquipNo + strDelimeter;

	// Login Mode
	strBaseStr += THEAPP.GetLoginMode() + strDelimeter;

	// Operator (Not used - 250925, jhkim)
	// strSave += THEAPP.GetOperator() + strDelimeter;

	// Recipe
	strBaseStr += m_sModelName + strDelimeter;

	// SW Version
	strTime.Format(_T("%d"), THEAPP.GetVersion());
	strBaseStr += strTime + strDelimeter;

	// Lot ID
	strBaseStr += THEAPP.m_pInspectResultDlg->CurrentLotID + strDelimeter;

	for (int imgIdx = 0; imgIdx < MAX_IMAGE_TAB; imgIdx++)
	{
		if (imgIdx >= THEAPP.m_iMaxInspImageNo)
			break;

		strSection.Format("%s", g_sImageName[THEAPP.m_iMachineInspType][imgIdx]);
		for (int iTabNum = 0; iTabNum < MAX_TEACHING_TAB; iTabNum++)
		{
			sCommonName.Format("%s_Tab%d", strSection, (iTabNum + 1));
			// TODO : need to optimize - 251105, jhkim
			FileAlgorithmParam.Load(INI, strSection, sCommonName);

			// 기본정보
			strBaseDefectInfo = strBaseStr;

			// 불량영역
			strBaseDefectInfo += g_sDefectPosName[m_AlgorithmParam[imgIdx][iTabNum].m_iDefectNameIdx] + strDelimeter;

			// 이미지타입
			strBaseDefectInfo += g_sImageName[THEAPP.m_iMachineInspType][imgIdx] + strDelimeter;

			// 불량항목
			strBaseDefectInfo += g_sDefectName[m_AlgorithmParam[imgIdx][iTabNum].m_iDefectNameIdx] + strDelimeter;

			// 이미지 번호
			strTime.Format(_T("%d"), (imgIdx + 1));
			strBaseDefectInfo += strTime + strDelimeter;

			// 검사탭 번호
			strTime.Format(_T("%d"), (iTabNum + 1));
			strBaseDefectInfo += strTime;

			// Refactored to reduce duplication - 251103, jhkim
			// Always use (current, reference) argument order for CompareNRecord
			auto CompareAllParams = [&](const CAlgorithmParam& current, const CAlgorithmParam& reference)
			{
				// ---------- BOOL 사용 여부 ----------
				CompareNRecord(current.m_bUseDefectConditionArea, reference.m_bUseDefectConditionArea, "m_bUseDefectConditionArea", strBaseDefectInfo, file);
				CompareNRecord(current.m_bUseDefectConditionLength, reference.m_bUseDefectConditionLength, "m_bUseDefectConditionLength", strBaseDefectInfo, file);
				CompareNRecord(current.m_bUseDefectConditionWidth, reference.m_bUseDefectConditionWidth, "m_bUseDefectConditionWidth", strBaseDefectInfo, file);
				CompareNRecord(current.m_bUseDefectConditionMean, reference.m_bUseDefectConditionMean, "m_bUseDefectConditionMean", strBaseDefectInfo, file);
				CompareNRecord(current.m_bUseDefectConditionStdev, reference.m_bUseDefectConditionStdev, "m_bUseDefectConditionStdev", strBaseDefectInfo, file);
				CompareNRecord(current.m_bUseDefectConditionAnisometry, reference.m_bUseDefectConditionAnisometry, "m_bUseDefectConditionAnisometry", strBaseDefectInfo, file);
				CompareNRecord(current.m_bUseDefectConditionCircularity, reference.m_bUseDefectConditionCircularity, "m_bUseDefectConditionCircularity", strBaseDefectInfo, file);
				CompareNRecord(current.m_bUseDefectConditionConvexity, reference.m_bUseDefectConditionConvexity, "m_bUseDefectConditionConvexity", strBaseDefectInfo, file);
				CompareNRecord(current.m_bUseDefectConditionEllipseRatio, reference.m_bUseDefectConditionEllipseRatio, "m_bUseDefectConditionEllipseRatio", strBaseDefectInfo, file);
				CompareNRecord(current.m_bDefectConditionXLength, reference.m_bDefectConditionXLength, "m_bDefectConditionXLength", strBaseDefectInfo, file);
				CompareNRecord(current.m_bDefectConditionYLength, reference.m_bDefectConditionYLength, "m_bDefectConditionYLength", strBaseDefectInfo, file);

				// ---------- MIN 계열 ----------
				CompareNRecord(current.m_iDefectConditionAreaMin, reference.m_iDefectConditionAreaMin, "m_iDefectConditionArea", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionLengthMin, reference.m_iDefectConditionLengthMin, "m_iDefectConditionLength", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionWidthMin, reference.m_iDefectConditionWidthMin, "m_iDefectConditionWidth", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionMeanMin, reference.m_iDefectConditionMeanMin, "m_iDefectConditionMean", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionStdevMin, reference.m_dDefectConditionStdevMin, "m_dDefectConditionStdev", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionAnisometryMin, reference.m_dDefectConditionAnisometryMin, "m_dDefectConditionAnisometry", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionCircularityMin, reference.m_dDefectConditionCircularityMin, "m_dDefectConditionCircularity", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionConvexityMin, reference.m_dDefectConditionConvexityMin, "m_dDefectConditionConvexity", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionEllipseRatioMin, reference.m_dDefectConditionEllipseRatioMin, "m_dDefectConditionEllipseRatio", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionXLengthMin, reference.m_iDefectConditionXLengthMin, "m_iDefectConditionXLength", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionYLengthMin, reference.m_iDefectConditionYLengthMin, "m_iDefectConditionYLength", strBaseDefectInfo, file);

				// ---------- MAX 계열 ----------
				CompareNRecord(current.m_iDefectConditionAreaMax, reference.m_iDefectConditionAreaMax, "m_iDefectConditionAreaMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionLengthMax, reference.m_iDefectConditionLengthMax, "m_iDefectConditionLengthMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionWidthMax, reference.m_iDefectConditionWidthMax, "m_iDefectConditionWidthMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionMeanMax, reference.m_iDefectConditionMeanMax, "m_iDefectConditionMeanMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionStdevMax, reference.m_dDefectConditionStdevMax, "m_dDefectConditionStdevMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionAnisometryMax, reference.m_dDefectConditionAnisometryMax, "m_dDefectConditionAnisometryMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionCircularityMax, reference.m_dDefectConditionCircularityMax, "m_dDefectConditionCircularityMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionConvexityMax, reference.m_dDefectConditionConvexityMax, "m_dDefectConditionConvexityMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_dDefectConditionEllipseRatioMax, reference.m_dDefectConditionEllipseRatioMax, "m_dDefectConditionEllipseRatioMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionXLengthMax, reference.m_iDefectConditionXLengthMax, "m_iDefectConditionXLengthMax", strBaseDefectInfo, file);
				CompareNRecord(current.m_iDefectConditionYLengthMax, reference.m_iDefectConditionYLengthMax, "m_iDefectConditionYLengthMax", strBaseDefectInfo, file);
			};

			// 람다함수 실제 사용부분.
			if (compareFileToCurrent)
				CompareAllParams(FileAlgorithmParam, m_AlgorithmParam[imgIdx][iTabNum]);
			else
				CompareAllParams(m_AlgorithmParam[imgIdx][iTabNum], FileAlgorithmParam);
		}
	}
}

void CModelDataManager::SaveRmsParamData()
{
	if (m_sModelName == ".")
		return;

	CString sFilePath = THEAPP.Struct_PreferenceStruct.m_strRmsSaveFolderPath;

	CAlgorithmParam AlgorithmParam;

	// LGIT 와 협의한 RMS 저장 포맷.
	CString sVisionName = _T("1-Side_V");
	CString sCommonName;
	CString sParamName, sParamDataStr;
	map<CString, CString> vParam;

	//////////////////////////////////////////////////////////////////////////
	///
	///		Algorithm Param
	///
	//////////////////////////////////////////////////////////////////////////

	for (int imgIdx = 0; imgIdx < THEAPP.m_iMaxInspImageNo; imgIdx++)
	{

		for (int tabIdx = 0; tabIdx < MAX_TEACHING_TAB; tabIdx++)
		{
			sCommonName.Format("%s_Image%d_Tab%d", sVisionName, imgIdx + 1, tabIdx + 1);

			sParamName.Format("%s_m_bUseDefectConditionArea", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bUseDefectConditionArea ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bUseDefectConditionLength", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bUseDefectConditionLength ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bUseDefectConditionWidth", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bUseDefectConditionWidth ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bDefectConditionXLength", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bDefectConditionXLength ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bDefectConditionYLength", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bDefectConditionYLength ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bUseDefectConditionMean", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bUseDefectConditionMean ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bUseDefectConditionStdev", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bUseDefectConditionStdev ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bUseDefectConditionAnisometry", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bUseDefectConditionAnisometry ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bUseDefectConditionCircularity", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bUseDefectConditionCircularity ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bUseDefectConditionConvexity", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bUseDefectConditionConvexity ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_bUseDefectConditionEllipseRatio", sCommonName);
			sParamDataStr.Format("%s", m_AlgorithmParam[imgIdx][tabIdx].m_bUseDefectConditionEllipseRatio ? "TRUE" : "FALSE");
			vParam[sParamName] = sParamDataStr;

			// Area Min/Max
			sParamName.Format("%s_m_iDefectConditionArea", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionAreaMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_iDefectConditionAreaMax", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionAreaMax);
			vParam[sParamName] = sParamDataStr;
			// Length Min/Max
			sParamName.Format("%s_m_iDefectConditionLength", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionLengthMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_iDefectConditionLengthMax", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionLengthMax);
			vParam[sParamName] = sParamDataStr;
			// Width Min/Max
			sParamName.Format("%s_m_iDefectConditionWidth", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionWidthMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_iDefectConditionWidthMax", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionWidthMax);
			vParam[sParamName] = sParamDataStr;
			// XLength Min/Max
			sParamName.Format("%s_m_iDefectConditionXLength", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionXLengthMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_iDefectConditionXLengthMax", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionXLengthMax);
			vParam[sParamName] = sParamDataStr;
			// YLength Min/Max
			sParamName.Format("%s_m_iDefectConditionYLength", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionYLengthMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_iDefectConditionYLengthMax", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionYLengthMax);
			vParam[sParamName] = sParamDataStr;
			// Bright Mean Min/Max
			sParamName.Format("%s_m_iDefectConditionMean", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionMeanMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_iDefectConditionMeanMax", sCommonName);
			sParamDataStr.Format("%d", m_AlgorithmParam[imgIdx][tabIdx].m_iDefectConditionMeanMax);
			vParam[sParamName] = sParamDataStr;
			// Bright Stdev Min/Max
			sParamName.Format("%s_m_dDefectConditionStdev", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionStdevMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_dDefectConditionStdevMax", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionStdevMax);
			vParam[sParamName] = sParamDataStr;
			// Anisometry(Shape) Min/Max
			sParamName.Format("%s_m_dDefectConditionAnisometry", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionAnisometryMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_dDefectConditionAnisometryMax", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionAnisometryMax);
			vParam[sParamName] = sParamDataStr;
			// Circularity Min/Max
			sParamName.Format("%s_m_dDefectConditionCircularity", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionCircularityMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_dDefectConditionCircularityMax", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionCircularityMax);
			vParam[sParamName] = sParamDataStr;
			// Convexity Min/Max
			sParamName.Format("%s_m_dDefectConditionConvexity", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionConvexityMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_dDefectConditionConvexityMax", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionConvexityMax);
			vParam[sParamName] = sParamDataStr;
			// Ellipse(타원) Ratio Min/Max
			sParamName.Format("%s_m_dDefectConditionEllipseRatio", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionEllipseRatioMin);
			vParam[sParamName] = sParamDataStr;
			sParamName.Format("%s_m_dDefectConditionEllipseRatioMax", sCommonName);
			sParamDataStr.Format("%.4f", m_AlgorithmParam[imgIdx][tabIdx].m_dDefectConditionEllipseRatioMax);
			vParam[sParamName] = sParamDataStr;
		}
	}

	THEAPP.SaveRmsData(sFilePath, m_sModelName, vParam);
}

void CModelDataManager::SetControllerLightInfo()
{
	/////////////////////*** Load 조명 Parameters ***////////////////////////////////////////////////////////////////////

	if (m_sModelName == ".")
		return;

	CString strModelFolder;
	if (!THEAPP.Struct_PreferenceStruct.m_bUseAbsolutePathModel) // Ver2629
		strModelFolder = THEAPP.GetWorkingDirectory() + "\\Model\\" + m_sModelName;
	else
		strModelFolder = THEAPP.GetWorkingEvmsDirectory() + "\\Model\\" + m_sModelName;

	//**********  조명 값을 파일에서 읽어옴.
	CString sLinfoPath = strModelFolder + "\\HW\\InspectLightInfo.ini";
	LoadLightInfo(sLinfoPath);

	sLinfoPath = strModelFolder + "\\HW\\LightAverageValue.ini";
	THEAPP.m_pModelDataManager->LoadLightAverageValue(sLinfoPath);

#ifdef INLINE_MODE
	// New Light Controller

	if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
	{
		// Inspection
		for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
		{
			if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
				break;

			// Light 1~5
			if (m_iLightPageNo[iImageIdx][1] >= 0)
			{
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel1 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_1];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel2 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_2];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel3 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_3];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel4 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_4];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel5 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_5];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel6 = 0;
				if (THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(1, 19200)) // Com1
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination_6CH(m_iLightPageNo[iImageIdx][1]);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();
				}
			}
		}

		// Barcode

		CLightControl BarcodeLightControl;
		int iBarcodePageIdx = WELDING_CAM_BARCODE_PAGE_IDX;

		// Light 1
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel1 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel2 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel3 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel4 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel5 = m_iBarcodeLightBright;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel6 = 0;
		if (BarcodeLightControl.m_ComPort.OpenPort(1, 19200)) // Com1
		{
			BarcodeLightControl.SetIllumination_6CH(iBarcodePageIdx);
			BarcodeLightControl.m_ComPort.ClosePort();
		}
	}
	else
	{
		// Inspection
		for (int iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
		{
			if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
				break;

			if (m_iLightPageNo[iImageIdx][0] >= 0)
			{
				// Light 1
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[m_iLightPageNo[iImageIdx][0]].uiChannel1 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_1];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[m_iLightPageNo[iImageIdx][0]].uiChannel2 = 0;
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[m_iLightPageNo[iImageIdx][0]].uiChannel3 = 0;
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_Page[m_iLightPageNo[iImageIdx][0]].uiChannel4 = 0;
				if (THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_ComPort.OpenPort(1, 19200)) // Com1
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].SetIllumination(m_iLightPageNo[iImageIdx][0]);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[0].m_ComPort.ClosePort();
				}
			}

			// Light 2~5
			if (m_iLightPageNo[iImageIdx][1] >= 0)
			{
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel1 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_2];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel2 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_3];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel3 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_4];
				THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_Page[m_iLightPageNo[iImageIdx][1]].uiChannel4 = m_iInspLightBright[iImageIdx][LIGHT_CHANNEL_5];
				if (THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.OpenPort(2, 19200)) // Com2
				{
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].SetIllumination(m_iLightPageNo[iImageIdx][1]);
					THEAPP.m_pTabControlDlg->m_pLightControlDlg->m_LightControl[1].m_ComPort.ClosePort();
				}
			}
		}

		// Barcode

		CLightControl BarcodeLightControl;
		int iBarcodePageIdx = 0;

		// Light 1
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel1 = m_iBarcodeLightBright;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel2 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel3 = 0;
		BarcodeLightControl.m_Page[iBarcodePageIdx].uiChannel4 = 0;
		if (BarcodeLightControl.m_ComPort.OpenPort(4, 19200)) // Com4
		{
			BarcodeLightControl.SetIllumination(iBarcodePageIdx);
			BarcodeLightControl.m_ComPort.ClosePort();
		}
	}

#endif
}

int CModelDataManager::GetLastPadID()
{
	if (m_pInspectionArea == NULL)
		return -1;

	int iLastPadID = -1;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_INSPECTION ||
			pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)
		{
			if (pInspectROIRgn->miPadID > iLastPadID)
			{
				iLastPadID = pInspectROIRgn->miPadID;
			}
		}
	}

	return iLastPadID;
}

Hobject CModelDataManager::GetInspectAreaRgn(int iGrabImageIdx, int iTabIdx)
{
	Hobject HInspectAreaRgn;
	gen_empty_obj(&HInspectAreaRgn);

	if (m_pInspectionArea == NULL)
		return HInspectAreaRgn;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if ((pInspectROIRgn->m_iTeachImageIndex - 1) != iGrabImageIdx)
			continue;

		// 24.03.03 LeeGW
		if (pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_INSPECTION &&
			pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_AI_INSPECTION &&
			pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_LOCAL_ALIGN &&
			(pInspectROIRgn->m_iInspectionType < INSPECTION_TYPE_FAI_START || pInspectROIRgn->m_iInspectionType > INSPECTION_TYPE_FAI_END))
			continue;
	}

	return HInspectAreaRgn;
}

// 24.03.18 Local Align - LeeGW Start
CAlgorithmParam CModelDataManager::GetInspROIAlgorithmParam(int iGrabImageIdx, int iTabIdx)
{
	CAlgorithmParam ROIAlgorithmParam;

	if (m_pInspectionArea == NULL)
		return ROIAlgorithmParam;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if ((pInspectROIRgn->m_iTeachImageIndex - 1) != iGrabImageIdx)
			continue;

		if (pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_INSPECTION &&
			pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_AI_INSPECTION &&
			pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_LOCAL_ALIGN &&
			(pInspectROIRgn->m_iInspectionType < INSPECTION_TYPE_FAI_START || pInspectROIRgn->m_iInspectionType > INSPECTION_TYPE_FAI_END))
			continue;

		ROIAlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];
		break;
	}

	return ROIAlgorithmParam;
}
// 24.03.18 Local Align - LeeGW End

void CModelDataManager::UpdatePolygonROIBoundary()
{
	if (m_pInspectionArea == NULL)
		return;

	GTRegion *pInspectROIRgn;

	int iNoInspectROI = 0;
	iNoInspectROI = m_pInspectionArea->GetChildTRegionCount();

	POINT LTPoint, RBPoint;
	Hlong lRow1, lRow2, lCol1, lCol2;

	for (int iIdx = 0; iIdx < iNoInspectROI; iIdx++)
	{
		pInspectROIRgn = m_pInspectionArea->GetChildTRegion(iIdx);
		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_bRegionROI)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(pInspectROIRgn->m_HTeachPolygonRgn))
			{
				smallest_rectangle1(pInspectROIRgn->m_HTeachPolygonRgn, &lRow1, &lCol1, &lRow2, &lCol2);
				LTPoint.x = lCol1;
				LTPoint.y = lRow1;
				RBPoint.x = lCol2;
				RBPoint.y = lRow2;

				pInspectROIRgn->SetLTPoint(LTPoint, THEAPP.m_pCalDataService);
				pInspectROIRgn->SetRBPoint(RBPoint, THEAPP.m_pCalDataService);
			}
		}
	}
}

void CModelDataManager::CopyFrom(const CModelDataManager &src)
{
	m_pInstance = src.m_pInstance;
	m_strLogin_Mode = src.m_strLogin_Mode;
	m_strOperator = src.m_strOperator;
	m_strPwd = src.m_strPwd;
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		for (int j = 0; j < MAX_TEACHING_TAB; j++)
			m_AlgorithmParam[i][j].CopyFrom(src.m_AlgorithmParam[i][j]);
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		m_dFAIMultiple[i] = src.m_dFAIMultiple[i];
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		m_dFAIOffset[i] = src.m_dFAIOffset[i];
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		m_dFAISpecMin[i] = src.m_dFAISpecMin[i];
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		m_dFAISpecMax[i] = src.m_dFAISpecMax[i];
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		m_bFAINGUse[i] = src.m_bFAINGUse[i];
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		m_bFAISpecialNGUse[i] = src.m_bFAISpecialNGUse[i];
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		m_bFAIUniqueOffsetUse[i] = src.m_bFAIUniqueOffsetUse[i];
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		for (int j = 0; j < MAX_CIRCULAR_TABLE_POCKET; j++)
			m_dFAIUniqueMultiple[i][j] = src.m_dFAIUniqueMultiple[i][j];
	for (int i = 0; i < MAX_FAI_ITEM; i++)
		for (int j = 0; j < MAX_CIRCULAR_TABLE_POCKET; j++)
			m_dFAIUniqueOffset[i][j] = src.m_dFAIUniqueOffset[i][j];
	m_iFAIInnerCircleFitType = src.m_iFAIInnerCircleFitType;
	m_iFAIOuterCircleFitType = src.m_iFAIOuterCircleFitType;
	m_iFAIDatumCircleFitType = src.m_iFAIDatumCircleFitType;
	m_iFAISimulPocketNum = src.m_iFAISimulPocketNum;
	m_bLensDirtLensInspect = src.m_bLensDirtLensInspect;
	m_iLensDirtLensFThresLow = src.m_iLensDirtLensFThresLow;
	m_iLensDirtLensFThresHigh = src.m_iLensDirtLensFThresHigh;
	m_iLensDirtLensRoiOuterMargin = src.m_iLensDirtLensRoiOuterMargin;
	m_bLensDirtLensUseConnection = src.m_bLensDirtLensUseConnection;
	m_iLensDirtLensConnectionLength = src.m_iLensDirtLensConnectionLength;
	m_iLensDirtLensDefectMinSize = src.m_iLensDirtLensDefectMinSize;
	m_bLensScratchLensInspect = src.m_bLensScratchLensInspect;
	m_iLensScratchLensDThresMediF = src.m_iLensScratchLensDThresMediF;
	m_iLensScratchLensDThresMeanF = src.m_iLensScratchLensDThresMeanF;
	m_iLensScratchLensDThresVal = src.m_iLensScratchLensDThresVal;
	m_iLensScratchLensRoiOuterMargin = src.m_iLensScratchLensRoiOuterMargin;
	m_bLensScratchLensUseConnection = src.m_bLensScratchLensUseConnection;
	m_iLensScratchLensConnectionLength = src.m_iLensScratchLensConnectionLength;
	m_iLensScratchLensDefectMinSize = src.m_iLensScratchLensDefectMinSize;
	m_iLensScratchLensScratchMinLength = src.m_iLensScratchLensScratchMinLength;
	m_bLensStainLensInspect = src.m_bLensStainLensInspect;
	m_bLensStainLensUseDThres = src.m_bLensStainLensUseDThres;
	m_iLensStainScaleFactor = src.m_iLensStainScaleFactor;
	m_iLensStainLensDThresMediF = src.m_iLensStainLensDThresMediF;
	m_iLensStainLensDThresMeanF = src.m_iLensStainLensDThresMeanF;
	m_iLensStainLensDThresVal = src.m_iLensStainLensDThresVal;
	m_bLensStainLensUseFThres = src.m_bLensStainLensUseFThres;
	m_iLensStainLensFThresLow = src.m_iLensStainLensFThresLow;
	m_iLensStainLensFThresHigh = src.m_iLensStainLensFThresHigh;
	m_iLensStainLensRoiOuterMargin = src.m_iLensStainLensRoiOuterMargin;
	m_iLensStainLensRoiEnforceErosion = src.m_iLensStainLensRoiEnforceErosion;
	m_iLensStainLensRoiCastThres = src.m_iLensStainLensRoiCastThres;
	m_iLensStainLensRoiCastMargin = src.m_iLensStainLensRoiCastMargin;
	m_iLensStainLensDefectMinSize = src.m_iLensStainLensDefectMinSize;
	m_bLensStainUseLength = src.m_bLensStainUseLength;
	m_iLensStainDefectMinLength = src.m_iLensStainDefectMinLength;
	m_iLensStainDefectMaxLength = src.m_iLensStainDefectMaxLength;
	m_bLensStainUseNoScale = src.m_bLensStainUseNoScale;
	m_iLensStainNoScaleThres = src.m_iLensStainNoScaleThres;
	m_iDecisonTopBarrelInner_RoiOuterMargin = src.m_iDecisonTopBarrelInner_RoiOuterMargin;
	m_iDecisonTopBarrelInner_RoiInnerMargin = src.m_iDecisonTopBarrelInner_RoiInnerMargin;
	m_bInspectBarcodeShift = src.m_bInspectBarcodeShift;
	m_dBarcodeShiftRefX = src.m_dBarcodeShiftRefX;
	m_dBarcodeShiftRefY = src.m_dBarcodeShiftRefY;
	m_dBarcodeShiftToleranceX = src.m_dBarcodeShiftToleranceX;
	m_dBarcodeShiftToleranceY = src.m_dBarcodeShiftToleranceY;
	m_bInspectBarcodeRotation = src.m_bInspectBarcodeRotation;
	m_dBarcodeShiftToleranceRotation = src.m_dBarcodeShiftToleranceRotation;
	m_dBarcodeShiftRefRotation = src.m_dBarcodeShiftRefRotation;
	m_bUseBarcodeShiftHoleCenter = src.m_bUseBarcodeShiftHoleCenter;
	m_iBarcodeShiftHoleCenterGvMin = src.m_iBarcodeShiftHoleCenterGvMin;
	m_iBarcodeShiftHoleCenterGvMax = src.m_iBarcodeShiftHoleCenterGvMax;
	m_iBarcodeShiftHoleCenterGvClosing = src.m_iBarcodeShiftHoleCenterGvClosing;
	m_iBarcodeShiftHoleCenterDilation = src.m_iBarcodeShiftHoleCenterDilation;
	m_bInspectBlackCoatingDiameter = src.m_bInspectBlackCoatingDiameter;
	m_dBlackCoatingRef = src.m_dBlackCoatingRef;
	m_dBlackCoatingTolerance = src.m_dBlackCoatingTolerance;
	m_dBlackCoatingOffset = src.m_dBlackCoatingOffset;
	m_iBlackCoatingImage = src.m_iBlackCoatingImage;
	m_iBlackCoatingContour = src.m_iBlackCoatingContour;
	m_iBlackCoatingOuterMargin = src.m_iBlackCoatingOuterMargin;
	m_iBlackCoatingInnerMargin = src.m_iBlackCoatingInnerMargin;
	m_iBlackCoatingMinGray = src.m_iBlackCoatingMinGray;
	m_iBlackCoatingMaxGray = src.m_iBlackCoatingMaxGray;
	m_iDatumImage = src.m_iDatumImage;
	m_iDatumContour = src.m_iDatumContour;
	m_iDatumOuterMargin = src.m_iDatumOuterMargin;
	m_iDatumInnerMargin = src.m_iDatumInnerMargin;
	m_iDatumMinGray = src.m_iDatumMinGray;
	m_iDatumMaxGray = src.m_iDatumMaxGray;
	m_bUseImageScaling_BC = src.m_bUseImageScaling_BC;
	m_bUseImageScaling_DT = src.m_bUseImageScaling_DT;
	m_iImageScaleMethod_BC = src.m_iImageScaleMethod_BC;
	m_iImageScaleMethod_DT = src.m_iImageScaleMethod_DT;
	m_iImageScaleMax_BC = src.m_iImageScaleMax_BC;
	m_iImageScaleMax_DT = src.m_iImageScaleMax_DT;
	m_iImageScaleMin_BC = src.m_iImageScaleMin_BC;
	m_iImageScaleMin_DT = src.m_iImageScaleMin_DT;
	m_iInspectionType = src.m_iInspectionType;
	m_iBlackCoatingWidth = src.m_iBlackCoatingWidth;
	m_iBlackCoatingHeight = src.m_iBlackCoatingHeight;
	m_iBlackCoatingDiffGray = src.m_iBlackCoatingDiffGray;
	m_iBlackCoatingTransition = src.m_iBlackCoatingTransition;
	m_iDatumWidth = src.m_iDatumWidth;
	m_iDatumHeight = src.m_iDatumHeight;
	m_iDatumDiffGray = src.m_iDatumDiffGray;
	m_iDatumTransition = src.m_iDatumTransition;
	m_dLensMinArea = src.m_dLensMinArea;
	m_dLensMidArea = src.m_dLensMidArea;
	m_dLensMaxArea = src.m_dLensMaxArea;
	m_dLensScratchMinArea = src.m_dLensScratchMinArea;
	m_dLensScratchMidArea = src.m_dLensScratchMidArea;
	m_dLensScratchMaxArea = src.m_dLensScratchMaxArea;
	m_dLensDirtMinArea = src.m_dLensDirtMinArea;
	m_dLensDirtMidArea = src.m_dLensDirtMidArea;
	m_dLensDirtMaxArea = src.m_dLensDirtMaxArea;
	m_dLensWhiteDotMinArea = src.m_dLensWhiteDotMinArea;
	m_dLensWhiteDotMidArea = src.m_dLensWhiteDotMidArea;
	m_dLensWhiteDotMaxArea = src.m_dLensWhiteDotMaxArea;
	m_iLensMinNumAccept = src.m_iLensMinNumAccept;
	m_iLensMidNumAccept = src.m_iLensMidNumAccept;
	m_iLensMaxNumAccept = src.m_iLensMaxNumAccept;
	m_iLensScratchMinNumAccept = src.m_iLensScratchMinNumAccept;
	m_iLensScratchMidNumAccept = src.m_iLensScratchMidNumAccept;
	m_iLensScratchMaxNumAccept = src.m_iLensScratchMaxNumAccept;
	m_iLensDirtMinNumAccept = src.m_iLensDirtMinNumAccept;
	m_iLensDirtMidNumAccept = src.m_iLensDirtMidNumAccept;
	m_iLensDirtMaxNumAccept = src.m_iLensDirtMaxNumAccept;
	m_iLensWhiteDotMinNumAccept = src.m_iLensWhiteDotMinNumAccept;
	m_iLensWhiteDotMidNumAccept = src.m_iLensWhiteDotMidNumAccept;
	m_iLensWhiteDotMaxNumAccept = src.m_iLensWhiteDotMaxNumAccept;
	m_dDirtMinArea_Top = src.m_dDirtMinArea_Top;
	m_dDirtMidArea_Top = src.m_dDirtMidArea_Top;
	m_dDirtMaxArea_Top = src.m_dDirtMaxArea_Top;
	m_dScratchMinArea_Top = src.m_dScratchMinArea_Top;
	m_dScratchMidArea_Top = src.m_dScratchMidArea_Top;
	m_dScratchMaxArea_Top = src.m_dScratchMaxArea_Top;
	m_dStainMinArea_Top = src.m_dStainMinArea_Top;
	m_dStainMidArea_Top = src.m_dStainMidArea_Top;
	m_dStainMaxArea_Top = src.m_dStainMaxArea_Top;
	m_dDentMinArea_Top = src.m_dDentMinArea_Top;
	m_dDentMidArea_Top = src.m_dDentMidArea_Top;
	m_dDentMaxArea_Top = src.m_dDentMaxArea_Top;
	m_dChipMinArea_Top = src.m_dChipMinArea_Top;
	m_dChipMidArea_Top = src.m_dChipMidArea_Top;
	m_dChipMaxArea_Top = src.m_dChipMaxArea_Top;
	m_dEpoxyMinArea_Top = src.m_dEpoxyMinArea_Top;
	m_dEpoxyMidArea_Top = src.m_dEpoxyMidArea_Top;
	m_dEpoxyMaxArea_Top = src.m_dEpoxyMaxArea_Top;
	m_dRes1MinArea_Top = src.m_dRes1MinArea_Top;
	m_dRes1MidArea_Top = src.m_dRes1MidArea_Top;
	m_dRes1MaxArea_Top = src.m_dRes1MaxArea_Top;
	m_dRes2MinArea_Top = src.m_dRes2MinArea_Top;
	m_dRes2MidArea_Top = src.m_dRes2MidArea_Top;
	m_dRes2MaxArea_Top = src.m_dRes2MaxArea_Top;
	m_dWhiteDotMinArea_Top = src.m_dWhiteDotMinArea_Top;
	m_dWhiteDotMidArea_Top = src.m_dWhiteDotMidArea_Top;
	m_dWhiteDotMaxArea_Top = src.m_dWhiteDotMaxArea_Top;
	m_iDirtMinNumAccept_Top = src.m_iDirtMinNumAccept_Top;
	m_iDirtMidNumAccept_Top = src.m_iDirtMidNumAccept_Top;
	m_iDirtMaxNumAccept_Top = src.m_iDirtMaxNumAccept_Top;
	m_iScratchMinNumAccept_Top = src.m_iScratchMinNumAccept_Top;
	m_iScratchMidNumAccept_Top = src.m_iScratchMidNumAccept_Top;
	m_iScratchMaxNumAccept_Top = src.m_iScratchMaxNumAccept_Top;
	m_iStainMinNumAccept_Top = src.m_iStainMinNumAccept_Top;
	m_iStainMidNumAccept_Top = src.m_iStainMidNumAccept_Top;
	m_iStainMaxNumAccept_Top = src.m_iStainMaxNumAccept_Top;
	m_iDentMinNumAccept_Top = src.m_iDentMinNumAccept_Top;
	m_iDentMidNumAccept_Top = src.m_iDentMidNumAccept_Top;
	m_iDentMaxNumAccept_Top = src.m_iDentMaxNumAccept_Top;
	m_iChipMinNumAccept_Top = src.m_iChipMinNumAccept_Top;
	m_iChipMidNumAccept_Top = src.m_iChipMidNumAccept_Top;
	m_iChipMaxNumAccept_Top = src.m_iChipMaxNumAccept_Top;
	m_iEpoxyMinNumAccept_Top = src.m_iEpoxyMinNumAccept_Top;
	m_iEpoxyMidNumAccept_Top = src.m_iEpoxyMidNumAccept_Top;
	m_iEpoxyMaxNumAccept_Top = src.m_iEpoxyMaxNumAccept_Top;
	m_iRes1MinNumAccept_Top = src.m_iRes1MinNumAccept_Top;
	m_iRes1MidNumAccept_Top = src.m_iRes1MidNumAccept_Top;
	m_iRes1MaxNumAccept_Top = src.m_iRes1MaxNumAccept_Top;
	m_iRes2MinNumAccept_Top = src.m_iRes2MinNumAccept_Top;
	m_iRes2MidNumAccept_Top = src.m_iRes2MidNumAccept_Top;
	m_iRes2MaxNumAccept_Top = src.m_iRes2MaxNumAccept_Top;
	m_iWhiteDotMinNumAccept_Top = src.m_iWhiteDotMinNumAccept_Top;
	m_iWhiteDotMidNumAccept_Top = src.m_iWhiteDotMidNumAccept_Top;
	m_iWhiteDotMaxNumAccept_Top = src.m_iWhiteDotMaxNumAccept_Top;
	m_dDirtMinArea_Btm = src.m_dDirtMinArea_Btm;
	m_dDirtMidArea_Btm = src.m_dDirtMidArea_Btm;
	m_dDirtMaxArea_Btm = src.m_dDirtMaxArea_Btm;
	m_dScratchMinArea_Btm = src.m_dScratchMinArea_Btm;
	m_dScratchMidArea_Btm = src.m_dScratchMidArea_Btm;
	m_dScratchMaxArea_Btm = src.m_dScratchMaxArea_Btm;
	m_dStainMinArea_Btm = src.m_dStainMinArea_Btm;
	m_dStainMidArea_Btm = src.m_dStainMidArea_Btm;
	m_dStainMaxArea_Btm = src.m_dStainMaxArea_Btm;
	m_dDentMinArea_Btm = src.m_dDentMinArea_Btm;
	m_dDentMidArea_Btm = src.m_dDentMidArea_Btm;
	m_dDentMaxArea_Btm = src.m_dDentMaxArea_Btm;
	m_dChipMinArea_Btm = src.m_dChipMinArea_Btm;
	m_dChipMidArea_Btm = src.m_dChipMidArea_Btm;
	m_dChipMaxArea_Btm = src.m_dChipMaxArea_Btm;
	m_dEpoxyMinArea_Btm = src.m_dEpoxyMinArea_Btm;
	m_dEpoxyMidArea_Btm = src.m_dEpoxyMidArea_Btm;
	m_dEpoxyMaxArea_Btm = src.m_dEpoxyMaxArea_Btm;
	m_dRes1MinArea_Btm = src.m_dRes1MinArea_Btm;
	m_dRes1MidArea_Btm = src.m_dRes1MidArea_Btm;
	m_dRes1MaxArea_Btm = src.m_dRes1MaxArea_Btm;
	m_dRes2MinArea_Btm = src.m_dRes2MinArea_Btm;
	m_dRes2MidArea_Btm = src.m_dRes2MidArea_Btm;
	m_dRes2MaxArea_Btm = src.m_dRes2MaxArea_Btm;
	m_dWhiteDotMinArea_Btm = src.m_dWhiteDotMinArea_Btm;
	m_dWhiteDotMidArea_Btm = src.m_dWhiteDotMidArea_Btm;
	m_dWhiteDotMaxArea_Btm = src.m_dWhiteDotMaxArea_Btm;
	m_iDirtMinNumAccept_Btm = src.m_iDirtMinNumAccept_Btm;
	m_iDirtMidNumAccept_Btm = src.m_iDirtMidNumAccept_Btm;
	m_iDirtMaxNumAccept_Btm = src.m_iDirtMaxNumAccept_Btm;
	m_iScratchMinNumAccept_Btm = src.m_iScratchMinNumAccept_Btm;
	m_iScratchMidNumAccept_Btm = src.m_iScratchMidNumAccept_Btm;
	m_iScratchMaxNumAccept_Btm = src.m_iScratchMaxNumAccept_Btm;
	m_iStainMinNumAccept_Btm = src.m_iStainMinNumAccept_Btm;
	m_iStainMidNumAccept_Btm = src.m_iStainMidNumAccept_Btm;
	m_iStainMaxNumAccept_Btm = src.m_iStainMaxNumAccept_Btm;
	m_iDentMinNumAccept_Btm = src.m_iDentMinNumAccept_Btm;
	m_iDentMidNumAccept_Btm = src.m_iDentMidNumAccept_Btm;
	m_iDentMaxNumAccept_Btm = src.m_iDentMaxNumAccept_Btm;
	m_iChipMinNumAccept_Btm = src.m_iChipMinNumAccept_Btm;
	m_iChipMidNumAccept_Btm = src.m_iChipMidNumAccept_Btm;
	m_iChipMaxNumAccept_Btm = src.m_iChipMaxNumAccept_Btm;
	m_iEpoxyMinNumAccept_Btm = src.m_iEpoxyMinNumAccept_Btm;
	m_iEpoxyMidNumAccept_Btm = src.m_iEpoxyMidNumAccept_Btm;
	m_iEpoxyMaxNumAccept_Btm = src.m_iEpoxyMaxNumAccept_Btm;
	m_iRes1MinNumAccept_Btm = src.m_iRes1MinNumAccept_Btm;
	m_iRes1MidNumAccept_Btm = src.m_iRes1MidNumAccept_Btm;
	m_iRes1MaxNumAccept_Btm = src.m_iRes1MaxNumAccept_Btm;
	m_iRes2MinNumAccept_Btm = src.m_iRes2MinNumAccept_Btm;
	m_iRes2MidNumAccept_Btm = src.m_iRes2MidNumAccept_Btm;
	m_iRes2MaxNumAccept_Btm = src.m_iRes2MaxNumAccept_Btm;
	m_iWhiteDotMinNumAccept_Btm = src.m_iWhiteDotMinNumAccept_Btm;
	m_iWhiteDotMidNumAccept_Btm = src.m_iWhiteDotMidNumAccept_Btm;
	m_iWhiteDotMaxNumAccept_Btm = src.m_iWhiteDotMaxNumAccept_Btm;
	m_dDirtMinArea_Inner = src.m_dDirtMinArea_Inner;
	m_dDirtMidArea_Inner = src.m_dDirtMidArea_Inner;
	m_dDirtMaxArea_Inner = src.m_dDirtMaxArea_Inner;
	m_dScratchMinArea_Inner = src.m_dScratchMinArea_Inner;
	m_dScratchMidArea_Inner = src.m_dScratchMidArea_Inner;
	m_dScratchMaxArea_Inner = src.m_dScratchMaxArea_Inner;
	m_dStainMinArea_Inner = src.m_dStainMinArea_Inner;
	m_dStainMidArea_Inner = src.m_dStainMidArea_Inner;
	m_dStainMaxArea_Inner = src.m_dStainMaxArea_Inner;
	m_dDentMinArea_Inner = src.m_dDentMinArea_Inner;
	m_dDentMidArea_Inner = src.m_dDentMidArea_Inner;
	m_dDentMaxArea_Inner = src.m_dDentMaxArea_Inner;
	m_dChipMinArea_Inner = src.m_dChipMinArea_Inner;
	m_dChipMidArea_Inner = src.m_dChipMidArea_Inner;
	m_dChipMaxArea_Inner = src.m_dChipMaxArea_Inner;
	m_dEpoxyMinArea_Inner = src.m_dEpoxyMinArea_Inner;
	m_dEpoxyMidArea_Inner = src.m_dEpoxyMidArea_Inner;
	m_dEpoxyMaxArea_Inner = src.m_dEpoxyMaxArea_Inner;
	m_dRes1MinArea_Inner = src.m_dRes1MinArea_Inner;
	m_dRes1MidArea_Inner = src.m_dRes1MidArea_Inner;
	m_dRes1MaxArea_Inner = src.m_dRes1MaxArea_Inner;
	m_dRes2MinArea_Inner = src.m_dRes2MinArea_Inner;
	m_dRes2MidArea_Inner = src.m_dRes2MidArea_Inner;
	m_dRes2MaxArea_Inner = src.m_dRes2MaxArea_Inner;
	m_dWhiteDotMinArea_Inner = src.m_dWhiteDotMinArea_Inner;
	m_dWhiteDotMidArea_Inner = src.m_dWhiteDotMidArea_Inner;
	m_dWhiteDotMaxArea_Inner = src.m_dWhiteDotMaxArea_Inner;
	m_iDirtMinNumAccept_Inner = src.m_iDirtMinNumAccept_Inner;
	m_iDirtMidNumAccept_Inner = src.m_iDirtMidNumAccept_Inner;
	m_iDirtMaxNumAccept_Inner = src.m_iDirtMaxNumAccept_Inner;
	m_iScratchMinNumAccept_Inner = src.m_iScratchMinNumAccept_Inner;
	m_iScratchMidNumAccept_Inner = src.m_iScratchMidNumAccept_Inner;
	m_iScratchMaxNumAccept_Inner = src.m_iScratchMaxNumAccept_Inner;
	m_iStainMinNumAccept_Inner = src.m_iStainMinNumAccept_Inner;
	m_iStainMidNumAccept_Inner = src.m_iStainMidNumAccept_Inner;
	m_iStainMaxNumAccept_Inner = src.m_iStainMaxNumAccept_Inner;
	m_iDentMinNumAccept_Inner = src.m_iDentMinNumAccept_Inner;
	m_iDentMidNumAccept_Inner = src.m_iDentMidNumAccept_Inner;
	m_iDentMaxNumAccept_Inner = src.m_iDentMaxNumAccept_Inner;
	m_iChipMinNumAccept_Inner = src.m_iChipMinNumAccept_Inner;
	m_iChipMidNumAccept_Inner = src.m_iChipMidNumAccept_Inner;
	m_iChipMaxNumAccept_Inner = src.m_iChipMaxNumAccept_Inner;
	m_iEpoxyMinNumAccept_Inner = src.m_iEpoxyMinNumAccept_Inner;
	m_iEpoxyMidNumAccept_Inner = src.m_iEpoxyMidNumAccept_Inner;
	m_iEpoxyMaxNumAccept_Inner = src.m_iEpoxyMaxNumAccept_Inner;
	m_iRes1MinNumAccept_Inner = src.m_iRes1MinNumAccept_Inner;
	m_iRes1MidNumAccept_Inner = src.m_iRes1MidNumAccept_Inner;
	m_iRes1MaxNumAccept_Inner = src.m_iRes1MaxNumAccept_Inner;
	m_iRes2MinNumAccept_Inner = src.m_iRes2MinNumAccept_Inner;
	m_iRes2MidNumAccept_Inner = src.m_iRes2MidNumAccept_Inner;
	m_iRes2MaxNumAccept_Inner = src.m_iRes2MaxNumAccept_Inner;
	m_iWhiteDotMinNumAccept_Inner = src.m_iWhiteDotMinNumAccept_Inner;
	m_iWhiteDotMidNumAccept_Inner = src.m_iWhiteDotMidNumAccept_Inner;
	m_iWhiteDotMaxNumAccept_Inner = src.m_iWhiteDotMaxNumAccept_Inner;
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		for (int j = 0; j < MAX_LIGHT_CHANNEL; j++)
			m_iInspLightBright[i][j] = src.m_iInspLightBright[i][j];
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		for (int j = 0; j < MAX_LIGHT_PAGE; j++)
			m_iLightPageNo[i][j] = src.m_iLightPageNo[i][j];
	m_iBarcodeLightBright = src.m_iBarcodeLightBright;
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		for (int j = 0; j < MAX_LIGHT_CHANNEL; j++)
			m_iLightAverageValue[i][j] = src.m_iLightAverageValue[i][j];
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_iLightAverageValueTotal[i] = src.m_iLightAverageValueTotal[i];
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_iLightValueStart[i] = src.m_iLightValueStart[i];
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_iLightValueEnd[i] = src.m_iLightValueEnd[i];
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_iLightValueInterval[i] = src.m_iLightValueInterval[i];
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_iLightValueInTol[i] = src.m_iLightValueInTol[i];
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
		m_iTotalImageValueTol[i] = src.m_iTotalImageValueTol[i];
}