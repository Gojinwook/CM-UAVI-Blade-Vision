#include "StdAfx.h"
#include "uScan.h"
#include "Algorithm.h"

Algorithm* Algorithm::m_pInstance = NULL;

Algorithm* Algorithm::GetInstance(BOOL bShowFlag)
{
	if (!m_pInstance) {
		m_pInstance = new Algorithm();
	}
	return m_pInstance;
}

void Algorithm::DeleteInstance()
{
	SAFE_DELETE(m_pInstance);
}

Algorithm::Algorithm(void)
{

	for (int img = 0; img < MAX_IMAGE_TAB; img++)
	{
		m_HAlgoInspectImage[img].Reset();
		gen_empty_obj(&m_HAlgoInspectImage[img]);
	}

	InspectBarcodeImage.Reset();
	gen_empty_obj(&InspectBarcodeImage);

	// 24.03.27 - 이미지별 Shape 매칭 - LeeGW Start
	for (int k = 0; k < BARREL_LENS_IMAGE_TAB; k++)
	{
		for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
		{
			InspectContour[k][cont].Reset();
			gen_empty_obj(&InspectContour[k][cont]);
		}
	}
	// 24.03.27 - 이미지별 Shape 매칭 - LeeGW Start

	gen_empty_obj(&(m_HDefectRgn_FAI));
	for (int i = 0; i < MAX_FAI_ITEM; i++)
	{
		for (int j = 0; j < MAX_ONE_FAI_MEASURE_VALUE; j++)
		{
			gen_empty_obj(&(m_HMeasureRgn_FAI_Item[i][j]));
			gen_empty_obj(&(m_HReviewXLD_FAI_Item[i][j]));
			m_iViewportCenter_FAI_Item[i][j].x = -1;
			m_iViewportCenter_FAI_Item[i][j].y = -1;
		}
	}

	Init_AlgorithmInspectParam();   //////  P : 딜레이 생길때 문제점 해결요망.
}


Algorithm::~Algorithm(void)
{
}



UINT InspectThreadAlgorithm(LPVOID lp)
{

	if (THEAPP.Struct_PreferenceStruct.m_bDxLogDetail == TRUE)
	{
		try
		{
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memInfo);

			DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;  // 사용 중인 물리 메모리
			double physMemUsedMegaByte = (double)physMemUsed / (1024 * 1024);	// 현재 프로세스가 사용중인 메모리
			double physMemUsedPercent = ((double)physMemUsed / (double)memInfo.ullTotalPhys) * 100;

			CString strMemLog;
			strMemLog.Format("Memory in Use Start Insp Thread : %.2fMB (%.2f/100.00)", physMemUsedMegaByte, physMemUsedPercent);
			THEAPP.SaveLog(strMemLog);
		}
		catch (HException &except)
		{
			THEAPP.SaveLog("Memory Check Failure!");
		}
	}

	CString sInspectTime;
	double tDetectStart_total, tDetectEnd_total;  // for 검출 전체 경과 시간 측정
	tDetectStart_total = GetTickCount();

	Algorithm* pAlgorithm = (Algorithm*)lp;

	int iModuleNo = pAlgorithm->m_nModuleNo;

	//////////////////////////////////////////////////////////////////////////
	//		Barcode Inspection

	CString sBarcodeResult;
	Hobject HBarcodeROIImage;
	double dBarcodePosX, dBarcodePosY; 

	if (THEAPP.m_pGFunction->ValidHImage(pAlgorithm->InspectBarcodeImage)==TRUE)
		copy_obj(pAlgorithm->InspectBarcodeImage, &HBarcodeROIImage, 1, 1);
	else
		HBarcodeROIImage.Reset();

	sBarcodeResult = _T("");
	dBarcodePosX = dBarcodePosY = 0;

	double dBarcodePosRotation = 0;
	pAlgorithm->InspectBarcode(&HBarcodeROIImage, &sBarcodeResult, &dBarcodePosX, &dBarcodePosY, &dBarcodePosRotation, THEAPP.m_pModelDataManager->m_dBarcodeShiftRefX, THEAPP.m_pModelDataManager->m_dBarcodeShiftRefY ); //Barcode Rotation Inspection

	pAlgorithm->m_sBarcodeResult = sBarcodeResult;
	pAlgorithm->m_dBarcodePosOffsetX = dBarcodePosX;
	pAlgorithm->m_dBarcodePosOffsetY = dBarcodePosY;
	pAlgorithm->m_bBarcodeShiftNG = FALSE;
	pAlgorithm->m_bBarcodeModuleMixNG = FALSE;
	//Barcode Rotation Log
	pAlgorithm->m_dBarcodePosOffsetRotation = dBarcodePosRotation;
	pAlgorithm->m_bBarcodeRotationNG = FALSE;

	//Multiple Defect Start
	int nVirtualMzIdx = THEAPP.m_stVirtualMagazineNo.GetVirtualMagazineNo(THEAPP.m_pInspectResultDlg->CurrentLotID);
	THEAPP.m_strMultipleDefectBarcode[nVirtualMzIdx][THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1] = sBarcodeResult;
	//Multiple Defect End

	if (sBarcodeResult=="NOGRAB" || sBarcodeResult=="NOREAD")
	{
#ifdef INLINE_MODE
		pAlgorithm->m_bBarcodeError = TRUE;
#endif
	}
	else
	{
		if (THEAPP.m_pModelDataManager->m_bInspectBarcodeShift)
		{
			if (abs(dBarcodePosX) >= THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceX || abs(dBarcodePosY) >= THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceY)
				pAlgorithm->m_bBarcodeShiftNG = TRUE;
			else
				pAlgorithm->m_bBarcodeShiftNG = FALSE;
		}

		//Barcode Rotation Inspection Start
		if (THEAPP.m_pModelDataManager->m_bInspectBarcodeRotation)
		{
			if (abs(dBarcodePosRotation) >= THEAPP.m_pModelDataManager->m_dBarcodeShiftToleranceRotation )
			{
				pAlgorithm->m_bBarcodeShiftNG = TRUE;
				pAlgorithm->m_bBarcodeRotationNG = TRUE; //Barcode Rotation Log 
			}
			
		}
		//Barcode Rotation Inspection End

		// Add Module Mix
		CString sEEER = sBarcodeResult;
		// 24.04.17. - v2645 - Y24 모델 바코드 믹싱 - LeeGW Start
		int iPlusPos = sEEER.Find('+');

		if (iPlusPos > -1)
		{
			
			sEEER = sEEER.Mid(iPlusPos-7, 7);

		}
		else
		{
			sEEER = sEEER.Right(6);
			sEEER = sEEER.Mid(0,4);
		}
		// 24.04.17. - v2645 - Y24 모델 바코드 믹싱 - LeeGW End

		CString sDataFolder = THEAPP.GetWorkingDirectory()+"\\Data\\";
		CIniFileCS EEER_INI(sDataFolder+"\\ModelBarcode.ini");
		CString strSection = "EEER Code";

		CString sModelName;
		sModelName = EEER_INI.Get_String(strSection,sEEER,"Undefined");

		int iRetIndex = -1;
		if (sModelName != "Undefined")
		{
			iRetIndex = THEAPP.m_pModelDataManager->m_sModelName.Find(sModelName, 0);

			if (iRetIndex<0)
				pAlgorithm->m_bBarcodeModuleMixNG = TRUE;
			else
				pAlgorithm->m_bBarcodeModuleMixNG = FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	////////////////////////////////// Image Save ///////////////////////////////////////////////////
	double dRawImageSaveStartTime, dRawImageSaveEndTime;

	dRawImageSaveStartTime = GetTickCount();

	if (THEAPP.Struct_PreferenceStruct.m_bSaveRawImage)
	{
		if(THEAPP.Struct_PreferenceStruct.m_bSaveRawImageMerge == FALSE)
		{
			// 24.07.11 - v2654 - RawImage 저장 Thread 검사 Thread에서 분리 - LeeGW START
			for (int img = 0; img < MAX_IMAGE_TAB; img++)
			{
				if (img >= THEAPP.m_iMaxInspImageNo)
					break;

				THEAPP.m_pCameraManager->CopyRawImageQueue(img+1, iModuleNo, &(pAlgorithm->m_HAlgoInspectImage[img]), pAlgorithm->m_sBarcodeResult); // 원본영상저장

				if (THEAPP.Struct_PreferenceStruct.m_bSaveLasImage)
				{
					Hobject HSaveImage;

					if (THEAPP.Struct_PreferenceStruct.m_bSaveRawImageResize == TRUE)
					{
						double dResizeRatio = THEAPP.Struct_PreferenceStruct.m_dSaveRawImageResizeRatio;
						if (dResizeRatio < 1)
							zoom_image_factor(pAlgorithm->m_HAlgoInspectImage[img], &HSaveImage, dResizeRatio, dResizeRatio, "none");
						else
							copy_image(pAlgorithm->m_HAlgoInspectImage[img], &HSaveImage);
					}
					else
					{
						copy_image(pAlgorithm->m_HAlgoInspectImage[img], &HSaveImage);
					}

					SYSTEMTIME time;
					GetLocalTime(&time);

					CString strFile;
					strFile.Format("%s\\%s__RawImage%d_%s_%04d%02d%02d%02d%02d%02d_%s", THEAPP.GetLASImageDirectory(), THEAPP.m_pInspectService->m_sLotID_H, img + 1, sBarcodeResult, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, THEAPP.GetPCID());
					THEAPP.m_pSaveManager->SaveImage(HSaveImage, "jpg", strFile);
				}
			}
			// 24.07.11 - v2654 - RawImage 저장 Thread 검사 Thread에서 분리 - LeeGW END
		}
		else
		{
			Hobject HConcatImage, HSaveImage, HCombineImage;
			double dResizeRatio;
			dResizeRatio= THEAPP.Struct_PreferenceStruct.m_dSaveRawImageResizeRatio;

			gen_empty_obj(&HConcatImage);

			for (int i = 0; i < MAX_IMAGE_TAB; i++)
			{
				if (i >= THEAPP.m_iMaxInspImageNo)
					break;

				if (THEAPP.m_pGFunction->ValidHImage(pAlgorithm->m_HAlgoInspectImage[i]))
				{
					try 
					{
						if(THEAPP.Struct_PreferenceStruct.m_bSaveRawImageResize)
						{
							if (dResizeRatio < 1)
								zoom_image_factor(pAlgorithm->m_HAlgoInspectImage[i], &HSaveImage, dResizeRatio, dResizeRatio, "none");
							else
								copy_image(pAlgorithm->m_HAlgoInspectImage[i], &HSaveImage);
						}
						else
						{
							copy_image(pAlgorithm->m_HAlgoInspectImage[i], &HSaveImage);
						}
					}
					catch(HException &except)
					{
						CString str; str.Format("Halcon Exception [CopyRawImageQueue-m_bSaveRawImageMerge : zoom_image_factor] : %s", except.message); THEAPP.SaveLog(str);
						copy_image(pAlgorithm->m_HAlgoInspectImage[i], &HSaveImage);
					}

					concat_obj(HConcatImage, HSaveImage, &HConcatImage);
				}
			}

			if (THEAPP.m_pGFunction->ValidHImage(HConcatImage) == TRUE)
				tile_images(HConcatImage, &HCombineImage, MAX_COMBINE_IMAGE_NUMBER, "horizontal");

			THEAPP.m_pCameraManager->CopyRawImageQueue(-100, iModuleNo, &HCombineImage, pAlgorithm->m_sBarcodeResult); // 원본영상저장

			if (THEAPP.Struct_PreferenceStruct.m_bSaveLasImage)
			{
				SYSTEMTIME time;
				GetLocalTime(&time);

				CString strFile;
				strFile.Format("%s\\%s__RawImage_%s_%04d%02d%02d%02d%02d%02d_%s", THEAPP.GetLASImageDirectory(), THEAPP.m_pInspectService->m_sLotID_H, sBarcodeResult, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, THEAPP.GetPCID());
				THEAPP.m_pSaveManager->SaveImage(HCombineImage, "jpg", strFile);
			}
		}
	}

	THEAPP.m_pCameraManager->CopyRawImageQueue(10, iModuleNo, &(pAlgorithm->InspectBarcodeImage), pAlgorithm->m_sBarcodeResult); // 원본영상저장		// 24.07.11 - v2654 - RawImage 저장 Thread 검사 Thread에서 분리 - LeeGW

	if (THEAPP.Struct_PreferenceStruct.m_bSaveLasImage)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);

		CString strFile;
		strFile.Format("%s\\%s__Barcode_%s_%04d%02d%02d%02d%02d%02d_%s", THEAPP.GetLASImageDirectory(), THEAPP.m_pInspectService->m_sLotID_H, sBarcodeResult, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, THEAPP.GetPCID());
		THEAPP.m_pSaveManager->SaveImage(pAlgorithm->InspectBarcodeImage, "jpg", strFile);
	}

	dRawImageSaveEndTime = GetTickCount();

	sInspectTime.Format("  *-- Module %2d RawImage Save Time: %.0lf ms", iModuleNo, dRawImageSaveEndTime-dRawImageSaveStartTime);
	THEAPP.SaveLog(sInspectTime);

	////////////////////////////////// added for CMI3000 2000 ====> ///////////////////////////////////////////////////

	////////// 조명이 정상인지 검토하고 이상있으면 검사 종료
	Hobject HRgnEmpty, HImgEmpty_pre, HImgEmpty;
	double iEmptyMean = 0, iEmptySigma = 0;

	//0524
	int iLightErrorMinThres = THEAPP.Struct_PreferenceStruct.m_iLightErrorMinLimit;
	int iLightErrorMaxThres = THEAPP.Struct_PreferenceStruct.m_iLightErrorMaxLimit;

	Hobject HDomainRgn;
	double dMean[MAX_IMAGE_TAB], dStdev; //APD (dMean single->array)

	get_domain(pAlgorithm->m_HAlgoInspectImage[TRIGGER_SURFACE], &HDomainRgn);
	
	for (int img = 0; img < MAX_IMAGE_TAB; img++)
	{
		if (img >= THEAPP.m_iMaxInspImageNo)
			break;

		intensity(HDomainRgn, pAlgorithm->m_HAlgoInspectImage[img], &dMean[img], &dStdev); //APD
		if (dMean[img]<iLightErrorMinThres || dMean[img]>iLightErrorMaxThres) //APD
		{
			pAlgorithm->m_bLightDisorder = TRUE;
			pAlgorithm->m_bMatchingSuccess = FALSE;
			pAlgorithm->ThreadFinishFlag = 1;
			
			CString temp;
			temp.Format("조명 이상 %d번 이미지", img +1);
			THEAPP.SaveLog(temp);

			return 0;
		}
	}

	HDomainRgn.Reset();

	////////// 현재 검사 모듈이 empty인지 판단하고 empty이면 검사 종료

	BOOL bEmptyCM = TRUE;

	Hobject HEmptyCheckRgn, HEmptyCheckImageReduced, HAmp, HDir, HEdgeRgn;
	Hlong lEdgeArea;
	double dEdgeCenterY, dEdgeCenterX;

	gen_circle (&HEmptyCheckRgn, THEAPP.m_pCameraManager->GetCamImageHeight()/2, THEAPP.m_pCameraManager->GetCamImageWidth()/2, THEAPP.Struct_PreferenceStruct.m_iEmptyCircleRadius);
	if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
		reduce_domain(pAlgorithm->m_HAlgoInspectImage[TRIGGER_SURFACE], HEmptyCheckRgn, &HEmptyCheckImageReduced);
	else
		reduce_domain (pAlgorithm->m_HAlgoInspectImage[TRIGGER_LENS1], HEmptyCheckRgn, &HEmptyCheckImageReduced);

	edges_image(HEmptyCheckImageReduced, &HAmp, &HDir, "canny", 1, "none", 20, 40);
	threshold(HAmp, &HEdgeRgn, 40, 255);
	area_center(HEdgeRgn,&lEdgeArea,&dEdgeCenterY,&dEdgeCenterX);
	if (lEdgeArea > THEAPP.Struct_PreferenceStruct.m_iEmptyMaxEdgePoint)
		bEmptyCM = FALSE;

	if (bEmptyCM==TRUE)
	{
		reduce_domain (pAlgorithm->m_HAlgoInspectImage[TRIGGER_SURFACE], HEmptyCheckRgn, &HEmptyCheckImageReduced);
		edges_image(HEmptyCheckImageReduced, &HAmp, &HDir, "canny", 1, "none", 20, 40);
		threshold(HAmp, &HEdgeRgn, 40, 255);
		area_center(HEdgeRgn,&lEdgeArea,&dEdgeCenterY,&dEdgeCenterX);
		if (lEdgeArea > THEAPP.Struct_PreferenceStruct.m_iEmptyMaxEdgePoint)
			bEmptyCM = FALSE;
	}
	if (bEmptyCM==TRUE)
	{
		reduce_domain (pAlgorithm->m_HAlgoInspectImage[TRIGGER_EDGE], HEmptyCheckRgn, &HEmptyCheckImageReduced);
		edges_image(HEmptyCheckImageReduced, &HAmp, &HDir, "canny", 1, "none", 20, 40);
		threshold(HAmp, &HEdgeRgn, 40, 255);
		area_center(HEdgeRgn,&lEdgeArea,&dEdgeCenterY,&dEdgeCenterX);
		if (lEdgeArea > THEAPP.Struct_PreferenceStruct.m_iEmptyMaxEdgePoint)
			bEmptyCM = FALSE;
	}
	if (bEmptyCM==TRUE)
	{
		if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
			reduce_domain(pAlgorithm->m_HAlgoInspectImage[TRIGGER_LENS1], HEmptyCheckRgn, &HEmptyCheckImageReduced);
		else
			reduce_domain (pAlgorithm->m_HAlgoInspectImage[TRIGGER_LENS2], HEmptyCheckRgn, &HEmptyCheckImageReduced);
		edges_image(HEmptyCheckImageReduced, &HAmp, &HDir, "canny", 1, "none", 20, 40);
		threshold(HAmp, &HEdgeRgn, 40, 255);
		area_center(HEdgeRgn,&lEdgeArea,&dEdgeCenterY,&dEdgeCenterX);
		if (lEdgeArea > THEAPP.Struct_PreferenceStruct.m_iEmptyMaxEdgePoint)
			bEmptyCM = FALSE;
	}
	
	HEdgeRgn.Reset();
	HEmptyCheckRgn.Reset();
	HEmptyCheckImageReduced.Reset();
	gen_empty_obj(&HEmptyCheckImageReduced);

	if (bEmptyCM==TRUE)
	{
		pAlgorithm->m_bModuleEmpty = TRUE;
		pAlgorithm->m_bMatchingSuccess = FALSE;
		pAlgorithm->ThreadFinishFlag = 1;

		tDetectEnd_total = GetTickCount();
		sInspectTime.Format("* Module %2d Defect 검출 시간 thread total: %.0lf ms - Module Empty!", iModuleNo, tDetectEnd_total - tDetectStart_total);
		THEAPP.SaveDetectLog(sInspectTime);
		THEAPP.SaveLog(sInspectTime);

		return 0;
	}

	//APD Start
	for( int nImgNo = 0; nImgNo < MAX_IMAGE_TAB; nImgNo++ )
	{
		if (nImgNo >= THEAPP.m_iMaxInspImageNo)
			break;

		THEAPP.m_dEachImgAvgGV[nImgNo] += dMean[nImgNo];
	}
	THEAPP.m_nTotalModuleCnt++;
	//APD End

	//Barcode Rotation Inspection Start
	if(pAlgorithm->m_bBarcodeShiftNG == TRUE)
	{
		CString strBarcodeShiftLog;
		strBarcodeShiftLog.Format("Barcode Shift NG X:%.2lf Y:%.2lf Deg:%.2lf",dBarcodePosX,dBarcodePosY,dBarcodePosRotation);
		THEAPP.SaveLog(strBarcodeShiftLog);
		THEAPP.m_pHandlerService->Set_BarcodeAlarmRequest("Barcode",THEAPP.m_pInspectResultDlg->CurrentLotID, HANDLER_ALARM_BARCODE_NG, dBarcodePosX, dBarcodePosY, dBarcodePosRotation );
	}
	//Barcode Rotation Inspection End

	double dTStart_depart = 0, dTEnd_depart = 0;		// for 구간 별 프로세스 타임 측정: 베럴, 렌즈, 후처리
	double dTStart = 0, dTEnd = 0;						// for 개별 프로세스 타임 측정

	//////////////////////////////////////////////////////////////////////////
	// Global Image Align
	int i;
	BOOL bGlobalMatchingSuccess = TRUE;
	double dRotationAngleDeg, dDeltaX, dDeltaY;

	for (i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		if (THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i] == TRUE)
		{
			bGlobalMatchingSuccess = pAlgorithm->ImageAlignShapeMatching(
				pAlgorithm->m_HAlgoInspectImage[i],
				pAlgorithm->m_HAlgoInspectImage,
				&(pAlgorithm->m_HInspectLAlignModelID[i]),
				THEAPP.m_pModelDataManager->m_bLocalAlignImage[i],
				THEAPP.m_pModelDataManager->m_iLocalMatchingAngleRange[i],
				THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMin[i],
				THEAPP.m_pModelDataManager->m_dLocalMatchingScaleMax[i],
				THEAPP.m_pModelDataManager->m_dLocalMatchingScore[i],
				THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefX[i],
				THEAPP.m_pModelDataManager->m_dLocalTeachAlignRefY[i],
				THEAPP.m_pModelDataManager->m_iLocalMatchingMethod[i],
				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTX[i],
				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectLTY[i],
				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBX[i],
				THEAPP.m_pModelDataManager->m_iLocalMatchingTeachingRectRBY[i],
				THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginX[i],
				THEAPP.m_pModelDataManager->m_iLocalMatchingSearchMarginY[i],
				THEAPP.m_pModelDataManager->m_bUseMatchingAffineConstant[i],
				&dRotationAngleDeg,
				&dDeltaX,
				&dDeltaY);

			if (bGlobalMatchingSuccess == FALSE)
			{
				break;
			}
		}
	}

	////////////////////////////// Matching /////////////////////////////////////////


	if (THEAPP.m_iMachineInspType == MACHINE_WELDING)
	{
		pAlgorithm->m_bMatchingSuccess = TRUE;
	}
	else
	{
		// 24.03.27 - v2644 - 이미지별 Shape 매칭 - LeeGW Start
		dTStart_depart = GetTickCount();
		CString strMatchingLog;

		double dContourCenterX, dContourCenterY;
		dContourCenterX = dContourCenterY = -1;

		Hobject contour[MAX_CONTOUR_NUM];

		pAlgorithm->m_bMatchingSuccess = pAlgorithm->MatchingAlgorithm(// 매칭 
			pAlgorithm->m_HAlgoInspectImage[TRIGGER_SURFACE], pAlgorithm->m_HAlgoInspectImage[TRIGGER_LENS1], pAlgorithm->m_HAlgoInspectImage[TRIGGER_EDGE], pAlgorithm->m_HAlgoInspectImage[TRIGGER_LENS2],
			&contour[MATCHING_CONTOUR_1], &contour[MATCHING_CONTOUR_2], &contour[MATCHING_CONTOUR_3], &contour[MATCHING_CONTOUR_4], &contour[MATCHING_CONTOUR_5],
			&pAlgorithm->InspectModelID[MATCHING_MODEL_1], &pAlgorithm->InspectModelID[MATCHING_MODEL_2], &pAlgorithm->InspectModelID[MATCHING_MODEL_3], &pAlgorithm->InspectModelID[MATCHING_MODEL_4],
			&dContourCenterX, &dContourCenterY);

		if (pAlgorithm->m_bMatchingSuccess == TRUE)
		{
			Hobject HMatchingContour;
			for (int imgIdx = 0; imgIdx < BARREL_LENS_IMAGE_TAB; imgIdx++)
			{
				for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
				{
					if (THEAPP.m_pModelDataManager->m_bOneBarrel && cont < MATCHING_CONTOUR_3)
					{
						pAlgorithm->InspectContour[imgIdx][cont].Reset();
						continue;
					}
					copy_obj(contour[cont], &pAlgorithm->InspectContour[imgIdx][cont], 1, 1);
				}
	
				// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
				// Template ROI Align
				if (THEAPP.m_pModelDataManager->m_bUseBLROIAlign[imgIdx] == TRUE)
				{
					if (THEAPP.m_pGFunction->ValidHImage(pAlgorithm->m_HAlgoInspectImage[imgIdx]) == FALSE)
						continue;

					for (int cont = 0; cont < MAX_CONTOUR_NUM; cont++)
					{
						if (THEAPP.m_pModelDataManager->m_bUseBLROIAlignContour[imgIdx][cont] == TRUE)
						{
							if (THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[imgIdx][cont] < 0)
								continue;

							if (THEAPP.m_pGFunction->ValidHXLD(pAlgorithm->InspectContour[imgIdx][cont]) == FALSE)
								continue;

							copy_obj(pAlgorithm->InspectContour[imgIdx][cont], &HMatchingContour, 1, 1);
							pAlgorithm->TemplateMatchingAlgorithm(pAlgorithm->m_HAlgoInspectImage[imgIdx], &HMatchingContour, &THEAPP.m_pModelDataManager->m_iBLROIAlignMatchingModel[imgIdx][cont]);

							copy_obj(HMatchingContour, &pAlgorithm->InspectContour[imgIdx][cont], 1, 1);
						}
					}
				}
				// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW END
			}
			
		}

		dTEnd_depart = GetTickCount();
		sInspectTime.Format("  *-- Module %2d time for Matching %.0lf ms", iModuleNo, dTEnd_depart-dTStart_depart);
		THEAPP.SaveDetectLog(sInspectTime);
		THEAPP.SaveLog(sInspectTime);
		// 24.03.27 - v2644 - 이미지별 Shape 매칭 - LeeGW End

		//////////////////////////// Matching /////////////////////////////////////////
	}
	if (bGlobalMatchingSuccess==FALSE)
		pAlgorithm->m_bMatchingSuccess = FALSE;

	//////////////////////////////////////////////////////////////////////////
	////////// 매칭 실패면 스레드 종료 /////////////////
	if(!pAlgorithm->m_bMatchingSuccess) 
	{
		pAlgorithm->ThreadFinishFlag = 1;
		THEAPP.SaveLog("매칭 실패");
		tDetectEnd_total = GetTickCount();
		sInspectTime.Format("* Module %2d Defect 검출 시간 thread total: %.0lf ms - Matching Fail!", iModuleNo, tDetectEnd_total - tDetectStart_total);
		THEAPP.SaveDetectLog(sInspectTime);
		THEAPP.SaveLog(sInspectTime);
		return 0;
	}
	else 
	{
		THEAPP.SaveLog("매칭 성공");
	}

	CAlgorithmParam AlgorithmParam, ROIAlgorithmParam;
	int iImageIdx, iTabIdx;
	Hobject HDefectRgn;
	Hobject HEpoxyDontCareRgn, HEpoxyCandidateRgn;

	//////////////////////////////////////////////////////////////////////////
	// 각 검사 영상에서의 불량 영역
	for (iImageIdx=0; iImageIdx<MAX_IMAGE_TAB; iImageIdx++)
	{
		gen_empty_obj(&(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx]));	
		gen_empty_obj(&(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx]));	
		gen_empty_obj(&(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx]));	
		gen_empty_obj(&(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx]));	
		gen_empty_obj(&(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx]));

		gen_empty_obj(&(pAlgorithm->m_HDefectEpoxyHole[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectNorthSpringNG[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HDefectEastSpringNG[iImageIdx]));
		gen_empty_obj(&(pAlgorithm->m_HdefectExtra[iImageIdx]));

		// AI 검사 추가 - LeeGW
		for (int iDefectType = 0; iDefectType < MAX_DEFECT_NAME; iDefectType++)
		{
			gen_empty_obj(&pAlgorithm->m_HTypeAIDefectRgn[iImageIdx][iDefectType]);
		}

		gen_empty_obj(&pAlgorithm->m_HImageAIDefectRgn[iImageIdx]);
		// AI 검사 추가 - LeeGW

	}
	//////////////////////////////////////////////////////////////////////////

	gen_empty_obj(&(pAlgorithm->m_HdefectLens));
	gen_empty_obj(&(pAlgorithm->m_HdefectBarrel));
	gen_empty_obj(&(pAlgorithm->m_HROIInspectAreaRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectDirtRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectScratchRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectStainRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectDentRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectChippingRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectEpoxyRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectOutsideCTRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectPeelOffRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectWhiteDotRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectLensCTRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectLensScratchRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectLensDirtRgn));
	gen_empty_obj(&(pAlgorithm->m_HDefectLensWhiteDotRgn));

	//////////////////////////////////////////////////////////////////////////
	// ROI Inspection 추가 - LeeGW

	if (THEAPP.Struct_PreferenceStruct.m_bDxLogDetail == TRUE) dTStart_depart = GetTickCount();

	Hobject HFoundInspectAreaRgn, HResultXld;
	gen_empty_obj(&HFoundInspectAreaRgn);

	int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();

	GTRegion* pInspectROIRgn;
	for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);
		pInspectROIRgn->ResetLocalAlignResult(iModuleNo-1);
	}

	Hobject HPartCheckAlignRgn, HPartCheckResultRgn;
	gen_empty_obj(&HPartCheckAlignRgn);
	gen_empty_obj(&HPartCheckResultRgn);

	BOOL bAnisoAlignShiftResult;
	Hobject HAnisoAlignRgn;
	gen_empty_obj(&HAnisoAlignRgn);

	for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

		iImageIdx = pInspectROIRgn->m_iTeachImageIndex - 1;

		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)		
			continue;

		for (iTabIdx = 0; iTabIdx < MAX_ROI_TEACHING_TAB; iTabIdx++)
		{
			Hobject HInspectAreaRgn = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);
			AlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];

			// Part Check
			if (AlgorithmParam.m_bROIPartCheckUse && AlgorithmParam.m_bROIPartCheckLocalAlignUse)
			{
				HPartCheckResultRgn = pAlgorithm->PartCheckAlgorithm(pInspectROIRgn->m_HPartModelID, pAlgorithm->m_HAlgoInspectImage[iImageIdx], HInspectAreaRgn, AlgorithmParam, &pInspectROIRgn->m_iLocalAlignDeltaX[iModuleNo - 1], &pInspectROIRgn->m_iLocalAlignDeltaY[iModuleNo - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngle[iModuleNo - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iModuleNo - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iModuleNo - 1], &HPartCheckAlignRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HPartCheckAlignRgn))
					pInspectROIRgn->m_HLocalAlignShapeRgn[iModuleNo - 1] = HPartCheckAlignRgn;
			}

			// 비등방 얼라인
			if (AlgorithmParam.m_bUseROIAnisoAlign && AlgorithmParam.m_bROIAnisoAlignLocalAlignUse)
			{
				HAnisoAlignRgn = pAlgorithm->AnisoAlignAlgorithm(pInspectROIRgn->m_HAnisoModelID, pAlgorithm->m_HAlgoInspectImage[iImageIdx], HInspectAreaRgn, AlgorithmParam, &bAnisoAlignShiftResult, &pInspectROIRgn->m_iLocalAlignDeltaX[iModuleNo - 1], &pInspectROIRgn->m_iLocalAlignDeltaY[iModuleNo - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngle[iModuleNo - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iModuleNo - 1], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iModuleNo - 1]);

				if (THEAPP.m_pGFunction->ValidHRegion(HAnisoAlignRgn))
					pInspectROIRgn->m_HLocalAlignShapeRgn[iModuleNo - 1] = HAnisoAlignRgn;
			}
		}
	}

	for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
	{
		pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

		iImageIdx = pInspectROIRgn->m_iTeachImageIndex-1;

		if (pInspectROIRgn == NULL)
			continue;

		if (pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_INSPECTION && pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_AI_INSPECTION && 
			(pInspectROIRgn->m_iInspectionType < INSPECTION_TYPE_FAI_START || pInspectROIRgn->m_iInspectionType > INSPECTION_TYPE_FAI_END))
			continue;

		for (iTabIdx = 0; iTabIdx < MAX_ROI_TEACHING_TAB; iTabIdx++)
		{	
			AlgorithmParam = pInspectROIRgn->m_AlgorithmParam[iTabIdx];

			if (AlgorithmParam.m_bInspect)
			{
				CString sPostion = _T("");

				HDefectRgn = pAlgorithm->CommonAlgorithmROI(pAlgorithm->m_HAlgoInspectImage[iImageIdx], pInspectROIRgn, AlgorithmParam, &HFoundInspectAreaRgn, &HResultXld, iModuleNo - 1);
				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_AI_INSPECTION)
					{
						// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW START
						if (AlgorithmParam.m_bSpecialNG)
							pAlgorithm->m_bAISpecialNGSortFlag = TRUE;
						// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW END


						if (THEAPP.Struct_PreferenceStruct.m_iADJImageCropType == ADJ_INSP_TYPE_BLOB)
							concat_obj(pAlgorithm->m_HTypeAIDefectRgn[iImageIdx][AlgorithmParam.m_iDefectNameIdx], HDefectRgn, &pAlgorithm->m_HTypeAIDefectRgn[iImageIdx][AlgorithmParam.m_iDefectNameIdx]); // AI 검사 - LeeGW
						else
							concat_obj(pAlgorithm->m_HTypeAIDefectRgn[iImageIdx][AlgorithmParam.m_iDefectNameIdx], HFoundInspectAreaRgn, &pAlgorithm->m_HTypeAIDefectRgn[iImageIdx][AlgorithmParam.m_iDefectNameIdx]); // 
					}
					else
					{
						// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW START
						if (AlgorithmParam.m_bSpecialNG)
							pAlgorithm->m_bSpecialNGSortFlag = TRUE;
						// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW END

						concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
					}
						
					int iExImageStartNo = 0;
					if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
						iExImageStartNo = BARREL_LENS_IMAGE_TAB;

					if (pInspectROIRgn->m_iTeachImageIndex >= iExImageStartNo)
					{
						switch (AlgorithmParam.m_iDefectNameIdx)
						{
						case	DEFECT_NAME_EPOXYHOLE:
							concat_obj(pAlgorithm->m_HDefectEpoxyHole[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectEpoxyHole[iImageIdx]));
							union1(pAlgorithm->m_HDefectEpoxyHole[iImageIdx], &pAlgorithm->m_HDefectEpoxyHole[iImageIdx]);
							copy_obj(pAlgorithm->m_HDefectEpoxyHole[iImageIdx], &pAlgorithm->m_HdefectExtra[iImageIdx], 1, -1);
							break;
						case	DEFECT_NAME_NORTH_SPRINGNG:
							concat_obj(pAlgorithm->m_HDefectNorthSpringNG[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectNorthSpringNG[iImageIdx]));
							union1(pAlgorithm->m_HDefectNorthSpringNG[iImageIdx], &pAlgorithm->m_HDefectNorthSpringNG[iImageIdx]);
							copy_obj(pAlgorithm->m_HDefectNorthSpringNG[iImageIdx], &pAlgorithm->m_HdefectExtra[iImageIdx], 1, -1);
							break;
						case	DEFECT_NAME_EAST_SPRINGNG:
							concat_obj(pAlgorithm->m_HDefectEastSpringNG[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectEastSpringNG[iImageIdx]));
							union1(pAlgorithm->m_HDefectEastSpringNG[iImageIdx], &pAlgorithm->m_HDefectEastSpringNG[iImageIdx]);
							copy_obj(pAlgorithm->m_HDefectEastSpringNG[iImageIdx], &pAlgorithm->m_HdefectExtra[iImageIdx], 1, -1);
							break;
						}
					}
					else
					{

						switch (AlgorithmParam.m_iDefectNameIdx)
						{
						case 	DEFECT_NAME_DIRT:
							concat_obj(pAlgorithm->m_HDefectDirtRgn, HDefectRgn, &(pAlgorithm->m_HDefectDirtRgn));
							concat_obj(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_SCRATCH:
							concat_obj(pAlgorithm->m_HDefectScratchRgn, HDefectRgn, &(pAlgorithm->m_HDefectScratchRgn));
							concat_obj(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_STAIN:
							concat_obj(pAlgorithm->m_HDefectStainRgn, HDefectRgn, &(pAlgorithm->m_HDefectStainRgn));
							concat_obj(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_DENT:
							concat_obj(pAlgorithm->m_HDefectDentRgn, HDefectRgn, &(pAlgorithm->m_HDefectDentRgn));
							concat_obj(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_CHIPPING:
							concat_obj(pAlgorithm->m_HDefectChippingRgn, HDefectRgn, &(pAlgorithm->m_HDefectChippingRgn));
							concat_obj(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_EPOXY:
							concat_obj(pAlgorithm->m_HDefectEpoxyRgn, HDefectRgn, &(pAlgorithm->m_HDefectEpoxyRgn));
							concat_obj(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_OUTSIDE_CT:
							concat_obj(pAlgorithm->m_HDefectOutsideCTRgn, HDefectRgn, &(pAlgorithm->m_HDefectOutsideCTRgn));
							concat_obj(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_PEEL_OFF:
							concat_obj(pAlgorithm->m_HDefectPeelOffRgn, HDefectRgn, &(pAlgorithm->m_HDefectPeelOffRgn));
							concat_obj(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_WHITEDOT:
							concat_obj(pAlgorithm->m_HDefectWhiteDotRgn, HDefectRgn, &(pAlgorithm->m_HDefectWhiteDotRgn));
							concat_obj(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_LENS_CONTAMINATION:
							concat_obj(pAlgorithm->m_HDefectLensCTRgn, HDefectRgn, &(pAlgorithm->m_HDefectLensCTRgn));
							concat_obj(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_LENS_SCRATCH:
							concat_obj(pAlgorithm->m_HDefectLensScratchRgn, HDefectRgn, &(pAlgorithm->m_HDefectLensScratchRgn));
							concat_obj(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_LENS_DIRT:
							concat_obj(pAlgorithm->m_HDefectLensDirtRgn, HDefectRgn, &(pAlgorithm->m_HDefectLensDirtRgn));
							concat_obj(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx]));
							break;
						case 	DEFECT_NAME_LENS_WHITEDOT:
							concat_obj(pAlgorithm->m_HDefectLensWhiteDotRgn, HDefectRgn, &(pAlgorithm->m_HDefectLensWhiteDotRgn));
							concat_obj(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx]));
							break;
						}
					}

					if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
					{
						switch (AlgorithmParam.m_iDefectNameIdx)
						{
						case 	DEFECT_NAME_FAI_14:
							copy_obj(HDefectRgn, &pAlgorithm->m_HMeasureRgn_FAI_Item[14][0], 1, -1);
							break;
						}
					}
					else if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
					{
						switch (AlgorithmParam.m_iDefectNameIdx)
						{
						case 	DEFECT_NAME_FAI_26:
							copy_obj(HDefectRgn, &pAlgorithm->m_HMeasureRgn_FAI_Item[26][0], 1, -1);
							break;
						case 	DEFECT_NAME_FAI_14:
							copy_obj(HDefectRgn, &pAlgorithm->m_HMeasureRgn_FAI_Item[14][1], 1, -1);
							break;
						}

					}
					else if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_SPOT)
					{
						switch (AlgorithmParam.m_iDefectNameIdx)
						{
						case 	DEFECT_NAME_FAI_EA:
							concat_obj(HDefectRgn, pAlgorithm->m_HMeasureRgn_FAI_Item[124][0], &pAlgorithm->m_HMeasureRgn_FAI_Item[124][0]);
							break;
						case 	DEFECT_NAME_FAI_DIAMETER:
						case 	DEFECT_NAME_FAI_POSITION:
							copy_obj(HDefectRgn, &pAlgorithm->m_HMeasureRgn_FAI_Item[125][pInspectROIRgn->m_iFAIWeldingSpotID - 1], 1, -1);
							copy_obj(HDefectRgn, &pAlgorithm->m_HMeasureRgn_FAI_Item[125 + pInspectROIRgn->m_iFAIWeldingSpotID][0], 1, -1);
							break;
						}
					}
					else if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_WELDING_POCKET)
					{
						switch (AlgorithmParam.m_iDefectNameIdx)
						{
						case 	DEFECT_NAME_FAI_POSITION:
							copy_obj(HDefectRgn, &pAlgorithm->m_HMeasureRgn_FAI_Item[125 + pInspectROIRgn->m_iFAIWeldingPocketID][1], 1, -1);
							break;
						}
					}

					// 파라미터 정보 저장 - LeeGW
					if (THEAPP.Struct_PreferenceStruct.m_bSaveDetectParamLog)
					{
						DetectParamInfoStruct stDetectParamInfo;
						stDetectParamInfo.iImageIdx = iImageIdx;
						stDetectParamInfo.iTabIdx = iTabIdx;
						stDetectParamInfo.bBarrelLens = FALSE;
						stDetectParamInfo.AlgorithmParam = AlgorithmParam;
						copy_obj(HDefectRgn, &stDetectParamInfo.HRegion, 1, -1);

						THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1][AlgorithmParam.m_iDefectNameIdx].push_back(stDetectParamInfo);
					}
					//THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, FALSE, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, iModuleNo, pAlgorithm->m_sBarcodeResult, iImageIdx, iTabIdx, AlgorithmParam);
				}


				// FAI 치수 측정	
				if (pInspectROIRgn->m_dLineStartX >= 0 && pInspectROIRgn->m_dLineEndX >= 0)
				{
					CString strLog;

					if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_INNER_CIRCLE)
					{
						// FAI-51
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dInnerChamferCircleX[51][pInspectROIRgn->m_iFAIInnerCircleID - 1] = (pInspectROIRgn->m_dLineStartX[iModuleNo - 1] + pInspectROIRgn->m_dLineEndX[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dInnerChamferCircleY[51][pInspectROIRgn->m_iFAIInnerCircleID - 1] = (pInspectROIRgn->m_dLineStartY[iModuleNo - 1] + pInspectROIRgn->m_dLineEndY[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dInnerChamferCircleES[51][pInspectROIRgn->m_iFAIInnerCircleID - 1] = pInspectROIRgn->m_dEdgeStrength[iModuleNo - 1];

						// FAI-26
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dInnerChamferCircleX[26][pInspectROIRgn->m_iFAIInnerCircleID - 1] = (pInspectROIRgn->m_dLineStartX[iModuleNo - 1] + pInspectROIRgn->m_dLineEndX[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dInnerChamferCircleY[26][pInspectROIRgn->m_iFAIInnerCircleID - 1] = (pInspectROIRgn->m_dLineStartY[iModuleNo - 1] + pInspectROIRgn->m_dLineEndY[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dInnerChamferCircleES[26][pInspectROIRgn->m_iFAIInnerCircleID - 1] = pInspectROIRgn->m_dEdgeStrength[iModuleNo - 1];

						// FAI-14
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dInnerChamferCircleX[14][pInspectROIRgn->m_iFAIInnerCircleID - 1] = (pInspectROIRgn->m_dLineStartX[iModuleNo - 1] + pInspectROIRgn->m_dLineEndX[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dInnerChamferCircleY[14][pInspectROIRgn->m_iFAIInnerCircleID - 1] = (pInspectROIRgn->m_dLineStartY[iModuleNo - 1] + pInspectROIRgn->m_dLineEndY[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dInnerChamferCircleES[14][pInspectROIRgn->m_iFAIInnerCircleID - 1] = pInspectROIRgn->m_dEdgeStrength[iModuleNo - 1];
					}
					else if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_OUTER_CIRCLE)
					{
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dOuterPrismCircleX[51][pInspectROIRgn->m_iFAIOuterCircleID - 1] = (pInspectROIRgn->m_dLineStartX[iModuleNo - 1] + pInspectROIRgn->m_dLineEndX[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dOuterPrismCircleY[51][pInspectROIRgn->m_iFAIOuterCircleID - 1] = (pInspectROIRgn->m_dLineStartY[iModuleNo - 1] + pInspectROIRgn->m_dLineEndY[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dOuterPrismCircleES[51][pInspectROIRgn->m_iFAIOuterCircleID - 1] = pInspectROIRgn->m_dEdgeStrength[iModuleNo - 1];
					}
					else if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_CIRCLE)
					{
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dDatumCircleX[26][pInspectROIRgn->m_iFAIDatumCircleID - 1] = (pInspectROIRgn->m_dLineStartX[iModuleNo - 1] + pInspectROIRgn->m_dLineEndX[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dDatumCircleY[26][pInspectROIRgn->m_iFAIDatumCircleID - 1] = (pInspectROIRgn->m_dLineStartY[iModuleNo - 1] + pInspectROIRgn->m_dLineEndY[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dDatumCircleES[26][pInspectROIRgn->m_iFAIDatumCircleID - 1] = pInspectROIRgn->m_dEdgeStrength[iModuleNo - 1];
					}
					else if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_DATUM_ELLIPSE)
					{
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dDatumEllipseX[26][pInspectROIRgn->m_iFAIDatumEllipseID - 1] = (pInspectROIRgn->m_dLineStartX[iModuleNo - 1] + pInspectROIRgn->m_dLineEndX[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dDatumEllipseY[26][pInspectROIRgn->m_iFAIDatumEllipseID - 1] = (pInspectROIRgn->m_dLineStartY[iModuleNo - 1] + pInspectROIRgn->m_dLineEndY[iModuleNo - 1]) * 0.50000000;
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dDatumEllipseES[26][pInspectROIRgn->m_iFAIDatumEllipseID - 1] = pInspectROIRgn->m_dEdgeStrength[iModuleNo - 1];
					}
					else if (pInspectROIRgn->m_iInspectionType == INSPECTION_TYPE_FAI_BRACKET)
					{
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dBracketLineStartX[123][pInspectROIRgn->m_iFAIBracketID - 1] = pInspectROIRgn->m_dLineStartX[iModuleNo - 1];
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dBracketLineStartY[123][pInspectROIRgn->m_iFAIBracketID - 1] = pInspectROIRgn->m_dLineStartY[iModuleNo - 1];
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dBracketLineEndX[123][pInspectROIRgn->m_iFAIBracketID - 1] = pInspectROIRgn->m_dLineEndX[iModuleNo - 1];
						THEAPP.m_StructFaiMeasure[THEAPP.m_pInspectService->m_iTrayNo_H - 1][iModuleNo - 1].m_dBracketLineEndY[123][pInspectROIRgn->m_iFAIBracketID - 1] = pInspectROIRgn->m_dLineEndY[iModuleNo - 1];
					}
				}
			}
		}
	}

	int iExImageStartNo = 0;
	if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
		iExImageStartNo = BARREL_LENS_IMAGE_TAB;

	for (int iImageIdx = iExImageStartNo; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
	{
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HdefectExtra[iImageIdx]) == TRUE)
			union1(pAlgorithm->m_HdefectExtra[iImageIdx], &pAlgorithm->m_HdefectExtra[iImageIdx]);
	}

	if (THEAPP.m_pGFunction->ValidHRegion(HFoundInspectAreaRgn) == TRUE)
	{
		concat_obj(pAlgorithm->m_HROIInspectAreaRgn, HFoundInspectAreaRgn, &(pAlgorithm->m_HROIInspectAreaRgn));
		union1(pAlgorithm->m_HROIInspectAreaRgn, &pAlgorithm->m_HROIInspectAreaRgn);
	}

	if (THEAPP.Struct_PreferenceStruct.m_bDxLogDetail == TRUE)
	{
		dTEnd_depart = GetTickCount();
		sInspectTime.Format("  *-- ROI 검출 %.0lf ms", dTEnd_depart - dTStart_depart);
		THEAPP.SaveDetectLog(sInspectTime);
		THEAPP.SaveLog(sInspectTime);
	}
	
	// *********** FAI 계산 *********** 
	pAlgorithm->InspectFAI(THEAPP.m_pInspectService->m_iTrayNo_H, THEAPP.m_pInspectService->m_iIndexNo_H, iModuleNo);
	// *********** FAI 계산 ***********

	////////////////////////////////////////////////////
	//////////			배럴/렌즈 검사
	////////////////////////////////////////////////////
	if (THEAPP.m_pInspectSummary->m_bUseBLInsp)
	{	
		if (THEAPP.m_iMachineInspType != MACHINE_WELDING)
		{
			if (THEAPP.Struct_PreferenceStruct.m_bDxLogDetail == TRUE) dTStart_depart = GetTickCount();

			gen_empty_obj(&HEpoxyDontCareRgn);

			for (iImageIdx = 0; iImageIdx < BARREL_LENS_IMAGE_TAB; iImageIdx++)
			{
				for (iTabIdx = 0; iTabIdx < MAX_TEACHING_TAB; iTabIdx++)
				{
					AlgorithmParam = THEAPP.m_pModelDataManager->m_AlgorithmParam[iImageIdx][iTabIdx];

					if (AlgorithmParam.m_bInspect)
					{
						if (AlgorithmParam.m_iDefectNameIdx == DEFECT_NAME_EPOXY)
						{
							if (iImageIdx != 0)
								continue;

							gen_empty_obj(&HEpoxyCandidateRgn);
							HDefectRgn = pAlgorithm->CommonAlgorithm(pAlgorithm->m_HAlgoInspectImage[iImageIdx], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_1], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_2], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_3], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_4], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_5], AlgorithmParam, &HEpoxyCandidateRgn, &(pAlgorithm->m_HAlgoInspectImage[TRIGGER_EDGE]));

							if (THEAPP.m_pGFunction->ValidHRegion(HEpoxyCandidateRgn) == TRUE)
								concat_obj(HEpoxyDontCareRgn, HEpoxyCandidateRgn, &HEpoxyDontCareRgn);
						}
						else
							HDefectRgn = pAlgorithm->CommonAlgorithm(pAlgorithm->m_HAlgoInspectImage[iImageIdx], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_1], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_2], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_3], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_4], pAlgorithm->InspectContour[iImageIdx][MATCHING_CONTOUR_5], AlgorithmParam, &HEpoxyDontCareRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
						{
							// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW START
							if (AlgorithmParam.m_bSpecialNG)
								pAlgorithm->m_bSpecialNGSortFlag = TRUE;
							// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW END

							switch (AlgorithmParam.m_iDefectNameIdx)
							{
							case DEFECT_NAME_EPOXY:
								concat_obj(pAlgorithm->m_HDefectEpoxyRgn, HDefectRgn, &(pAlgorithm->m_HDefectEpoxyRgn));
								concat_obj(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_DIRT:
								concat_obj(pAlgorithm->m_HDefectDirtRgn, HDefectRgn, &(pAlgorithm->m_HDefectDirtRgn));
								concat_obj(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_SCRATCH:
								concat_obj(pAlgorithm->m_HDefectScratchRgn, HDefectRgn, &(pAlgorithm->m_HDefectScratchRgn));
								concat_obj(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_STAIN:
								concat_obj(pAlgorithm->m_HDefectStainRgn, HDefectRgn, &(pAlgorithm->m_HDefectStainRgn));
								concat_obj(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_DENT:
								concat_obj(pAlgorithm->m_HDefectDentRgn, HDefectRgn, &(pAlgorithm->m_HDefectDentRgn));
								concat_obj(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_CHIPPING:
								concat_obj(pAlgorithm->m_HDefectChippingRgn, HDefectRgn, &(pAlgorithm->m_HDefectChippingRgn));
								concat_obj(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_OUTSIDE_CT:
								concat_obj(pAlgorithm->m_HDefectOutsideCTRgn, HDefectRgn, &(pAlgorithm->m_HDefectOutsideCTRgn));
								concat_obj(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_PEEL_OFF:
								concat_obj(pAlgorithm->m_HDefectPeelOffRgn, HDefectRgn, &(pAlgorithm->m_HDefectPeelOffRgn));
								concat_obj(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_WHITEDOT:
								concat_obj(pAlgorithm->m_HDefectWhiteDotRgn, HDefectRgn, &(pAlgorithm->m_HDefectWhiteDotRgn));
								concat_obj(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_LENS_CONTAMINATION:
								concat_obj(pAlgorithm->m_HDefectLensCTRgn, HDefectRgn, &(pAlgorithm->m_HDefectLensCTRgn));
								concat_obj(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_LENS_SCRATCH:
								concat_obj(pAlgorithm->m_HDefectLensScratchRgn, HDefectRgn, &(pAlgorithm->m_HDefectLensScratchRgn));
								concat_obj(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_LENS_DIRT:
								concat_obj(pAlgorithm->m_HDefectLensDirtRgn, HDefectRgn, &(pAlgorithm->m_HDefectLensDirtRgn));
								concat_obj(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							case DEFECT_NAME_LENS_WHITEDOT:
								concat_obj(pAlgorithm->m_HDefectLensWhiteDotRgn, HDefectRgn, &(pAlgorithm->m_HDefectLensWhiteDotRgn));
								concat_obj(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx], HDefectRgn, &(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx]));
								concat_obj(pAlgorithm->m_HImageAIDefectRgn[iImageIdx], HDefectRgn, &pAlgorithm->m_HImageAIDefectRgn[iImageIdx]); // AI 검사 - LeeGW
								break;
							}

							// 파라미터 정보 저장 - LeeGW
							if (THEAPP.Struct_PreferenceStruct.m_bSaveDetectParamLog)
							{
								DetectParamInfoStruct stDetectParamInfo;
								stDetectParamInfo.iImageIdx = iImageIdx;
								stDetectParamInfo.iTabIdx = iTabIdx;
								stDetectParamInfo.bBarrelLens = TRUE;
								stDetectParamInfo.AlgorithmParam = AlgorithmParam;
								copy_obj(HDefectRgn, &stDetectParamInfo.HRegion, 1, -1);

								THEAPP.m_vDetectParamInfo[THEAPP.m_pInspectService->m_iTrayNo_H-1][iModuleNo-1][AlgorithmParam.m_iDefectNameIdx].push_back(stDetectParamInfo);
							}		
							//THEAPP.SaveDefectParamLog(THEAPP.Struct_PreferenceStruct.m_bSaveLasLog, TRUE, THEAPP.m_pInspectService->m_sLotID_H, THEAPP.m_pInspectService->m_iTrayNo_H, iModuleNo, pAlgorithm->m_sBarcodeResult, iImageIdx, iTabIdx, AlgorithmParam);
						}
					}
				}
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HEpoxyDontCareRgn)==TRUE)
			union1(HEpoxyDontCareRgn, &HEpoxyDontCareRgn);
	
		// 에폭시
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectEpoxyRgn) == TRUE)
			union1(pAlgorithm->m_HDefectEpoxyRgn, &pAlgorithm->m_HDefectEpoxyRgn);

		// Dirt 이물
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectDirtRgn) == TRUE)
			union1(pAlgorithm->m_HDefectDirtRgn, &pAlgorithm->m_HDefectDirtRgn);

		// Scratch
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectScratchRgn) == TRUE)
			union1(pAlgorithm->m_HDefectScratchRgn, &pAlgorithm->m_HDefectScratchRgn);

		// Stain 얼룩
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectStainRgn) == TRUE)
			union1(pAlgorithm->m_HDefectStainRgn, &pAlgorithm->m_HDefectStainRgn);

		// Dent
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectDentRgn) == TRUE)
			union1(pAlgorithm->m_HDefectDentRgn, &pAlgorithm->m_HDefectDentRgn);

		// Chipping
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectChippingRgn) == TRUE)
			union1(pAlgorithm->m_HDefectChippingRgn, &pAlgorithm->m_HDefectChippingRgn);

		// 외곽오염
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectOutsideCTRgn) == TRUE)
			union1(pAlgorithm->m_HDefectOutsideCTRgn, &pAlgorithm->m_HDefectOutsideCTRgn);

		// Peel Off
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectPeelOffRgn) == TRUE)
			union1(pAlgorithm->m_HDefectPeelOffRgn, &pAlgorithm->m_HDefectPeelOffRgn);

		// White Dot
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectWhiteDotRgn) == TRUE)
			union1(pAlgorithm->m_HDefectWhiteDotRgn, &pAlgorithm->m_HDefectWhiteDotRgn);

		// 렌즈 오염
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensCTRgn) == TRUE)
			union1(pAlgorithm->m_HDefectLensCTRgn, &(pAlgorithm->m_HDefectLensCTRgn));

		// 렌즈 스크래치
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensScratchRgn) == TRUE)
			union1(pAlgorithm->m_HDefectLensScratchRgn, &(pAlgorithm->m_HDefectLensScratchRgn));

		// 렌즈 이물
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensDirtRgn) == TRUE)
			union1(pAlgorithm->m_HDefectLensDirtRgn, &(pAlgorithm->m_HDefectLensDirtRgn));

		// 렌즈 White Dot
		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensWhiteDotRgn) == TRUE)
			union1(pAlgorithm->m_HDefectLensWhiteDotRgn, &(pAlgorithm->m_HDefectLensWhiteDotRgn));
	
		if (THEAPP.Struct_PreferenceStruct.m_bDxLogDetail == TRUE)
		{
			dTEnd_depart = GetTickCount();
			sInspectTime.Format("  *-- 베럴/렌즈 검출 %.0lf ms", dTEnd_depart - dTStart_depart);
			THEAPP.SaveDetectLog(sInspectTime);
			THEAPP.SaveLog(sInspectTime);
		}

		////////////////////////////////////////////////////
		//////////			불량 영역 정리
		////////////////////////////////////////////////////
		if (THEAPP.Struct_PreferenceStruct.m_bDxLogDetail == TRUE) dTStart_depart = GetTickCount();

		Hobject HAllDefectRgn_SE[MAX_IMAGE_TAB], HAllDefectRgn_LENS[MAX_IMAGE_TAB];	// 24.05.16 - v2602 - 이미지별 불량 표기를 위해 배열로 변경 - LeeGW
		for (iImageIdx = 0; iImageIdx < MAX_IMAGE_TAB; iImageIdx++)
		{
			gen_empty_obj(&HAllDefectRgn_SE[iImageIdx]);
			gen_empty_obj(&(HAllDefectRgn_LENS[iImageIdx]));
		}

		// 24.06.07 - v2647 - 이미지별 불량 표기를 위해 for문 추가, EachImaege[idx]로 변경 - LeeGW
		for (iImageIdx=0; iImageIdx< MAX_IMAGE_TAB; iImageIdx++)
		{
			if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
				break;

			// Dirt 이물
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx])) {
				move_region (pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx], 1, 1);				// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);				// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx]));						// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// Scratch
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx])) {
				move_region (pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx], 1, 1);		// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);			// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx]));					// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// Stain 얼룩
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx])) {
				move_region (pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx], 1, 1);			// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);				// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx]));						// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// Dent
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx])) {
				move_region (pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx], 1, 1);				// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);				// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx]));						// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// Chipping
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx])) {
				move_region (pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx], 1, 1);		// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);			// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx]));				// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// Epoxy
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx])) {
				move_region (pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx], 1, 1);		// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);			// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx]));				// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// 외곽오염
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx])) {
				move_region (pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx], 1, 1);		// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);			// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx]));				// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// Peel Off
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx])) {
				move_region (pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx], 1, 1);		// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);			// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx]));				// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// White Dot
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx])) {
				move_region (pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx], 1, 1);		// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);			// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx]));				// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// 렌즈 오염
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx])) {
				move_region(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], 1, 1);		// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_LENS[iImageIdx], pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], &HAllDefectRgn_LENS[iImageIdx]);			// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx]));					// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// 렌즈 스크래치
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx])) {
				move_region(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx], 1, 1);	// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_LENS[iImageIdx], pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx], &HAllDefectRgn_LENS[iImageIdx]);		// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx]));			// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// 렌즈 이물
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx])) {
				move_region(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx], 1, 1);	// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_LENS[iImageIdx], pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx], &HAllDefectRgn_LENS[iImageIdx]);			// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx]));				// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			// White dot
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx])) {
				move_region(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx], &pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx], 1, 1);	// 올바른 디스플레이를 위해 shift
				concat_obj(HAllDefectRgn_LENS[iImageIdx], pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx], &HAllDefectRgn_LENS[iImageIdx]);		// 전체 defect 영역에 더한다
				union1(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx]));			// InspectionService:ResultSaveThread에서 처리 위해 따로 저장
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HAllDefectRgn_SE[iImageIdx]))			// 베럴 defect 전체
				union1(HAllDefectRgn_SE[iImageIdx], &HAllDefectRgn_SE[iImageIdx]);

			if (THEAPP.m_pGFunction->ValidHRegion(HAllDefectRgn_LENS[iImageIdx]))
				union1(HAllDefectRgn_LENS[iImageIdx], &HAllDefectRgn_LENS[iImageIdx]);		// 렌즈 defect 전체
		}
		// 24.06.07 - v2647 - 이미지별 불량 표기를 위해 for문 추가, EachImaege[idx]로 변경 - LeeGW End

		if (THEAPP.Struct_PreferenceStruct.m_bDxLogDetail == TRUE)
		{
			dTEnd_depart = GetTickCount();
			sInspectTime.Format("  *-- 불량 정리 %.0lf ms", dTEnd_depart-dTStart_depart);
			THEAPP.SaveDetectLog(sInspectTime);
			THEAPP.SaveLog(sInspectTime);
		}

		///////////////////////////////////////////////////////////////////////////////////////defect 연결

		for (iImageIdx=0; iImageIdx< MAX_IMAGE_TAB; iImageIdx++)
		{
			if (iImageIdx >= THEAPP.m_iMaxInspImageNo)
				break;

			concat_obj(HAllDefectRgn_SE[iImageIdx], pAlgorithm->m_HdefectBarrel, &pAlgorithm->m_HdefectBarrel);
			concat_obj(HAllDefectRgn_LENS[iImageIdx], pAlgorithm->m_HdefectLens, &pAlgorithm->m_HdefectLens);
			concat_obj(HAllDefectRgn_SE[iImageIdx], HAllDefectRgn_LENS[iImageIdx], &pAlgorithm->m_HdefectBarrelLens[iImageIdx]);

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HdefectBarrelLens[iImageIdx]))
				union1(pAlgorithm->m_HdefectBarrelLens[iImageIdx], &(pAlgorithm->m_HdefectBarrelLens[iImageIdx]));
		}

		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HdefectBarrel))
			union1(pAlgorithm->m_HdefectBarrel, &(pAlgorithm->m_HdefectBarrel));

		if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HdefectLens))
			union1(pAlgorithm->m_HdefectLens, &(pAlgorithm->m_HdefectLens));


		//////////////////////////////////////////////////////////////////////////
		// 각 검사 영상에서의 불량 영역
		for (iImageIdx=0; iImageIdx<MAX_IMAGE_TAB; iImageIdx++)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectDirtRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectScratchRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectStainRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectDentRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectChippingRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectEpoxyRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectOutsideCTRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectPeelOffRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectWhiteDotRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensDirtRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensScratchRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensCTRgn_EachImage[iImageIdx]), 1, 1);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx])) 
			{
				union1(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx]));
				move_region (pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx], &(pAlgorithm->m_HDefectLensWhiteDotRgn_EachImage[iImageIdx]), 1, 1);
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}
	pAlgorithm->ThreadFinishFlag = 1;

	tDetectEnd_total = GetTickCount();
	sInspectTime.Format("* Module %2d Defect 검출 시간 thread total: %.0lf ms", iModuleNo, tDetectEnd_total - tDetectStart_total);
	THEAPP.SaveDetectLog(sInspectTime);
	THEAPP.SaveLog(sInspectTime);

	THEAPP.m_InspectTime[iModuleNo] = (tDetectEnd_total - tDetectStart_total) / 1000.0;

	// 24.07.23 SW 다운 확인 - LeeGW 
	if (THEAPP.Struct_PreferenceStruct.m_bDxLogDetail == TRUE)
	{
		try
		{
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memInfo);

			DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;  // 사용 중인 물리 메모리
			double physMemUsedMegaByte = (double)physMemUsed / (1024 * 1024);	// 현재 프로세스가 사용중인 메모리
			double physMemUsedPercent = ((double)physMemUsed / (double)memInfo.ullTotalPhys) * 100;

			CString strMemLog;
			strMemLog.Format("Memory in Use End Insp Thread : %.2fMB (%.2f/100.00)", physMemUsedMegaByte, physMemUsedPercent);
			THEAPP.SaveLog(strMemLog);
		}
		catch (HException &except)
		{
			THEAPP.SaveLog("Memory Check Failure!");
		}
	}

	return 0;
}

void Algorithm::CopyInspectInfomation(Hobject *HImageList, Hobject* pHBarcodeImage, Hobject *HContourList, HTuple *HModleIDList, HTuple* pHInspectAlignModelID)
{
	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		m_HAlgoInspectImage[i].Reset();
		gen_empty_obj(&m_HAlgoInspectImage[i]);
	}

	for (int i = 0; i < MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		copy_image(HImageList[i], &m_HAlgoInspectImage[i]);
	}

	for (int i=0; i < BARREL_LENS_IMAGE_TAB; i++)
	{
		for (int j = 0; j < MAX_CONTOUR_NUM; j++)
		{
			InspectContour[i][j].Reset();
			gen_empty_obj(&InspectContour[i][j]);

		}
	}

	InspectBarcodeImage.Reset();
	gen_empty_obj(&InspectBarcodeImage);
	if (THEAPP.m_pGFunction->ValidHImage(*pHBarcodeImage))
		copy_image(*pHBarcodeImage, &InspectBarcodeImage);

	for (int i = 0; i < MAX_MATCHING_MODEL_NUM; i++)
	{
		InspectModelID[i] = HModleIDList[i];
	}

	for (int i=0; i< MAX_IMAGE_TAB; i++)
	{
		if (i >= THEAPP.m_iMaxInspImageNo)
			break;

		if (THEAPP.m_pModelDataManager->m_bUseLocalAlignMatching[i])
			m_HInspectLAlignModelID[i] = pHInspectAlignModelID[i];
		else
			m_HInspectLAlignModelID[i] = -1;
	}

	for (int i = 0; i < BARREL_LENS_IMAGE_TAB; i++)
	{
		for (int j = 0; j < BARREL_LENS_IMAGE_TAB; j++)
		{
			m_bMatchingContourSuccess[i][j] = FALSE;
		}
	}

	ThreadFinishFlag = 0;
	m_bMatchingSuccess = FALSE;
	m_bModuleEmpty = FALSE;
	m_bLightDisorder = FALSE;
	m_bBarcodeError = FALSE;
	m_bBarcodeShiftNG = FALSE;
	m_bBarcodeModuleMixNG = FALSE;
	m_bBlackCoatingDiameterNG = FALSE;
	m_bDisplayed = FALSE;

	m_bTotalMatchingSuccess = TRUE;	// 24.07.01 - v2650 - 전체 이미지 매칭 결과 확인 변수 추가 - LeeGW
	m_iBlackMatchingImageType = THEAPP.m_pModelDataManager->m_iBlackMatchingImageType;		// 24.07.01 - v2650 - 전체 이미지 매칭 결과 확인 변수 추가 - LeeGW

	m_bSpecialNGSortFlag = FALSE;	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW
	m_bAISpecialNGSortFlag = FALSE;// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

	AfxBeginThread(InspectThreadAlgorithm, this);
}


void Algorithm::Init_AlgorithmInspectParam()
{
	ThreadFinishFlag = 0;
	MaxDefectSize = 200;
	DefectSize = 50;
	WhiteChppingDefectSensitive = 50;
	BlackChppingDefectSensitive = 50;
	Distance_referValue = 25;
	m_pCheckFinishConnection = FALSE;

	m_sBarcodeResult = _T("");
	m_dBarcodePosOffsetX = 999999;
	m_dBarcodePosOffsetY = 999999;
	m_dBarcodePosOffsetRotation = 999999; //Barcode Rotation Log

	m_bBarcodeError = FALSE;
}

BOOL Algorithm::InspectBarcode(Hobject *pHBarcodeImage, CString *psBarcodeResult, double *pdOffsetX, double *pdOffsetY, double *pdOffsetRotation, double dBarcodeShiftRefX, double dBarcodeShiftRefY ) //Barcode Rotation Inspection modify
{
	*pdOffsetX = 9999999;
	*pdOffsetY = 9999999;

	if (THEAPP.m_pGFunction->ValidHImage(*pHBarcodeImage)==FALSE)
	{
		*psBarcodeResult = _T("NOGRAB");
		return FALSE;
	}

	HTuple DataCodeHandle, ResultHandles, DecodedDataStrings;
	Hobject SymbolXLDs, HBarcodeDetectRgn, HBarcodeROIImage, HFilteredImage;

	Hlong lArea;
	double dCenterY, dCenterX;

	HTuple HStrLength;
	int iLength;

	int iNoBarcodeString = 0;
	Hobject HBrightRgn;
	BOOL bFirstSuccess = FALSE;

	create_data_code_2d_model(HTuple("Data Matrix ECC 200"),HTuple(),HTuple(),&DataCodeHandle);

	BOOL bFindBarcodeData = FALSE;
	try
	{
		// Standard Edition
		set_data_code_2d_param(DataCodeHandle,HTuple("default_parameters"),HTuple("standard_recognition"));

		crop_part(*pHBarcodeImage, &HBarcodeROIImage, THEAPP.m_pModelDataManager->m_iBarcodeLTPointY, THEAPP.m_pModelDataManager->m_iBarcodeLTPointX,
			THEAPP.m_pModelDataManager->m_iBarcodeRBPointY, THEAPP.m_pModelDataManager->m_iBarcodeRBPointX);

		SymbolXLDs.Reset();
		find_data_code_2d(HBarcodeROIImage,&SymbolXLDs,DataCodeHandle,HTuple(),HTuple(),&ResultHandles,&DecodedDataStrings);
		iNoBarcodeString = DecodedDataStrings.Num();
		if(iNoBarcodeString > 0)
		{
			tuple_strlen(DecodedDataStrings, &HStrLength);
			iLength = HStrLength[0].I();

			if (iLength>=10)
			{
				bFindBarcodeData = TRUE;
				bFirstSuccess = TRUE;
			}
		}

		if (bFindBarcodeData==FALSE)
		{
			emphasize(HBarcodeROIImage, &HFilteredImage, 7, 7, 1);

			SymbolXLDs.Reset();
			find_data_code_2d(HFilteredImage,&SymbolXLDs,DataCodeHandle,HTuple(),HTuple(),&ResultHandles,&DecodedDataStrings);
			iNoBarcodeString = DecodedDataStrings.Num();
			if(iNoBarcodeString > 0)
			{
				tuple_strlen(DecodedDataStrings, &HStrLength);
				iLength = HStrLength[0].I();

				if (iLength>=10)
				{
					bFindBarcodeData = TRUE;
					bFirstSuccess = TRUE;
				}
			}
		}

		if (bFindBarcodeData==FALSE)
		{
			median_image (HBarcodeROIImage, &HFilteredImage, "circle", 2, "mirrored");

			SymbolXLDs.Reset();
			find_data_code_2d(HFilteredImage,&SymbolXLDs,DataCodeHandle,HTuple(),HTuple(),&ResultHandles,&DecodedDataStrings);
			iNoBarcodeString = DecodedDataStrings.Num();
			if(iNoBarcodeString > 0)
			{
				tuple_strlen(DecodedDataStrings, &HStrLength);
				iLength = HStrLength[0].I();

				if (iLength>=10)
				{
					bFindBarcodeData = TRUE;
					bFirstSuccess = TRUE;
				}
			}
		}

		if (bFindBarcodeData==FALSE)
		{
			SymbolXLDs.Reset();
			find_data_code_2d(*pHBarcodeImage,&SymbolXLDs,DataCodeHandle,HTuple(),HTuple(),&ResultHandles,&DecodedDataStrings);
			iNoBarcodeString = DecodedDataStrings.Num();
			if(iNoBarcodeString > 0)
			{
				tuple_strlen(DecodedDataStrings, &HStrLength);
				iLength = HStrLength[0].I();

				if (iLength>=10)
				{
					bFindBarcodeData = TRUE;
				}
			}
		}

		if (bFindBarcodeData==FALSE)
		{
			zoom_image_factor(*pHBarcodeImage, &HFilteredImage, 0.98, 0.98, "constant");

			SymbolXLDs.Reset();
			find_data_code_2d(HFilteredImage,&SymbolXLDs,DataCodeHandle,HTuple(),HTuple(),&ResultHandles,&DecodedDataStrings);
			iNoBarcodeString = DecodedDataStrings.Num();
			if(iNoBarcodeString > 0)
			{
				tuple_strlen(DecodedDataStrings, &HStrLength);
				iLength = HStrLength[0].I();

				if (iLength>=10)
				{
					bFindBarcodeData = TRUE;
				}
			}
		}

		if (bFindBarcodeData==FALSE)		// Enhanced Edition
		{
			set_data_code_2d_param(DataCodeHandle,HTuple("default_parameters"),HTuple("enhanced_recognition"));

			SymbolXLDs.Reset();
			find_data_code_2d(HBarcodeROIImage,&SymbolXLDs,DataCodeHandle,HTuple(),HTuple(),&ResultHandles,&DecodedDataStrings);
			iNoBarcodeString = DecodedDataStrings.Num();
			if(iNoBarcodeString > 0)
			{
				tuple_strlen(DecodedDataStrings, &HStrLength);
				iLength = HStrLength[0].I();

				if (iLength>=10)
				{
					bFindBarcodeData = TRUE;
					bFirstSuccess = TRUE;
				}
			}

			if (bFindBarcodeData==FALSE)
			{
				emphasize(HBarcodeROIImage, &HFilteredImage, 7, 7, 1);

				SymbolXLDs.Reset();
				find_data_code_2d(HFilteredImage,&SymbolXLDs,DataCodeHandle,HTuple(),HTuple(),&ResultHandles,&DecodedDataStrings);
				iNoBarcodeString = DecodedDataStrings.Num();
				if(iNoBarcodeString > 0)
				{
					tuple_strlen(DecodedDataStrings, &HStrLength);
					iLength = HStrLength[0].I();

					if (iLength>=10)
					{
						bFindBarcodeData = TRUE;
						bFirstSuccess = TRUE;
					}
				}
			}

			if (bFindBarcodeData==FALSE)
			{
				median_image (HBarcodeROIImage, &HFilteredImage, "circle", 2, "mirrored");

				SymbolXLDs.Reset();
				find_data_code_2d(HFilteredImage,&SymbolXLDs,DataCodeHandle,HTuple(),HTuple(),&ResultHandles,&DecodedDataStrings);
				iNoBarcodeString = DecodedDataStrings.Num();
				if(iNoBarcodeString > 0)
				{
					tuple_strlen(DecodedDataStrings, &HStrLength);
					iLength = HStrLength[0].I();

					if (iLength>=10)
					{
						bFindBarcodeData = TRUE;
						bFirstSuccess = TRUE;
					}
				}
			}

			if (bFindBarcodeData==FALSE)
			{
				SymbolXLDs.Reset();
				find_data_code_2d(*pHBarcodeImage,&SymbolXLDs,DataCodeHandle,HTuple(),HTuple(),&ResultHandles,&DecodedDataStrings);
				iNoBarcodeString = DecodedDataStrings.Num();
				if(iNoBarcodeString > 0)
				{
					tuple_strlen(DecodedDataStrings, &HStrLength);
					iLength = HStrLength[0].I();

					if (iLength>=10)
					{
						bFindBarcodeData = TRUE;
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////

		if (bFindBarcodeData==TRUE)
		{
			gen_region_contour_xld(SymbolXLDs, &HBarcodeDetectRgn, "filled");

			if (bFirstSuccess)
				move_region(HBarcodeDetectRgn, &HBarcodeDetectRgn, THEAPP.m_pModelDataManager->m_iBarcodeLTPointY, THEAPP.m_pModelDataManager->m_iBarcodeLTPointX);

			//Barcode Hole Center Start //if문 추가
			if( !THEAPP.m_pModelDataManager->m_bUseBarcodeShiftHoleCenter)
			{
				area_center(HBarcodeDetectRgn, &lArea, &dCenterY, &dCenterX);

				*pdOffsetX = (dCenterX - (double)PGCAM_IMAGE_WIDTH*0.5) * PGCAM_PIXEL_RESOLUTION - dBarcodeShiftRefX;
				*pdOffsetY = ((double)PGCAM_IMAGE_HEIGHT*0.5 - dCenterY) * PGCAM_PIXEL_RESOLUTION - dBarcodeShiftRefY;
			}
			else
			{
				try
				{
					double dBarcodeCenterX,dBarcodeCenterY,dHoleCenterX,dHoleCenterY;
					Hobject HHoleSearchRectRgn;
					Hobject HTempImage;
					Hobject HTempHoleRgn;
					gen_rectangle1(&HHoleSearchRectRgn,THEAPP.m_pModelDataManager->m_iBarcodeLTPointY,THEAPP.m_pModelDataManager->m_iBarcodeLTPointX,THEAPP.m_pModelDataManager->m_iBarcodeRBPointY,THEAPP.m_pModelDataManager->m_iBarcodeRBPointX);
					dilation_circle(HHoleSearchRectRgn,&HHoleSearchRectRgn,THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterDilation+0.5);
					HTempImage = *pHBarcodeImage;

					reduce_domain( HTempImage, HHoleSearchRectRgn, &HTempImage );
					threshold( HTempImage, &HTempHoleRgn, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMin, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvMax);
					fill_up(HTempHoleRgn,&HTempHoleRgn);
					connection( HTempHoleRgn, &HTempHoleRgn );
					select_shape_std(HTempHoleRgn,&HTempHoleRgn,"max_area", 70 );
					if( THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing > 0 )
						closing_circle( HTempHoleRgn, &HTempHoleRgn, THEAPP.m_pModelDataManager->m_iBarcodeShiftHoleCenterGvClosing + 0.5);

					area_center(HBarcodeDetectRgn, &lArea, &dBarcodeCenterY, &dBarcodeCenterX);
					area_center(HTempHoleRgn, &lArea, &dHoleCenterY, &dHoleCenterX);
					
					*pdOffsetX = (dBarcodeCenterX - dHoleCenterX)*PGCAM_PIXEL_RESOLUTION - dBarcodeShiftRefX;
					*pdOffsetY = (dBarcodeCenterY - dHoleCenterY)*PGCAM_PIXEL_RESOLUTION - dBarcodeShiftRefY;
				}
				catch(HException &except)
				{

				}
			}
			//Barcode Hole Center End

			//Barcode Rotation Inspection Start
			HTuple htTempRow, htTempCol, htTempPhi, htTempLength1 , htTempLength2, htDeg;
			smallest_rectangle2( HBarcodeDetectRgn, &htTempRow, &htTempCol, &htTempPhi, &htTempLength1 , &htTempLength2 );
			tuple_deg(htTempPhi, pdOffsetRotation);

			if( *pdOffsetRotation < -45 )
			{
				*pdOffsetRotation = 90 + *pdOffsetRotation;
			}
			else if(*pdOffsetRotation > 45 )
			{
				*pdOffsetRotation = 90 - *pdOffsetRotation;
			}

			*pdOffsetRotation = *pdOffsetRotation - THEAPP.m_pModelDataManager->m_dBarcodeShiftRefRotation;
			//Barcode Rotation Inspection End
		}
	}
	catch(HException& e)
	{
		bFindBarcodeData = FALSE;
	}

	clear_data_code_2d_model(DataCodeHandle);	
	HBarcodeROIImage.Reset();

	CString sBarcodeNoReadSaveFolderName = THEAPP.Struct_PreferenceStruct.m_strBasic_FolderPath+"\\CMI_Results\\Barcode_NoRead";
	CString sBarcodeFilename;

	if (bFindBarcodeData)
	{
		*psBarcodeResult = DecodedDataStrings[0].S();
		return TRUE;
	}
	else
	{
		*psBarcodeResult = _T("NOREAD");

		SYSTEMTIME time;
		GetLocalTime(&time);

		sBarcodeFilename.Format("%s\\Barcode_%04d%02d%02d_%02d%02d%02d_%03d", sBarcodeNoReadSaveFolderName, 
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

		//if (THEAPP.Struct_PreferenceStruct.m_iBarcodeNoReadImageType==0)
		//	write_image(*pHBarcodeImage, "bmp", 0, sBarcodeFilename);
		//else
		//	write_image(*pHBarcodeImage, "jpg", 0, sBarcodeFilename);
		
		if (THEAPP.Struct_PreferenceStruct.m_iBarcodeNoReadImageType == 0)
			THEAPP.m_pSaveManager->SaveImage(*pHBarcodeImage, "bmp", sBarcodeFilename);
		else
			THEAPP.m_pSaveManager->SaveImage(*pHBarcodeImage, "jpg", sBarcodeFilename);

		return FALSE;
	}

}

BOOL Algorithm::MatchingAlgorithm (Hobject HImage1, Hobject HImgRef, Hobject HImage2, Hobject HImage4,
	Hobject* Contour1, Hobject* Contour2, Hobject* Contour3, Hobject* Contour4, Hobject* Contour5,
	HTuple* ModelID1, HTuple* ModelID2, HTuple* ModelID3, HTuple* ModelID4, double *pdOutmostContourCenterX, double *pdOutmostContourCenterY)
{
try{
	Hobject HImgSub;
	HTuple  Row1, Column1, Angle1, ScaleR1, ScaleC1, Score1;
	HTuple  Row2, Column2, Angle2, ScaleR2, ScaleC2, Score2;
	HTuple  Row3, Column3, Angle3, ScaleR3, ScaleC3, Score3;
	HTuple  Row4, Column4, Angle4, ScaleR4, ScaleC4, Score4;
	HTuple  HomMat2DIdentity1,HomMat2DIdentity2,HomMat2DIdentity3,HomMat2DIdentity4;
	HTuple  HomMat2DTranslate1,HomMat2DTranslate2,HomMat2DTranslate3,HomMat2DTranslate4;
	HTuple  HomMat2DRotate1,HomMat2DRotate2,HomMat2DRotate3,HomMat2DRotate4;
	HTuple  HomMat2DScale1,HomMat2DScale2,HomMat2DScale3,HomMat2DScale4;
	BOOL	bMatchingSuccess[4];	// 1, 2, 3, 4
	Hlong	lNoFoundNumber;
	int i;

	for (i=0; i<BARREL_LENS_IMAGE_TAB; i++)
	{
		bMatchingSuccess[i] = FALSE;
	}

	if (THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)
	{
		get_shape_model_contours(&*Contour1, *ModelID1, 1);
		get_shape_model_contours(&*Contour2, *ModelID2, 1);
	}
	else
	{
		Contour1->Reset();
		Contour2->Reset();
	}

	get_shape_model_contours(&*Contour3, *ModelID3, 1);
	get_shape_model_contours(&*Contour4, *ModelID4, 1);

	double dAngleRangeRad;
	tuple_rad((double)THEAPP.m_pModelDataManager->m_iMatchingAngleRange, &dAngleRangeRad);

	if (THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)		// Double-Barrel
	{
		gen_empty_obj(&HImgSub);

		if (m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_BSURFACE)
			copy_obj (HImage1, &HImgSub, 1, 1);
		else if (m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_LENS1)
			copy_obj(HImgRef, &HImgSub, 1, 1);
		else if (m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_BEDGE)
			copy_obj(HImage2, &HImgSub, 1, 1);
		else if (m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_LENS2)
			copy_obj(HImage4, &HImgSub, 1, 1);
		
		if (THEAPP.m_pModelDataManager->m_bUseMatchingScale)
		{
			double dScaleMin = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMin) * 0.01;
			double dScaleMax = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMax) * 0.01;

			if (THEAPP.m_pModelDataManager->m_bUseMatchingAngle)
				find_scaled_shape_model(HImgSub, *ModelID1, -dAngleRangeRad, dAngleRangeRad*2.0, dScaleMin, dScaleMax, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row1, &Column1, &Angle1, &ScaleR1, &Score1);
			else
			{
				find_scaled_shape_model(HImgSub, *ModelID1, 0, 0, dScaleMin, dScaleMax, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row1, &Column1, &Angle1, &ScaleR1, &Score1);

				Angle1 = 0;
			}

			tuple_length(Score1, &lNoFoundNumber);
			if (lNoFoundNumber>0);
			{
				bMatchingSuccess[0] = TRUE;
				ScaleC1 = ScaleR1;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][0] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}

			find_scaled_shape_model(HImgSub, *ModelID3, 0, 0, 0.98, 1.02, 0.65, 1, 0.5, "interpolation", 0, 0.9, &Row3, &Column3, &Angle3, &ScaleR3, &Score3);
			tuple_length(Score3, &lNoFoundNumber);
			if (lNoFoundNumber>0)
			{
				bMatchingSuccess[2] = TRUE;
				ScaleC3 = ScaleR3;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][2] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}

			find_scaled_shape_model(HImgSub, *ModelID4, 0, 0, 0.98, 1.02, 0.65, 1, 0.5, "interpolation", 0, 0.9, &Row4, &Column4, &Angle4, &ScaleR4, &Score4);
			tuple_length(Score4, &lNoFoundNumber);
			if (lNoFoundNumber>0)
			{
				bMatchingSuccess[3] = TRUE;
				ScaleC4 = ScaleR4;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][3] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}
		}
		else
		{
			if (THEAPP.m_pModelDataManager->m_bUseMatchingAngle)
				find_shape_model(HImgSub, *ModelID1, -dAngleRangeRad, dAngleRangeRad*2.0, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row1, &Column1, &Angle1, &Score1);
			else
			{
				find_shape_model(HImgSub, *ModelID1, 0, 0, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row1, &Column1, &Angle1, &Score1);

				Angle1 = 0;
			}

			tuple_length(Score1, &lNoFoundNumber);
			if (lNoFoundNumber>0)
			{
				bMatchingSuccess[0] = TRUE;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][0] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}

			ScaleR1 = 1.0;
			ScaleC1 = 1.0;

			find_shape_model(HImgSub, *ModelID3, 0, 0, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row3, &Column3, &Angle3, &Score3);
			tuple_length(Score3, &lNoFoundNumber);
			if (lNoFoundNumber>0)
			{
				bMatchingSuccess[2] = TRUE;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][2] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}

			find_shape_model(HImgSub, *ModelID4, 0, 0, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row4, &Column4, &Angle4, &Score4);
			tuple_length(Score4, &lNoFoundNumber);
			if (lNoFoundNumber>0)
			{
				bMatchingSuccess[3] = TRUE;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][3] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}

			ScaleR3 = 1.0;
			ScaleC3 = 1.0;
			ScaleR4 = 1.0;
			ScaleC4 = 1.0;
		}

		if (!THEAPP.m_pInspectSummary->m_bUseAccelMatching)
		{
			if (THEAPP.m_pModelDataManager->m_bUseMatchingScale)
			{
				double dScaleMin = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMin) * 0.01;
				double dScaleMax = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMax) * 0.01;

				find_scaled_shape_model(HImgSub, *ModelID2, 0, 0, dScaleMin, dScaleMax, 0.65, 1, 0.5, "interpolation", 0, 0.9, &Row2, &Column2, &Angle2, &ScaleR2, &Score2);
				tuple_length(Score2, &lNoFoundNumber);
				if (lNoFoundNumber>0)
				{
					bMatchingSuccess[1] = TRUE;
					ScaleC2 = ScaleR2;
					m_bMatchingContourSuccess[m_iBlackMatchingImageType][1] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
				}
			}
			else
			{
				find_shape_model(HImgSub, *ModelID2, 0, 0, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row2, &Column2, &Angle2, &Score2);
				tuple_length(Score2, &lNoFoundNumber);
				if (lNoFoundNumber>0)
				{
					bMatchingSuccess[1] = TRUE;
					m_bMatchingContourSuccess[m_iBlackMatchingImageType][1] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
				}

				ScaleR2 = 1.0;
				ScaleC2 = 1.0;
			}
		}

	}
	else		// One-Barrel
	{
		gen_empty_obj(&HImgSub);

		if (m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_BSURFACE)
			copy_obj(HImage1, &HImgSub, 1, 1);
		else if (m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_LENS1)
			copy_obj(HImgRef, &HImgSub, 1, 1);
		else if (m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_BEDGE)
			copy_obj(HImage2, &HImgSub, 1, 1);
		else if (m_iBlackMatchingImageType==BLACK_MATCHING_IMAGE_TYPE_LENS2)
			copy_obj(HImage4, &HImgSub, 1, 1);

		if (THEAPP.m_pModelDataManager->m_bUseMatchingScale)
		{
			double dScaleMin = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMin) * 0.01;
			double dScaleMax = double(THEAPP.m_pModelDataManager->m_iMatchingScaleMax) * 0.01;

			if (THEAPP.m_pModelDataManager->m_bUseMatchingAngle)
				find_scaled_shape_model(HImgSub, *ModelID3, -dAngleRangeRad, dAngleRangeRad*2.0, dScaleMin, dScaleMax, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row3, &Column3, &Angle3, &ScaleR3, &Score3);
			else
			{
				find_scaled_shape_model(HImgSub, *ModelID3, 0, 0, dScaleMin, dScaleMax, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row3, &Column3, &Angle3, &ScaleR3, &Score3);

				Angle3 = 0;
			}

			tuple_length(Score3, &lNoFoundNumber);
			if (lNoFoundNumber>0)
			{
				bMatchingSuccess[2] = TRUE;
				ScaleC3 = ScaleR3;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][2] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}

			find_scaled_shape_model(HImgSub, *ModelID4, 0, 0, 0.98, 1.02, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row4, &Column4, &Angle4, &ScaleR4, &Score4);

			tuple_length(Score4, &lNoFoundNumber);
			if (lNoFoundNumber>0)
			{
				bMatchingSuccess[3] = TRUE;
				ScaleC4 = ScaleR4;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][3] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}

		}
		else
		{
			if (THEAPP.m_pModelDataManager->m_bUseMatchingAngle)
				find_shape_model(HImgSub, *ModelID3, -dAngleRangeRad, dAngleRangeRad*2.0, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row3, &Column3, &Angle3, &Score3);
			else
			{
				find_shape_model(HImgSub, *ModelID3, 0, 0, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row3, &Column3, &Angle3, &Score3);

				Angle3 = 0;
			}

			tuple_length(Score3, &lNoFoundNumber);
			if (lNoFoundNumber>0)
			{
				bMatchingSuccess[2] = TRUE;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][2] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}

			find_shape_model(HImgSub, *ModelID4, 0, 0, 0.5, 1, 0.5, "interpolation", 0, 0.9, &Row4, &Column4, &Angle4, &Score4);
			
			tuple_length(Score4, &lNoFoundNumber);
			if (lNoFoundNumber>0)
			{
				bMatchingSuccess[3] = TRUE;
				m_bMatchingContourSuccess[m_iBlackMatchingImageType][3] = TRUE;	// 24.07.01 - v2650 - ROI Shift 시 ROI 대체하기 위한 매칭 Score 추가 - LeeGW
			}

			ScaleR3 = 1.0;
			ScaleC3 = 1.0;
			ScaleR4 = 1.0;
			ScaleC4 = 1.0;

		}

	}

	// Decision

	int iTransModelIndex = -1;

	if(THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)
	{
		for (i=0; i<4; i++)
		{
			if (i==1)	// No usage, m_bUseAccelMatching
				continue;

			if (bMatchingSuccess[i]==TRUE)
			{
				iTransModelIndex = i;
				break;
			}
		}
	}
	else
	{
		for (i=2; i<4; i++)
		{
			if (bMatchingSuccess[i]==TRUE)
			{
				iTransModelIndex = i;
				break;
			}
		}
	}

	if (iTransModelIndex == -1)
	{
		CString str; 
		str.Format("All Matching Fail"); 
		THEAPP.SaveLog(str);
		return FALSE;
	}
    	
	if(THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)
	{
		if (bMatchingSuccess[0])
		{
			hom_mat2d_identity(&HomMat2DIdentity1);
			hom_mat2d_translate(HomMat2DIdentity1, Row1, Column1, &HomMat2DTranslate1);
			hom_mat2d_rotate(HomMat2DTranslate1, Angle1, Row1, Column1, &HomMat2DRotate1);
			hom_mat2d_scale(HomMat2DRotate1, ScaleR1, ScaleC1, Row1, Column1, &HomMat2DScale1);
			*pdOutmostContourCenterX = Column1[0].D();
			*pdOutmostContourCenterY = Row1[0].D();
		}
		else
		{
			if (iTransModelIndex==2)
			{
				hom_mat2d_identity(&HomMat2DIdentity1);
				hom_mat2d_translate(HomMat2DIdentity1, Row3, Column3, &HomMat2DScale1);

				*pdOutmostContourCenterX = Column3[0].D();
				*pdOutmostContourCenterY = Row3[0].D();
			}
			else if (iTransModelIndex==3)
			{
				hom_mat2d_identity(&HomMat2DIdentity1);
				hom_mat2d_translate(HomMat2DIdentity1, Row4, Column4, &HomMat2DScale1);

				*pdOutmostContourCenterX = Column4[0].D();
				*pdOutmostContourCenterY = Row4[0].D();
			}
		}

		if (!THEAPP.m_pInspectSummary->m_bUseAccelMatching)
		{
			if (bMatchingSuccess[1])
			{
				hom_mat2d_identity(&HomMat2DIdentity2);
				hom_mat2d_translate(HomMat2DIdentity2, Row2, Column2, &HomMat2DTranslate2);
				hom_mat2d_rotate(HomMat2DTranslate2, Angle2, Row2, Column2, &HomMat2DRotate2);
				hom_mat2d_scale(HomMat2DRotate2, ScaleR2, ScaleC2, Row2, Column2, &HomMat2DScale2);
			}
			else
			{
				if (iTransModelIndex==0)
				{
					hom_mat2d_identity(&HomMat2DIdentity2);
					hom_mat2d_translate(HomMat2DIdentity2, Row1, Column1, &HomMat2DScale2);
				}
				else if (iTransModelIndex==2)
				{
					hom_mat2d_identity(&HomMat2DIdentity2);
					hom_mat2d_translate(HomMat2DIdentity2, Row3, Column3, &HomMat2DScale2);
				}
				else if (iTransModelIndex==3)
				{
					hom_mat2d_identity(&HomMat2DIdentity2);
					hom_mat2d_translate(HomMat2DIdentity2, Row4, Column4, &HomMat2DScale2);
				}
			}
		}
	}

	if (bMatchingSuccess[2])
	{
		hom_mat2d_identity(&HomMat2DIdentity3);
		hom_mat2d_translate(HomMat2DIdentity3, Row3, Column3, &HomMat2DTranslate3);
		hom_mat2d_rotate(HomMat2DTranslate3, Angle3, Row3, Column3, &HomMat2DRotate3);
		hom_mat2d_scale(HomMat2DRotate3, ScaleR3, ScaleC3, Row3, Column3, &HomMat2DScale3);

		if(THEAPP.m_pModelDataManager->m_bOneBarrel)
		{
			*pdOutmostContourCenterX = Column3[0].D();
			*pdOutmostContourCenterY = Row3[0].D();
		}
	}
	else
	{
		if (iTransModelIndex==0)
		{
			hom_mat2d_identity(&HomMat2DIdentity3);
			hom_mat2d_translate(HomMat2DIdentity3, Row1, Column1, &HomMat2DScale3);

			if(THEAPP.m_pModelDataManager->m_bOneBarrel)
			{
				*pdOutmostContourCenterX = Column1[0].D();
				*pdOutmostContourCenterY = Row1[0].D();
			}
		}
		else if (iTransModelIndex==3)
		{
			hom_mat2d_identity(&HomMat2DIdentity3);
			hom_mat2d_translate(HomMat2DIdentity3, Row4, Column4, &HomMat2DScale3);

			if(THEAPP.m_pModelDataManager->m_bOneBarrel)
			{
				*pdOutmostContourCenterX = Column4[0].D();
				*pdOutmostContourCenterY = Row4[0].D();
			}
		}
	}

	//*    
	if (bMatchingSuccess[3])
	{
		hom_mat2d_identity(&HomMat2DIdentity4);
		hom_mat2d_translate(HomMat2DIdentity4, Row4, Column4, &HomMat2DTranslate4);
		hom_mat2d_rotate(HomMat2DTranslate4, Angle4, Row4, Column4, &HomMat2DRotate4);
		hom_mat2d_scale(HomMat2DRotate4, ScaleR4, ScaleC4, Row4, Column4, &HomMat2DScale4);
	}
	else
	{
		if (iTransModelIndex==0)
		{
			hom_mat2d_identity(&HomMat2DIdentity4);
			hom_mat2d_translate(HomMat2DIdentity4, Row1, Column1, &HomMat2DScale4);
		}
		else if (iTransModelIndex==2)
		{
			hom_mat2d_identity(&HomMat2DIdentity4);
			hom_mat2d_translate(HomMat2DIdentity4, Row3, Column3, &HomMat2DScale4);
		}
	}

	//* 
	if(THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)
	{
		affine_trans_contour_xld(*Contour1, &*Contour1, HomMat2DScale1);
		if (!THEAPP.m_pInspectSummary->m_bUseAccelMatching)
			affine_trans_contour_xld(*Contour2, &*Contour2, HomMat2DScale2);
	}

	affine_trans_contour_xld(*Contour3, &*Contour3, HomMat2DScale3);
	affine_trans_contour_xld(*Contour4, &*Contour4, HomMat2DScale4);
	//*

	if(THEAPP.m_pModelDataManager->m_bOneBarrel==FALSE)
	{
		if (THEAPP.m_pInspectSummary->m_bUseAccelMatching)		// 매칭 가속 옵션 사용 시 하단 베럴 내곽라인(contour2)은 contour3의 비례값으로 만든다
		{
			Hobject HRgn2, HRgn3;
			double d3CenterR = 0, d3CenterC = 0, d3Radius = 0;

			gen_region_contour_xld (*Contour3, &HRgn3, "filled");
			smallest_circle (HRgn3, &d3CenterR, &d3CenterC, &d3Radius);

			double d2Radius = d3Radius/THEAPP.m_pModelDataManager->m_dContRatio23, d3Margin = d2Radius - d3Radius;
			dilation_circle (HRgn3, &HRgn2, d3Margin);
			gen_contour_region_xld (HRgn2, &*Contour2, "border");
		}
	}

	Hobject HRgn4, HRgn5;
	double d4CenterR = 0, d4CenterC = 0, d4Radius = 0;

	gen_region_contour_xld (*Contour4, &HRgn4, "filled");
	smallest_circle (HRgn4, &d4CenterR, &d4CenterC, &d4Radius);

	gen_circle(&HRgn4, d4CenterR, d4CenterC, d4Radius);	//WCS 2020/03/18
		
	double d5Radius = d4Radius*THEAPP.m_pModelDataManager->m_dContRatio45, d4Margin = d4Radius - d5Radius;
	erosion_circle (HRgn4, &HRgn5, d4Margin);
	gen_contour_region_xld (HRgn5, &*Contour5, "border");
	

	HomMat2DIdentity1.Reset();
	HomMat2DIdentity2.Reset();
	HomMat2DIdentity3.Reset();
	HomMat2DIdentity4.Reset();

	HomMat2DTranslate1.Reset();
	HomMat2DTranslate2.Reset();
	HomMat2DTranslate3.Reset();
	HomMat2DTranslate4.Reset();

	HomMat2DRotate1.Reset();
	HomMat2DRotate2.Reset();
	HomMat2DRotate3.Reset();
	HomMat2DRotate4.Reset();

	HomMat2DScale1.Reset();
	HomMat2DScale2.Reset();
	HomMat2DScale3.Reset();
	HomMat2DScale4.Reset();

	HRgn4.Reset();
	HRgn5.Reset();
	HImgSub.Reset();
	gen_empty_obj(&HImgSub);

	return TRUE;
}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [Algorithm::MatchingAlgorithm] : %s", except.message); THEAPP.SaveLog(str);
	return FALSE;
}
}


BOOL Algorithm::AllignImages (HTuple *ModelID)
{
try {
	// Local control variables 
	HTuple  RefCenterRow, RefCenterColumn;
	HTuple  Angle, ScaleR, ScaleC, Score;
	HTuple  HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;
	HTuple  Surface_CenterX, Surface_CenterY, Edge_CenterX, Edge_CenterY, Lens1_CenterX, Lens1_CenterY, Lens2_CenterX, Lens2_CenterY;

	find_aniso_shape_model (m_HAlgoInspectImage[TRIGGER_LENS1], *ModelID, 0, 0, 0.98, 1.02, 0.98, 1.02, 0.65, 1, 0.3, "interpolation", 3, 0.9, &RefCenterRow, &RefCenterColumn, &Angle, &ScaleR, &ScaleC, &Score);
	Lens1_CenterX = RefCenterColumn;
	Lens1_CenterY = RefCenterRow;

	find_aniso_shape_model (m_HAlgoInspectImage[TRIGGER_SURFACE], *ModelID, 0, 0, 0.98, 1.02, 0.98, 1.02, 0.65, 1, 0.3, "interpolation", 3, 0.9, &RefCenterRow, &RefCenterColumn, &Angle, &ScaleR, &ScaleC, &Score);
	Surface_CenterX = RefCenterColumn;
	Surface_CenterY = RefCenterRow;

	find_aniso_shape_model(m_HAlgoInspectImage[TRIGGER_EDGE], *ModelID, 0, 0, 0.98, 1.02, 0.98, 1.02, 0.65, 1, 0.3, "interpolation", 3, 0.9, &RefCenterRow, &RefCenterColumn, &Angle, &ScaleR, &ScaleC, &Score);
	Edge_CenterX = RefCenterColumn;
	Edge_CenterY = RefCenterRow;

	find_aniso_shape_model(m_HAlgoInspectImage[TRIGGER_LENS2], *ModelID, 0, 0, 0.98, 1.02, 0.98, 1.02, 0.65, 1, 0.3, "interpolation", 3, 0.9, &RefCenterRow, &RefCenterColumn, &Angle, &ScaleR, &ScaleC, &Score);
	Lens2_CenterX = RefCenterColumn;
	Lens2_CenterY = RefCenterRow;

	hom_mat2d_identity (&HomMat2DIdentity);
	if ( (int)(Lens1_CenterX-Surface_CenterX)[0].I() != 0 || (int)(Lens1_CenterY-Surface_CenterY)[0].I() !=0 ) {
		hom_mat2d_translate (HomMat2DIdentity, (int)(Lens1_CenterY-Surface_CenterY)[0].I(), (int)(Lens1_CenterX-Surface_CenterX)[0].I(), &HomMat2DTranslate);
		affine_trans_image (m_HAlgoInspectImage[TRIGGER_SURFACE], &m_HAlgoInspectImage[TRIGGER_SURFACE], HomMat2DTranslate, "none", "false");
	}

	if ( (int)(Lens1_CenterX-Edge_CenterX)[0].I() != 0 || (int)(Lens1_CenterY-Edge_CenterY)[0].I() !=0 ) {
		hom_mat2d_translate (HomMat2DIdentity, (int)(Lens1_CenterY-Edge_CenterY)[0].I(), (int)(Lens1_CenterX-Edge_CenterX)[0].I(), &HomMat2DTranslate);
		affine_trans_image (m_HAlgoInspectImage[TRIGGER_EDGE], &m_HAlgoInspectImage[TRIGGER_EDGE], HomMat2DTranslate, "none", "false");
	}

	if ( (int)(Lens1_CenterX-Lens2_CenterX)[0].I() != 0 || (int)(Lens1_CenterY-Lens2_CenterY)[0].I() !=0 ) {
		hom_mat2d_translate (HomMat2DIdentity, (int)(Lens1_CenterY-Lens2_CenterY)[0].I(), (int)(Lens1_CenterX-Lens2_CenterX)[0].I(), &HomMat2DTranslate);
		affine_trans_image (m_HAlgoInspectImage[TRIGGER_LENS2], &m_HAlgoInspectImage[TRIGGER_LENS2], HomMat2DTranslate, "none", "false");
	}

	CString sTmp; sTmp.Format("Module %d: Dev Surface (%d, %d), Dev Edge (%d, %d), Dev Lens2 (%d, %d)", m_nModuleNo,
		(int)(Lens1_CenterX-Surface_CenterX)[0].I(), (int)(Lens1_CenterY-Surface_CenterY)[0].I(),
		(int)(Lens1_CenterX-Edge_CenterX)[0].I(), (int)(Lens1_CenterY-Edge_CenterY)[0].I(),
		(int)(Lens1_CenterX-Lens2_CenterX)[0].I(), (int)(Lens1_CenterY-Lens2_CenterY)[0].I());
	THEAPP.SaveLog (sTmp);

	return TRUE;
}

catch(HException &except)
{
	CString str; str.Format("Halcon Exception [Algorithm::AllignImages] : %s", except.message); THEAPP.SaveLog(str);
	return FALSE;
}
}




////////////////////////////////// changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
Hobject Algorithm::Make_ROI_LensInner(Hobject HImageOri, Hobject Contour4, double dLensInner_InnerErosion, double dLensInner_OuterInnerErosion, int iThreshold, int iMargin)
{
try{
	Hobject HImage_LensInner;
	Hobject HImage;

	copy_image(HImageOri, &HImage);

	int iLensBGThreshold = iThreshold;
	int iLensInspectMargin = iMargin;

	Hobject HImage_LensOuter, HLensImageReduced, HLensDarkImageReduced, HLensDontCareRgn;
	Hobject HRegionErosion, HLensDarkRgn, HLensDarkClosingRgn, HLensDarkOpeningRgn, HLensDarkFillUpRgn, HLensDarkRgnContour;
	HTuple Row, Column, Radius, StartPhi, EndPhi, PointOrder;

	BOOL	bDebug = FALSE;

	if(bDebug)
	{
		write_image(HImage, "bmp", 0, "C:\\DualTest\\Lens1.bmp");
		write_contour_xld_arc_info(Contour4, "C:\\DualTest\\Lens1.xld");
	}
		

	// Detect Dark Boundary Area
	gen_region_contour_xld(Contour4,&Contour4,"filled");

	erosion_circle(Contour4, &HRegionErosion, dLensInner_OuterInnerErosion);
	reduce_domain(HImage, HRegionErosion, &HLensImageReduced);
	threshold(HLensImageReduced,&HLensDarkRgn,0,iLensBGThreshold);
	closing_circle(HLensDarkRgn, &HLensDarkClosingRgn, 20.5);
	opening_circle(HLensDarkClosingRgn, &HLensDarkOpeningRgn, 20.5);
	fill_up(HLensDarkOpeningRgn, &HLensDarkFillUpRgn);
	connection(HLensDarkFillUpRgn, &HLensDarkFillUpRgn);
	select_shape_std (HLensDarkFillUpRgn, &HLensDarkRgn, "max_area", 70);		
	erosion_circle(HLensDarkRgn,&HLensDarkRgn,4.5);
	gen_contour_region_xld(HLensDarkRgn, &HLensDarkRgnContour, "border");
	fit_circle_contour_xld(HLensDarkRgnContour, "algebraic", -1, 0, 0, 3, 2, &Row, &Column, &Radius, &StartPhi, &EndPhi, &PointOrder);
	gen_circle_contour_xld(&HLensDarkRgnContour, Row, Column, Radius, 0, 6.28318, "positive", 1);
	gen_region_contour_xld(HLensDarkRgnContour, &HLensDarkRgn, "filled");

	// Detect Inspecting Area ( Remove Reflected Area )
	reduce_domain (HImage, HLensDarkRgn, &HLensDarkImageReduced);
	threshold (HLensDarkImageReduced, &HLensDontCareRgn, iLensBGThreshold+1, 255);
	closing_circle (HLensDontCareRgn, &HLensDontCareRgn, 5.5);
	connection (HLensDontCareRgn, &HLensDontCareRgn);
	dilation_circle (HLensDontCareRgn, &HLensDontCareRgn, 1.5);
	select_shape_std (HLensDontCareRgn, &HLensDontCareRgn, "max_area",70);

	Hlong lNoConnectedRgn, lNoHoles;
	Hobject HRegionFillUp, HRegionDifference, HRegionTrans, HRegionBorder, HContour, HLensInspectRgn;

	connect_and_holes (HLensDontCareRgn, &lNoConnectedRgn, &lNoHoles);
	if (lNoHoles > 0)
	{
		Hobject HRgnDiff_conn, HRgnDiff_select;
		fill_up (HLensDontCareRgn, &HRegionFillUp);
		difference (HRegionFillUp, HLensDontCareRgn, &HRegionDifference);
		connection (HRegionDifference, &HRgnDiff_conn);
		select_shape_std (HRgnDiff_conn, &HRgnDiff_select, "max_area", 70);
		union1 (HRgnDiff_select, &HRgnDiff_select);
		shape_trans (HRgnDiff_select, &HRegionTrans, "convex");
		//shape_trans(HRegionDifference, &HRegionTrans, "convex");
		boundary (HRegionTrans, &HRegionBorder, "inner");
		gen_contour_region_xld(HRegionBorder, &HContour, "border");
		fit_circle_contour_xld(HContour, "algebraic", -1, 0, 0, 3, 2, &Row, &Column, &Radius, &StartPhi, &EndPhi, &PointOrder);

		if (Radius > 50)
		{
			gen_circle_contour_xld(&HContour, Row, Column, Radius, 0, 6.28318, "positive", 1);
			gen_region_contour_xld(HContour, &HLensInspectRgn, "filled");
			erosion_circle(HLensInspectRgn, &HLensInspectRgn, (double)iLensInspectMargin+0.5);
		}
		else
		{
			erosion_circle (HRegionErosion, &HLensInspectRgn, dLensInner_InnerErosion*2.8);		// TopRing 조명용 임시 프로세싱	
		}
	}
	else		//R 검출 실패시
	{
		if (dLensInner_OuterInnerErosion > 100) {
			erosion_circle (HRegionErosion, &HLensInspectRgn, dLensInner_InnerErosion*2.8);		// 55 조명용 임시 프로세싱	
		}
		else {
			erosion_circle (HLensDarkRgn, &HLensInspectRgn, dLensInner_InnerErosion);			// 조명 투영 영역을 제외한다

			difference(HLensDarkRgn, HLensDontCareRgn, &HLensDarkRgn);
			copy_obj(HLensDarkRgn, &HLensInspectRgn, 1, 1);
			//}
		}
	}

	boundary (HLensInspectRgn, &HRegionBorder, "inner");
	gen_contour_region_xld(HRegionBorder, &HContour, "border");
	fit_circle_contour_xld(HContour, "algebraic", -1, 0, 0, 3, 2, &Row, &Column, &Radius, &StartPhi, &EndPhi, &PointOrder);

	//////// 조명 반사 영역을 잡지 못하고 렌즈 전체를 영역으로 잡으면 검사영역을 줄여준다. 얼룩 검사는 렌즈 중심부근만
	if (Radius * PXLLEN > 800) erosion_circle (HRegionErosion, &HLensInspectRgn, dLensInner_InnerErosion*1.8);

	reduce_domain(HImage, HLensInspectRgn, &HImage_LensInner);

	return HImage_LensInner;
}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [Algorithm Make_ROI_LensInner] : %s", except.message); THEAPP.SaveLog(str);
	Hobject HNull; gen_empty_obj(&HNull); return HNull;
}
}

Hobject Algorithm::Make_ROI_LensCast(Hobject HImgOri, Hobject Contour5, int iLensMargin, int iEnforceMargin, int iCastThres, int iCastMargin)
{
	try{
		Hobject HRgnCast_pre, HRgnCast_select;
		Hobject HRgnLens;
		Hobject HImgLens;
		HTuple Row, Col, Radius, StartPhi, EndPhi, PointOrder;
		HTuple RefRow, RefCol, RefRadius, RefStartPhi, RefEndPhi, RefPointOrder;

		fit_circle_contour_xld(Contour5, "algebraic", -1, 0, 0, 3, 2, &RefRow, &RefCol, &RefRadius, &RefStartPhi, &RefEndPhi, &RefPointOrder);

		BOOL bDebugSave = FALSE;

		if (bDebugSave)
		{
			write_image(HImgOri, "bmp", 0, "c:\\DualTest\\LensImage");
			write_contour_xld_arc_info(Contour5, "C:\\DualTest\\Contour5.xld");
		}

		// Extract Cast Area (조명 반사상 포함 밝은 무언가를 추출해낸다)
		gen_empty_obj(&HRgnCast_pre); gen_empty_obj(&HRgnLens);

		THEAPP.m_pAlgorithm->GetInspectArea_Circle (Contour5, iLensMargin, &HRgnLens);
		reduce_domain (HImgOri, HRgnLens, &HImgLens);
		threshold (HImgLens, &HRgnCast_pre, iCastThres, 255);
		closing_circle (HRgnCast_pre, &HRgnCast_pre, 5.5);
		connection (HRgnCast_pre, &HRgnCast_pre);
		dilation_circle (HRgnCast_pre, &HRgnCast_pre, 1.5);
		select_shape (HRgnCast_pre, &HRgnCast_select, "area", "and", 500, MAX_DEF);
		select_shape_std (HRgnCast_select, &HRgnCast_select, "max_area", 70);


		// Get Inspect Area
		Hlong lNoConnectedRgn, lNoHoles;
		Hobject HRgnCast_fill, HRgnDiff, HRgnDiff_trans, HRgnDiff_border, HContour;
		Hobject HRgnInspect; gen_empty_obj(&HRgnInspect);
		if (THEAPP.m_pGFunction->ValidHRegion(HRgnCast_select) == TRUE)
		{
			connect_and_holes (HRgnCast_select, &lNoConnectedRgn, &lNoHoles);
			if (lNoHoles > 0)
			{
				Hobject HRgnDiff_conn, HRgnDiff_select;
				fill_up (HRgnCast_select, &HRgnCast_fill);
				difference (HRgnCast_fill, HRgnCast_select, &HRgnDiff);
				connection (HRgnDiff, &HRgnDiff_conn);
				select_shape_std (HRgnDiff_conn, &HRgnDiff_select, "max_area", 70);
				union1 (HRgnDiff_select, &HRgnDiff_select);
				shape_trans (HRgnDiff_select, &HRgnDiff_trans, "convex");
				boundary (HRgnDiff_trans, &HRgnDiff_border, "inner");
				if (THEAPP.m_pGFunction->ValidHRegion(HRgnDiff_border) == TRUE)
				{
					gen_contour_region_xld(HRgnDiff_border, &HContour, "border");
					fit_circle_contour_xld(HContour, "algebraic", -1, 0, 0, 3, 2, &Row, &Col, &Radius, &StartPhi, &EndPhi, &PointOrder);

					HTuple HTuCircularity;
					circularity(HRgnDiff_trans, &HTuCircularity);

					if (fabs((RefRow-Row)[0].D()) <= RefRadius[0].D()*0.2 && fabs((RefCol-Col)[0].D()) <= RefRadius[0].D()*0.2	// 공백은 중앙 부근에
						&& HTuCircularity[0].D() >= 0.9)				// 공백은 원모양이어야 한다	
					{
						if (Radius[0].D() > 50)
						{
							gen_circle_contour_xld(&HContour, Row, Col, Radius, 0, 6.28318, "positive", 1);
							gen_region_contour_xld(HContour, &HRgnInspect, "filled");
							erosion_circle(HRgnInspect, &HRgnInspect, (double)iCastMargin+0.5);
						}
						else
						{
							erosion_circle (HRgnLens, &HRgnInspect, (double)iEnforceMargin);		// 강제로 내부 원을 만들어준다
						}

						boundary (HRgnInspect, &HRgnDiff_border, "inner");
						gen_contour_region_xld(HRgnDiff_border, &HContour, "border");
						fit_circle_contour_xld(HContour, "algebraic", -1, 0, 0, 3, 2, &Row, &Col, &Radius, &StartPhi, &EndPhi, &PointOrder);

						//////// 조명 반사 영역을 잡지 못하고 렌즈 전체를 영역으로 잡으면 검사영역을 강제로 줄여준다. 얼룩 검사는 렌즈 중심부근만
						if (Radius[0].D() > RefRadius[0].D()*0.9)
							erosion_circle (HRgnLens, &HRgnInspect, (double)iEnforceMargin);

						return HRgnInspect;
					}
				}
			}
		}


		if (abs(iLensMargin) > 100)
			erosion_circle (HRgnLens, &HRgnInspect, (double)iEnforceMargin);			// 55 조명용 임시 프로세싱	
		else
			difference(HRgnLens, HRgnCast_select, &HRgnInspect);

		fill_up (HRgnInspect, &HRgnInspect);


		//////// 조명 반사 영역을 잡지 못하고 렌즈 전체를 영역으로 잡으면 검사영역을 강제로 줄여준다. 얼룩 검사는 렌즈 중심부근만
		gen_empty_obj(&HRgnDiff_border);
		boundary (HRgnInspect, &HRgnDiff_border, "inner");
		if (THEAPP.m_pGFunction->ValidHRegion(HRgnDiff_border) == TRUE)
		{
			gen_contour_region_xld(HRgnDiff_border, &HContour, "border");
			fit_circle_contour_xld(HContour, "algebraic", -1, 0, 0, 3, 2, &Row, &Col, &Radius, &StartPhi, &EndPhi, &PointOrder);

			HTuple HTuCircularity;
			circularity(HRgnInspect, &HTuCircularity);

			if (fabs((RefRow-Row)[0].D()) > RefRadius[0].D()*0.2 || fabs((RefCol-Col)[0].D()) > RefRadius[0].D()*0.2	// 공백은 중앙 부근에
				|| HTuCircularity < 0.9									// 검사영역은 원모양이어야 한다
				|| Radius[0].D() > RefRadius[0].D()*0.9)				// 검사영역이 너무 크면 영역을 잘못 잡은 것
				//if (Radius[0].D() * PXLLEN > 800)
			{
				erosion_circle (HRgnLens, &HRgnInspect, (double)iEnforceMargin);
			}
		}

		return HRgnInspect;
	}
	catch(HException &except)
	{
		CString str; str.Format("Halcon Exception [Algorithm Make_ROI_LensCast] : %s", except.message); THEAPP.SaveLog(str);
		Hobject HNull; gen_empty_obj(&HNull); return HNull;
	}
}

void Algorithm::InspectCircleFitting(HTuple X, HTuple Y, double dNormalRadius, double& dCenterX, double& dCenterY, double& dRadius)
{
	//////////////////////////////////////////////////////////////////////////
	/// Least-Square Hole Center Search
	//////////////////////////////////////////////////////////////////////////

	HTuple  x_appr, num, B, A, Result, unknowns, redundancy;
	HTuple  Iter, NumberOfMaxIterations, dx_hat, w, invBBT;
	HTuple  i, w1_a, w1_b, w2_a, w2_b, w3_a, w3_b, BBT, ATinvBBT;
	HTuple  ATinvBBTA, invATinvBBTA, neg_invATinvBBTA, neg_invATinvBBTAAT;
	HTuple  neg_invATinvBBTAAT_invBBT, M_x_appr, x_hat, x_appr_tmp;
	HTuple  Tdx_hat, Adx_hat, Adx_hatw, neg_invBBT, k_hat, v_hat;
	HTuple  Pow2, Mean, MeanValues;

	HTuple HMatchingCenterX, HMatchingCenterY;
	BOOL bLMSSuccess = FALSE;

	unknowns = 2;
	NumberOfMaxIterations = 8;
	//Approximations
	//tuple_gen_const(0, 0, &x_appr);
	x_appr.Reset();
	x_appr.Append(X.Mean());
	x_appr.Append(Y.Mean());
	x_appr.Append(dNormalRadius);
	//Create matrices
	num = X.Num();
	//Initialize matrices A and B
	create_matrix(num, num*2, 0, &B);
	create_matrix(num, 2, 0, &A);
	Result = 1;
	redundancy = num-unknowns;
	Iter = 0;
	while ((Result>1e-6)&&(Iter<NumberOfMaxIterations))
	{
		Iter += 1;
		if (Iter>1)
		{
			clear_matrix((dx_hat.Concat(w)).Concat(invBBT));
		}
		//Set matrices A and B with values
		for (i=0; i<=num-1; i+=1)
		{
			set_value_matrix(B, i.Concat(i), (i*2).Concat((i*2)+1), (2*(HTuple(X[i])-HTuple(x_appr[0]))).Concat(2*(HTuple(Y[i])-HTuple(x_appr[1]))));
			set_value_matrix(A, i.Concat(i), (HTuple(0).Append(1)), (-2*(HTuple(X[i])-HTuple(x_appr[0]))).Concat(-2*(HTuple(Y[i])-HTuple(x_appr[1]))));
		}
		//Calculate w = (X-x_appr[0])^2 + (Y-x_appr[1])^2 - x_appr[2]^2
		create_matrix(num, 1, X-HTuple(x_appr[0]), &w1_a);
		mult_element_matrix(w1_a, w1_a, &w1_b);
		create_matrix(num, 1, Y-HTuple(x_appr[1]), &w2_a);
		mult_element_matrix(w2_a, w2_a, &w2_b);
		create_matrix(num, 1, HTuple(x_appr[2]), &w3_a);
		mult_element_matrix(w3_a, w3_a, &w3_b);
		add_matrix_mod(w1_b, w2_b);
		sub_matrix(w1_b, w3_b, &w);
		clear_matrix((w1_a.Concat(w2_a)).Concat(w3_a));
		clear_matrix((w1_b.Concat(w2_b)).Concat(w3_b));
		//Adjustment
		//Calculate dx_hat = -inv(A' * inv(B*B') * A) * A' * inv(B*B') * w
		mult_matrix(B, B, "ABT", &BBT);
		invert_matrix(BBT, "symmetric", 0, &invBBT);
		mult_matrix(A, invBBT, "ATB", &ATinvBBT);
		mult_matrix(ATinvBBT, A, "AB", &ATinvBBTA);
		invert_matrix(ATinvBBTA, "symmetric", 0, &invATinvBBTA);
		scale_matrix(invATinvBBTA, -1, &neg_invATinvBBTA);
		mult_matrix(neg_invATinvBBTA, A, "ABT", &neg_invATinvBBTAAT);
		mult_matrix(neg_invATinvBBTAAT, invBBT, "AB", &neg_invATinvBBTAAT_invBBT);
		mult_matrix(neg_invATinvBBTAAT_invBBT, w, "AB", &dx_hat);

		//Calculate x_hat = M_x_appr + dx_hat
		create_matrix(2, 1, HTuple(x_appr[0]).Concat(HTuple(x_appr[1])), &M_x_appr);
		add_matrix(M_x_appr, dx_hat, &x_hat);
		get_full_matrix(x_hat, &x_appr_tmp);
		x_appr = x_appr_tmp.Concat(HTuple(x_appr[2]));
		get_full_matrix(dx_hat, &Tdx_hat);
		Result = (Tdx_hat.Abs()).Max();
		clear_matrix((((BBT.Concat(ATinvBBT)).Concat(ATinvBBTA)).Concat(invATinvBBTA)).Concat(neg_invATinvBBTA));
		clear_matrix(neg_invATinvBBTAAT.Concat(neg_invATinvBBTAAT_invBBT));
		clear_matrix(M_x_appr.Concat(x_hat));
	}
	
	if (Iter<NumberOfMaxIterations)
	{
		//Adjustment of a hole is successful.  The hole is displayed.
		//Calculate k_hat = -inv(B*B') -(A * dx_hat + w)
		mult_matrix(A, dx_hat, "AB", &Adx_hat);
		add_matrix(Adx_hat, w, &Adx_hatw);
		scale_matrix(invBBT, -1, &neg_invBBT);
		mult_matrix(neg_invBBT, Adx_hatw, "AB", &k_hat);
		//Calculate v_hat = B * k_hat
		mult_matrix(B, k_hat, "ATB", &v_hat);
		pow_scalar_element_matrix(v_hat, 2, &Pow2);
		mean_matrix(Pow2, "full", &Mean);
		sqrt_matrix_mod(Mean);
		get_full_matrix(Mean, &MeanValues);
		clear_matrix((((((Adx_hat.Concat(Adx_hatw)).Concat(neg_invBBT)).Concat(k_hat)).Concat(v_hat)).Concat(Pow2)).Concat(Mean));
		dCenterX = x_appr[0];
		dCenterY = x_appr[1];
		dRadius = x_appr[2];

		bLMSSuccess = TRUE;
	}
	else
	{
		bLMSSuccess = FALSE;
	}

	clear_matrix((dx_hat.Concat(w)).Concat(invBBT));
	clear_matrix(A.Concat(B));

}
////////////////////////////////// <==== Added for CMI3000 2000 ///////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
///	New

BOOL Algorithm::GetInspectArea_Circle (Hobject HContFit, int iOuterMargin, Hobject *pHRgnInspect)
{
try
{
	Hobject HRgnFit;
	gen_region_contour_xld (HContFit, &HRgnFit, "filled");

	if (iOuterMargin >= 0)
		dilation_circle(HRgnFit, pHRgnInspect, (double)iOuterMargin + 0.5);
	else
		erosion_circle(HRgnFit, pHRgnInspect, (double)-iOuterMargin + 0.5);


	return TRUE;
}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [Algorithm::GetInspectArea_Circle] : %s", except.message); THEAPP.SaveLog(str);
	return FALSE;
}
}

BOOL Algorithm::GetInspectArea_Lens (Hobject HRgnInspectPre, int iOuterMargin, Hobject *pHRgnInspect)
{
try
{
	if (iOuterMargin >= 0)
		dilation_circle(HRgnInspectPre, pHRgnInspect, (double)iOuterMargin + 0.5);
	else
		erosion_circle(HRgnInspectPre, pHRgnInspect, (double)-iOuterMargin + 0.5);

	return TRUE;
}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [Algorithm::GetInspectArea_Lens] : %s", except.message); THEAPP.SaveLog(str);
	return FALSE;
}
}


BOOL Algorithm::GetInspectArea_Barrel(Hobject HInnerFitLine, Hobject HOuterFitLine, int iInnerMargin, int iOuterMargin, Hobject *pHInspectArea)
{
try
{
	Hobject HInnerFitRgn, HOuterFitRgn;
	Hobject HInnerMarginRgn, HOuterMarginRgn;

	gen_region_contour_xld(HInnerFitLine, &HInnerFitRgn, "filled");
	gen_region_contour_xld(HOuterFitLine, &HOuterFitRgn, "filled");

	if (iInnerMargin >= 0)
		dilation_circle(HInnerFitRgn, &HInnerMarginRgn, (double)iInnerMargin + 0.5);
	else
		erosion_circle(HInnerFitRgn, &HInnerMarginRgn, (double)-iInnerMargin + 0.5);

	if (iOuterMargin >= 0)
		dilation_circle(HOuterFitRgn, &HOuterMarginRgn, (double)iOuterMargin + 0.5);
	else
		erosion_circle(HOuterFitRgn, &HOuterMarginRgn, (double)-iOuterMargin + 0.5);

	difference (HOuterMarginRgn, HInnerMarginRgn, pHInspectArea);

	return TRUE;
}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [Algorithm::GetInspectArea_Barrel] : %s", except.message); THEAPP.SaveLog(str);
	return FALSE;
}
}

BOOL Algorithm::GetInspectArea_Edge(Hobject HInnerFitLine, Hobject HOuterFitLine, BOOL bInspectInnerEdge, int iInnerMargin_Inside, int iInnerMargin_Outside, BOOL bInspectOuterEdge, int iOuterMargin_Inside, int iOuterMargin_Outside, Hobject *pHInspectArea)
{
try
{
	Hobject HInnerFitRgn, HOuterFitRgn;
	Hobject HInnerMarginRgn, HOuterMarginRgn;

	gen_empty_obj(pHInspectArea);

	Hobject HEdgeInspectRgn;

	if (bInspectInnerEdge)
	{
		gen_region_contour_xld(HInnerFitLine, &HInnerFitRgn, "filled");

		if (iInnerMargin_Inside >= 0)
			dilation_circle(HInnerFitRgn, &HInnerMarginRgn, (double)iInnerMargin_Inside + 0.5);
		else
			erosion_circle(HInnerFitRgn, &HInnerMarginRgn, (double)-iInnerMargin_Inside + 0.5);

		if (iInnerMargin_Outside >= 0)
			dilation_circle(HInnerFitRgn, &HOuterMarginRgn, (double)iInnerMargin_Outside + 0.5);
		else
			erosion_circle(HInnerFitRgn, &HOuterMarginRgn, (double)-iInnerMargin_Outside + 0.5);

		difference(HOuterMarginRgn, HInnerMarginRgn, &HEdgeInspectRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HEdgeInspectRgn)==TRUE)
			concat_obj(*pHInspectArea, HEdgeInspectRgn, pHInspectArea);
	}

	if (bInspectOuterEdge)
	{
		gen_region_contour_xld(HOuterFitLine, &HOuterFitRgn, "filled");

		if (iOuterMargin_Inside >= 0)
			dilation_circle(HOuterFitRgn, &HInnerMarginRgn, (double)iOuterMargin_Inside + 0.5);
		else
			erosion_circle(HOuterFitRgn, &HInnerMarginRgn, (double)-iOuterMargin_Inside + 0.5);

		if (iOuterMargin_Outside >= 0)
			dilation_circle(HOuterFitRgn, &HOuterMarginRgn, (double)iOuterMargin_Outside + 0.5);
		else
			erosion_circle(HOuterFitRgn, &HOuterMarginRgn, (double)-iOuterMargin_Outside + 0.5);

		difference(HOuterMarginRgn, HInnerMarginRgn, &HEdgeInspectRgn);

		if (THEAPP.m_pGFunction->ValidHRegion(HEdgeInspectRgn)==TRUE)
			concat_obj(*pHInspectArea, HEdgeInspectRgn, pHInspectArea);
	}

	if (THEAPP.m_pGFunction->ValidHRegion(*pHInspectArea)==TRUE)
		union1(*pHInspectArea, pHInspectArea);

	return TRUE;
}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [Algorithm::GetInspectArea_Edge] : %s", except.message); THEAPP.SaveLog(str);
	return FALSE;
}
}

BOOL Algorithm::GetInspectArea_Inner(Hobject HFitLine, int iInnerMargin, int iOuterMargin, Hobject *pHInspectArea)
{
try
{
	Hobject HFitRgn;
	Hobject HInnerMarginRgn, HOuterMarginRgn;

	gen_region_contour_xld(HFitLine, &HFitRgn, "filled");

	if (iInnerMargin >= 0)
		dilation_circle(HFitRgn, &HInnerMarginRgn, (double)iInnerMargin + 0.5);
	else
		erosion_circle(HFitRgn, &HInnerMarginRgn, (double)-iInnerMargin + 0.5);

	if (iOuterMargin >= 0)
		dilation_circle(HFitRgn, &HOuterMarginRgn, (double)iOuterMargin + 0.5);
	else
		erosion_circle(HFitRgn, &HOuterMarginRgn, (double)-iOuterMargin + 0.5);

	difference (HOuterMarginRgn, HInnerMarginRgn, pHInspectArea);

	return TRUE;
}
catch(HException &except)
{
	CString str; str.Format("Halcon Exception [Algorithm::GetInspectArea_Inner] : %s", except.message); THEAPP.SaveLog(str);
	return FALSE;
}
}

BOOL Algorithm::GetInspectArea_Trans(Hobject HCont5, Hobject HCont4, Hobject HCont3, Hobject HCont2,
		BOOL bInspectTransTop, int iTopInnerMargin, int iTopOuterMargin, BOOL bInspectTransBtm, int iBtmInnerMargin, int iBtmOuterMargin, Hobject *pHInspectArea)
{
try
{
	Hobject HRgnIn, HRgnOut, HRgnInnMargin, HRgnOutMargin, HRgnInspect;

	if (bInspectTransTop)
	{
		gen_region_contour_xld(HCont5, &HRgnIn, "filled");
		if (iTopInnerMargin >= 0)
			dilation_circle(HRgnIn, &HRgnInnMargin, (double)iTopInnerMargin + 0.5);
		else
			erosion_circle(HRgnIn, &HRgnInnMargin, (double)-iTopInnerMargin + 0.5);

		gen_region_contour_xld(HCont4, &HRgnOut, "filled");
		if (iTopOuterMargin >= 0)
			dilation_circle(HRgnOut, &HRgnOutMargin, (double)iTopOuterMargin + 0.5);
		else
			erosion_circle(HRgnOut, &HRgnOutMargin, (double)-iTopOuterMargin + 0.5);

		difference (HRgnOutMargin, HRgnInnMargin, &HRgnInspect);
		if (THEAPP.m_pGFunction->ValidHRegion(HRgnInspect)==TRUE)
			concat_obj(*pHInspectArea, HRgnInspect, pHInspectArea);
	}

	if (bInspectTransBtm)
	{
		gen_region_contour_xld(HCont3, &HRgnIn, "filled");
		if (iBtmInnerMargin >= 0)
			dilation_circle(HRgnIn, &HRgnInnMargin, (double)iBtmInnerMargin + 0.5);
		else
			erosion_circle(HRgnIn, &HRgnInnMargin, (double)-iBtmInnerMargin + 0.5);

		gen_region_contour_xld(HCont2, &HRgnOut, "filled");
		if (iBtmOuterMargin >= 0)
			dilation_circle(HRgnOut, &HRgnOutMargin, (double)iBtmOuterMargin + 0.5);
		else
			erosion_circle(HRgnOut, &HRgnOutMargin, (double)-iBtmOuterMargin + 0.5);

		difference (HRgnOutMargin, HRgnInnMargin, &HRgnInspect);
		if (THEAPP.m_pGFunction->ValidHRegion(HRgnInspect)==TRUE)
			concat_obj(*pHInspectArea, HRgnInspect, pHInspectArea);
	}

	if (THEAPP.m_pGFunction->ValidHRegion(*pHInspectArea)==TRUE)
		union1(*pHInspectArea, pHInspectArea);

	return TRUE;
}

catch(HException &except)
{
	CString str; str.Format("Halcon Exception [Algorithm::GetInspectArea_Trans] : %s", except.message); THEAPP.SaveLog(str);
	return FALSE;
}
}

int Algorithm::ApplyLensInspectionCondition(Hobject HLensAllDefectRgn, Hobject *pHCTDefectRgn, Hobject *pHScratchDefectRgn, Hobject *pHDirtDefectRgn, Hobject *pHWhiteDotDefectRgn, Hobject LensImageReduced, Hobject *pHLensMinDefectRgn)
{
	try
	{
		Hobject HDefectRgnCopy,HThresRegion,Select_ObjectRegion,IntersectionObj1;
		HTuple HdefectCount;
		HTuple Area,Row,Column;
		int i;
		Hobject HLensSmallDefectRgn, HLensMidDefectRgn, HLensBigDefectRgn, HLensMaxDefectRgn;
		Hobject HScratchSmallDefectRgn, HScratchMidDefectRgn, HScratchBigDefectRgn, HScratchMaxDefectRgn;
		Hobject HDirtSmallDefectRgn, HDirtMidDefectRgn, HDirtBigDefectRgn, HDirtMaxDefectRgn;
		Hobject HWhiteDotSmallDefectRgn, HWhiteDotMidDefectRgn, HWhiteDotBigDefectRgn, HWhiteDotMaxDefectRgn;

		gen_empty_obj(&HLensSmallDefectRgn);
		gen_empty_obj(&HLensMidDefectRgn);
		gen_empty_obj(&HLensBigDefectRgn);
		gen_empty_obj(&HLensMaxDefectRgn);
		gen_empty_obj(&HScratchSmallDefectRgn);
		gen_empty_obj(&HScratchMidDefectRgn);
		gen_empty_obj(&HScratchBigDefectRgn);
		gen_empty_obj(&HScratchMaxDefectRgn);
		gen_empty_obj(&HDirtSmallDefectRgn);
		gen_empty_obj(&HDirtMidDefectRgn);
		gen_empty_obj(&HDirtBigDefectRgn);
		gen_empty_obj(&HDirtMaxDefectRgn);
		gen_empty_obj(&HWhiteDotSmallDefectRgn);
		gen_empty_obj(&HWhiteDotMidDefectRgn);
		gen_empty_obj(&HWhiteDotBigDefectRgn);
		gen_empty_obj(&HWhiteDotMaxDefectRgn);

		//////////////////////////////////////////////////////////////////////////

		gen_empty_obj(pHLensMinDefectRgn);

		int LensCountSmall = 0, LensCountMid = 0 , LensCountBiG = 0;

		copy_obj(HLensAllDefectRgn,&HDefectRgnCopy,1,-1);

		connection(HDefectRgnCopy, &HDefectRgnCopy);
		count_obj(HDefectRgnCopy, &HdefectCount);
		threshold(LensImageReduced, &HThresRegion,0,255);		// HThresRegion: 렌즈 검사 영역

		int  SortingDefectNumber =0;

		// 1st Step: 렌즈 전체에 대한 검사 조건 수행

		for (i=0; i<HdefectCount; i++)
		{
			select_obj (HDefectRgnCopy, &Select_ObjectRegion, i+1);

			intersection(HThresRegion,Select_ObjectRegion,&IntersectionObj1);

			if (THEAPP.m_pGFunction->ValidHRegion(IntersectionObj1))		// 렌즈 검사 영역과 만난다면
			{
				area_center (IntersectionObj1, &Area, &Row, &Column);
				double dArea = ceil((Area[0].D() * PXLAREA)/1000)/1000;		////////// 렌즈 검사 영역 내부의 불량 면적만 따짐

				////////////////////////////////// Changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
				if (dArea > 0 && dArea < THEAPP.m_pModelDataManager->m_dLensMinArea)
				{
					concat_obj(HLensSmallDefectRgn, IntersectionObj1, &HLensSmallDefectRgn);
					LensCountSmall++;
				}
				if (dArea >= THEAPP.m_pModelDataManager->m_dLensMinArea)
				{
					concat_obj(HLensMidDefectRgn, IntersectionObj1, &HLensMidDefectRgn);
					LensCountMid++;
				}
				if (dArea >= THEAPP.m_pModelDataManager->m_dLensMidArea)
				{
					concat_obj(HLensBigDefectRgn, IntersectionObj1, &HLensBigDefectRgn);
					LensCountBiG++;
				}
				if (dArea >= THEAPP.m_pModelDataManager->m_dLensMaxArea)
				{
					concat_obj(HLensMaxDefectRgn, IntersectionObj1, &HLensMaxDefectRgn);
					SortingDefectNumber = -1;
				}
			}
		}

		if (SortingDefectNumber <= 0 && THEAPP.m_pGFunction->ValidHRegion(*pHCTDefectRgn))
		{
			if (LensCountBiG > THEAPP.m_pModelDataManager->m_iLensMaxNumAccept)
			{
				concat_obj(*pHLensMinDefectRgn, HLensBigDefectRgn, pHLensMinDefectRgn);
				SortingDefectNumber = 1;
			}
			else
			{
				if (LensCountMid > THEAPP.m_pModelDataManager->m_iLensMidNumAccept)
				{
					concat_obj(*pHLensMinDefectRgn, HLensMidDefectRgn, pHLensMinDefectRgn);
					SortingDefectNumber = 2;
				}
				else
				{
					if (LensCountSmall > THEAPP.m_pModelDataManager->m_iLensMinNumAccept)
					{
						concat_obj(*pHLensMinDefectRgn, HLensSmallDefectRgn, pHLensMinDefectRgn);
						SortingDefectNumber = 3;
					}
				}
			}
		}
		else
			concat_obj(*pHLensMinDefectRgn, HLensMaxDefectRgn, pHLensMinDefectRgn);

		// 렌즈 White Dot - LeeGW

		if(SortingDefectNumber <= 0 && THEAPP.m_pGFunction->ValidHRegion(*pHWhiteDotDefectRgn))
		{
			copy_obj(*pHWhiteDotDefectRgn,&HDefectRgnCopy,1,-1);

			connection(HDefectRgnCopy, &HDefectRgnCopy);
			count_obj(HDefectRgnCopy, &HdefectCount);

			LensCountSmall = LensCountMid = LensCountBiG = 0;

			for (i=0; i<HdefectCount; i++)
			{
				select_obj (HDefectRgnCopy, &Select_ObjectRegion, i+1);

				intersection(HThresRegion,Select_ObjectRegion,&IntersectionObj1);

				if (THEAPP.m_pGFunction->ValidHRegion(IntersectionObj1))		// 렌즈 검사 영역과 만난다면
				{
					area_center (IntersectionObj1, &Area, &Row, &Column);
					double dArea = ceil((Area[0].D() * PXLAREA)/1000)/1000;		////////// 렌즈 검사 영역 내부의 불량 면적만 따짐

					////////////////////////////////// Changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
					if (dArea > 0 && dArea < THEAPP.m_pModelDataManager->m_dLensWhiteDotMinArea)
					{
						concat_obj(HWhiteDotSmallDefectRgn, IntersectionObj1, &HWhiteDotSmallDefectRgn);
						LensCountSmall++;
					}
					if (dArea >= THEAPP.m_pModelDataManager->m_dLensWhiteDotMinArea)
					{
						concat_obj(HWhiteDotMidDefectRgn, IntersectionObj1, &HWhiteDotMidDefectRgn);
						LensCountMid++;
					}
					if (dArea >= THEAPP.m_pModelDataManager->m_dLensWhiteDotMidArea)
					{
						concat_obj(HWhiteDotBigDefectRgn, IntersectionObj1, &HWhiteDotBigDefectRgn);
						LensCountBiG++;
					}
					if (dArea >= THEAPP.m_pModelDataManager->m_dLensWhiteDotMaxArea)
					{
						concat_obj(HWhiteDotMaxDefectRgn, IntersectionObj1, &HWhiteDotMaxDefectRgn);
						SortingDefectNumber=6;
					}
				}
			}

			if(SortingDefectNumber <= 0)
			{
				if(LensCountBiG > THEAPP.m_pModelDataManager->m_iLensWhiteDotMaxNumAccept)
				{
					concat_obj(*pHLensMinDefectRgn, HWhiteDotBigDefectRgn, pHLensMinDefectRgn);
					SortingDefectNumber = 6;
				}
				else
				{
					if(LensCountMid > THEAPP.m_pModelDataManager->m_iLensWhiteDotMidNumAccept)
					{
						concat_obj(*pHLensMinDefectRgn, HWhiteDotMidDefectRgn, pHLensMinDefectRgn);
						SortingDefectNumber = 6;
					}
					else
					{
						if(LensCountSmall > THEAPP.m_pModelDataManager->m_iLensWhiteDotMinNumAccept)
						{
							concat_obj(*pHLensMinDefectRgn, HWhiteDotSmallDefectRgn, pHLensMinDefectRgn);
							SortingDefectNumber = 6;
						}
					}
				}
			}
			else
				concat_obj(*pHLensMinDefectRgn, HWhiteDotMaxDefectRgn, pHLensMinDefectRgn);
		}

		// 2nd Step: 1st Step에서 불량 미검출 시에 렌즈 스크래치에 대한 검사 조건 수행

		if(SortingDefectNumber <= 0 && THEAPP.m_pGFunction->ValidHRegion(*pHScratchDefectRgn))
		{
			copy_obj(*pHScratchDefectRgn,&HDefectRgnCopy,1,-1);

			connection(HDefectRgnCopy, &HDefectRgnCopy);
			count_obj(HDefectRgnCopy, &HdefectCount);

			LensCountSmall = LensCountMid = LensCountBiG = 0;

			for (i=0; i<HdefectCount; i++)
			{
				select_obj (HDefectRgnCopy, &Select_ObjectRegion, i+1);

				intersection(HThresRegion,Select_ObjectRegion,&IntersectionObj1);

				if (THEAPP.m_pGFunction->ValidHRegion(IntersectionObj1))		// 렌즈 검사 영역과 만난다면
				{
					area_center (IntersectionObj1, &Area, &Row, &Column);
					double dArea = ceil((Area[0].D() * PXLAREA)/1000)/1000;		////////// 렌즈 검사 영역 내부의 불량 면적만 따짐

					////////////////////////////////// Changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
					if (dArea > 0 && dArea < THEAPP.m_pModelDataManager->m_dLensScratchMinArea)
					{
						concat_obj(HScratchSmallDefectRgn, IntersectionObj1, &HScratchSmallDefectRgn);
						LensCountSmall++;
					}
					if (dArea >= THEAPP.m_pModelDataManager->m_dLensScratchMinArea)
					{
						concat_obj(HScratchMidDefectRgn, IntersectionObj1, &HScratchMidDefectRgn);
						LensCountMid++;
					}
					if (dArea >= THEAPP.m_pModelDataManager->m_dLensScratchMidArea)
					{
						concat_obj(HScratchBigDefectRgn, IntersectionObj1, &HScratchBigDefectRgn);
						LensCountBiG++;
					}
					if (dArea >= THEAPP.m_pModelDataManager->m_dLensScratchMaxArea)
					{
						concat_obj(HScratchMaxDefectRgn, IntersectionObj1, &HScratchMaxDefectRgn);
						SortingDefectNumber=4;
					}
				}
			}

			if(SortingDefectNumber <= 0)
			{
				if(LensCountBiG > THEAPP.m_pModelDataManager->m_iLensScratchMaxNumAccept)
				{
					concat_obj(*pHLensMinDefectRgn, HScratchBigDefectRgn, pHLensMinDefectRgn);
					SortingDefectNumber = 4;
				}
				else
				{
					if(LensCountMid > THEAPP.m_pModelDataManager->m_iLensScratchMidNumAccept)
					{
						concat_obj(*pHLensMinDefectRgn, HScratchMidDefectRgn, pHLensMinDefectRgn);
						SortingDefectNumber = 4;
					}
					else
					{
						if(LensCountSmall > THEAPP.m_pModelDataManager->m_iLensScratchMinNumAccept)
						{
							concat_obj(*pHLensMinDefectRgn, HScratchSmallDefectRgn, pHLensMinDefectRgn);
							SortingDefectNumber = 4;
						}
					}
				}
			}
			else
				concat_obj(*pHLensMinDefectRgn, HScratchMaxDefectRgn, pHLensMinDefectRgn);
		}

		// 3rd Step: 2nd Step에서 렌즈 스크래치 미검출 시에 렌즈 미세불량에 대한 검사 조건 수행

		if(SortingDefectNumber <= 0 && THEAPP.m_pGFunction->ValidHRegion(*pHDirtDefectRgn))
		{
			copy_obj(*pHDirtDefectRgn,&HDefectRgnCopy,1,-1);

			connection(HDefectRgnCopy, &HDefectRgnCopy);
			count_obj(HDefectRgnCopy, &HdefectCount);

			LensCountSmall = LensCountMid = LensCountBiG = 0;

			for (i=0; i<HdefectCount; i++)
			{
				select_obj (HDefectRgnCopy, &Select_ObjectRegion, i+1);

				intersection(HThresRegion,Select_ObjectRegion,&IntersectionObj1);

				if (THEAPP.m_pGFunction->ValidHRegion(IntersectionObj1))		// 렌즈 검사 영역과 만난다면
				{
					area_center (IntersectionObj1, &Area, &Row, &Column);
					double dArea = ceil((Area[0].D() * PXLAREA)/1000)/1000;		////////// 렌즈 검사 영역 내부의 불량 면적만 따짐

					////////////////////////////////// Changed for CMI3000 2000 ====> ///////////////////////////////////////////////////
					if (dArea > 0 && dArea < THEAPP.m_pModelDataManager->m_dLensDirtMinArea)
					{
						concat_obj(HDirtSmallDefectRgn, IntersectionObj1, &HDirtSmallDefectRgn);
						LensCountSmall++;
					}
					if (dArea >= THEAPP.m_pModelDataManager->m_dLensDirtMinArea)
					{
						concat_obj(HDirtMidDefectRgn, IntersectionObj1, &HDirtMidDefectRgn);
						LensCountMid++;
					}
					if (dArea >= THEAPP.m_pModelDataManager->m_dLensDirtMidArea)
					{
						concat_obj(HDirtBigDefectRgn, IntersectionObj1, &HDirtBigDefectRgn);
						LensCountBiG++;
					}
					if (dArea >= THEAPP.m_pModelDataManager->m_dLensDirtMaxArea)
					{
						concat_obj(HDirtMaxDefectRgn, IntersectionObj1, &HDirtMaxDefectRgn);
						SortingDefectNumber=5;
					}
				}
			}

			if(SortingDefectNumber <= 0)
			{
				if(LensCountBiG > THEAPP.m_pModelDataManager->m_iLensDirtMaxNumAccept)
				{
					concat_obj(*pHLensMinDefectRgn, HDirtBigDefectRgn, pHLensMinDefectRgn);
					SortingDefectNumber = 5;
				}
				else
				{
					if(LensCountMid > THEAPP.m_pModelDataManager->m_iLensDirtMidNumAccept)
					{
						concat_obj(*pHLensMinDefectRgn, HDirtMidDefectRgn, pHLensMinDefectRgn);
						SortingDefectNumber = 5;
					}
					else
					{
						if(LensCountSmall > THEAPP.m_pModelDataManager->m_iLensDirtMinNumAccept)
						{
							concat_obj(*pHLensMinDefectRgn, HDirtSmallDefectRgn, pHLensMinDefectRgn);
							SortingDefectNumber = 5;
						}
					}
				}
			}
			else
				concat_obj(*pHLensMinDefectRgn, HDirtMaxDefectRgn, pHLensMinDefectRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(*pHLensMinDefectRgn))
			union1(*pHLensMinDefectRgn,pHLensMinDefectRgn);

		if (SortingDefectNumber == -1)
			SortingDefectNumber = 1;

		return SortingDefectNumber;
	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [Algorithm ApplyLensInspectionCondition] : %s", except.message);
		THEAPP.SaveLog(str);
	}
}


int Algorithm::SortingDefectRegion (Hobject HRgnOrg, Hobject *HRgnMin, Hobject *HRgnMid, double dAreaMin, double dAreaMid, double dAreaMax, int iAcceptMin, int iAcceptMid, int iAcceptMax)
{
	Hobject HRgnOrg_conn, HRgnOrg_select;
	union1(HRgnOrg, &HRgnOrg);
	connection(HRgnOrg, &HRgnOrg_conn);

	Hlong lNoDefect;
	int  SortingDefectNumber = -1;

	// >= MinArea
	double dFloorArea = dAreaMin*10000;
	int iAreaDecison = int((floor(dFloorArea + 0.5f)-1)/10)*1000/PXLAREA;
	gen_empty_obj(&HRgnOrg_select);
	select_shape(HRgnOrg_conn, &HRgnOrg_select, "area", "and", iAreaDecison, MAX_DEF);
	count_obj(HRgnOrg_select, &lNoDefect);
	
	if (lNoDefect <= 0) {
		SortingDefectNumber = -1;
		return SortingDefectNumber;
	}
	if (lNoDefect > 0)
	{
		union1(HRgnOrg_select, HRgnMin);	// MinArea 이상인 건 모두 선택한다
		SortingDefectNumber = 0;

		if (lNoDefect > iAcceptMid)
		{
			SortingDefectNumber = 3;
			return SortingDefectNumber;
		}
	}

	// >= MidArea
	dFloorArea = dAreaMid*10000;
	iAreaDecison = int((floor(dFloorArea + 0.5f)-1)/10)*1000/PXLAREA;
	gen_empty_obj(&HRgnOrg_select);
	select_shape(HRgnOrg_conn, &HRgnOrg_select, "area", "and", iAreaDecison, MAX_DEF);
	count_obj(HRgnOrg_select, &lNoDefect);
	if (lNoDefect > 0)
	{
		union1(HRgnOrg_select, HRgnMid);	// MidArea 이상인 건 모두 선택한다

		if (lNoDefect > iAcceptMax)
		{
			SortingDefectNumber = 2;
			return SortingDefectNumber;
		}
	}

	// >= MaxArea
	dFloorArea = dAreaMax*10000;
	iAreaDecison = int((floor(dFloorArea + 0.5f)-1)/10)*1000/PXLAREA;
	gen_empty_obj(&HRgnOrg_select);
	select_shape(HRgnOrg_conn, &HRgnOrg_select, "area", "and", iAreaDecison, MAX_DEF);
	count_obj(HRgnOrg_select, &lNoDefect);
	if (lNoDefect > 0)
	{
		SortingDefectNumber = 1;
		return SortingDefectNumber;
	}

	return SortingDefectNumber;
}

/////////** RefImage를 참조해서 선명한 defect 처리 ** ////////////////////
int Algorithm::SortingDefectRegion (Hobject HRgnOrg, Hobject HImgRef, Hobject *HRgnMin, Hobject *HRgnMid, double dAreaMin, double dAreaMid, double dAreaMax, int iAcceptMin, int iAcceptMid, int iAcceptMax, int iAvrBright)
{
	Hobject HRgnOrg_conn, HRgnOrg_select;
	union1(HRgnOrg, &HRgnOrg);
	connection(HRgnOrg, &HRgnOrg_conn);

	Hlong lNoDefect;
	int  SortingDefectNumber = -1;

	// >= MinArea
	double dFloorArea = dAreaMin*10000;
	int iAreaDecison = int((floor(dFloorArea + 0.5f)-1)/10)*1000/PXLAREA;
	gen_empty_obj(&HRgnOrg_select);
	select_shape(HRgnOrg_conn, &HRgnOrg_select, "area", "and", iAreaDecison, MAX_DEF);
	count_obj(HRgnOrg_select, &lNoDefect);
	
	if (lNoDefect <= 0) {
		SortingDefectNumber = -1;
		return SortingDefectNumber;
	}
	if (lNoDefect > 0)
	{
		union1(HRgnOrg_select, HRgnMin);	// MinArea 이상인 건 모두 선택한다
		SortingDefectNumber = 0;

		if (lNoDefect > iAcceptMid)
		{
			SortingDefectNumber = 3;
			return SortingDefectNumber;
		}

		for (int i=0; i<lNoDefect; i++)
		{
			Hobject HRgn_select;
			select_obj(HRgnOrg_select, &HRgn_select, i+1);

			if(THEAPP.m_pGFunction->ValidHRegion(HRgn_select)==FALSE) continue;


			double dMean = 0, dSigma = 0;
			intensity (HRgn_select, HImgRef, &dMean, &dSigma);
			if ((int)dMean >= iAvrBright)
			{
				SortingDefectNumber = 3;
				return SortingDefectNumber;
			}
		}
	}

	// >= MidArea
	dFloorArea = dAreaMid*10000;
	iAreaDecison = int((floor(dFloorArea + 0.5f)-1)/10)*1000/PXLAREA;
	gen_empty_obj(&HRgnOrg_select);
	select_shape(HRgnOrg_conn, &HRgnOrg_select, "area", "and", iAreaDecison, MAX_DEF);
	count_obj(HRgnOrg_select, &lNoDefect);
	if (lNoDefect > 0)
	{
		union1(HRgnOrg_select, HRgnMid);	// MidArea 이상인 건 모두 선택한다

		if (lNoDefect > iAcceptMax)
		{
			SortingDefectNumber = 2;
			return SortingDefectNumber;
		}
	}

	// >= MaxArea
	dFloorArea = dAreaMax*10000;
	iAreaDecison = int((floor(dFloorArea + 0.5f)-1)/10)*1000/PXLAREA;
	gen_empty_obj(&HRgnOrg_select);
	select_shape(HRgnOrg_conn, &HRgnOrg_select, "area", "and", iAreaDecison, MAX_DEF);
	count_obj(HRgnOrg_select, &lNoDefect);
	if (lNoDefect > 0)
	{
		SortingDefectNumber = 1;
		return SortingDefectNumber;
	}

	return SortingDefectNumber;
}

/////////** RefImage 두개를 참조해서 선명한 defect 처리 ** ////////////////////
int Algorithm::SortingDefectRegion (Hobject HRgnOrg, Hobject HImgRef1, Hobject HImgRef2, BOOL bAnd, Hobject *HRgnMin, Hobject *HRgnMid, double dAreaMin, double dAreaMid, double dAreaMax, int iAcceptMin, int iAcceptMid, int iAcceptMax, int iAvrBright)
{
	Hobject HRgnOrg_conn, HRgnOrg_select;
	union1(HRgnOrg, &HRgnOrg);
	connection(HRgnOrg, &HRgnOrg_conn);

	Hlong lNoDefect;
	int  SortingDefectNumber = -1;

	// >= MinArea
	double dFloorArea = dAreaMin*10000;
	int iAreaDecison = int((floor(dFloorArea + 0.5f)-1)/10)*1000/PXLAREA;
	gen_empty_obj(&HRgnOrg_select);
	select_shape(HRgnOrg_conn, &HRgnOrg_select, "area", "and", iAreaDecison, MAX_DEF);
	count_obj(HRgnOrg_select, &lNoDefect);
	
	if (lNoDefect <= 0) {
		SortingDefectNumber = -1;
		return SortingDefectNumber;
	}
	if (lNoDefect > 0)
	{
		union1(HRgnOrg_select, HRgnMin);	// MinArea 이상인 건 모두 선택한다
		SortingDefectNumber = 0;

		if (lNoDefect > iAcceptMid)
		{
			SortingDefectNumber = 3;
			return SortingDefectNumber;
		}

		for (int i=0; i<lNoDefect; i++)
		{
			Hobject HRgn_select;
			select_obj(HRgnOrg_select, &HRgn_select, i+1);

			if(THEAPP.m_pGFunction->ValidHRegion(HRgn_select)==FALSE) continue;


			double dMean1 = 0, dMean2 = 0, dSigma1 = 0, dSigma2 = 0;
			intensity (HRgn_select, HImgRef1, &dMean1, &dSigma1);
			intensity (HRgn_select, HImgRef2, &dMean2, &dSigma2);

			BOOL bTRUE = FALSE;
			if (bAnd == TRUE) bTRUE = ((int)dMean1 >= iAvrBright) && ((int)dMean2 >= iAvrBright);
			else bTRUE = ((int)dMean1 >= iAvrBright) || ((int)dMean2 >= iAvrBright);
			if (bTRUE == TRUE)
			{
				SortingDefectNumber = 3;
				return SortingDefectNumber;
			}
		}
	}

	// >= MidArea
	dFloorArea = dAreaMid*10000;
	iAreaDecison = int((floor(dFloorArea + 0.5f)-1)/10)*1000/PXLAREA;
	gen_empty_obj(&HRgnOrg_select);
	select_shape(HRgnOrg_conn, &HRgnOrg_select, "area", "and", iAreaDecison, MAX_DEF);
	count_obj(HRgnOrg_select, &lNoDefect);
	if (lNoDefect > 0)
	{
		union1(HRgnOrg_select, HRgnMid);	// MidArea 이상인 건 모두 선택한다

		if (lNoDefect > iAcceptMax)
		{
			SortingDefectNumber = 2;
			return SortingDefectNumber;
		}
	}

	// >= MaxArea
	dFloorArea = dAreaMax*10000;
	iAreaDecison = int((floor(dFloorArea + 0.5f)-1)/10)*1000/PXLAREA;
	gen_empty_obj(&HRgnOrg_select);
	select_shape(HRgnOrg_conn, &HRgnOrg_select, "area", "and", iAreaDecison, MAX_DEF);
	count_obj(HRgnOrg_select, &lNoDefect);
	if (lNoDefect > 0)
	{
		SortingDefectNumber = 1;
		return SortingDefectNumber;
	}

	return SortingDefectNumber;
}

int Algorithm::SortingDefectRegion_AreaSum (Hobject HRgnOrg, Hobject *HRgnDefect, double dAreaMin, double dAreaMax)
{
	Hobject Copy_obj_Himage;
	HTuple HdefectCount;
	HTuple Area, Row, Column;

	union1(HRgnOrg, &HRgnOrg);

	int  SortingDefectNumber = -1;


	area_center(HRgnOrg, &Area, &Row, &Column);
	double dAreaTotal = 0;
	dAreaTotal = ceil((Area[0].D() * PXLAREA)/1000)/1000;


	if (dAreaTotal > dAreaMax)						// 영역 합이 Max 이상이면 1번 처리
	{
		union1(HRgnOrg, HRgnDefect);
		SortingDefectNumber = 1;
		return SortingDefectNumber;
	}
	if (dAreaTotal > dAreaMin)
	{
		//union1(HRgnOrg, HRgnDefect);				// MinArea 이상이면 색상으로 디스플레이 한다
		SortingDefectNumber = 0;
		return SortingDefectNumber;
	}

	SortingDefectNumber = -1;	// 안전빵
	return SortingDefectNumber;
}


BOOL Algorithm::BlobUnion(Hobject *pInputRgn, long lMergeLength)
{
	try
	{
		long lDilationSize;

		lDilationSize = lMergeLength/2;
		if ((lMergeLength%2)==1)
			lDilationSize += 1;

		Hobject HConnInputRgn, HUnionInputRgn, HDilatedRgn, HSelectedRgn;

		connection(*pInputRgn, &HConnInputRgn);

		union1(*pInputRgn, &HUnionInputRgn);
		dilation_circle(HUnionInputRgn, &HDilatedRgn, lDilationSize);
		connection(HDilatedRgn, &HDilatedRgn);

		Hlong lNoBlob, lNoObj;
		Hobject HUnionRgn;

		count_obj(HDilatedRgn, &lNoBlob);

		gen_empty_obj(pInputRgn);

		int i, iIndex1, iIndex2;
		Hobject HConnectRgn, HObjectSelected1, HObjectSelected2, HConnectLineRgn;
		HTuple HRow, HCol;
		double dMinDistance;
		Hlong lRow1, lRow2, lCol1, lCol2;

		for (i=1; i<=lNoBlob; i++)
		{
			select_obj(HDilatedRgn, &HSelectedRgn, i);
			select_shape_proto(HConnInputRgn, HSelectedRgn, &HUnionRgn, "overlaps_abs", 1, MAX_DEF);

			lNoObj = 0;
			count_obj(HUnionRgn, &lNoObj);

			if (lNoObj>1)
			{
				gen_empty_obj(&HConnectRgn);
				for (iIndex1=1; iIndex1<=(lNoObj-1); iIndex1++)
				{
					select_obj(HUnionRgn, &HObjectSelected1, iIndex1);
					for (iIndex2=iIndex1+1; iIndex2<=lNoObj; iIndex2++)
					{
						select_obj(HUnionRgn, &HObjectSelected2, iIndex2);
						distance_rr_min(HObjectSelected1, HObjectSelected2, &dMinDistance, &lRow1, &lCol1, &lRow2, &lCol2);

						if (dMinDistance <= (double)lMergeLength)
						{
							tuple_gen_const(0, 0, &HRow);
							tuple_gen_const(0, 0, &HCol);
							tuple_concat(HRow, HTuple(lRow1), &HRow);
							tuple_concat(HRow, HTuple(lRow2), &HRow);
							tuple_concat(HCol, HTuple(lCol1), &HCol);
							tuple_concat(HCol, HTuple(lCol2), &HCol);
							gen_region_polygon(&HConnectLineRgn, HRow, HCol);
							concat_obj(HConnectRgn, HConnectLineRgn, &HConnectRgn);
						}
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HConnectRgn)==TRUE)
					concat_obj(HUnionRgn, HConnectRgn, &HUnionRgn);

				union1(HUnionRgn, &HUnionRgn);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HUnionRgn))
				concat_obj(*pInputRgn, HUnionRgn, pInputRgn);
		}



		return TRUE;

	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [BlobUnion] : %s", except.message);
		THEAPP.SaveLog(str);

		return FALSE;
	}
}











Hobject Algorithm::CutDefectRegionToImage(Hobject HImage,Hobject HRegion)
{
	try{
		THEAPP.SaveLog("CutDefectRegionToImage 들어옴");
		Hobject HCutReduceImage;
		Hobject HThresRegion,HDifferenceRegion;

		threshold(HImage,&HThresRegion,0,255);
		difference(HThresRegion,HRegion,&HDifferenceRegion);
		reduce_domain(HImage,HDifferenceRegion,&HCutReduceImage);

		return HCutReduceImage;
	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [Algorithm CutDefectRegionToImage] : %s", except.message);
		THEAPP.SaveLog(str);
	}
}











/*
BOOL Algorithm::BlobUnionAngle(Hobject *pInputRgn, long lMergeLength, long lOrientationMinDiameter)
{
	try
	{
		long lDilationSize;

		lDilationSize = lMergeLength/2;
		if ((lMergeLength%2)==1)
			lDilationSize += 1;

		Hobject HConnInputRgn, HUnionInputRgn, HDilatedRgn, HSelectedRgn;

		connection(*pInputRgn, &HConnInputRgn);

		union1(*pInputRgn, &HUnionInputRgn);
		dilation_circle(HUnionInputRgn, &HDilatedRgn, lDilationSize);
		connection(HDilatedRgn, &HDilatedRgn);

		Hlong lNoBlob, lNoObj;
		Hobject HUnionRgn;

		count_obj(HDilatedRgn, &lNoBlob);

		gen_empty_obj(pInputRgn);

		int i, iIndex1, iIndex2;
		Hobject HConnectRgn, HObjectSelected1, HObjectSelected2, HConnectLineRgn;
		HTuple HRow, HCol;
		double dMinDistance;
		Hlong lRow1, lRow2, lCol1, lCol2;

		Hlong lMaxLengthIndex;
		HTuple HRow1, HRow2, HCol1, HCol2, HDiameter;
		HTuple HIndex, HInverted;
		double dMaxLengthDiameter;
		Hobject HMaxDiameterRgn, HDirectionalSelectedRgn, HOrientedRectRgn;
		double dRect2CenterY, dRect2CenterX, dPhi, dLength1, dLength2;
		Hlong lNoSelectedObj;

		double dHalfLengthTol = 200.0;
		double dHalfWidthTol = 15.0;

		for (i=1; i<=lNoBlob; i++)
		{
			select_obj(HDilatedRgn, &HSelectedRgn, i);
			select_shape_proto(HConnInputRgn, HSelectedRgn, &HUnionRgn, "overlaps_abs", 1, MAX_DEF);

			lNoObj = 0;
			count_obj(HUnionRgn, &lNoObj);

			if (lNoObj>1)
			{
				diameter_region(HUnionRgn, &HRow1, &HCol1, &HRow2, &HCol2, &HDiameter);
				tuple_sort_index(HDiameter, &HIndex);
				tuple_inverse(HIndex, &HInverted);

				lMaxLengthIndex = HInverted[0].L();
				dMaxLengthDiameter = HDiameter[lMaxLengthIndex].D();
				if (dMaxLengthDiameter < (double)lOrientationMinDiameter)
					continue;

				select_obj(HUnionRgn, &HMaxDiameterRgn, lMaxLengthIndex+1);

				smallest_rectangle2(HMaxDiameterRgn, &dRect2CenterY, &dRect2CenterX, &dPhi, &dLength1, &dLength2);
				gen_rectangle2(&HOrientedRectRgn, dRect2CenterY, dRect2CenterX, dPhi, dHalfLengthTol, dHalfWidthTol);  

				gen_empty_obj(&HDirectionalSelectedRgn);
				select_shape_proto(HUnionRgn, HOrientedRectRgn, &HDirectionalSelectedRgn, "overlaps_abs", 1, MAX_DEF);

				lNoSelectedObj = 0;
				count_obj(HDirectionalSelectedRgn, &lNoSelectedObj);

				if (lNoSelectedObj<=1)
				{
					concat_obj(*pInputRgn, HUnionRgn, pInputRgn);
					continue;
				}	

				gen_empty_obj(&HConnectRgn);
				for (iIndex1=1; iIndex1<=(lNoSelectedObj-1); iIndex1++)
				{
					select_obj(HDirectionalSelectedRgn, &HObjectSelected1, iIndex1);
					for (iIndex2=iIndex1+1; iIndex2<=lNoSelectedObj; iIndex2++)
					{
						select_obj(HDirectionalSelectedRgn, &HObjectSelected2, iIndex2);
						distance_rr_min(HObjectSelected1, HObjectSelected2, &dMinDistance, &lRow1, &lCol1, &lRow2, &lCol2);

						if (dMinDistance <= (double)lMergeLength)
						{
							tuple_gen_const(0, 0, &HRow);
							tuple_gen_const(0, 0, &HCol);
							tuple_concat(HRow, HTuple(lRow1), &HRow);
							tuple_concat(HRow, HTuple(lRow2), &HRow);
							tuple_concat(HCol, HTuple(lCol1), &HCol);
							tuple_concat(HCol, HTuple(lCol2), &HCol);
							gen_region_polygon(&HConnectLineRgn, HRow, HCol);
							concat_obj(HConnectRgn, HConnectLineRgn, &HConnectRgn);
						}
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HConnectRgn)==TRUE)
					concat_obj(HDirectionalSelectedRgn, HConnectRgn, &HDirectionalSelectedRgn);

				union1(HDirectionalSelectedRgn, &HDirectionalSelectedRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HDirectionalSelectedRgn))
					concat_obj(*pInputRgn, HDirectionalSelectedRgn, pInputRgn);
			}
			else
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HUnionRgn))
					concat_obj(*pInputRgn, HUnionRgn, pInputRgn);
			}
		}

		return TRUE;

	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [BlobUnionAngle] : %s", except.message);
		THEAPP.SaveLog(str);

		return FALSE;
	}
}
*/

BOOL Algorithm::BlobUnionAngle(Hobject *pInputRgn, long lMergeLength, long lOrientationMinDiameter)
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (bDebugSave)
		{
			write_region(*pInputRgn, "c:\\DualTest\\BlobUnion.reg");
		}

		long lDilationSize;

		lDilationSize = lMergeLength/2;
		if ((lMergeLength%2)==1)
			lDilationSize += 1;

		Hobject HConnInputRgn, HUnionInputRgn, HDilatedRgn, HSelectedRgn;

		connection(*pInputRgn, &HConnInputRgn);

		union1(*pInputRgn, &HUnionInputRgn);
		dilation_circle(HUnionInputRgn, &HDilatedRgn, lDilationSize);
		connection(HDilatedRgn, &HDilatedRgn);

		Hlong lNoBlob, lNoObj;
		Hobject HUnionRgn;

		count_obj(HDilatedRgn, &lNoBlob);

		gen_empty_obj(pInputRgn);

		int i, iIndex1, iIndex2;
		Hobject HConnectRgn, HObjectSelected1, HObjectSelected2, HConnectLineRgn;
		HTuple HRow, HCol;
		double dMinDistance;
		Hlong lRow1, lRow2, lCol1, lCol2;

		Hlong lMaxLengthIndex;
		HTuple HRow1, HRow2, HCol1, HCol2, HDiameter;
		HTuple HIndex, HInverted;
		double dMaxLengthDiameter;
		Hobject HMaxDiameterRgn, HDirectionalSelectedRgn, HOrientedRectRgn;
		double dRect2CenterY, dRect2CenterX, dPhi, dLength1, dLength2;
		Hlong lNoSelectedObj;

		double dHalfLengthTol = 200.0;
		double dHalfWidthTol = 15.0;

		for (i=1; i<=lNoBlob; i++)
		{
			select_obj(HDilatedRgn, &HSelectedRgn, i);
			select_shape_proto(HConnInputRgn, HSelectedRgn, &HUnionRgn, "overlaps_abs", 1, MAX_DEF);

			lNoObj = 0;
			count_obj(HUnionRgn, &lNoObj);

			if (lNoObj>1)
			{
				diameter_region(HUnionRgn, &HRow1, &HCol1, &HRow2, &HCol2, &HDiameter);
				tuple_sort_index(HDiameter, &HIndex);
				tuple_inverse(HIndex, &HInverted);

				lMaxLengthIndex = HInverted[0].L();
				dMaxLengthDiameter = HDiameter[lMaxLengthIndex].D();
				if (dMaxLengthDiameter < (double)lOrientationMinDiameter)
					continue;

				//select_obj(HUnionRgn, &HMaxDiameterRgn, lMaxLengthIndex+1);

				union1(HUnionRgn, &HMaxDiameterRgn);

				smallest_rectangle2(HMaxDiameterRgn, &dRect2CenterY, &dRect2CenterX, &dPhi, &dLength1, &dLength2);
				gen_rectangle2(&HOrientedRectRgn, dRect2CenterY, dRect2CenterX, dPhi, dHalfLengthTol, dHalfWidthTol);  

				gen_empty_obj(&HDirectionalSelectedRgn);
				select_shape_proto(HUnionRgn, HOrientedRectRgn, &HDirectionalSelectedRgn, "overlaps_abs", 1, MAX_DEF);

				lNoSelectedObj = 0;
				count_obj(HDirectionalSelectedRgn, &lNoSelectedObj);

				if (lNoSelectedObj<=1)
				{
					concat_obj(*pInputRgn, HUnionRgn, pInputRgn);
					continue;
				}	

				gen_empty_obj(&HConnectRgn);
				for (iIndex1=1; iIndex1<=(lNoSelectedObj-1); iIndex1++)
				{
					select_obj(HDirectionalSelectedRgn, &HObjectSelected1, iIndex1);
					for (iIndex2=iIndex1+1; iIndex2<=lNoSelectedObj; iIndex2++)
					{
						select_obj(HDirectionalSelectedRgn, &HObjectSelected2, iIndex2);
						distance_rr_min(HObjectSelected1, HObjectSelected2, &dMinDistance, &lRow1, &lCol1, &lRow2, &lCol2);

						if (dMinDistance <= (double)lMergeLength)
						{
							tuple_gen_const(0, 0, &HRow);
							tuple_gen_const(0, 0, &HCol);
							tuple_concat(HRow, HTuple(lRow1), &HRow);
							tuple_concat(HRow, HTuple(lRow2), &HRow);
							tuple_concat(HCol, HTuple(lCol1), &HCol);
							tuple_concat(HCol, HTuple(lCol2), &HCol);
							gen_region_polygon(&HConnectLineRgn, HRow, HCol);
							concat_obj(HConnectRgn, HConnectLineRgn, &HConnectRgn);
						}
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HConnectRgn)==TRUE)
					concat_obj(HDirectionalSelectedRgn, HConnectRgn, &HDirectionalSelectedRgn);

				union1(HDirectionalSelectedRgn, &HDirectionalSelectedRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HDirectionalSelectedRgn))
					concat_obj(*pInputRgn, HDirectionalSelectedRgn, pInputRgn);
			}
			else
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HUnionRgn))
					concat_obj(*pInputRgn, HUnionRgn, pInputRgn);
			}
		}

		return TRUE;

	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [BlobUnionAngle] : %s", except.message);
		THEAPP.SaveLog(str);

		return FALSE;
	}
}


BOOL Algorithm::BlobLengthWidthCondition(Hobject *pInputRgn, BOOL bUseLengthCondition, BOOL bUseWidthCondition, int iLengthValue, int iWidthValue)
{
	try
	{
		Hobject HConnInputRgn, HSelectedRgn;
		Hlong lNoBlob, lNoObj;

		connection(*pInputRgn, &HConnInputRgn);
		count_obj(HConnInputRgn, &lNoBlob);
		gen_empty_obj(pInputRgn);

		int i;
		Hlong lRow1, lRow2, lCol1, lCol2;
		double dBlobLength, dBlobWidth;
		double dCircleRow, dCircleCol, dCircleRadius;
		BOOL bLengthConditionPass, bWidthConditionPass;

		for (i=1; i<=lNoBlob; i++)
		{
			select_obj(HConnInputRgn, &HSelectedRgn, i);

			if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn)==FALSE)
				continue;

			bLengthConditionPass = TRUE;
			bWidthConditionPass = TRUE;

			diameter_region(HSelectedRgn, &lRow1, &lCol1, &lRow2, &lCol2, &dBlobLength);
			inner_circle(HSelectedRgn, &dCircleRow, &dCircleCol, &dCircleRadius);
			dBlobWidth =  dCircleRadius * 2.0;

			if (bUseLengthCondition && (dBlobLength<(double)iLengthValue))
				bLengthConditionPass = FALSE;

			if (bUseWidthCondition && (dBlobWidth<(double)iWidthValue))
				bWidthConditionPass = FALSE;

			if (bLengthConditionPass==TRUE && bWidthConditionPass==TRUE)
				concat_obj(*pInputRgn, HSelectedRgn, pInputRgn);
		}

		return TRUE;

	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [BlobLengthWidthCondition] : %s", except.message);
		THEAPP.SaveLog(str);

		return FALSE;
	}
}





Hobject Algorithm::Connection_nearRgn (Hobject HConnecedRgn)
{
	try{

		while(TRUE)
		{
			if(m_pCheckFinishConnection)
			{
				Sleep(1);
			}
			else
			{
				break;
			}
		}
		m_pCheckFinishConnection = TRUE;
		THEAPP.SaveLog("Connection_nearRgn 들어옴");
		// Local iconic variables 
		Hobject  ObjectSelected1, ObjectSelected2, WBConnectedRegions;
		Hobject  ObjContours1, ObjContours2, ContEllipse1, ContEllipse2;
		Hobject  ContEllipseRegion1, ContEllipseRegion2, ObjContSelRegionSkeleton1;
		Hobject  ObjContSelRegionSkeleton2, SelRegionSkeletonContours1;
		Hobject  SelRegionSkeletonContours2, ConnectRegionLines;
		Hobject  SelectedLineRegion, ConnectRegionLinesDilation;


		// Local control variables 
		HTuple  WBConnectedNumber;
		HTuple  ResultContoursPhi, DistanceArray, CNT, Flag, Index;
		HTuple  Ob1_Area, Ob1_Row, Ob1_Column, Index1, Ob2_Area;
		HTuple  Ob2_Row, Ob2_Column, Contours1_Row, Contours1_Column;
		HTuple  Contours1_Phi, Contours1_Radius1, Contours1_Radius2;
		HTuple  Contours1_StartPhi, Contours1_EndPhi, Contours1_PointOrder;
		HTuple  Contours2_Row, Contours2_Column, Contours2_Phi;
		HTuple  Contours2_Radius1, Contours2_Radius2, Contours2_StartPhi;
		HTuple  Contours2_EndPhi, Contours2_PointOrder, SkeletonContoursRowBegin1;
		HTuple  SkeletonContoursColBegin1, SkeletonContoursRowEnd1;
		HTuple  SkeletonContoursColEnd1, Nr1, Nc1, Dist1, SkeletonContoursRowBegin2;
		HTuple  SkeletonContoursColBegin2, SkeletonContoursRowEnd2;
		HTuple  SkeletonContoursColEnd2, Nr2, Nc2, Dist2, DistanceVal1;
		HTuple  DistanceSqrt1, DistanceVal2, DistanceSqrt2, DistanceVal3;
		HTuple  DistanceSqrt3, DistanceVal4, DistanceSqrt4, DistanceBOOL;
		HTuple  SelectedLineRegionArea, SelectedLineRegionRow, SelectedLineRegionColumn;
		HTuple  Area1, Row5, Column5;
		HTuple  Area2, Row6, Column6;







		WBConnectedRegions = HConnecedRgn;
		connection(WBConnectedRegions,&WBConnectedRegions);
		count_obj(WBConnectedRegions,&WBConnectedNumber);


		ResultContoursPhi = 0.0;
		DistanceArray = HTuple();
		CNT = 0;
		Flag = 1;
		//* 
		if (0 != WBConnectedNumber)
		{
			for (Index=1; Index<=WBConnectedNumber; Index+=1)
			{
				Flag = 0;
				select_obj(WBConnectedRegions, &ObjectSelected1, Index);
				connection(ObjectSelected1, &ObjectSelected1);
				select_shape_std(ObjectSelected1, &ObjectSelected1, "max_area", 70);
				area_center(ObjectSelected1, &Ob1_Area, &Ob1_Row, &Ob1_Column);
				if (Ob1_Area>25)
				{
					dilation_circle(ObjectSelected1, &ObjectSelected1, 2);
					gen_contour_region_xld(ObjectSelected1, &ObjContours1, "border");
					for (Index1=Index+1; Index1<=WBConnectedNumber; Index1+=1)
					{
						select_obj(WBConnectedRegions, &ObjectSelected2, Index1);
						connection(ObjectSelected2, &ObjectSelected2);
						select_shape_std(ObjectSelected2, &ObjectSelected2, "max_area", 70);
						area_center(ObjectSelected2, &Ob2_Area, &Ob2_Row, &Ob2_Column);
						if (Ob2_Area>25)
						{
							//*                         
							//*                         
							gen_contour_region_xld(ObjectSelected2, &ObjContours2, "border");
							fit_ellipse_contour_xld(ObjContours1, "fitzgibbon", -1, 0, 0, 200, 
								3, 2, &Contours1_Row, &Contours1_Column, &Contours1_Phi, &Contours1_Radius1, 
								&Contours1_Radius2, &Contours1_StartPhi, &Contours1_EndPhi, &Contours1_PointOrder);
							fit_ellipse_contour_xld(ObjContours2, "fitzgibbon", -1, 0, 0, 200, 
								3, 2, &Contours2_Row, &Contours2_Column, &Contours2_Phi, &Contours2_Radius1, 
								&Contours2_Radius2, &Contours2_StartPhi, &Contours2_EndPhi, &Contours2_PointOrder);
							gen_ellipse_contour_xld(&ContEllipse1, Contours1_Row, Contours1_Column, 
								Contours1_Phi, Contours1_Radius1, Contours1_Radius2, Contours1_StartPhi, 
								Contours1_EndPhi, Contours1_PointOrder, 1.5);
							gen_ellipse_contour_xld(&ContEllipse2, Contours2_Row, Contours2_Column, 
								Contours2_Phi, Contours2_Radius1, Contours2_Radius2, Contours2_StartPhi, 
								Contours2_EndPhi, Contours2_PointOrder, 1.5);
							gen_region_contour_xld(ContEllipse1, &ContEllipseRegion1, "filled");
							gen_region_contour_xld(ContEllipse2, &ContEllipseRegion2, "filled");
							skeleton(ContEllipseRegion1, &ObjContSelRegionSkeleton1);
							skeleton(ContEllipseRegion2, &ObjContSelRegionSkeleton2);
							area_center(ObjContSelRegionSkeleton1, &Area1, &Row5, &Column5);
							//*                         
							area_center(ObjContSelRegionSkeleton2, &Area2, &Row6, &Column6);
							if (Area1<5)
							{
								continue;
							}
							if (Area2<5)
							{
								continue;
							}
							//*                         
							gen_contours_skeleton_xld(ObjContSelRegionSkeleton1, &SelRegionSkeletonContours1, 
								1, "generalize1");
							gen_contours_skeleton_xld(ObjContSelRegionSkeleton2, &SelRegionSkeletonContours2, 
								1, "generalize1");
							//*                         
							union_adjacent_contours_xld(SelRegionSkeletonContours1, &SelRegionSkeletonContours1, 
								10, 10, "attr_keep");
							union_adjacent_contours_xld(SelRegionSkeletonContours2, &SelRegionSkeletonContours2, 
								10, 10, "attr_keep");
							//*                         
							//*                         
							fit_line_contour_xld(SelRegionSkeletonContours1, "tukey", -1, 0, 1, 
								3, &SkeletonContoursRowBegin1, &SkeletonContoursColBegin1, &SkeletonContoursRowEnd1, 
								&SkeletonContoursColEnd1, &Nr1, &Nc1, &Dist1);
							fit_line_contour_xld(SelRegionSkeletonContours2, "tukey", -1, 0, 1, 
								3, &SkeletonContoursRowBegin2, &SkeletonContoursColBegin2, &SkeletonContoursRowEnd2, 
								&SkeletonContoursColEnd2, &Nr2, &Nc2, &Dist2);
							//*                         
							DistanceVal1 = ((SkeletonContoursRowBegin1-SkeletonContoursRowBegin2)*(SkeletonContoursRowBegin1-SkeletonContoursRowBegin2))+((SkeletonContoursColBegin1-SkeletonContoursColBegin2)*(SkeletonContoursColBegin1-SkeletonContoursColBegin2));
							tuple_sqrt(DistanceVal1, &DistanceSqrt1);
							DistanceVal2 = ((SkeletonContoursRowEnd1-SkeletonContoursRowBegin2)*(SkeletonContoursRowEnd1-SkeletonContoursRowBegin2))+((SkeletonContoursColEnd1-SkeletonContoursColBegin2)*(SkeletonContoursColEnd1-SkeletonContoursColBegin2));
							tuple_sqrt(DistanceVal2, &DistanceSqrt2);
							DistanceVal3 = ((SkeletonContoursRowBegin1-SkeletonContoursRowEnd2)*(SkeletonContoursRowBegin1-SkeletonContoursRowEnd2))+((SkeletonContoursColBegin1-SkeletonContoursColEnd2)*(SkeletonContoursColBegin1-SkeletonContoursColEnd2));
							tuple_sqrt(DistanceVal3, &DistanceSqrt3);
							DistanceVal4 = ((SkeletonContoursRowEnd1-SkeletonContoursRowEnd2)*(SkeletonContoursRowEnd1-SkeletonContoursRowEnd2))+((SkeletonContoursColEnd1-SkeletonContoursColEnd2)*(SkeletonContoursColEnd1-SkeletonContoursColEnd2));
							tuple_sqrt(DistanceVal4, &DistanceSqrt4);
							//*                         
							DistanceBOOL = 1;
							//*                         
							if (0 != DistanceBOOL)
							{
								if (DistanceSqrt1<Distance_referValue)
								{
									DistanceBOOL = 0;
									Ob1_Row = SkeletonContoursRowBegin1;
									Ob1_Column = SkeletonContoursColBegin1;
									Ob2_Row = SkeletonContoursRowBegin2;
									Ob2_Column = SkeletonContoursColBegin2;
								}
							}
							//*                         
							//*                         
							if (0 != DistanceBOOL)
							{
								if (DistanceSqrt2<Distance_referValue)
								{
									DistanceBOOL = 0;
									Ob1_Row = SkeletonContoursRowEnd1;
									Ob1_Column = SkeletonContoursColEnd1;
									Ob2_Row = SkeletonContoursRowBegin2;
									Ob2_Column = SkeletonContoursColBegin2;
								}
							}
							//*                         
							//*                         
							if (0 != DistanceBOOL)
							{
								if (DistanceSqrt3<Distance_referValue)
								{
									DistanceBOOL = 0;
									Ob1_Row = SkeletonContoursRowBegin1;
									Ob1_Column = SkeletonContoursColBegin1;
									Ob2_Row = SkeletonContoursRowEnd2;
									Ob2_Column = SkeletonContoursColEnd2;
								}
							}
							//*                         
							//*                         
							if (0 != DistanceBOOL)
							{
								if (DistanceSqrt4<Distance_referValue)
								{
									DistanceBOOL = 0;
									Ob1_Row = SkeletonContoursRowEnd1;
									Ob1_Column = SkeletonContoursColEnd1;
									Ob2_Row = SkeletonContoursRowEnd2;
									Ob2_Column = SkeletonContoursColEnd2;
								}
							}
							//*                         
							//*                         
							if (DistanceBOOL<1)
							{
								//* 



								gen_region_line(&ConnectRegionLines, Ob1_Row[0].L(), Ob1_Column[0].L(), Ob2_Row[0].L(), Ob2_Column[0].L());
								select_shape(ConnectRegionLines, &SelectedLineRegion, "contlength", "and", 1, MAX_DEF);
								//*                             
								area_center(SelectedLineRegion, &SelectedLineRegionArea, &SelectedLineRegionRow, 
									&SelectedLineRegionColumn);
								if (SelectedLineRegionArea>5)
								{
									//*                                 
									//*                                 
									dilation_circle(ConnectRegionLines, &ConnectRegionLinesDilation, 
										2.5);
									concat_obj(WBConnectedRegions, ConnectRegionLinesDilation, &WBConnectedRegions
										);
									union1(WBConnectedRegions, &WBConnectedRegions);
									fill_up(WBConnectedRegions, &WBConnectedRegions);
									connection(WBConnectedRegions, &WBConnectedRegions);
									count_obj(WBConnectedRegions, &WBConnectedNumber);
									SkeletonContoursRowBegin1 = 0;
									SkeletonContoursColBegin1 = 0;
									SkeletonContoursRowEnd1 = 0;
									SkeletonContoursColEnd1 = 0;
									SkeletonContoursRowBegin2 = 0;
									SkeletonContoursColBegin2 = 0;
									SkeletonContoursRowEnd2 = 0;
									SkeletonContoursColEnd2 = 0;
									DistanceSqrt1 = 0;
									DistanceSqrt2 = 0;
									DistanceSqrt3 = 0;
									DistanceSqrt4 = 0;
									DistanceVal1 = 0;
									DistanceVal2 = 0;
									DistanceVal3 = 0;
									DistanceVal4 = 0;
									Flag = 1;
								}
							}
						}
						if (0 != Flag)
						{
							break;
						}
					}
				}
				if (0 != Flag)
				{
					break;
				}
			}

		}
		union1(WBConnectedRegions, &WBConnectedRegions);
		fill_up(WBConnectedRegions, &WBConnectedRegions);
		connection(WBConnectedRegions, &WBConnectedRegions);
		select_shape(WBConnectedRegions, &WBConnectedRegions, "area", "and", 50, MAX_DEF);

		//* 
		m_pCheckFinishConnection = FALSE;

		return WBConnectedRegions;
	}
	catch(HException &except)
	{
		CString str;
		str.Format("Halcon Exception [Algorithm Connection_nearRgn] : %s", except.message);
		THEAPP.SaveLog(str);
	}

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///
///   Go 추가 (2016/06/02)
///
//////////////////////////////////////////////////////////////////////////

BOOL Algorithm::GetInspectArea(Hobject HContour1, Hobject HContour2, Hobject HContour3, Hobject HContour4, Hobject HContour5, CAlgorithmParam Param, Hobject *pHInspectArea)
{
	try
	{
		Hobject HInnerFitRgn, HOuterFitRgn;
		Hobject HContourRgn1, HContourRgn2;
		Hobject HMarginRgn1, HMarginRgn2;
		Hlong lArea1, lArea2;
		double dCenterX, dCenterY;
		Hobject HInspectRgn;
		int iMargin;

		gen_empty_obj(pHInspectArea);

		for (int i=0; i<3; i++)
		{
			if (Param.m_bUseInspectArea[i])
			{
				switch (Param.m_iInspectAreaContour1Number[i])
				{
				case MATCHING_CONTOUR_1:
					{
						if (THEAPP.m_pGFunction->ValidHXLD(HContour1)==FALSE)
							continue;
						gen_region_contour_xld(HContour1, &HContourRgn1, "filled");
						break;
					}
				case MATCHING_CONTOUR_2:
					if (THEAPP.m_pGFunction->ValidHXLD(HContour2)==FALSE)
						continue;
					gen_region_contour_xld(HContour2, &HContourRgn1, "filled");
					break;
				case MATCHING_CONTOUR_3:
					if (THEAPP.m_pGFunction->ValidHXLD(HContour3)==FALSE)
						continue;
					gen_region_contour_xld(HContour3, &HContourRgn1, "filled");
					break;
				case MATCHING_CONTOUR_4:
					if (THEAPP.m_pGFunction->ValidHXLD(HContour4)==FALSE)
						continue;
					gen_region_contour_xld(HContour4, &HContourRgn1, "filled");
					break;
				case MATCHING_CONTOUR_5:
					if (THEAPP.m_pGFunction->ValidHXLD(HContour5)==FALSE)
						continue;
					gen_region_contour_xld(HContour5, &HContourRgn1, "filled");
					break;
				default:
					return FALSE;
				}

				switch (Param.m_iInspectAreaContour2Number[i])
				{
				case MATCHING_CONTOUR_1:
					if (THEAPP.m_pGFunction->ValidHXLD(HContour1)==FALSE)
						continue;
					gen_region_contour_xld(HContour1, &HContourRgn2, "filled");
					break;
				case MATCHING_CONTOUR_2:
					if (THEAPP.m_pGFunction->ValidHXLD(HContour2)==FALSE)
						continue;
					gen_region_contour_xld(HContour2, &HContourRgn2, "filled");
					break;
				case MATCHING_CONTOUR_3:
					if (THEAPP.m_pGFunction->ValidHXLD(HContour3)==FALSE)
						continue;
					gen_region_contour_xld(HContour3, &HContourRgn2, "filled");
					break;
				case MATCHING_CONTOUR_4:
					if (THEAPP.m_pGFunction->ValidHXLD(HContour4)==FALSE)
						continue;
					gen_region_contour_xld(HContour4, &HContourRgn2, "filled");
					break;
				case MATCHING_CONTOUR_5:
					if (THEAPP.m_pGFunction->ValidHXLD(HContour5)==FALSE)
						continue;
					gen_region_contour_xld(HContour5, &HContourRgn2, "filled");
					break;
				default:
					return FALSE;
				}

				if (Param.m_iInspectAreaContour1Margin[i] >= 0)
					dilation_circle(HContourRgn1, &HMarginRgn1, (double)Param.m_iInspectAreaContour1Margin[i] + 0.5);
				else
				{
					iMargin = abs(Param.m_iInspectAreaContour1Margin[i]);
					erosion_circle(HContourRgn1, &HMarginRgn1, (double)iMargin + 0.5);
				}

				if (Param.m_iInspectAreaContour2Margin[i] >= 0)
					dilation_circle(HContourRgn2, &HMarginRgn2, (double)Param.m_iInspectAreaContour2Margin[i] + 0.5);
				else
				{
					iMargin = abs(Param.m_iInspectAreaContour2Margin[i]);
					erosion_circle(HContourRgn2, &HMarginRgn2, (double)iMargin + 0.5);
				}

				area_center(HMarginRgn1, &lArea1, &dCenterY, &dCenterX);
				area_center(HMarginRgn2, &lArea2, &dCenterY, &dCenterX);

				if (lArea1>=lArea2)
					difference(HMarginRgn1, HMarginRgn2, &HInspectRgn);
				else
					difference(HMarginRgn2, HMarginRgn1, &HInspectRgn);

				if (THEAPP.m_pGFunction->ValidHRegion(HInspectRgn)==TRUE)
					concat_obj(*pHInspectArea, HInspectRgn, pHInspectArea);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(*pHInspectArea)==TRUE)
			union1(*pHInspectArea, pHInspectArea);

		return TRUE;
	}
	catch(HException &except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::GetInspectArea] : %s", except.message); THEAPP.SaveLog(str);
		return FALSE;
	}
}

BOOL Algorithm::GetInspectAreaROI(Hobject HImage, Hobject HInspectAreaRgn, CAlgorithmParam Param, Hobject *pHInspectArea, int iThradIdx)
{
	try
	{
		gen_empty_obj(pHInspectArea);

		Hobject HNewInspectArea;
		gen_empty_obj(&HNewInspectArea);

		HNewInspectArea = HInspectAreaRgn;

		//////////////////////////////////////////////////////////////////////////
		//  Flexible Inspection ROI
		// 24.03.18 Local Align - LeeGW
		if (Param.m_bUseLocalAlign == TRUE)
			ApplyLocalAlignResult(&HNewInspectArea, Param, iThradIdx);

		// 24.03.18 Local Align - LeeGW

		// ROI 검사 추가(영역 확대,축소) - LeeGW
		Hobject HMarginRgn1, HMarginRgn2;
		HTuple HlArea1, HlArea2, HdCenterX, HdCenterY;
		Hlong lArea1, lArea2;

		double dCenterX, dCenterY;

		HTuple HlNoObj;
		count_obj(HNewInspectArea, &HlNoObj);

		Hlong lNoObj = 0;
		lNoObj = HlNoObj[0].L();

		Hobject HInspectRgn;
		gen_empty_obj(&HInspectRgn);

		Hobject HSelectedRgn;
		gen_empty_obj(&HSelectedRgn);

		int iMargin;
		for (int j = 0; j < lNoObj; j++)
		{
			select_obj(HNewInspectArea, &HSelectedRgn, j + 1);

			for (int i = 0; i < 3; i++)
			{
				if (Param.m_bUseInspectArea[i])
				{

					if (Param.m_iInspectAreaContour1Margin[i] >= 0)
					{
						dilation_circle(HSelectedRgn, &HMarginRgn1, (double)Param.m_iInspectAreaContour1Margin[i] + 0.5);
					}
					else
					{
						iMargin = abs(Param.m_iInspectAreaContour1Margin[i]);
						erosion_circle(HSelectedRgn, &HMarginRgn1, (double)iMargin + 0.5);
					}

					if (Param.m_iInspectAreaContour2Margin[i] >= 0)
					{
						dilation_circle(HSelectedRgn, &HMarginRgn2, (double)Param.m_iInspectAreaContour2Margin[i] + 0.5);
					}
					else
					{
						iMargin = abs(Param.m_iInspectAreaContour2Margin[i]);
						erosion_circle(HSelectedRgn, &HMarginRgn2, (double)iMargin + 0.5);
					}

					area_center(HMarginRgn1, &HlArea1, &HdCenterY, &HdCenterX);
					area_center(HMarginRgn2, &HlArea2, &HdCenterY, &HdCenterX);
					lArea1 = HlArea1[0].L();
					lArea2 = HlArea2[0].L();

					if (lArea1 >= lArea2)
						difference(HMarginRgn1, HMarginRgn2, &HInspectRgn);
					else
						difference(HMarginRgn2, HMarginRgn1, &HInspectRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInspectRgn) == TRUE)
						concat_obj(*pHInspectArea, HInspectRgn, pHInspectArea);
				}
			}
		}

		if (Param.m_bUseLineFit)
		{
			HNewInspectArea = *pHInspectArea;

			double dRectangularity;
			rectangularity(HNewInspectArea, &dRectangularity);

			if (dRectangularity > 0.9)
			{
				BOOL bLineFitRet = FALSE;

				if (THEAPP.m_pGFunction->ValidHImage(HImage))
					bLineFitRet = EdgeMeasureAlgorithm(HImage, HInspectAreaRgn, Param, &HNewInspectArea);
				else
					bLineFitRet = EdgeMeasureAlgorithm(m_HAlgoInspectImage[TRIGGER_EXTRA1], HInspectAreaRgn, Param, &HNewInspectArea);

				if (bLineFitRet)
				{
					if (THEAPP.m_pGFunction->ValidHRegion(HNewInspectArea))
						*pHInspectArea = HNewInspectArea;
				}
			}
		}

		// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW Start
		if (Param.m_bUseApplyDontCare)
		{
			HNewInspectArea = *pHInspectArea;
			RemoveDontCare(&HNewInspectArea, Param, iThradIdx);
			*pHInspectArea = HNewInspectArea;
		}

		if (THEAPP.m_pGFunction->ValidHRegion(*pHInspectArea) == TRUE)
			union1(*pHInspectArea, pHInspectArea);

		return TRUE;
	}
	catch (HException &except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::GetInspectArea_ROI] : %s", except.message); THEAPP.SaveLog(str);
		return FALSE;
	}
}

void Algorithm::GetCircleFitInspectArea(Hobject HContour, int iOuterMargin, int iInnerMargin, Hobject *pHInspectArea)
{
	try
	{
		Hobject HContourRgn, HCircleContour;
		Hobject HMarginRgn1, HMarginRgn2;
		Hlong lArea1, lArea2;
		double dCenterX, dCenterY;
		Hobject HInspectRgn;
		int iMargin;
		HTuple HContour1Row, HContour1Column, HContour1Radius, HTuRow, HTuCol;

		if (THEAPP.m_pGFunction->ValidHXLD(HContour))
		{
			smallest_circle_xld(HContour, &HContour1Row, &HContour1Column, &HContour1Radius);
			gen_circle_contour_xld(&HCircleContour, HContour1Row, HContour1Column, HContour1Radius, 0, 6.28318, "positive", 1);
			gen_region_contour_xld(HCircleContour, &HContourRgn, "filled");

			if (iOuterMargin >= 0)
				dilation_circle(HContourRgn, &HMarginRgn1, (double)iOuterMargin + 0.5);
			else
			{
				iMargin = abs(iOuterMargin);
				erosion_circle(HContourRgn, &HMarginRgn1, (double)iMargin + 0.5);
			}

			if (iInnerMargin >= 0)
				dilation_circle(HContourRgn, &HMarginRgn2, (double)iOuterMargin + 0.5);
			else
			{
				iMargin = abs(iInnerMargin);
				erosion_circle(HContourRgn, &HMarginRgn2, (double)iMargin + 0.5);
			}

			area_center(HMarginRgn1, &lArea1, &dCenterY, &dCenterX);
			area_center(HMarginRgn2, &lArea2, &dCenterY, &dCenterX);

			if (lArea1>=lArea2)
				difference(HMarginRgn1, HMarginRgn2, pHInspectArea);
			else
				difference(HMarginRgn2, HMarginRgn1, pHInspectArea);
		}
	}
	catch(HException &except)
	{
		CString str; 
		str.Format("Halcon Exception [Algorithm::GetCircleFitInspectArea] : %s", except.message); 
		THEAPP.SaveLog(str);
	}
}


// Ellipse Fitting Algorithm 추가 - LeeGW
void Algorithm::InspectEllipseFitting(HTuple X, HTuple Y, double dNormalRadius, double& dCenterX, double& dCenterY, double& dSemiMajor, double& dSemiMinor, double& dAngle)
{
	//////////////////////////////////////////////////////////////////////
	/// Least-Square Hole Center Search
	//////////////////////////////////////////////////////////////////////

	HTuple  x_appr, B, A, Result;
	HTuple  dx_hat, w, invBBT;
	HTuple  w1_a, w1_b, w2_a, w2_b, w3_a, w3_b, BBT, ATinvBBT;
	HTuple  ATinvBBTA, invATinvBBTA, neg_invATinvBBTA, neg_invATinvBBTAAT;
	HTuple  neg_invATinvBBTAAT_invBBT, M_x_appr, x_hat, x_appr_tmp;
	HTuple  Tdx_hat, Adx_hat, Adx_hatw, neg_invBBT, k_hat, v_hat;
	HTuple  Pow2, Mean, MeanValues;

	BOOL bLMSSuccess = FALSE;
	int  Iter, NumberOfMaxIterations, unknowns, redundancy, num;

	unknowns = 5;  // 타원의 중심 (x, y), 반지름 (장축, 단축)과 회전 각도 총 5개
	NumberOfMaxIterations = 8;

	// 초기 근사값
	x_appr.Reset();
	x_appr.Append(X.Mean());
	x_appr.Append(Y.Mean());
	x_appr.Append(dNormalRadius);  // 장축
	x_appr.Append(dNormalRadius * 0.8);  // 단축
	x_appr.Append(0.0);  // 회전 각도 (라디안)

	// A, B 초기화
	num = X.Num();
	create_matrix(num, num * 2, 0, &B);
	create_matrix(num, unknowns, 0, &A);
	Result = 1;
	redundancy = num - unknowns;
	Iter = 0;

	try
	{
		while ((Result > 1e-6) && (Iter < NumberOfMaxIterations))
		{
			Iter += 1;
			if (Iter > 1)
			{
				clear_matrix((dx_hat.Concat(w)).Concat(invBBT));
			}

			for (int i = 0; i <= num - 1; i += 1)
			{
				// 회전된 좌표 계산
				double x_shifted = X[i] - x_appr[0];
				double y_shifted = Y[i] - x_appr[1];
				double rotated_x = cos(x_appr[4]) * x_shifted + sin(x_appr[4]) * y_shifted;
				double rotated_y = -sin(x_appr[4]) * x_shifted + cos(x_appr[4]) * y_shifted;

				// A 행렬
				set_value_matrix(A, i, 0, -2 * rotated_x);
				set_value_matrix(A, i, 1, -2 * rotated_y);
				set_value_matrix(A, i, 2, 1);
				set_value_matrix(A, i, 3, rotated_x * rotated_x);
				set_value_matrix(A, i, 4, rotated_y * rotated_y);

				// B 행렬
				set_value_matrix(B, i, (i * 2), 2 * rotated_x);          // x 방향 값
				set_value_matrix(B, i, (i * 2) + 1, 2 * rotated_y);      // y 방향 값
			}

			// 타원 방정식 오차 계산
			create_matrix(num, 1, X - HTuple(x_appr[0]), &w1_a);  
			mult_element_matrix(w1_a, w1_a, &w1_b); 
			create_matrix(num, 1, Y - HTuple(x_appr[1]), &w2_a); 
			mult_element_matrix(w2_a, w2_a, &w2_b);   
			create_matrix(num, 1, HTuple(x_appr[4]), &w3_a);
			mult_element_matrix(w3_a, w3_a, &w3_b);
			add_matrix_mod(w1_b, w2_b);                              
			sub_matrix(w1_b, w3_b, &w);
			clear_matrix((w1_a.Concat(w2_a)).Concat(w3_a));
			clear_matrix((w1_b.Concat(w2_b)).Concat(w3_b));

			mult_matrix(B, B, "ABT", &BBT);
			invert_matrix(BBT, "symmetric", 0, &invBBT);
			mult_matrix(A, invBBT, "ATB", &ATinvBBT);
			mult_matrix(ATinvBBT, A, "AB", &ATinvBBTA);
			invert_matrix(ATinvBBTA, "symmetric", 0, &invATinvBBTA);
			scale_matrix(invATinvBBTA, -1, &neg_invATinvBBTA);
			mult_matrix(neg_invATinvBBTA, A, "ABT", &neg_invATinvBBTAAT);
			mult_matrix(neg_invATinvBBTAAT, invBBT, "AB", &neg_invATinvBBTAAT_invBBT);
			mult_matrix(neg_invATinvBBTAAT_invBBT, w, "AB", &dx_hat);

			// 새로운 중심 좌표, 각도 근사
			HTuple dx_hat_val;
			get_full_matrix(dx_hat, &dx_hat_val);

			HTuple new_x_center = x_appr[0].D() + dx_hat_val[0].D();
			HTuple new_y_center = x_appr[1].D() + dx_hat_val[1].D();
			HTuple new_angle = x_appr[4].D() + dx_hat_val[4].D();

			x_appr = new_x_center.Concat(new_y_center).Concat(x_appr[2].D()).Concat(x_appr[3].D()).Concat(new_angle);

			// 최대 오차 값 계산
			get_full_matrix(dx_hat, &Tdx_hat);
			HTuple hat = Tdx_hat.Abs()[0];
			hat.Concat(Tdx_hat.Abs()[1]).Concat(Tdx_hat.Abs()[4]);
			Result = hat.Max();

			double r = Result[0].D();

			clear_matrix((((BBT.Concat(ATinvBBT)).Concat(ATinvBBTA)).Concat(invATinvBBTA)).Concat(neg_invATinvBBTA));
			clear_matrix(neg_invATinvBBTAAT.Concat(neg_invATinvBBTAAT_invBBT));
			clear_matrix(M_x_appr.Concat(x_hat));
		}

		dCenterX = x_appr[0].D();
		dCenterY = x_appr[1].D();
		dSemiMajor = x_appr[2].D();
		dSemiMinor = x_appr[3].D();
		dAngle = x_appr[4].D();

		clear_matrix((dx_hat.Concat(w)).Concat(invBBT));
		clear_matrix(A.Concat(B));
	}
	catch (HException& except)
	{
		CString str;
		str.Format("Halcon Exception [Algorithm::InspectEllipseFitting] : %s", except.message);
		THEAPP.SaveLog(str);

		dCenterX = x_appr[0].D();
		dCenterY = x_appr[1].D();
		dSemiMajor = x_appr[2].D();
		dSemiMinor = x_appr[3].D();
		dAngle = x_appr[4].D();

		clear_matrix((((BBT.Concat(ATinvBBT)).Concat(ATinvBBTA)).Concat(invATinvBBTA)).Concat(neg_invATinvBBTA));
		clear_matrix(neg_invATinvBBTAAT.Concat(neg_invATinvBBTAAT_invBBT));
	}
}


Hobject Algorithm::CommonAlgorithm(Hobject HImage, Hobject HContour1, Hobject HContour2, Hobject HContour3, Hobject HContour4, Hobject HContour5, CAlgorithmParam Param, Hobject *pHEpoxyDontCareRgn, Hobject *pHImage2)
{
	try
	{
		BOOL bDebugSave = FALSE;

		Hobject HDefectRgn;
		gen_empty_obj(&HDefectRgn);

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		get_image_pointer1(HImage, (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

		BOOL bEpoxyInspection = FALSE;
		if (pHImage2!=NULL)
		{
			if (THEAPP.m_pGFunction->ValidHImage(*pHImage2))
				bEpoxyInspection = TRUE;
		}

		Hobject HInspectAreaRgn;
		gen_empty_obj(&HInspectAreaRgn);

		GetInspectArea(HContour1, HContour2, HContour3, HContour4, HContour5, Param, &HInspectAreaRgn);


		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn)==FALSE)
			return HDefectRgn;

		Hobject HProcessImage;

		HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
		long lPeak, lHystUpperThreshold, lHystLowThreshold;

		if (Param.m_bUseImageScaling)
		{
			int iScaleMin, iScaleMax;
			int iScalePeakUpperOffset = 20;
			int iScalePeakLowerOffset = 10;

			if (0)
			{
				write_image(HImage, "bmp", 0, "c:\\DualTest\\ScaleSurface");
				write_region(HInspectAreaRgn, "c:\\DualTest\\ScaleInsepctArea.reg");
			}

			if (Param.m_iImageScaleMethod==IMAGE_SCALE_AUTO)
			{
				gray_histo(HInspectAreaRgn, HImage, &HAbsHisto, &HRelHisto);
				create_funct_1d_array(HAbsHisto, &HFunction);
				smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
				tuple_last_n(HSmoothFunction, 3, &HAbsHisto);
				tuple_sort_index(HAbsHisto, &Indices);
				tuple_inverse(Indices, &Inverted);
				lPeak = Inverted[0].L();
				
				iScaleMax = lPeak + iScalePeakUpperOffset;
				if (iScaleMax>255)
					iScaleMax = 255;
				
				iScaleMin = lPeak - iScalePeakLowerOffset;
				if (iScaleMin<0)
					iScaleMin = 0;
			}
			else
			{
				if (Param.m_iImageScaleMin<0)
					iScaleMin = 0;
				else if (Param.m_iImageScaleMin>255)
					iScaleMin = 255;
				else
					iScaleMin = Param.m_iImageScaleMin;

				if (Param.m_iImageScaleMax<0)
					iScaleMax = 0;
				else if (Param.m_iImageScaleMax>255)
					iScaleMax = 255;
				else
					iScaleMax = Param.m_iImageScaleMax;

				if (iScaleMax<iScaleMin)
					iScaleMax = iScaleMin;
			}

			THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
			binomial_filter(HProcessImage, &HProcessImage, 3, 3);
		}

		Hobject HImageReduced;

		if (Param.m_bUseImageScaling)
			reduce_domain(HProcessImage, HInspectAreaRgn, &HImageReduced);
		else
			reduce_domain(HImage, HInspectAreaRgn, &HImageReduced);

		Hobject HInspectAreaConnRgn;
		Hlong lNoProcessRgn;
		int i, j, k;
		Hobject HOutRgn, HInRgn, HInspectROIRgn;

		Hobject HBrightRgn, HDarkRgn, HThresRgn;
		Hobject HExpandReduceImage;

		HTuple dInCenterX, dInCenterY, dInRadius;
		HTuple dOutCenterX, dOutCenterY, dOutRadius;
		HTuple iRingSize, iRadius;

		Hobject HDTImageReduced, HImagePolar, HPartImage, Hmean_image, HDynThreshRgn;
		Hobject HInvertImageReduced;
		double dScratchMaxRadius;

		Hobject HEpoxyCandidateRgn;
		gen_empty_obj(&HEpoxyCandidateRgn);

		if (bEpoxyInspection)
		{
			if (FALSE)
			{
				write_image(HImage, "bmp", 0, "c:\\DualTest\\EpoxySurface");
				write_image(*pHImage2, "bmp", 0, "c:\\DualTest\\EpoxyEdge");
				write_region(HInspectAreaRgn, "c:\\DualTest\\EpoxyInsepctAreaRgn.reg");
			}

			Hobject HSurfaceEdgeImage, HEdgeSurfaceImage;
			Hobject HSE_ThresRgn, HES_ThresRgn;

			sub_image(HImage, *pHImage2, &HSurfaceEdgeImage, 1, 0);
			sub_image(*pHImage2, HImage, &HEdgeSurfaceImage, 1, 0);

			reduce_domain(HSurfaceEdgeImage, HInspectAreaRgn, &HSurfaceEdgeImage);
			reduce_domain(HEdgeSurfaceImage, HInspectAreaRgn, &HEdgeSurfaceImage);

			threshold(HSurfaceEdgeImage, &HSE_ThresRgn, Param.m_iEpoxySEThres, 255);
			threshold(HEdgeSurfaceImage, &HES_ThresRgn, Param.m_iEpoxyESThres, 255);

			BOOL bEpoxyExist = FALSE;

			Hobject HTempRgn1, HTempRgn2, HTempRgn3, HTempRgn4;
			Hlong lNoHoleCheckBlob;
			Hobject HIntersectBlobRgn;
			double dTempRow, dTempCol, dTempRadius;

			Hobject HEpoxyImageReduced;
			double dMean, dDev;

			double dEpoxyOpeningSize = (double)Param.m_iEpoxyOpeningSize+0.5;

			int iMinSizeLarge = 50;
			int iMinSizeSmall = 5;

			if (THEAPP.m_pGFunction->ValidHRegion(HSE_ThresRgn)==TRUE && THEAPP.m_pGFunction->ValidHRegion(HES_ThresRgn)==TRUE)
			{
				if (Param.m_bEpoxyConditionHole)
				{
					// SE가 ES안에 포함 체크
					closing_circle(HES_ThresRgn, &HTempRgn1, (double)Param.m_iEpoxyClosingSize+0.5);
					fill_up_shape(HTempRgn1, &HTempRgn1, "area", 1, 1000);
					opening_circle(HTempRgn1, &HTempRgn1, (double)Param.m_iEpoxyOpeningSize+0.5);
					connection(HTempRgn1, &HTempRgn1);
					select_shape(HTempRgn1, &HTempRgn1, "area", "and", iMinSizeLarge, 5000);
					select_shape(HTempRgn1, &HTempRgn1, "inner_radius", "and", 2, 100);
					select_shape(HTempRgn1, &HTempRgn1, "max_diameter", "and", 20, 250);

					connection(HSE_ThresRgn, &HTempRgn2);
					select_shape(HTempRgn2, &HTempRgn2, "area", "and", iMinSizeSmall, 3000);	// 10픽셀 이상이 내부에 존재한다면

					count_obj(HTempRgn1, &lNoHoleCheckBlob);

					for (i=0; i<lNoHoleCheckBlob; i++)
					{
						select_obj(HTempRgn1, &HTempRgn3, i+1);

						gen_empty_obj(&HIntersectBlobRgn);
						select_shape_proto(HTempRgn2, HTempRgn3, &HIntersectBlobRgn, "overlaps_rel", 80, 100);

						if (THEAPP.m_pGFunction->ValidHRegion(HIntersectBlobRgn))
						{
							inner_circle(HTempRgn3, &dTempRow, &dTempCol, &dTempRadius);
							dTempRadius = dTempRadius * 0.25;
							if (dTempRadius < dEpoxyOpeningSize)
								dTempRadius = dEpoxyOpeningSize;

							opening_circle(HTempRgn3, &HTempRgn3, dTempRadius);
							connection(HTempRgn3, &HTempRgn3);
							select_shape(HTempRgn3, &HTempRgn3, "area", "and", 300, 5000);
							union1(HTempRgn3, &HTempRgn3);

							if (THEAPP.m_pGFunction->ValidHRegion(HTempRgn3))
							{
								reduce_domain(HImage, HTempRgn3, &HEpoxyImageReduced);
								bin_threshold(HEpoxyImageReduced, &HDarkRgn);
								difference(HTempRgn3, HDarkRgn, &HBrightRgn);

								if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn))
								{
									intensity(HBrightRgn, HImage, &dMean, &dDev);
									if (dMean>128)
									{
										concat_obj(HEpoxyCandidateRgn, HTempRgn3, &HEpoxyCandidateRgn);
										bEpoxyExist = TRUE;
									}
								}
							}
						}
					}


					if (bEpoxyExist==FALSE)
					{
						// ES가 SE안에 포함 체크
						closing_circle(HSE_ThresRgn, &HTempRgn1, (double)Param.m_iEpoxyClosingSize+0.5);
						fill_up_shape(HTempRgn1, &HTempRgn1, "area", 1, 1000);
						opening_circle(HTempRgn1, &HTempRgn1, (double)Param.m_iEpoxyOpeningSize+0.5);
						connection(HTempRgn1, &HTempRgn1);
						select_shape(HTempRgn1, &HTempRgn1, "area", "and", iMinSizeLarge, 5000);
						select_shape(HTempRgn1, &HTempRgn1, "inner_radius", "and", 2, 100);
						select_shape(HTempRgn1, &HTempRgn1, "max_diameter", "and", 20, 250);

						connection(HES_ThresRgn, &HTempRgn2);
						select_shape(HTempRgn2, &HTempRgn2, "area", "and", iMinSizeSmall, 3000);	// 10픽셀 이상이 내부에 존재한다면

						count_obj(HTempRgn1, &lNoHoleCheckBlob);

						for (i=0; i<lNoHoleCheckBlob; i++)
						{
							select_obj(HTempRgn1, &HTempRgn3, i+1);

							gen_empty_obj(&HIntersectBlobRgn);
							select_shape_proto(HTempRgn2, HTempRgn3, &HIntersectBlobRgn, "overlaps_rel", 80, 100);

							if (THEAPP.m_pGFunction->ValidHRegion(HIntersectBlobRgn))
							{
								inner_circle(HTempRgn3, &dTempRow, &dTempCol, &dTempRadius);
								dTempRadius = dTempRadius * 0.25;
								if (dTempRadius < dEpoxyOpeningSize)
									dTempRadius = dEpoxyOpeningSize;

								opening_circle(HTempRgn3, &HTempRgn3, dTempRadius);
								connection(HTempRgn3, &HTempRgn3);
								select_shape(HTempRgn3, &HTempRgn3, "area", "and", 300, 5000);
								union1(HTempRgn3, &HTempRgn3);

								if (THEAPP.m_pGFunction->ValidHRegion(HTempRgn3))
								{
									reduce_domain(HImage, HTempRgn3, &HEpoxyImageReduced);
									bin_threshold(HEpoxyImageReduced, &HDarkRgn);
									difference(HTempRgn3, HDarkRgn, &HBrightRgn);

									if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn))
									{
										intensity(HBrightRgn, HImage, &dMean, &dDev);
										if (dMean>128)
										{
											concat_obj(HEpoxyCandidateRgn, HTempRgn3, &HEpoxyCandidateRgn);
											bEpoxyExist = TRUE;
										}
									}
								}
							}
						}
					}

				}

				if (Param.m_bEpoxyConditionNear)
				{
					connection(HES_ThresRgn, &HTempRgn1);
					select_shape(HTempRgn1, &HTempRgn1, "area", "and", Param.m_iEpoxyNearMinSize, MAX_DEF);
					connection(HSE_ThresRgn, &HTempRgn2);
					select_shape(HTempRgn2, &HTempRgn2, "area", "and", Param.m_iEpoxyNearMinSize, MAX_DEF);

					count_obj(HTempRgn1, &lNoHoleCheckBlob);

					for (i=0; i<lNoHoleCheckBlob; i++)
					{
						select_obj(HTempRgn1, &HTempRgn3, i+1);
						dilation_circle(HTempRgn3, &HTempRgn4, (double)Param.m_iEpoxyNearDistance+1.5);		// 1픽셀 추가

						gen_empty_obj(&HIntersectBlobRgn);
						select_shape_proto(HTempRgn2, HTempRgn4, &HIntersectBlobRgn, "overlaps_abs", 1, MAX_DEF);

						if (THEAPP.m_pGFunction->ValidHRegion(HIntersectBlobRgn))
						{
							union1(HIntersectBlobRgn, &HIntersectBlobRgn);
							union2(HIntersectBlobRgn, HTempRgn4, &HIntersectBlobRgn);
							fill_up(HIntersectBlobRgn, &HIntersectBlobRgn);

							inner_circle(HIntersectBlobRgn, &dTempRow, &dTempCol, &dTempRadius);
							dTempRadius = dTempRadius * 0.25;
							opening_circle(HIntersectBlobRgn, &HIntersectBlobRgn, dTempRadius);

							concat_obj(HEpoxyCandidateRgn, HIntersectBlobRgn, &HEpoxyCandidateRgn);
							bEpoxyExist = TRUE;
						}		
					}
				}
			}	// if (THEAPP.m_pGFunction->ValidHRegion(HSE_ThresRgn)==TRUE && THEAPP.m_pGFunction->ValidHRegion(HES_ThresRgn)==TRUE)

			if (THEAPP.m_pGFunction->ValidHRegion(HEpoxyCandidateRgn))
			{
				// 검출 조건으로 GoGo~~
				union1(HEpoxyCandidateRgn, &HDefectRgn);

				// Epoxy Don't Care Area
				union1(HEpoxyCandidateRgn, pHEpoxyDontCareRgn);
			}

		}	// if (bEpoxyInspection)

		Hobject HBrightFixedThresholdConnectedRgn, HDarkFixedThresholdConnectedRgn;

		int iPolarResolution = 640;

		gen_empty_obj(&HBrightRgn);
		gen_empty_obj(&HDarkRgn);
		gen_empty_obj(&HDynThreshRgn);
	
		if (Param.m_bUseBrightDT || Param.m_bUseDarkDT)
		{
			connection(HInspectAreaRgn, &HInspectAreaConnRgn);
			count_obj(HInspectAreaConnRgn, &lNoProcessRgn);

			for (i=0; i<lNoProcessRgn; i++)
			{
				select_obj(HInspectAreaConnRgn, &HInspectROIRgn, i+1);
				if (THEAPP.m_pGFunction->ValidHRegion(HInspectROIRgn)==FALSE)
					continue;

				fill_up(HInspectROIRgn, &HOutRgn);
				difference(HOutRgn, HInspectROIRgn, &HInRgn);

				smallest_circle(HInRgn, &dInCenterY, &dInCenterX, &dInRadius);
				smallest_circle(HOutRgn, &dOutCenterY, &dOutCenterX, &dOutRadius);
				iRingSize = dOutRadius-dInRadius;
				tuple_int(iRingSize, &iRingSize);
				iRadius = dOutRadius;
				tuple_int(iRadius, &iRadius);

				if (Param.m_bUseImageScaling)
					reduce_domain(HProcessImage, HInspectROIRgn, &HDTImageReduced);
				else
					reduce_domain(HImage, HInspectROIRgn, &HDTImageReduced);

				polar_trans_image(HDTImageReduced, &HImagePolar, dOutCenterY, dOutCenterX, (Hlong)iPolarResolution, iRadius+5);

				if (Param.m_bUseBrightDT)
				{
					crop_part(HImagePolar, &HPartImage, iRadius-iRingSize, 0, iPolarResolution, iRingSize); 
					if (Param.m_iBrightMedianFilterSize>0)
						median_image(HPartImage, &HPartImage, "circle", Param.m_iBrightMedianFilterSize, "mirrored");
					if (Param.m_iBrightClosingFilterSize>0)
						gray_closing_shape(HPartImage, &HPartImage, Param.m_iBrightClosingFilterSize, Param.m_iBrightClosingFilterSize, "octagon");

					if (Param.m_bUseBrightLineNormalization)
					{
						BYTE *pPartData;
						Hlong lPartImageWidth, lPartImageHeight;
						get_image_pointer1(HPartImage, (Hlong*)&pPartData, type, &lPartImageWidth, &lPartImageHeight);

						Hobject HPartDomainRgn;
						get_domain(HPartImage, &HPartDomainRgn);

						double dPartMean, dPartSD;
						intensity(HPartDomainRgn, HPartImage, &dPartMean, &dPartSD);

						Hobject HLineDomainRgn;
						double dLineMean, dLineSD, dGrayRatio;

						int ii, jj;
						int iNewGrayVal, iGrayVal;

						for (ii=0; ii<lPartImageHeight; ii++)
						{
							gen_rectangle1(&HLineDomainRgn, ii, 0, ii, lPartImageWidth-1);
							intensity(HLineDomainRgn, HPartImage, &dLineMean, &dLineSD);

							if (dLineMean==0)
								dGrayRatio = 1.0;
							else
								dGrayRatio = dPartMean / dLineMean;

							for (jj=0; jj<lPartImageWidth; jj++)
							{
								iGrayVal = *(pPartData+ii*lPartImageWidth+jj);
								iNewGrayVal = (int)(dGrayRatio * (double)iGrayVal + 0.5);
								if (iNewGrayVal>255)
									iNewGrayVal = 255;
								if (iNewGrayVal<0)
									iNewGrayVal = 0;

								*(pPartData+ii*lPartImageWidth+jj) = (BYTE)iNewGrayVal;
							}
						}

					}

					mean_image(HPartImage, &Hmean_image, Param.m_iBrightDTFilterSize, 3);
					dyn_threshold(HPartImage, Hmean_image, &HDynThreshRgn, Param.m_iBrightDTValue, "light");
					if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn)==TRUE)
					{
						union1(HDynThreshRgn, &HDynThreshRgn);
						polar_trans_region_inv(HDynThreshRgn, &HDynThreshRgn, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius-iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, lImageWidth, lImageHeight, "nearest_neighbor");
						concat_obj(HBrightRgn, HDynThreshRgn, &HBrightRgn);
					}
				}

				if (Param.m_bUseDarkDT)
				{
					crop_part(HImagePolar, &HPartImage, iRadius-iRingSize, 0, iPolarResolution, iRingSize); 
					if (Param.m_iDarkMedianFilterSize>0)
						median_image(HPartImage, &HPartImage, "circle", Param.m_iDarkMedianFilterSize, "mirrored");
					if (Param.m_iDarkClosingFilterSize>0)
						gray_closing_shape(HPartImage, &HPartImage, Param.m_iDarkClosingFilterSize, Param.m_iDarkClosingFilterSize, "octagon");

					if (Param.m_bUseBrightLineNormalization)
					{
						BYTE *pPartData;
						Hlong lPartImageWidth, lPartImageHeight;
						get_image_pointer1(HPartImage, (Hlong*)&pPartData, type, &lPartImageWidth, &lPartImageHeight);

						Hobject HPartDomainRgn;
						get_domain(HPartImage, &HPartDomainRgn);

						double dPartMean, dPartSD;
						intensity(HPartDomainRgn, HPartImage, &dPartMean, &dPartSD);

						Hobject HLineDomainRgn;
						double dLineMean, dLineSD, dGrayRatio;

						int ii, jj;
						int iNewGrayVal, iGrayVal;

						for (ii=0; ii<lPartImageHeight; ii++)
						{
							gen_rectangle1(&HLineDomainRgn, ii, 0, ii, lPartImageWidth-1);
							intensity(HLineDomainRgn, HPartImage, &dLineMean, &dLineSD);

							if (dLineMean==0)
								dGrayRatio = 1.0;
							else
								dGrayRatio = dPartMean / dLineMean;

							for (jj=0; jj<lPartImageWidth; jj++)
							{
								iGrayVal = *(pPartData+ii*lPartImageWidth+jj);
								iNewGrayVal = (int)(dGrayRatio * (double)iGrayVal + 0.5);
								if (iNewGrayVal>255)
									iNewGrayVal = 255;
								if (iNewGrayVal<0)
									iNewGrayVal = 0;

								*(pPartData+ii*lPartImageWidth+jj) = (BYTE)iNewGrayVal;
							}
						}
					}

					mean_image(HPartImage, &Hmean_image, Param.m_iDarkDTFilterSize, 3);
					dyn_threshold(HPartImage, Hmean_image, &HDynThreshRgn, Param.m_iDarkDTValue, "dark");
					if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn)==TRUE)
					{
						union1(HDynThreshRgn, &HDynThreshRgn);
						polar_trans_region_inv(HDynThreshRgn, &HDynThreshRgn, dOutCenterY, dOutCenterX, 6.28319, 0, iRadius-iRingSize, iRadius, (Hlong)iPolarResolution, iRingSize, lImageWidth, lImageHeight, "nearest_neighbor");
						concat_obj(HDarkRgn, HDynThreshRgn, &HDarkRgn);
					}
				}
			}
		}

		gen_empty_obj(&HBrightFixedThresholdConnectedRgn);

		if (Param.m_bUseBrightFixedThres)
		{
			threshold(HImageReduced, &HThresRgn, Param.m_iBrightLowerThres, Param.m_iBrightUpperThres);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn)==TRUE)
			{
				concat_obj(HBrightRgn, HThresRgn, &HBrightRgn);

				if (Param.m_bUseFTConnected)
				{
					connection(HThresRgn, &HBrightFixedThresholdConnectedRgn);

					if (Param.m_bUseFTConnectedArea && Param.m_iFTConnectedAreaMin>0)
						select_shape(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn, "area", "and", Param.m_iFTConnectedAreaMin, MAX_DEF);
					if (Param.m_bUseFTConnectedLength && Param.m_iFTConnectedLengthMin>0)
						select_shape(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn, "max_diameter", "and", Param.m_iFTConnectedLengthMin, MAX_DEF);
					if (Param.m_bUseFTConnectedWidth && Param.m_iFTConnectedWidthMin>0)
						select_shape(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn, "inner_radius", "and", Param.m_iFTConnectedWidthMin/2, MAX_DEF);

					union1(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn);
				}
			}
		}

		if (Param.m_bUseBrightUniformityCheck)
		{
			gray_histo(HInspectAreaRgn, HImageReduced, &HAbsHisto, &HRelHisto);
			create_funct_1d_array(HAbsHisto, &HFunction);
			smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
			tuple_last_n(HSmoothFunction, 3, &HAbsHisto);
			tuple_sort_index(HAbsHisto, &Indices);
			tuple_inverse(Indices, &Inverted);
			lPeak = Inverted[0].L();

			lHystUpperThreshold = lPeak + Param.m_iBrightUniformityOffset;
			if (lHystUpperThreshold>255)
				lHystUpperThreshold = 255;

			lHystLowThreshold = lHystUpperThreshold - Param.m_iBrightUniformityHystOffset;	
			if (lHystLowThreshold<0)
				lHystLowThreshold = 0;

			if (Param.m_iBrightUniformityHystLength>0)
				hysteresis_threshold(HImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, Param.m_iBrightUniformityHystLength);
			else
				threshold(HImageReduced, &HThresRgn, lHystUpperThreshold, 255);

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn)==TRUE)
				concat_obj(HBrightRgn, HThresRgn, &HBrightRgn);
		}

		if (Param.m_bUseBrightHystThres)
		{
			if (Param.m_iBrightHystSecureThres>=Param.m_iBrightHystPotentialThres)
			{
				hysteresis_threshold(HImageReduced, &HThresRgn, Param.m_iBrightHystPotentialThres, Param.m_iBrightHystSecureThres, Param.m_iBrightHystPotentialLength); 
				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn)==TRUE)
					concat_obj(HBrightRgn, HThresRgn, &HBrightRgn);
			}
		}

		gen_empty_obj(&HDarkFixedThresholdConnectedRgn);

		if (Param.m_bUseDarkFixedThres)
		{
			threshold(HImageReduced, &HThresRgn, Param.m_iDarkLowerThres, Param.m_iDarkUpperThres);
			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn)==TRUE)
			{
				concat_obj(HDarkRgn, HThresRgn, &HDarkRgn);

				if (Param.m_bUseFTConnected)
				{
					connection(HThresRgn, &HDarkFixedThresholdConnectedRgn);

					if (Param.m_bUseFTConnectedArea && Param.m_iFTConnectedAreaMin>0)
						select_shape(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn, "area", "and", Param.m_iFTConnectedAreaMin, MAX_DEF);
					if (Param.m_bUseFTConnectedLength && Param.m_iFTConnectedLengthMin>0)
						select_shape(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn, "max_diameter", "and", Param.m_iFTConnectedLengthMin, MAX_DEF);
					if (Param.m_bUseFTConnectedWidth && Param.m_iFTConnectedWidthMin>0)
						select_shape(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn, "inner_radius", "and", Param.m_iFTConnectedWidthMin/2, MAX_DEF);

					union1(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn);
				}
			}
		}

		if (Param.m_bUseDarkUniformityCheck)
		{
			invert_image(HImageReduced, &HInvertImageReduced);

			gray_histo(HInspectAreaRgn, HInvertImageReduced, &HAbsHisto, &HRelHisto);
			create_funct_1d_array(HAbsHisto, &HFunction);
			smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
			tuple_last_n(HSmoothFunction, 3, &HAbsHisto);
			tuple_sort_index(HAbsHisto, &Indices);
			tuple_inverse(Indices, &Inverted);
			lPeak = Inverted[0].L();

			lHystUpperThreshold = lPeak + Param.m_iDarkUniformityOffset;
			if (lHystUpperThreshold>255)
				lHystUpperThreshold = 255;

			lHystLowThreshold = lHystUpperThreshold - Param.m_iDarkUniformityHystOffset;	
			if (lHystLowThreshold<0)
				lHystLowThreshold = 0;

			if (Param.m_iDarkUniformityHystLength>0)
				hysteresis_threshold(HInvertImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, Param.m_iDarkUniformityHystLength);
			else
				threshold(HInvertImageReduced, &HThresRgn, lHystUpperThreshold, 255);

			if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn)==TRUE)
				concat_obj(HDarkRgn, HThresRgn, &HDarkRgn);
		}

		if (Param.m_bUseDarkHystThres)
		{
			if (Param.m_iDarkHystSecureThres<=Param.m_iDarkHystPotentialThres)
			{
				int iTempSecureThres, iTempPotentialThres;
				iTempSecureThres = 255 - Param.m_iDarkHystSecureThres;
				if (iTempSecureThres<0)
					iTempSecureThres = 0;
				if (iTempSecureThres>255)
					iTempSecureThres = 255;
				iTempPotentialThres = 255 - Param.m_iDarkHystPotentialThres;
				if (iTempPotentialThres<0)
					iTempPotentialThres = 0;
				if (iTempPotentialThres>255)
					iTempPotentialThres = 255;

				if (iTempSecureThres>=iTempPotentialThres)
				{
					invert_image(HImageReduced, &HInvertImageReduced);

					hysteresis_threshold(HInvertImageReduced, &HThresRgn, iTempPotentialThres, iTempSecureThres, Param.m_iDarkHystPotentialLength); 
					if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn)==TRUE)
						concat_obj(HDarkRgn, HThresRgn, &HDarkRgn);
				}
			}
		}

		gen_empty_obj(&HInvertImageReduced);

		union1(HBrightRgn, &HBrightRgn);

		if (Param.m_bUseFTConnected)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HBrightFixedThresholdConnectedRgn) == TRUE)
			{
				connection(HBrightRgn, &HBrightRgn);
				select_shape_proto(HBrightRgn, HBrightFixedThresholdConnectedRgn, &HBrightRgn, "overlaps_abs", 1, 9999999);
				union1(HBrightRgn, &HBrightRgn);
			}
			else
				gen_empty_obj(&HBrightRgn);
		}

		union1(HDarkRgn, &HDarkRgn);

		if (Param.m_bUseFTConnected)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDarkFixedThresholdConnectedRgn) == TRUE)
			{
				connection(HDarkRgn, &HDarkRgn);
				select_shape_proto(HDarkRgn, HDarkFixedThresholdConnectedRgn, &HDarkRgn, "overlaps_abs", 1, 9999999);
				union1(HDarkRgn, &HDarkRgn);
			}
			else
				gen_empty_obj(&HDarkRgn);
		}

		if (bEpoxyInspection==FALSE)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(*pHEpoxyDontCareRgn))
			{
				difference(HBrightRgn, *pHEpoxyDontCareRgn, &HBrightRgn);
				difference(HDarkRgn, *pHEpoxyDontCareRgn, &HDarkRgn);
			}
		}

		if (Param.m_bCombineBrightDarkBlob)
		{
			union2(HBrightRgn, HDarkRgn, &HBrightRgn);
			gen_empty_obj(&HDarkRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn)==TRUE)
		{
			if (Param.m_iOpeningSize>0)
				opening_circle(HBrightRgn, &HBrightRgn, (double)Param.m_iOpeningSize+0.5);
			if (Param.m_iClosingSize>0)
				closing_circle(HBrightRgn, &HBrightRgn, (double)Param.m_iClosingSize+0.5);
			if (Param.m_bFillup)
			{
				connection(HBrightRgn, &HBrightRgn);
				fill_up(HBrightRgn, &HBrightRgn);
			}

			if (Param.m_bUseConnection)
			{
				connection(HBrightRgn, &HBrightRgn);

				select_shape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iConnectionMinSize, MAX_DEF);

				if (Param.m_bUseAngle)
				{
					dScratchMaxRadius = (double)Param.m_iConnectionMaxWidth * 0.5;
					select_shape(HBrightRgn, &HBrightRgn, "inner_radius", "and", 0, dScratchMaxRadius);
					BlobUnionAngle(&HBrightRgn, Param.m_iConnectionLength, 10);
				}
				else
				{
					BlobUnion(&HBrightRgn, Param.m_iConnectionLength);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn)==TRUE)
			{
				union1(HBrightRgn, &HBrightRgn);
				concat_obj(HDefectRgn, HBrightRgn, &HDefectRgn);
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn)==TRUE)
		{
			if (Param.m_iOpeningSize>0)
				opening_circle(HDarkRgn, &HDarkRgn, (double)Param.m_iOpeningSize+0.5);
			if (Param.m_iClosingSize>0)
				closing_circle(HDarkRgn, &HDarkRgn, (double)Param.m_iClosingSize+0.5);
			if (Param.m_bFillup)
			{
				connection(HDarkRgn, &HDarkRgn);
				fill_up(HDarkRgn, &HDarkRgn);
			}

			if (Param.m_bUseConnection)
			{
				connection(HDarkRgn, &HDarkRgn);

				select_shape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iConnectionMinSize, MAX_DEF);

				if (Param.m_bUseAngle)
				{
					dScratchMaxRadius = (double)Param.m_iConnectionMaxWidth * 0.5;
					select_shape(HDarkRgn, &HDarkRgn, "inner_radius", "and", 0, dScratchMaxRadius);
					BlobUnionAngle(&HDarkRgn, Param.m_iConnectionLength, 10);
				}
				else
				{
					BlobUnion(&HDarkRgn, Param.m_iConnectionLength);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn)==TRUE)
			{
				union1(HDarkRgn, &HDarkRgn);
				concat_obj(HDefectRgn, HDarkRgn, &HDefectRgn);
			}
		}

		if (Param.m_bUseEdge)
		{
			Hobject HExpandReduceImage, HZoomImage;
			Hobject HEdgeContour, HEdgeDetectionRgn;
			double dInverse;
			int iEdgeLow, iEdgeHigh;
			HTuple HomMat2DIdentity, HomMat2DScale;

			gen_empty_obj(&HEdgeDetectionRgn);

			if (Param.m_dEdgeZoomRatio<=0)
				Param.m_dEdgeZoomRatio = 0.2;

			dInverse = 1.0/Param.m_dEdgeZoomRatio;

			expand_domain_gray(HImageReduced, &HExpandReduceImage, (long)(dInverse*2.0));
			reduce_domain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

			zoom_image_factor(HExpandReduceImage, &HZoomImage, Param.m_dEdgeZoomRatio, Param.m_dEdgeZoomRatio, "constant");

			if (THEAPP.m_pGFunction->ValidHImage(HZoomImage)==TRUE)
			{
				if (Param.m_iEdgeStrength<6)
					iEdgeHigh = 6;
				else
					iEdgeHigh = Param.m_iEdgeStrength;
				iEdgeLow = iEdgeHigh / 2;

				edges_sub_pix(HZoomImage, &HEdgeContour, "canny", 1.0, iEdgeLow, iEdgeHigh);
				if (THEAPP.m_pGFunction->ValidHXLD(HEdgeContour)==TRUE)
				{
					hom_mat2d_identity(&HomMat2DIdentity);
					hom_mat2d_scale(HomMat2DIdentity, dInverse, dInverse, 0.0, 0.0, &HomMat2DScale);
					affine_trans_contour_xld(HEdgeContour, &HEdgeContour, HomMat2DScale);

					union_adjacent_contours_xld(HEdgeContour, &HEdgeContour, (double)Param.m_iEdgeConnectionDistance, 1.0, "attr_forget");
					select_contours_xld(HEdgeContour, &HEdgeContour, "contour_length", (double)Param.m_iEdgeMinContourLength, 1e9, -0.5, 0.5);
					if (THEAPP.m_pGFunction->ValidHXLD(HEdgeContour)==TRUE)
					{
						select_contours_xld(HEdgeContour, &HEdgeContour, "closed", 0.5, (double)Param.m_iEdgeMaxEndpointDistance, -0.5, 0.5);
						if (THEAPP.m_pGFunction->ValidHXLD(HEdgeContour)==TRUE)
						{
							close_contours_xld(HEdgeContour, &HEdgeContour);
							gen_region_contour_xld(HEdgeContour, &HEdgeDetectionRgn, "filled");

							if (THEAPP.m_pGFunction->ValidHRegion(HEdgeDetectionRgn)==TRUE)
							{
								union1(HEdgeDetectionRgn, &HEdgeDetectionRgn);

								if (bEpoxyInspection==FALSE)
								{
									if (THEAPP.m_pGFunction->ValidHRegion(*pHEpoxyDontCareRgn))
									{
										difference(HEdgeDetectionRgn, *pHEpoxyDontCareRgn, &HEdgeDetectionRgn);
									}
								}

								concat_obj(HDefectRgn, HEdgeDetectionRgn, &HDefectRgn);
							}
						}
					}
				}
			}
		}

		intersection(HInspectAreaRgn, HDefectRgn, &HDefectRgn);

		connection(HDefectRgn, &HDefectRgn);

		// 길이/너비/면적 조건
		if (ValidParam(Param.m_bDefectConditionXLength, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, PARAM_SCALE_MAX))
			select_shape(HDefectRgn, &HDefectRgn, "width", "and", Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax);
		if (ValidParam(Param.m_bDefectConditionYLength, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, PARAM_SCALE_MAX))
			select_shape(HDefectRgn, &HDefectRgn, "height", "and", Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax);
		if (ValidParam(Param.m_bUseDefectConditionArea, Param.m_iDefectConditionAreaMin, Param.m_iDefectConditionAreaMax, PARAM_SCALE_MAX))
			select_shape(HDefectRgn, &HDefectRgn, "area", "and", Param.m_iDefectConditionAreaMin, Param.m_iDefectConditionAreaMax);
			
		// 폭 조건
		if (ValidParam(Param.m_bUseDefectConditionLength, Param.m_iDefectConditionLengthMin, Param.m_iDefectConditionLengthMax, PARAM_SCALE_MAX))
			select_shape(HDefectRgn, &HDefectRgn, "max_diameter", "and", Param.m_iDefectConditionLengthMin, Param.m_iDefectConditionLengthMax);
		if (ValidParam(Param.m_bUseDefectConditionWidth, Param.m_iDefectConditionWidthMin, Param.m_iDefectConditionWidthMax, PARAM_SCALE_MAX))
			select_shape(HDefectRgn, &HDefectRgn, "min_diameter", "and", Param.m_iDefectConditionWidthMin, Param.m_iDefectConditionWidthMax);

		// 밝기 조건
		if (ValidParam(Param.m_bUseDefectConditionMean, Param.m_iDefectConditionMeanMin, Param.m_iDefectConditionMeanMax, PARAM_BRIGHT_MAX))
			select_gray(HDefectRgn, HImage, &HDefectRgn, "mean", "and", (double)Param.m_iDefectConditionMeanMin, (double)Param.m_iDefectConditionMeanMax);
		if (ValidParam(Param.m_bUseDefectConditionStdev, Param.m_dDefectConditionStdevMin, Param.m_dDefectConditionStdevMax, PARAM_RATIO_MAX))
			select_gray(HDefectRgn, HImage, &HDefectRgn, "deviation", "and", (double)Param.m_dDefectConditionStdevMin, (double)Param.m_dDefectConditionStdevMax);

		// 형태 조건 (원형/컨벡스 유사도)
		if (ValidParam(Param.m_bUseDefectConditionCircularity, Param.m_dDefectConditionCircularityMin, Param.m_dDefectConditionCircularityMax, PARAM_SIMILARITY_MAX))
			select_shape(HDefectRgn, &HDefectRgn, "circularity", "and", Param.m_dDefectConditionCircularityMin, Param.m_dDefectConditionCircularityMax);
		if (ValidParam(Param.m_bUseDefectConditionConvexity, Param.m_dDefectConditionConvexityMin, Param.m_dDefectConditionConvexityMax, PARAM_SIMILARITY_MAX))
			select_shape(HDefectRgn, &HDefectRgn, "convexity", "and", Param.m_dDefectConditionConvexityMin, Param.m_dDefectConditionConvexityMax);

		// 장축/단축 비율
		if (ValidParam(Param.m_bUseDefectConditionEllipseRatio, Param.m_dDefectConditionEllipseRatioMin, Param.m_dDefectConditionEllipseRatioMax, PARAM_RATIO_MAX))
			select_shape(HDefectRgn, &HDefectRgn, "anisometry", "and", Param.m_dDefectConditionEllipseRatioMin, Param.m_dDefectConditionEllipseRatioMax);

		// 길이/폭 비율
		if (ValidParam(Param.m_bUseDefectConditionAnisometry, Param.m_dDefectConditionAnisometryMin, Param.m_dDefectConditionAnisometryMax, PARAM_RATIO_MAX))
		{
			Hlong lNoBlob;
			Hlong lRow1, lRow2, lCol1, lCol2;
			double dBlobLength, dBlobWidth;
			double dCircleRow, dCircleCol, dCircleRadius, dRatio;
			Hobject HConnInputRgn, HSelectedRgn;

			connection(HDefectRgn, &HConnInputRgn);
			count_obj(HConnInputRgn, &lNoBlob);

			gen_empty_obj(&HDefectRgn);

			for (i=1; i<=lNoBlob; i++)
			{
				select_obj(HConnInputRgn, &HSelectedRgn, i);

				if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn)==FALSE)
					continue;

				diameter_region(HSelectedRgn, &lRow1, &lCol1, &lRow2, &lCol2, &dBlobLength);
				inner_circle(HSelectedRgn, &dCircleRow, &dCircleCol, &dCircleRadius);
				dBlobWidth =  dCircleRadius * 2.0;

				if (dBlobWidth<=0)
					continue;

				dRatio = dBlobLength / dBlobWidth;

				if (dRatio <= Param.m_dDefectConditionAnisometryMax && dRatio >= Param.m_dDefectConditionAnisometryMin)
					concat_obj(HDefectRgn, HSelectedRgn, &HDefectRgn);
			}
		}

		if (Param.m_bUseDefectConditionInnerDist && Param.m_iDefectConditionInnerDist>=0)	// 내측 기준선
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn)==TRUE)
			{
				Hobject HInspectAreaFillRgn, HInspectAreaInnerRgn;

				Hlong lNoBlob;
				Hobject HInspectAreaConnRgn;
				Hobject HConnInputRgn, HSelectedRgn, HDiffRgn;;

				connection(HInspectAreaRgn, &HInspectAreaConnRgn);
				count_obj(HInspectAreaConnRgn, &lNoBlob);

				if (lNoBlob==1)		// 검사영역 1개 있을때만 사용
				{
					fill_up(HInspectAreaRgn, &HInspectAreaFillRgn);
					difference(HInspectAreaFillRgn, HInspectAreaRgn, &HInspectAreaInnerRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaInnerRgn)==TRUE)
					{
						dilation_circle(HInspectAreaInnerRgn, &HInspectAreaInnerRgn, (double)Param.m_iDefectConditionInnerDist+0.5);

						connection(HDefectRgn, &HConnInputRgn);
						count_obj(HConnInputRgn, &lNoBlob);

						gen_empty_obj(&HDefectRgn);

						for (i=1; i<=lNoBlob; i++)
						{
							select_obj(HConnInputRgn, &HSelectedRgn, i);

							if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn)==FALSE)
								continue;

							difference(HSelectedRgn, HInspectAreaInnerRgn, &HDiffRgn);

							if (THEAPP.m_pGFunction->ValidHRegion(HDiffRgn)==TRUE)
								concat_obj(HDefectRgn, HSelectedRgn, &HDefectRgn);
						}

					}
				}
			}
		}

		if (Param.m_bUseDefectConditionOuterDist && Param.m_iDefectConditionOuterDist>=0)		// 외측 기준선
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn)==TRUE)
			{
				Hobject HInspectAreaOuterRgn;

				fill_up(HInspectAreaRgn, &HInspectAreaOuterRgn);
				erosion_circle(HInspectAreaOuterRgn, &HInspectAreaOuterRgn, (double)Param.m_iDefectConditionOuterDist+0.5);

				if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaOuterRgn)==TRUE)
				{
					select_shape_proto(HDefectRgn, HInspectAreaOuterRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
				}
			}
		}

		if (Param.m_bUseDefectConditionEpoxyDist && Param.m_iDefectConditionEpoxyDist>=0)	// Epoxy 기준선
		{
			if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn)==TRUE)
			{
				Hobject HInspectAreaFillRgn, HInspectAreaInnerRgn;

				Hlong lNoBlob;
				Hobject HInspectAreaConnRgn;
				Hobject HConnInputRgn, HSelectedRgn, HDiffRgn;;

				connection(HInspectAreaRgn, &HInspectAreaConnRgn);
				count_obj(HInspectAreaConnRgn, &lNoBlob);

				if (lNoBlob==1)		// 검사영역 1개 있을때만 사용
				{
					fill_up(HInspectAreaRgn, &HInspectAreaFillRgn);
					difference(HInspectAreaFillRgn, HInspectAreaRgn, &HInspectAreaInnerRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaInnerRgn)==TRUE)
					{
						dilation_circle(HInspectAreaInnerRgn, &HInspectAreaInnerRgn, (double)Param.m_iDefectConditionEpoxyDist+0.5);
						select_shape_proto(HDefectRgn, HInspectAreaInnerRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
					}
				}
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn)==TRUE)
			union1(HDefectRgn, &HDefectRgn);

		return HDefectRgn;
	}
	catch (HException& except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::CommonAlgorithm] : %s", except.message);	THEAPP.SaveLog(str);
		Hobject HDefectRgn;	gen_empty_obj(&HDefectRgn);	return HDefectRgn;
	}
}

Hobject Algorithm::CommonAlgorithmROI(Hobject HOrgImage, GTRegion* pInspectROIRgn, CAlgorithmParam Param, Hobject *pHFoundInspectAreaRgn, Hobject *pHResultXld, int iThreadIdx)
{
	try
	{
		BOOL bDebugSave = FALSE;

		Hobject HDefectRgn, HInspectAreaRgn;
		gen_empty_obj(&HDefectRgn);
		gen_empty_obj(pHResultXld);
		gen_empty_obj(pHFoundInspectAreaRgn);

		HInspectAreaRgn = pInspectROIRgn->GetROIHRegion(THEAPP.m_pCalDataService);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;
		get_image_pointer1(HOrgImage, (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

		//////////////////////////////////////////////////////////////////////////
		//  Flexible Inspection ROI
		GetInspectAreaROI(HOrgImage, HInspectAreaRgn, Param, &HInspectAreaRgn, iThreadIdx);
		union1(HInspectAreaRgn, pHFoundInspectAreaRgn);

		Hobject HImage;
		if (Param.m_bUseImageProcessFilter == TRUE)
			GetPreprocessImage(TRUE, HOrgImage, HInspectAreaRgn, Param, &HImage);
		else
			copy_image(HOrgImage, &HImage);

		/////////////////////////////////////////////////////////////////////////
		//비등방
		Hobject HAnisoAlignRgn;
		BOOL bAnisoAlignShiftResult;
		if (Param.m_bUseROIAnisoAlign && Param.m_bROIAnisoAlignInspectShift)
		{
			HAnisoAlignRgn = AnisoAlignAlgorithm(pInspectROIRgn->m_HAnisoModelID, HImage, HInspectAreaRgn, Param, &bAnisoAlignShiftResult, &pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx], &pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx], &pInspectROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx]);

			if (bAnisoAlignShiftResult == TRUE)
				concat_obj(HDefectRgn, HAnisoAlignRgn, &HDefectRgn);
		}

		// Part 확인 알고리즘
		Hobject HPartCheckDefectRgn, HPartCheckAlignRgn;
		int iPartCheckLocalAlignDeltaX, iPartCheckLocalAlignDeltaY;
		double dPartCheckLocalAlignDeltaAngle, dPartCheckLocalAlignDeltaAngleFixedPointX, dPartCheckLocalAlignDeltaAngleFixedPointY;
		if (Param.m_bROIPartCheckUse && (Param.m_bROIPartCheckExistInspUse || Param.m_bROIPartCheckShiftInspUse))
		{
			HPartCheckDefectRgn = PartCheckAlgorithm(pInspectROIRgn->m_HPartModelID, HImage, HInspectAreaRgn, Param, &pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx], &pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx], &pInspectROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx], &pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx], &HPartCheckAlignRgn);

			if (THEAPP.m_pGFunction->ValidHRegion(HPartCheckDefectRgn) == TRUE)
				concat_obj(HDefectRgn, HPartCheckDefectRgn, &HDefectRgn);
		}

		// Edge 측정
		BOOL bEdgeMeasureSuccess = FALSE;
		Hobject HLineFitXLD;
		gen_empty_obj(&HLineFitXLD);
		if (Param.m_bROIEdgeMeasureUse)
		{
			EdgeMeasureAlgorithm(HImage, HInspectAreaRgn, Param, &HLineFitXLD, &pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx], &pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx], &pInspectROIRgn->m_dEdgeCenterX[iThreadIdx], &pInspectROIRgn->m_dEdgeCenterY[iThreadIdx], &pInspectROIRgn->m_dLineStartX[iThreadIdx], &pInspectROIRgn->m_dLineStartY[iThreadIdx], &pInspectROIRgn->m_dLineEndX[iThreadIdx], &pInspectROIRgn->m_dLineEndY[iThreadIdx], &pInspectROIRgn->m_dEdgeStrength[iThreadIdx]);

			if (THEAPP.m_pGFunction->ValidHXLD(HLineFitXLD) == TRUE)
				concat_obj(HLineFitXLD, *pHResultXld, pHResultXld);
			else
				gen_empty_obj(pHResultXld);
		}


		// 기본 검사 알고리즘
		if (Param.m_bUseCommonAlgorithm)
		{
			Hobject HProcessImage;

			HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
			long lPeak, lHystUpperThreshold, lHystLowThreshold;

			if (Param.m_bUseImageScaling)
			{
				int iScaleMin, iScaleMax;
				int iScalePeakUpperOffset = 20;
				int iScalePeakLowerOffset = 10;

				if (FALSE)
				{
					write_image(HImage, "bmp", 0, "c:\\DualTest\\ScaleSurface");
					write_region(HInspectAreaRgn, "c:\\DualTest\\ScaleInsepctArea.reg");
				}

				if (Param.m_iImageScaleMethod == IMAGE_SCALE_AUTO)
				{
					gray_histo(HInspectAreaRgn, HImage, &HAbsHisto, &HRelHisto);
					create_funct_1d_array(HAbsHisto, &HFunction);
					smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
					tuple_last_n(HSmoothFunction, 3, &HAbsHisto);
					tuple_sort_index(HAbsHisto, &Indices);
					tuple_inverse(Indices, &Inverted);
					lPeak = Inverted[0].L();

					iScaleMax = lPeak + iScalePeakUpperOffset;
					if (iScaleMax > 255)
						iScaleMax = 255;

					iScaleMin = lPeak - iScalePeakLowerOffset;
					if (iScaleMin < 0)
						iScaleMin = 0;
				}
				else
				{
					if (Param.m_iImageScaleMin < 0)
						iScaleMin = 0;
					else if (Param.m_iImageScaleMin > 255)
						iScaleMin = 255;
					else
						iScaleMin = Param.m_iImageScaleMin;

					if (Param.m_iImageScaleMax < 0)
						iScaleMax = 0;
					else if (Param.m_iImageScaleMax > 255)
						iScaleMax = 255;
					else
						iScaleMax = Param.m_iImageScaleMax;

					if (iScaleMax < iScaleMin)
						iScaleMax = iScaleMin;
				}

				THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
				binomial_filter(HProcessImage, &HProcessImage, 3, 3);
			}

			Hobject HImageReduced;

			if (Param.m_bUseImageScaling)
				reduce_domain(HProcessImage, HInspectAreaRgn, &HImageReduced);
			else
				reduce_domain(HImage, HInspectAreaRgn, &HImageReduced);

			Hobject HInspectAreaConnRgn;
			Hlong lNoProcessRgn;
			int i, j, k;
			Hobject HOutRgn, HInRgn, HInspectROIRgn;

			Hobject HBrightRgn, HDarkRgn, HThresRgn;
			Hobject HExpandReduceImage;

			HTuple dInCenterX, dInCenterY, dInRadius;
			HTuple dOutCenterX, dOutCenterY, dOutRadius;
			HTuple iRingSize, iRadius;

			Hobject HDTImageReduced, HImagePolar, HPartImage, Hmean_image, HDynThreshRgn;
			Hobject HInvertImageReduced;
			double dScratchMaxRadius;

			Hobject HBrightFixedThresholdConnectedRgn, HDarkFixedThresholdConnectedRgn;

			int iPolarResolution = 640;

			gen_empty_obj(&HBrightRgn);
			gen_empty_obj(&HDarkRgn);
			gen_empty_obj(&HDynThreshRgn);

			if (Param.m_bUseBrightDT)
			{
				gen_empty_obj(&HDynThreshRgn);

				HDTImageReduced = HImageReduced;
				if (Param.m_iBrightMedianFilterSize > 0)
					median_image(HDTImageReduced, &HDTImageReduced, "circle", Param.m_iBrightMedianFilterSize, "mirrored");
				if (Param.m_iBrightClosingFilterSize > 0)
					gray_closing_shape(HDTImageReduced, &HDTImageReduced, Param.m_iBrightClosingFilterSize, Param.m_iBrightClosingFilterSize, "octagon");

				int iFilterX, iFilterY;
				iFilterX = Param.m_iBrightDTFilterSize;
				if (iFilterX <= 0)
					iFilterX = 1;
				iFilterY = Param.m_iBrightDTFilterSize;
				if (iFilterY <= 0)
					iFilterY = 1;

				int iExpandSize;
				if (Param.m_iBrightDTFilterSize > Param.m_iBrightDTFilterSize)
					iExpandSize = Param.m_iBrightDTFilterSize / 2;
				else
					iExpandSize = Param.m_iBrightDTFilterSize / 2;

				expand_domain_gray(HDTImageReduced, &HExpandReduceImage, iExpandSize);
				reduce_domain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

				mean_image(HExpandReduceImage, &Hmean_image, iFilterX, iFilterY);
				dyn_threshold(HExpandReduceImage, Hmean_image, &HDynThreshRgn, Param.m_iBrightDTValue, "light");

				if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
				{
					concat_obj(HBrightRgn, HDynThreshRgn, &HBrightRgn);
				}
			}

			if (Param.m_bUseDarkDT)
			{
				gen_empty_obj(&HDynThreshRgn);

				HDTImageReduced = HImageReduced;
				if (Param.m_iDarkMedianFilterSize > 0)
					median_image(HDTImageReduced, &HDTImageReduced, "circle", Param.m_iDarkMedianFilterSize, "mirrored");
				if (Param.m_iDarkClosingFilterSize > 0)
					gray_closing_shape(HDTImageReduced, &HDTImageReduced, Param.m_iDarkClosingFilterSize, Param.m_iDarkClosingFilterSize, "octagon");

				int iFilterX, iFilterY;
				iFilterX = Param.m_iDarkDTFilterSize;
				if (iFilterX <= 0)
					iFilterX = 1;
				iFilterY = Param.m_iDarkDTFilterSize;
				if (iFilterY <= 0)
					iFilterY = 1;

				int iExpandSize;
				if (Param.m_iDarkDTFilterSize > Param.m_iDarkDTFilterSize)
					iExpandSize = Param.m_iDarkDTFilterSize / 2;
				else
					iExpandSize = Param.m_iDarkDTFilterSize / 2;

				expand_domain_gray(HDTImageReduced, &HExpandReduceImage, iExpandSize);
				reduce_domain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

				mean_image(HExpandReduceImage, &Hmean_image, iFilterX, iFilterY);
				dyn_threshold(HExpandReduceImage, Hmean_image, &HDynThreshRgn, Param.m_iDarkDTValue, "dark");

				if (THEAPP.m_pGFunction->ValidHRegion(HDynThreshRgn) == TRUE)
				{
					concat_obj(HDarkRgn, HDynThreshRgn, &HDarkRgn);
				}
			}

			gen_empty_obj(&HBrightFixedThresholdConnectedRgn);

			if (Param.m_bUseBrightFixedThres)
			{
				threshold(HImageReduced, &HThresRgn, Param.m_iBrightLowerThres, Param.m_iBrightUpperThres);
				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
				{
					concat_obj(HBrightRgn, HThresRgn, &HBrightRgn);

					if (Param.m_bUseFTConnected)
					{
						connection(HThresRgn, &HBrightFixedThresholdConnectedRgn);

						if (Param.m_bUseFTConnectedArea && Param.m_iFTConnectedAreaMin > 0)
							select_shape(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn, "area", "and", Param.m_iFTConnectedAreaMin, MAX_DEF);
						if (Param.m_bUseFTConnectedLength && Param.m_iFTConnectedLengthMin > 0)
							select_shape(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn, "max_diameter", "and", Param.m_iFTConnectedLengthMin, MAX_DEF);
						if (Param.m_bUseFTConnectedWidth && Param.m_iFTConnectedWidthMin > 0)
							select_shape(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn, "inner_radius", "and", Param.m_iFTConnectedWidthMin / 2, MAX_DEF);

						union1(HBrightFixedThresholdConnectedRgn, &HBrightFixedThresholdConnectedRgn);
					}
				}
			}

			if (Param.m_bUseBrightUniformityCheck)
			{
				gray_histo(HInspectAreaRgn, HImageReduced, &HAbsHisto, &HRelHisto);
				create_funct_1d_array(HAbsHisto, &HFunction);
				smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
				tuple_last_n(HSmoothFunction, 3, &HAbsHisto);
				tuple_sort_index(HAbsHisto, &Indices);
				tuple_inverse(Indices, &Inverted);
				lPeak = Inverted[0].L();

				lHystUpperThreshold = lPeak + Param.m_iBrightUniformityOffset;
				if (lHystUpperThreshold > 255)
					lHystUpperThreshold = 255;

				lHystLowThreshold = lHystUpperThreshold - Param.m_iBrightUniformityHystOffset;
				if (lHystLowThreshold < 0)
					lHystLowThreshold = 0;

				if (Param.m_iBrightUniformityHystLength > 0)
					hysteresis_threshold(HImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, Param.m_iBrightUniformityHystLength);
				else
					threshold(HImageReduced, &HThresRgn, lHystUpperThreshold, 255);

				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
					concat_obj(HBrightRgn, HThresRgn, &HBrightRgn);
			}

			if (Param.m_bUseBrightHystThres)
			{
				if (Param.m_iBrightHystSecureThres >= Param.m_iBrightHystPotentialThres)
				{
					hysteresis_threshold(HImageReduced, &HThresRgn, Param.m_iBrightHystPotentialThres, Param.m_iBrightHystSecureThres, Param.m_iBrightHystPotentialLength);
					if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
						concat_obj(HBrightRgn, HThresRgn, &HBrightRgn);
				}
			}

			gen_empty_obj(&HDarkFixedThresholdConnectedRgn);

			if (Param.m_bUseDarkFixedThres)
			{
				threshold(HImageReduced, &HThresRgn, Param.m_iDarkLowerThres, Param.m_iDarkUpperThres);
				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
				{
					concat_obj(HDarkRgn, HThresRgn, &HDarkRgn);

					if (Param.m_bUseFTConnected)
					{
						connection(HThresRgn, &HDarkFixedThresholdConnectedRgn);

						if (Param.m_bUseFTConnectedArea && Param.m_iFTConnectedAreaMin > 0)
							select_shape(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn, "area", "and", Param.m_iFTConnectedAreaMin, MAX_DEF);
						if (Param.m_bUseFTConnectedLength && Param.m_iFTConnectedLengthMin > 0)
							select_shape(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn, "max_diameter", "and", Param.m_iFTConnectedLengthMin, MAX_DEF);
						if (Param.m_bUseFTConnectedWidth && Param.m_iFTConnectedWidthMin > 0)
							select_shape(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn, "inner_radius", "and", Param.m_iFTConnectedWidthMin / 2, MAX_DEF);

						union1(HDarkFixedThresholdConnectedRgn, &HDarkFixedThresholdConnectedRgn);
					}
				}
			}

			if (Param.m_bUseDarkUniformityCheck)
			{
				invert_image(HImageReduced, &HInvertImageReduced);

				gray_histo(HInspectAreaRgn, HInvertImageReduced, &HAbsHisto, &HRelHisto);
				create_funct_1d_array(HAbsHisto, &HFunction);
				smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
				tuple_last_n(HSmoothFunction, 3, &HAbsHisto);
				tuple_sort_index(HAbsHisto, &Indices);
				tuple_inverse(Indices, &Inverted);
				lPeak = Inverted[0].L();

				lHystUpperThreshold = lPeak + Param.m_iDarkUniformityOffset;
				if (lHystUpperThreshold > 255)
					lHystUpperThreshold = 255;

				lHystLowThreshold = lHystUpperThreshold - Param.m_iDarkUniformityHystOffset;
				if (lHystLowThreshold < 0)
					lHystLowThreshold = 0;

				if (Param.m_iDarkUniformityHystLength > 0)
					hysteresis_threshold(HInvertImageReduced, &HThresRgn, lHystLowThreshold, lHystUpperThreshold, Param.m_iDarkUniformityHystLength);
				else
					threshold(HInvertImageReduced, &HThresRgn, lHystUpperThreshold, 255);

				if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
					concat_obj(HDarkRgn, HThresRgn, &HDarkRgn);
			}

			if (Param.m_bUseDarkHystThres)
			{
				if (Param.m_iDarkHystSecureThres <= Param.m_iDarkHystPotentialThres)
				{
					int iTempSecureThres, iTempPotentialThres;
					iTempSecureThres = 255 - Param.m_iDarkHystSecureThres;
					if (iTempSecureThres < 0)
						iTempSecureThres = 0;
					if (iTempSecureThres > 255)
						iTempSecureThres = 255;
					iTempPotentialThres = 255 - Param.m_iDarkHystPotentialThres;
					if (iTempPotentialThres < 0)
						iTempPotentialThres = 0;
					if (iTempPotentialThres > 255)
						iTempPotentialThres = 255;

					if (iTempSecureThres >= iTempPotentialThres)
					{
						invert_image(HImageReduced, &HInvertImageReduced);

						hysteresis_threshold(HInvertImageReduced, &HThresRgn, iTempPotentialThres, iTempSecureThres, Param.m_iDarkHystPotentialLength);
						if (THEAPP.m_pGFunction->ValidHRegion(HThresRgn) == TRUE)
							concat_obj(HDarkRgn, HThresRgn, &HDarkRgn);
					}
				}
			}

			gen_empty_obj(&HInvertImageReduced);

			union1(HBrightRgn, &HBrightRgn);

			if (Param.m_bUseFTConnected)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HBrightFixedThresholdConnectedRgn) == TRUE)
				{
					connection(HBrightRgn, &HBrightRgn);
					select_shape_proto(HBrightRgn, HBrightFixedThresholdConnectedRgn, &HBrightRgn, "overlaps_abs", 1, 9999999);
					union1(HBrightRgn, &HBrightRgn);
				}
				else
					gen_empty_obj(&HBrightRgn);
			}

			union1(HDarkRgn, &HDarkRgn);

			if (Param.m_bUseFTConnected)
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HDarkFixedThresholdConnectedRgn) == TRUE)
				{
					connection(HDarkRgn, &HDarkRgn);
					select_shape_proto(HDarkRgn, HDarkFixedThresholdConnectedRgn, &HDarkRgn, "overlaps_abs", 1, 9999999);
					union1(HDarkRgn, &HDarkRgn);
				}
				else
					gen_empty_obj(&HDarkRgn);
			}

			if (Param.m_bCombineBrightDarkBlob)
			{
				union2(HBrightRgn, HDarkRgn, &HBrightRgn);
				gen_empty_obj(&HDarkRgn);
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
			{
				if (Param.m_iOpeningSize > 0)
					opening_circle(HBrightRgn, &HBrightRgn, (double)Param.m_iOpeningSize + 0.5);
				if (Param.m_iClosingSize > 0)
					closing_circle(HBrightRgn, &HBrightRgn, (double)Param.m_iClosingSize + 0.5);
				if (Param.m_bFillup)
				{
					connection(HBrightRgn, &HBrightRgn);
					fill_up(HBrightRgn, &HBrightRgn);
				}

				if (Param.m_bUseConnection)
				{
					connection(HBrightRgn, &HBrightRgn);

					select_shape(HBrightRgn, &HBrightRgn, "area", "and", Param.m_iConnectionMinSize, MAX_DEF);

					if (Param.m_bUseAngle)
					{
						dScratchMaxRadius = (double)Param.m_iConnectionMaxWidth * 0.5;
						select_shape(HBrightRgn, &HBrightRgn, "inner_radius", "and", 0, dScratchMaxRadius);
						BlobUnionAngle(&HBrightRgn, Param.m_iConnectionLength, 10);
					}
					else
					{
						BlobUnion(&HBrightRgn, Param.m_iConnectionLength);
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HBrightRgn) == TRUE)
				{
					union1(HBrightRgn, &HBrightRgn);
					concat_obj(HDefectRgn, HBrightRgn, &HDefectRgn);
				}
			}

			if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
			{
				if (Param.m_iOpeningSize > 0)
					opening_circle(HDarkRgn, &HDarkRgn, (double)Param.m_iOpeningSize + 0.5);
				if (Param.m_iClosingSize > 0)
					closing_circle(HDarkRgn, &HDarkRgn, (double)Param.m_iClosingSize + 0.5);
				if (Param.m_bFillup)
				{
					connection(HDarkRgn, &HDarkRgn);
					fill_up(HDarkRgn, &HDarkRgn);
				}

				if (Param.m_bUseConnection)
				{
					connection(HDarkRgn, &HDarkRgn);

					select_shape(HDarkRgn, &HDarkRgn, "area", "and", Param.m_iConnectionMinSize, MAX_DEF);

					if (Param.m_bUseAngle)
					{
						dScratchMaxRadius = (double)Param.m_iConnectionMaxWidth * 0.5;
						select_shape(HDarkRgn, &HDarkRgn, "inner_radius", "and", 0, dScratchMaxRadius);
						BlobUnionAngle(&HDarkRgn, Param.m_iConnectionLength, 10);
					}
					else
					{
						BlobUnion(&HDarkRgn, Param.m_iConnectionLength);
					}
				}

				if (THEAPP.m_pGFunction->ValidHRegion(HDarkRgn) == TRUE)
				{
					union1(HDarkRgn, &HDarkRgn);
					concat_obj(HDefectRgn, HDarkRgn, &HDefectRgn);
				}
			}

			if (Param.m_bUseEdge)
			{
				Hobject HExpandReduceImage, HZoomImage;
				Hobject HEdgeContour, HEdgeDetectionRgn;
				double dInverse;
				int iEdgeLow, iEdgeHigh;
				HTuple HomMat2DIdentity, HomMat2DScale;

				gen_empty_obj(&HEdgeDetectionRgn);

				if (Param.m_dEdgeZoomRatio <= 0)
					Param.m_dEdgeZoomRatio = 0.2;

				dInverse = 1.0 / Param.m_dEdgeZoomRatio;

				expand_domain_gray(HImageReduced, &HExpandReduceImage, (long)(dInverse * 2.0));
				reduce_domain(HExpandReduceImage, HInspectAreaRgn, &HExpandReduceImage);

				zoom_image_factor(HExpandReduceImage, &HZoomImage, Param.m_dEdgeZoomRatio, Param.m_dEdgeZoomRatio, "constant");

				if (THEAPP.m_pGFunction->ValidHImage(HZoomImage) == TRUE)
				{
					if (Param.m_iEdgeStrength < 6)
						iEdgeHigh = 6;
					else
						iEdgeHigh = Param.m_iEdgeStrength;
					iEdgeLow = iEdgeHigh / 2;

					edges_sub_pix(HZoomImage, &HEdgeContour, "canny", 1.0, iEdgeLow, iEdgeHigh);
					if (THEAPP.m_pGFunction->ValidHXLD(HEdgeContour) == TRUE)
					{
						hom_mat2d_identity(&HomMat2DIdentity);
						hom_mat2d_scale(HomMat2DIdentity, dInverse, dInverse, 0.0, 0.0, &HomMat2DScale);
						affine_trans_contour_xld(HEdgeContour, &HEdgeContour, HomMat2DScale);

						union_adjacent_contours_xld(HEdgeContour, &HEdgeContour, (double)Param.m_iEdgeConnectionDistance, 1.0, "attr_forget");
						select_contours_xld(HEdgeContour, &HEdgeContour, "contour_length", (double)Param.m_iEdgeMinContourLength, 1e9, -0.5, 0.5);
						if (THEAPP.m_pGFunction->ValidHXLD(HEdgeContour) == TRUE)
						{
							select_contours_xld(HEdgeContour, &HEdgeContour, "closed", 0.5, (double)Param.m_iEdgeMaxEndpointDistance, -0.5, 0.5);
							if (THEAPP.m_pGFunction->ValidHXLD(HEdgeContour) == TRUE)
							{
								close_contours_xld(HEdgeContour, &HEdgeContour);
								gen_region_contour_xld(HEdgeContour, &HEdgeDetectionRgn, "filled");

								if (THEAPP.m_pGFunction->ValidHRegion(HEdgeDetectionRgn) == TRUE)
								{
									union1(HEdgeDetectionRgn, &HEdgeDetectionRgn);

									concat_obj(HDefectRgn, HEdgeDetectionRgn, &HDefectRgn);
								}
							}
						}
					}
				}
			}

			intersection(HInspectAreaRgn, HDefectRgn, &HDefectRgn);

			connection(HDefectRgn, &HDefectRgn);
			// 길이/너비/면적 조건
			if (ValidParam(Param.m_bDefectConditionXLength, Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax, PARAM_SCALE_MAX))
				select_shape(HDefectRgn, &HDefectRgn, "width", "and", Param.m_iDefectConditionXLengthMin, Param.m_iDefectConditionXLengthMax);
			if (ValidParam(Param.m_bDefectConditionYLength, Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax, PARAM_SCALE_MAX))
				select_shape(HDefectRgn, &HDefectRgn, "height", "and", Param.m_iDefectConditionYLengthMin, Param.m_iDefectConditionYLengthMax);
			if (ValidParam(Param.m_bUseDefectConditionArea, Param.m_iDefectConditionAreaMin, Param.m_iDefectConditionAreaMax, PARAM_SCALE_MAX))
				select_shape(HDefectRgn, &HDefectRgn, "area", "and", Param.m_iDefectConditionAreaMin, Param.m_iDefectConditionAreaMax);
				
			// 폭 조건
			if (ValidParam(Param.m_bUseDefectConditionLength, Param.m_iDefectConditionLengthMin, Param.m_iDefectConditionLengthMax, PARAM_SCALE_MAX))
				select_shape(HDefectRgn, &HDefectRgn, "max_diameter", "and", Param.m_iDefectConditionLengthMin, Param.m_iDefectConditionLengthMax);
			if (ValidParam(Param.m_bUseDefectConditionWidth, Param.m_iDefectConditionWidthMin, Param.m_iDefectConditionWidthMax, PARAM_SCALE_MAX))
				select_shape(HDefectRgn, &HDefectRgn, "min_diameter", "and", Param.m_iDefectConditionWidthMin, Param.m_iDefectConditionWidthMax);

			// 밝기 조건
			if (ValidParam(Param.m_bUseDefectConditionMean, Param.m_iDefectConditionMeanMin, Param.m_iDefectConditionMeanMax, PARAM_BRIGHT_MAX))
				select_gray(HDefectRgn, HImage, &HDefectRgn, "mean", "and", (double)Param.m_iDefectConditionMeanMin, (double)Param.m_iDefectConditionMeanMax);
			if (ValidParam(Param.m_bUseDefectConditionStdev, Param.m_dDefectConditionStdevMin, Param.m_dDefectConditionStdevMax, PARAM_RATIO_MAX))
				select_gray(HDefectRgn, HImage, &HDefectRgn, "deviation", "and", (double)Param.m_dDefectConditionStdevMin, (double)Param.m_dDefectConditionStdevMax);

			// 형태 조건 (원형/컨벡스 유사도)
			if (ValidParam(Param.m_bUseDefectConditionCircularity, Param.m_dDefectConditionCircularityMin, Param.m_dDefectConditionCircularityMax, PARAM_SIMILARITY_MAX))
				select_shape(HDefectRgn, &HDefectRgn, "circularity", "and", Param.m_dDefectConditionCircularityMin, Param.m_dDefectConditionCircularityMax);
			if (ValidParam(Param.m_bUseDefectConditionConvexity, Param.m_dDefectConditionConvexityMin, Param.m_dDefectConditionConvexityMax, PARAM_SIMILARITY_MAX))
				select_shape(HDefectRgn, &HDefectRgn, "convexity", "and", Param.m_dDefectConditionConvexityMin, Param.m_dDefectConditionConvexityMax);

			// 장축/단축 비율
			if (ValidParam(Param.m_bUseDefectConditionEllipseRatio, Param.m_dDefectConditionEllipseRatioMin, Param.m_dDefectConditionEllipseRatioMax, PARAM_RATIO_MAX))
				select_shape(HDefectRgn, &HDefectRgn, "anisometry", "and", Param.m_dDefectConditionEllipseRatioMin, Param.m_dDefectConditionEllipseRatioMax);

			// 길이/폭 비율
			if (ValidParam(Param.m_bUseDefectConditionAnisometry, Param.m_dDefectConditionAnisometryMin, Param.m_dDefectConditionAnisometryMax, PARAM_RATIO_MAX))
			{
				Hlong lNoBlob;
				Hlong lRow1, lRow2, lCol1, lCol2;
				double dBlobLength, dBlobWidth;
				double dCircleRow, dCircleCol, dCircleRadius, dRatio;
				Hobject HConnInputRgn, HSelectedRgn;

				connection(HDefectRgn, &HConnInputRgn);
				count_obj(HConnInputRgn, &lNoBlob);

				gen_empty_obj(&HDefectRgn);

				for (i = 1; i <= lNoBlob; i++)
				{
					select_obj(HConnInputRgn, &HSelectedRgn, i);

					if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
						continue;

					diameter_region(HSelectedRgn, &lRow1, &lCol1, &lRow2, &lCol2, &dBlobLength);
					inner_circle(HSelectedRgn, &dCircleRow, &dCircleCol, &dCircleRadius);
					dBlobWidth = dCircleRadius * 2.0;

					if (dBlobWidth <= 0)
						continue;

					dRatio = dBlobLength / dBlobWidth;

					if (dRatio <= Param.m_dDefectConditionAnisometryMax && dRatio >= Param.m_dDefectConditionAnisometryMin)
						concat_obj(HDefectRgn, HSelectedRgn, &HDefectRgn);
				}
			}

			if (Param.m_bDefectConditionContrast)
			{
				Hobject HDefectConRgn;

				connection(HDefectRgn, &HDefectConRgn);

				Hlong lNoDefect;
				count_obj(HDefectConRgn, &lNoDefect);

				Hobject HSelectedRgn, HFillupRgn, HDilatedRgn, HBGRgn, HBGRgnImageReduced, HDefectBoundaryRgn;
				double dBgGray, dDefectGray, dSd, dContrast;
				int iBGMinThres, iBGMaxThres;

				int iContrastCheckWidthPxl = 7;

				gen_empty_obj(&HDefectRgn);

				for (i = 1; i <= lNoDefect; i++)
				{
					select_obj(HDefectConRgn, &HSelectedRgn, i);

					if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
						continue;

					fill_up(HSelectedRgn, &HFillupRgn);
					dilation_circle(HFillupRgn, &HDilatedRgn, (double)iContrastCheckWidthPxl + 0.5);
					difference(HDilatedRgn, HFillupRgn, &HBGRgn);
					//intersection(HBGRgn, HInspectAreaRgn, &HBGRgn);

					if (THEAPP.m_pGFunction->ValidHRegion(HBGRgn) == FALSE)	// Contrast 측정 불가
					{
						concat_obj(HDefectRgn, HSelectedRgn, &HDefectRgn);
						continue;
					}

					intensity(HBGRgn, HImage, &dBgGray, &dSd);

					dilation_circle(HBGRgn, &HDilatedRgn, (double)iContrastCheckWidthPxl + 0.5);
					intersection(HSelectedRgn, HDilatedRgn, &HDefectBoundaryRgn);

					intensity(HDefectBoundaryRgn, HImage, &dDefectGray, &dSd);

					dContrast = fabs(dBgGray - dDefectGray);

					if (dContrast >= Param.m_iDefectConditionContrastMin && dContrast <= Param.m_iDefectConditionContrastMax)
						concat_obj(HDefectRgn, HSelectedRgn, &HDefectRgn);
				}
			}

			if (Param.m_bUseDefectConditionInnerDist && Param.m_iDefectConditionInnerDist >= 0)	// 내측 기준선
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					Hobject HInspectAreaFillRgn, HInspectAreaInnerRgn;

					Hlong lNoBlob;
					Hobject HInspectAreaConnRgn;
					Hobject HConnInputRgn, HSelectedRgn, HDiffRgn;;

					connection(HInspectAreaRgn, &HInspectAreaConnRgn);
					count_obj(HInspectAreaConnRgn, &lNoBlob);

					if (lNoBlob == 1)		// 검사영역 1개 있을때만 사용
					{
						fill_up(HInspectAreaRgn, &HInspectAreaFillRgn);
						difference(HInspectAreaFillRgn, HInspectAreaRgn, &HInspectAreaInnerRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaInnerRgn) == TRUE)
						{
							dilation_circle(HInspectAreaInnerRgn, &HInspectAreaInnerRgn, (double)Param.m_iDefectConditionInnerDist + 0.5);

							connection(HDefectRgn, &HConnInputRgn);
							count_obj(HConnInputRgn, &lNoBlob);

							gen_empty_obj(&HDefectRgn);

							for (i = 1; i <= lNoBlob; i++)
							{
								select_obj(HConnInputRgn, &HSelectedRgn, i);

								if (THEAPP.m_pGFunction->ValidHRegion(HSelectedRgn) == FALSE)
									continue;

								difference(HSelectedRgn, HInspectAreaInnerRgn, &HDiffRgn);

								if (THEAPP.m_pGFunction->ValidHRegion(HDiffRgn) == TRUE)
									concat_obj(HDefectRgn, HSelectedRgn, &HDefectRgn);
							}

						}
					}
				}
			}

			if (Param.m_bUseDefectConditionOuterDist && Param.m_iDefectConditionOuterDist >= 0)		// 외측 기준선
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					Hobject HInspectAreaOuterRgn;

					fill_up(HInspectAreaRgn, &HInspectAreaOuterRgn);
					erosion_circle(HInspectAreaOuterRgn, &HInspectAreaOuterRgn, (double)Param.m_iDefectConditionOuterDist + 0.5);

					if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaOuterRgn) == TRUE)
					{
						select_shape_proto(HDefectRgn, HInspectAreaOuterRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
					}
				}
			}

			if (Param.m_bUseDefectConditionEpoxyDist && Param.m_iDefectConditionEpoxyDist >= 0)	// Epoxy 기준선
			{
				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
				{
					Hobject HInspectAreaFillRgn, HInspectAreaInnerRgn;

					Hlong lNoBlob;
					Hobject HInspectAreaConnRgn;
					Hobject HConnInputRgn, HSelectedRgn, HDiffRgn;;

					connection(HInspectAreaRgn, &HInspectAreaConnRgn);
					count_obj(HInspectAreaConnRgn, &lNoBlob);

					if (lNoBlob == 1)		// 검사영역 1개 있을때만 사용
					{
						fill_up(HInspectAreaRgn, &HInspectAreaFillRgn);
						difference(HInspectAreaFillRgn, HInspectAreaRgn, &HInspectAreaInnerRgn);

						if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaInnerRgn) == TRUE)
						{
							dilation_circle(HInspectAreaInnerRgn, &HInspectAreaInnerRgn, (double)Param.m_iDefectConditionEpoxyDist + 0.5);
							select_shape_proto(HDefectRgn, HInspectAreaInnerRgn, &HDefectRgn, "overlaps_abs", 1, 9999999);
						}
					}
				}
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn) == TRUE)
			union1(HDefectRgn, &HDefectRgn);

		return HDefectRgn;
	}
	catch (HException& except)
	{
		CString str;
		str.Format("Halcon Exception [Algorithm::CommonAlgorithmROI] : %s", except.message);
		THEAPP.SaveLog(str);

		Hobject HDefectRgn;
		gen_empty_obj(&HDefectRgn);

		return HDefectRgn;
	}
}

BOOL Algorithm::GetScaledImage(Hobject HImage, Hobject HROIRgn, CAlgorithmParam Param, Hobject *pHScaledImage, int iThreadIdx)
{
	try
	{
		Hobject HInspectAreaRgn;
		gen_empty_obj(&HInspectAreaRgn);

		GetInspectAreaROI(HImage, HROIRgn, Param, &HInspectAreaRgn, iThreadIdx);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return FALSE;

		Hobject HProcessImage;

		HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
		long lPeak, lHystUpperThreshold, lHystLowThreshold;

		int iScaleMin, iScaleMax;
		int iScalePeakUpperOffset = 20;
		int iScalePeakLowerOffset = 10;

		if (FALSE)
		{
			write_image(HImage, "bmp", 0, "c:\\DualTest\\ScaleSurface");
			write_region(HInspectAreaRgn, "c:\\DualTest\\ScaleInsepctArea.reg");
		}

		if (Param.m_iImageScaleMethod == IMAGE_SCALE_AUTO)
		{
			gray_histo(HInspectAreaRgn, HImage, &HAbsHisto, &HRelHisto);
			create_funct_1d_array(HAbsHisto, &HFunction);
			smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
			tuple_last_n(HSmoothFunction, 3, &HAbsHisto);
			tuple_sort_index(HAbsHisto, &Indices);
			tuple_inverse(Indices, &Inverted);
			lPeak = Inverted[0].L();

			iScaleMax = lPeak + iScalePeakUpperOffset;
			if (iScaleMax > 255)
				iScaleMax = 255;

			iScaleMin = lPeak - iScalePeakLowerOffset;
			if (iScaleMin < 0)
				iScaleMin = 0;
		}
		else
		{
			if (Param.m_iImageScaleMin < 0)
				iScaleMin = 0;
			else if (Param.m_iImageScaleMin > 255)
				iScaleMin = 255;
			else
				iScaleMin = Param.m_iImageScaleMin;

			if (Param.m_iImageScaleMax < 0)
				iScaleMax = 0;
			else if (Param.m_iImageScaleMax > 255)
				iScaleMax = 255;
			else
				iScaleMax = Param.m_iImageScaleMax;

			if (iScaleMax < iScaleMin)
				iScaleMax = iScaleMin;
		}

		THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
		binomial_filter(HProcessImage, &HProcessImage, 3, 3);

		copy_obj(HProcessImage, pHScaledImage, 1, 1);
		return TRUE;
	}
	catch (HException& except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::GetScaledImage] : %s", except.message);	THEAPP.SaveLog(str);
		return FALSE;
	}
}

BOOL Algorithm::GetScaledImage(Hobject HImage, Hobject HContour1, Hobject HContour2, Hobject HContour3, Hobject HContour4, Hobject HContour5, CAlgorithmParam Param, Hobject *pHScaledImage)
{
	try
	{
		Hobject HInspectAreaRgn;
		gen_empty_obj(&HInspectAreaRgn);

		GetInspectArea(HContour1, HContour2, HContour3, HContour4, HContour5, Param, &HInspectAreaRgn);
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn)==FALSE)
			return FALSE;

		Hobject HProcessImage;

		HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
		long lPeak, lHystUpperThreshold, lHystLowThreshold;

		int iScaleMin, iScaleMax;
		int iScalePeakUpperOffset = 20;
		int iScalePeakLowerOffset = 10;

		if (FALSE)
		{
			write_image(HImage, "bmp", 0, "c:\\DualTest\\ScaleSurface");
			write_region(HInspectAreaRgn, "c:\\DualTest\\ScaleInsepctArea.reg");
		}

		if (Param.m_iImageScaleMethod==IMAGE_SCALE_AUTO)
		{
			gray_histo(HInspectAreaRgn, HImage, &HAbsHisto, &HRelHisto);
			create_funct_1d_array(HAbsHisto, &HFunction);
			smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
			tuple_last_n(HSmoothFunction, 3, &HAbsHisto);
			tuple_sort_index(HAbsHisto, &Indices);
			tuple_inverse(Indices, &Inverted);
			lPeak = Inverted[0].L();

			iScaleMax = lPeak + iScalePeakUpperOffset;
			if (iScaleMax>255)
				iScaleMax = 255;

			iScaleMin = lPeak - iScalePeakLowerOffset;
			if (iScaleMin<0)
				iScaleMin = 0;
		}
		else
		{
			if (Param.m_iImageScaleMin<0)
				iScaleMin = 0;
			else if (Param.m_iImageScaleMin>255)
				iScaleMin = 255;
			else
				iScaleMin = Param.m_iImageScaleMin;

			if (Param.m_iImageScaleMax<0)
				iScaleMax = 0;
			else if (Param.m_iImageScaleMax>255)
				iScaleMax = 255;
			else
				iScaleMax = Param.m_iImageScaleMax;

			if (iScaleMax<iScaleMin)
				iScaleMax = iScaleMin;
		}

		THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
		binomial_filter(HProcessImage, &HProcessImage, 3, 3);

		copy_obj(HProcessImage, pHScaledImage, 1, 1);

		return TRUE;
	}
	catch (HException& except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::GetScaledImage] : %s", except.message);	THEAPP.SaveLog(str);
		return FALSE;
	}
}

BOOL Algorithm::GetScaledImage(Hobject HImage, Hobject HInspectAreaRgn, int iImageScaleMethod, int iImageScaleMax, int iImageScaleMin, Hobject *pHScaledImage)
{
	try
	{
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn)==FALSE)
			return FALSE;

		Hobject HProcessImage;

		HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
		long lPeak, lHystUpperThreshold, lHystLowThreshold;

		int iScaleMin, iScaleMax;
		int iScalePeakUpperOffset = 20;
		int iScalePeakLowerOffset = 10;

		if (iImageScaleMethod==IMAGE_SCALE_AUTO)
		{
			gray_histo(HInspectAreaRgn, HImage, &HAbsHisto, &HRelHisto);
			create_funct_1d_array(HAbsHisto, &HFunction);
			smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
			tuple_last_n(HSmoothFunction, 3, &HAbsHisto);
			tuple_sort_index(HAbsHisto, &Indices);
			tuple_inverse(Indices, &Inverted);
			lPeak = Inverted[0].L();

			iScaleMax = lPeak + iScalePeakUpperOffset;
			if (iScaleMax>255)
				iScaleMax = 255;

			iScaleMin = lPeak - iScalePeakLowerOffset;
			if (iScaleMin<0)
				iScaleMin = 0;
		}
		else
		{
			if (iImageScaleMin<0)
				iScaleMin = 0;
			else if (iImageScaleMin>255)
				iScaleMin = 255;
			else
				iScaleMin = iImageScaleMin;

			if (iImageScaleMax<0)
				iScaleMax = 0;
			else if (iImageScaleMax>255)
				iScaleMax = 255;
			else
				iScaleMax = iImageScaleMax;

			if (iScaleMax<iScaleMin)
				iScaleMax = iScaleMin;
		}

		THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
		binomial_filter(HProcessImage, &HProcessImage, 3, 3);

		copy_obj(HProcessImage, pHScaledImage, 1, 1);

		return TRUE;
	}
	catch (HException& except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::GetScaledImage] : %s", except.message);	THEAPP.SaveLog(str);
		return FALSE;
	}
}

// Hobject *HConcatDefect: 검사 조건 만족한 모든 Blob Union
int Algorithm::ApplyInspectionCondition(Hobject Hdefect, Hobject HRgnInspectTop, Hobject HRgnInspectBtm, Hobject HRgnInspectInner, Hobject *HConcatDefect,
	double dAreaMin_Top, double dAreaMid_Top, double dAreaMax_Top, int iAcceptMin_Top, int iAcceptMid_Top, int iAcceptMax_Top,
	double dAreaMin_Btm, double dAreaMid_Btm, double dAreaMax_Btm, int iAcceptMin_Btm, int iAcceptMid_Btm, int iAcceptMax_Btm,
	double dAreaMin_Inner, double dAreaMid_Inner, double dAreaMax_Inner, int iAcceptMin_Inner, int iAcceptMid_Inner, int iAcceptMax_Inner)
{
	try{
		Hobject Select_ObjectRegion,HThresRegion;
		Hobject SurfaceConnection;
		Hobject IntersectionObj1,IntersectionObj2;

		gen_empty_obj(&Select_ObjectRegion);
		gen_empty_obj(&HThresRegion);
		gen_empty_obj(&SurfaceConnection);
		gen_empty_obj(&IntersectionObj1);
		gen_empty_obj(&IntersectionObj2);

		HTuple HdefectCount;
		HTuple Area,Row,Column;
		HTuple Intersection_Number;

		int TopCountLessSmall, TopCountSmall, TopCountMid, TopCountBiG;
		int BottomCountLessSmall, BottomCountSmall, BottomCountMid, BottomCountBiG;
		int InnerBarrelCountLessSmall, InnerBarrelCountSmall, InnerBarrelCountMid, InnerBarrelCountBiG;

		int SortingDefectNumberTop=0;		// 불량이면 1
		int SortingDefectNumberInner=0;
		int SortingDefectNumberBottom=0;

		int SortingDefectNumberResult = -1;

		TopCountLessSmall=0;
		TopCountSmall=0;
		TopCountMid=0;
		TopCountBiG=0;
		BottomCountLessSmall=0;
		BottomCountSmall=0;
		BottomCountMid=0;
		BottomCountBiG=0;
		InnerBarrelCountLessSmall=0;
		InnerBarrelCountSmall=0;
		InnerBarrelCountMid=0;
		InnerBarrelCountBiG=0;

		Hobject Copy_obj_Himage;

		copy_obj(Hdefect,&Copy_obj_Himage,1,-1);
		connection(Copy_obj_Himage,&Copy_obj_Himage);
		count_obj(Copy_obj_Himage,&HdefectCount);		// Number of Barrel Defect 

		Hobject HInnerRegion, HOuterRegion;
		gen_empty_obj(&HInnerRegion);
		gen_empty_obj(&HOuterRegion);
		copy_obj(HRgnInspectTop, &HInnerRegion, 1, -1);
		if(THEAPP.m_pGFunction->ValidHRegion(HRgnInspectBtm))
			copy_obj(HRgnInspectBtm, &HOuterRegion,1,-1);

		BOOL bTopBarrelDefect;
		int i;

		Hobject HTopBarrelDefectRgn, HInnerBarrelDefectRgn, HBottomBarrelDefectRgn;

		gen_empty_obj(&HTopBarrelDefectRgn);
		gen_empty_obj(&HInnerBarrelDefectRgn);
		gen_empty_obj(&HBottomBarrelDefectRgn);

		// 우선 순위: Top>Bottom

		for (i=0; i<HdefectCount; i++)
		{
			select_obj(Copy_obj_Himage,&Select_ObjectRegion,i+1);

			area_center(Select_ObjectRegion,&Area,&Row,&Column);
			double dArea = ceil((Area[0].D() * PXLAREA)/1000)/1000;		////////// added for CMI 3000 2000, 단위 mm2으로 변환

			intersection(HInnerRegion,Select_ObjectRegion,&IntersectionObj1);

			// Top Barrel
			if (THEAPP.m_pGFunction->ValidHRegion(IntersectionObj1))		// Caution!!: Top 배럴에 1픽셀이라도 걸리면 Top 배럴 불량 처리
			{
				//				if (dArea >= dAreaMin_Top) // Top 배럴 Defect
				if (dArea > 0) // Top 배럴 Defect
				{
					concat_obj(HTopBarrelDefectRgn, Select_ObjectRegion, &HTopBarrelDefectRgn); 
				}

				// Inner Barrel
				Hobject HInnerIntersectionObj; 
				gen_empty_obj(&HInnerIntersectionObj);
				intersection(HRgnInspectInner, Select_ObjectRegion, &HInnerIntersectionObj);

				if (THEAPP.m_pGFunction->ValidHRegion(HInnerIntersectionObj))
				{					
					//					if(dArea >= dAreaMin_Inner) // Inner 배럴 Defect
					if(dArea > 0) // Inner 배럴 Defect
					{
						concat_obj(HInnerBarrelDefectRgn, Select_ObjectRegion, &HInnerBarrelDefectRgn);
					}
				}
			}
			else	// Bottom Barrel (Top Barrel에 걸치지 않은 Blob)
			{
				intersection(HOuterRegion,Select_ObjectRegion,&IntersectionObj2);
				if(THEAPP.m_pGFunction->ValidHRegion(IntersectionObj2))
				{
					//					if(dArea >= dAreaMin_Btm)	// Bottom 배럴 Defect
					if(dArea > 0)	// Bottom 배럴 Defect
					{
						concat_obj(HBottomBarrelDefectRgn, Select_ObjectRegion, &HBottomBarrelDefectRgn);
					}
				}
			}
		}

		// Defect Counting & Get Defect Region

		gen_empty_obj(HConcatDefect);	// All Defect Rgn

		Hobject HTopDefectRgn, HBottomDefectRgn, HInnerDefectRgn;
		gen_empty_obj(&HTopDefectRgn);
		gen_empty_obj(&HBottomDefectRgn);
		gen_empty_obj(&HInnerDefectRgn);

		Hobject HLessMinDefectRgn, HMinDefectRgn, HMidDefectRgn, HMaxDefectRgn;

		Hlong lNoTopBarrelDefect, lNoInnerBarrelDefect, lNoBottomBarrelDefect;

		lNoTopBarrelDefect = lNoInnerBarrelDefect = lNoBottomBarrelDefect = 0;

		//////////////////////////////////////////////////////////////////////////
		// Inner Barrel Check
		//////////////////////////////////////////////////////////////////////////

		gen_empty_obj(&HLessMinDefectRgn);
		gen_empty_obj(&HMinDefectRgn);
		gen_empty_obj(&HMidDefectRgn);
		gen_empty_obj(&HMaxDefectRgn);

		count_obj(HInnerBarrelDefectRgn, &lNoInnerBarrelDefect);
		for (i=0; i<lNoInnerBarrelDefect; i++)
		{
			select_obj(HInnerBarrelDefectRgn, &Select_ObjectRegion, i+1);

			if(THEAPP.m_pGFunction->ValidHRegion(Select_ObjectRegion)==FALSE)
				continue;

			area_center(Select_ObjectRegion, &Area, &Row, &Column);
			double dArea = ceil((Area[0].D() * PXLAREA)/1000)/1000;	////////// unit: mm2

			if (dArea < dAreaMin_Inner)
			{
				InnerBarrelCountLessSmall++;
				concat_obj(HLessMinDefectRgn, Select_ObjectRegion, &HLessMinDefectRgn);
			}
			if (dArea >= dAreaMin_Inner)
			{
				InnerBarrelCountSmall++;
				concat_obj(HMinDefectRgn, Select_ObjectRegion, &HMinDefectRgn);
			}
			if (dArea >= dAreaMid_Inner)
			{
				InnerBarrelCountMid++;
				concat_obj(HMidDefectRgn, Select_ObjectRegion, &HMidDefectRgn);
			}
			if (dArea >= dAreaMax_Inner)
			{
				InnerBarrelCountBiG++;
				concat_obj(HMaxDefectRgn, Select_ObjectRegion, &HMaxDefectRgn);
			}
		}

		if (InnerBarrelCountBiG>0)
		{
			concat_obj(HInnerDefectRgn, HMaxDefectRgn, &HInnerDefectRgn);
		}
		if (InnerBarrelCountMid > iAcceptMax_Inner)
		{
			concat_obj(HInnerDefectRgn, HMidDefectRgn, &HInnerDefectRgn);
		}
		if (InnerBarrelCountSmall > iAcceptMid_Inner)
		{
			concat_obj(HInnerDefectRgn, HMinDefectRgn, &HInnerDefectRgn);
		}
		if (InnerBarrelCountLessSmall > iAcceptMin_Inner)
		{
			concat_obj(HInnerDefectRgn, HLessMinDefectRgn, &HInnerDefectRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HInnerDefectRgn))
		{
			union1(HInnerDefectRgn, &HInnerDefectRgn);
			concat_obj(*HConcatDefect, HInnerDefectRgn, HConcatDefect);	

			SortingDefectNumberInner = 1;
		}

		//////////////////////////////////////////////////////////////////////////
		// Top Barrel Check
		//////////////////////////////////////////////////////////////////////////

		gen_empty_obj(&HLessMinDefectRgn);
		gen_empty_obj(&HMinDefectRgn);
		gen_empty_obj(&HMidDefectRgn);
		gen_empty_obj(&HMaxDefectRgn);

		count_obj(HTopBarrelDefectRgn, &lNoTopBarrelDefect);
		for (i=0; i<lNoTopBarrelDefect; i++)
		{
			select_obj(HTopBarrelDefectRgn, &Select_ObjectRegion, i+1);

			if(THEAPP.m_pGFunction->ValidHRegion(Select_ObjectRegion)==FALSE)
				continue;

			area_center(Select_ObjectRegion, &Area, &Row, &Column);
			double dArea = ceil((Area[0].D() * PXLAREA)/1000)/1000;	////////// unit: mm2

			if (dArea < dAreaMin_Top)
			{
				TopCountLessSmall++;
				concat_obj(HLessMinDefectRgn, Select_ObjectRegion, &HLessMinDefectRgn);
			}
			if (dArea >= dAreaMin_Top)
			{
				TopCountSmall++;
				concat_obj(HMinDefectRgn, Select_ObjectRegion, &HMinDefectRgn);
			}
			if (dArea >= dAreaMid_Top)
			{
				TopCountMid++;
				concat_obj(HMidDefectRgn, Select_ObjectRegion, &HMidDefectRgn);
			}
			if (dArea >= dAreaMax_Top)
			{
				TopCountBiG++;
				concat_obj(HMaxDefectRgn, Select_ObjectRegion, &HMaxDefectRgn);
			}
		}

		if (TopCountBiG>0)
		{
			concat_obj(HTopDefectRgn, HMaxDefectRgn, &HTopDefectRgn);
		}
		if (TopCountMid > iAcceptMax_Top)
		{
			concat_obj(HTopDefectRgn, HMidDefectRgn, &HTopDefectRgn);
		}
		if (TopCountSmall > iAcceptMid_Top)
		{
			concat_obj(HTopDefectRgn, HMinDefectRgn, &HTopDefectRgn);
		}
		if (TopCountLessSmall > iAcceptMin_Top)
		{
			concat_obj(HTopDefectRgn, HLessMinDefectRgn, &HTopDefectRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HTopDefectRgn))
		{
			union1(HTopDefectRgn, &HTopDefectRgn);
			concat_obj(*HConcatDefect, HTopDefectRgn, HConcatDefect);	

			SortingDefectNumberTop = 1;
		}

		//////////////////////////////////////////////////////////////////////////
		// Bottom Barrel Check
		//////////////////////////////////////////////////////////////////////////

		gen_empty_obj(&HLessMinDefectRgn);
		gen_empty_obj(&HMinDefectRgn);
		gen_empty_obj(&HMidDefectRgn);
		gen_empty_obj(&HMaxDefectRgn);

		count_obj(HBottomBarrelDefectRgn, &lNoBottomBarrelDefect);
		for (i=0; i<lNoBottomBarrelDefect; i++)
		{
			select_obj(HBottomBarrelDefectRgn, &Select_ObjectRegion, i+1);

			if(THEAPP.m_pGFunction->ValidHRegion(Select_ObjectRegion)==FALSE)
				continue;

			area_center(Select_ObjectRegion, &Area, &Row, &Column);
			double dArea = ceil((Area[0].D() * PXLAREA)/1000)/1000;	////////// unit: mm2

			if (dArea < dAreaMin_Btm)
			{
				BottomCountLessSmall++;
				concat_obj(HLessMinDefectRgn, Select_ObjectRegion, &HLessMinDefectRgn);
			}
			if (dArea >= dAreaMin_Btm)
			{
				BottomCountSmall++;
				concat_obj(HMinDefectRgn, Select_ObjectRegion, &HMinDefectRgn);
			}
			if (dArea >= dAreaMid_Btm)
			{
				BottomCountMid++;
				concat_obj(HMidDefectRgn, Select_ObjectRegion, &HMidDefectRgn);
			}
			if (dArea >= dAreaMax_Btm)
			{
				BottomCountBiG++;
				concat_obj(HMaxDefectRgn, Select_ObjectRegion, &HMaxDefectRgn);
			}
		}

		if (BottomCountBiG>0)
		{
			concat_obj(HBottomDefectRgn, HMaxDefectRgn, &HBottomDefectRgn);
		}
		if (BottomCountMid > iAcceptMax_Btm)
		{
			concat_obj(HBottomDefectRgn, HMidDefectRgn, &HBottomDefectRgn);
		}
		if (BottomCountSmall > iAcceptMid_Btm)
		{
			concat_obj(HBottomDefectRgn, HMinDefectRgn, &HBottomDefectRgn);
		}
		if (BottomCountLessSmall > iAcceptMin_Btm)
		{
			concat_obj(HBottomDefectRgn, HLessMinDefectRgn, &HBottomDefectRgn);
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HBottomDefectRgn))
		{
			union1(HBottomDefectRgn, &HBottomDefectRgn);
			concat_obj(*HConcatDefect, HBottomDefectRgn, HConcatDefect);	
			SortingDefectNumberBottom = 1;
		}

		union1(*HConcatDefect, HConcatDefect);

		if (SortingDefectNumberInner==1)
			SortingDefectNumberResult = DP_INNER_BARREL;
		else if (SortingDefectNumberInner==0 && SortingDefectNumberTop==1)
			SortingDefectNumberResult = DP_TOP_BARREL;
		else if (SortingDefectNumberInner==0 && SortingDefectNumberTop==0 && SortingDefectNumberBottom==1)
			SortingDefectNumberResult = DP_BOTTOM_BARREL;

		return SortingDefectNumberResult;
	}
	catch(HException &except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::ApplyInspectionCondition] : %s", except.message); THEAPP.SaveLog(str);
	}
}

BOOL Algorithm::ImageAlignShapeMatching(Hobject HMatchingImage, Hobject *pHAlignImage, HTuple *plAlignModelID, BOOL *pbAlignImageFlag, double dMatchingAngleRange, double dMatchingScaleMin, double dMatchingScaleMax,
	double dMatchingScore, double dTeachAlignRefX, double dTeachAlignRefY, int iMatchingMethod, int iMatchingLTX, int iMatchingLTY, int iMatchingRBX, int iMatchingRBY, int iMatchingSearchMarginX, int iMatchingSearchMarginY, BOOL bMatchingApplyAffineConstant, 
	double *pdRotationAngleDeg, double *pdDeltaX, double *pdDeltaY, HTuple *pHMatchingHomMat)
{
	try
	{
		
		BOOL bDebugSave = FALSE;

		if (bDebugSave)
		{
			write_image(HMatchingImage, "bmp", 0, "c:\\DualTest\\MatchingImage");
		}

		*pdRotationAngleDeg = 0;
		*pdDeltaX = 0;
		*pdDeltaY = 0;

		HTuple dAngleRangeRad;
		tuple_rad(dMatchingAngleRange, &dAngleRangeRad);

		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, Scale, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;

		double dTransX, dTransY, dRotationAngle;
		Hobject HModelContour, HModelRgn, HMatchingImageReduced;
		HTuple HomMat2D;

		if (*plAlignModelID >= 0)
		{
			if (iMatchingMethod==ALIGN_MATCHING_TEMPLATE)
			{
				gen_rectangle1(&HModelRgn, iMatchingLTY, iMatchingLTX, iMatchingRBY, iMatchingRBX);
				dilation_rectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX*2+1, iMatchingSearchMarginY*2+1);
			}
			else if (iMatchingMethod==ALIGN_MATCHING_SHAPE)
			{
				get_shape_model_contours(&HModelContour, *plAlignModelID, 1);
				gen_region_contour_xld(HModelContour, &HModelRgn, "filled");
				union1(HModelRgn, &HModelRgn);
				shape_trans(HModelRgn, &HModelRgn, "rectangle1");
				dilation_rectangle1(HModelRgn, &HModelRgn, iMatchingSearchMarginX*2+1, iMatchingSearchMarginY*2+1);
				move_region(HModelRgn, &HModelRgn, (Hlong)dTeachAlignRefY, (Hlong)dTeachAlignRefX);
			}

			reduce_domain(HMatchingImage, HModelRgn, &HMatchingImageReduced);

			if (bDebugSave)
			{
				write_image(HMatchingImageReduced, "bmp", 0, "c:\\DualTest\\HMatchingImageReduced");
			}

			Row = HTuple(0.0);
			Column = HTuple(0.0);
			Angle = HTuple(0.0);
			Scale = HTuple(0.0);
			Score = HTuple(0.0);

			int iNoTeachNumLevel = 0;
			HTuple NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast;

			if (iMatchingMethod==ALIGN_MATCHING_TEMPLATE)
			{
				get_ncc_model_params(*plAlignModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &Metric);

				iNoTeachNumLevel = NumLevels[0].L();
				if (iNoTeachNumLevel>5)
					iNoTeachNumLevel = 5;
				else
					iNoTeachNumLevel = 0;

				find_ncc_model(HMatchingImageReduced,
					*plAlignModelID,							// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,							// Extent of the rotation angles.
					dMatchingScore,								// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"true",										// Subpixel accuracy
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Score);									// Score of the found instances of the model.
			}
			else if (iMatchingMethod==ALIGN_MATCHING_SHAPE)
			{
				get_shape_model_params(*plAlignModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &ScaleMin, &ScaleMax, &ScaleStep, &Metric, &MinContrast);

				iNoTeachNumLevel = NumLevels[0].L();
				if (iNoTeachNumLevel>5)
					iNoTeachNumLevel = 5;
				else
					iNoTeachNumLevel = 0;

				find_scaled_shape_model(HMatchingImageReduced,
					*plAlignModelID,							// Handle of the model
					-dAngleRangeRad,							// Smallest rotation of the model
					dAngleRangeRad*2.0,							// Extent of the rotation angles.
					dMatchingScaleMin,
					dMatchingScaleMax,
					dMatchingScore,								// Minumum score of the instances of the model to be found
					1,											// Number of instances of the model to be found
					0.5,										// Maximum overlap of the instances of the model to be found
					"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
					iNoTeachNumLevel,							// Number of pyramid levels used in the matching
					0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
					&Row,										// Row coordinate of the found instances of the model.
					&Column,									// Column coordinate of the found instances of the model.
					&Angle,										// Rotation angle of the found instances of the model. (radian)
					&Scale,
					&Score);									// Score of the found instances of the model.
			}

			tuple_length(Score, &lNoFoundNumber);

			if (lNoFoundNumber<=0)
			{
				return FALSE;
			}

			if (iMatchingMethod==ALIGN_MATCHING_TEMPLATE || iMatchingMethod==ALIGN_MATCHING_SHAPE)
			{
				dTransX = dTeachAlignRefX - Column[0].D();
				dTransY = dTeachAlignRefY - Row[0].D();
				dRotationAngle = -1.0 * Angle[0].D();

				tuple_deg(Angle, &Angle);
				*pdRotationAngleDeg = Angle[0].D();	// ROI to Image

				*pdDeltaX = dTransX;		// Image to ROI
				*pdDeltaY = dTransY;		// Image to ROI

				hom_mat2d_identity(&HomMat2DIdentity);
				hom_mat2d_translate(HomMat2DIdentity, dTransY, dTransX, &HomMat2DTranslate);
				hom_mat2d_rotate(HomMat2DTranslate, dRotationAngle, dTeachAlignRefY, dTeachAlignRefX, &HomMat2DRotate);

				for (int i = 0; i < MAX_IMAGE_TAB; i++)
				{
					if (i >= THEAPP.m_iMaxInspImageNo)
						break;

					if (pbAlignImageFlag[i] == TRUE)
					{
						if (THEAPP.m_pGFunction->ValidHImage(pHAlignImage[i]))
						{
							if (bMatchingApplyAffineConstant)
								affine_trans_image(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "constant", "false");		// "constant"
							else
								affine_trans_image(pHAlignImage[i], &(pHAlignImage[i]), HomMat2DRotate, "nearest_neighbor", "false");		// "constant"
						}
					}
				}

				if (pHMatchingHomMat!=NULL)
					*pHMatchingHomMat = HomMat2DRotate;

				HomMat2DIdentity.Reset();
				HomMat2DTranslate.Reset();
				HomMat2DRotate.Reset();
			}

			HModelContour.Reset();
			HModelRgn.Reset();

			HMatchingImageReduced.Reset();
			gen_empty_obj(&HMatchingImageReduced);

		}
		else
		{

			return FALSE;
		}

		return TRUE;
	}
	catch(HException &except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::ImageAlignShapeMatching] : %s", except.message); THEAPP.SaveLog(str);

		return FALSE;
	}
}

BOOL Algorithm::EdgeMeasureAlgorithm(Hobject HImage, Hobject HInspectAreaRgn, CAlgorithmParam Param, Hobject *pHNewInspectAreaRgn)
{
	try
	{
		BOOL bDebugSave = FALSE;

		BYTE *pImageData;
		char type[30];
		Hlong lImageWidth, lImageHeight;

		get_image_pointer1(HImage, (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

		if (Param.m_iEdgeMeasureMultiPointNumber <= 0)
			Param.m_iEdgeMeasureMultiPointNumber = 2;

		if (Param.m_bEdgeMeasureEndPoint)
			Param.m_iEdgeMeasureMultiPointNumber = 2;

		// Edge Measure

		double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
		int iROISizeX, iROISizeY;

		Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;

		smallest_rectangle1(HInspectAreaRgn, &lROIRow1, &lROICol1, &lROIRow2, &lROICol2);

		int iPitch;

		if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
			iPitch = (lROICol2 - lROICol1) / Param.m_iEdgeMeasureMultiPointNumber;
		else
			iPitch = (lROIRow2 - lROIRow1) / Param.m_iEdgeMeasureMultiPointNumber;

		int iSubLTPointX, iSubLTPointY, iSubRBPointX, iSubRBPointY;
		int i;

		HTuple  MeasureHandle;
		HTuple RowEdge, ColumnEdge, Amplitude, Distance, Indices, Inverted;;
		Hlong NoEdge, MaxAmpEdgeIndex;

		double dEgePosX, dEgePosY;
		int iEdgePosX, iEdgePosY;
		Hobject RegressContour;

		HTuple HEdgePosX, HEdgePosY;
		tuple_gen_const(0, 0, &HEdgePosX);
		tuple_gen_const(0, 0, &HEdgePosY);

		for (i = 0; i < Param.m_iEdgeMeasureMultiPointNumber; i++)
		{
			if (Param.m_bEdgeMeasureEndPoint)
			{
				if (i==0)	// Start Position
				{
					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						iSubLTPointX = lROICol1;
						iSubRBPointX = lROICol1 + Param.m_iEdgeMeasureStartLength;
						iSubLTPointY = lROIRow1;
						iSubRBPointY = lROIRow2;
					}
					else
					{
						iSubLTPointX = lROICol1;
						iSubRBPointX = lROICol2;
						iSubLTPointY = lROIRow1;
						iSubRBPointY = lROIRow1 + Param.m_iEdgeMeasureStartLength;
					}
				}			// End Position
				else
				{
					if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
					{
						iSubLTPointX = lROICol2 - 1 - Param.m_iEdgeMeasureEndLength;
						iSubRBPointX = lROICol2 - 1;
						iSubLTPointY = lROIRow1;
						iSubRBPointY = lROIRow2;
					}
					else
					{
						iSubLTPointX = lROICol1;
						iSubRBPointX = lROICol2;
						iSubLTPointY = lROIRow2 -1 - Param.m_iEdgeMeasureEndLength;
						iSubRBPointY = lROIRow2 -1;
					}
				}
			}
			else
			{
				if (Param.m_iEdgeMeasureDir == 1)		// Y Dir
				{
					iSubLTPointX = lROICol1 + i * iPitch;
					iSubRBPointX = lROICol1 + (i+1) * iPitch;
					iSubLTPointY = lROIRow1;
					iSubRBPointY = lROIRow2;
				}
				else
				{
					iSubLTPointX = lROICol1;
					iSubRBPointX = lROICol2;
					iSubLTPointY = lROIRow1 + i * iPitch;
					iSubRBPointY = lROIRow1 + (i+1) * iPitch;
				}
			}

			dEdgeMeasureCenterX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
			dEdgeMeasureCenterY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

			iROISizeX = iSubRBPointX - iSubLTPointX;
			iROISizeY = iSubRBPointY - iSubLTPointY;

			if (Param.m_iEdgeMeasureDir==1)		// Y Dir
			{
				gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14/2, iROISizeY/2, iROISizeX/2, lImageWidth, 
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (Param.m_iEdgeMeasurePos==0)
				{
					if (Param.m_iEdgeMeasureGv==0)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all", 
						"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==1)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative", 
						"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==2)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive", 
						"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePos==1)
				{
					if (Param.m_iEdgeMeasureGv==0)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all", 
						"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==1)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative", 
						"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==2)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive", 
						"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePos==2)
				{
					if (Param.m_iEdgeMeasureGv==0)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all", 
						"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==1)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative", 
						"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==2)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive", 
						"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}
			else		// X Dir
			{
				gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, 3.14, iROISizeX/2, iROISizeY/2, lImageWidth, 
					lImageHeight, "nearest_neighbor", &MeasureHandle);

				if (Param.m_iEdgeMeasurePos==0)
				{
					if (Param.m_iEdgeMeasureGv==0)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all", 
						"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==1)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative", 
						"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==2)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive", 
						"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePos==1)
				{
					if (Param.m_iEdgeMeasureGv==0)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all", 
						"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==1)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative", 
						"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==2)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive", 
						"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
				else if (Param.m_iEdgeMeasurePos==2)
				{
					if (Param.m_iEdgeMeasureGv==0)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "all", 
						"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==1)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "negative", 
						"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					else if (Param.m_iEdgeMeasureGv==2)
						measure_pos(HImage, MeasureHandle, Param.m_dEdgeMeasureSmFactor, (double)Param.m_iEdgeMeasureEdgeStr, "positive", 
						"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
				}
			}

			close_measure(MeasureHandle);

			tuple_length(Amplitude, &NoEdge);

			if (NoEdge<=0)
			{
				continue;
			}
			else
			{
				tuple_abs(Amplitude, &Amplitude);
				tuple_sort_index(Amplitude, &Indices);
				tuple_inverse(Indices, &Inverted);
				MaxAmpEdgeIndex = Inverted[0].L();

				if (Param.m_iEdgeMeasureDir==1)		// Y Dir
				{
					dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
					dEgePosX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
				}
				else
				{
					dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
					dEgePosY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;
				}

				tuple_concat(HEdgePosX, dEgePosX, &HEdgePosX);
				tuple_concat(HEdgePosY, dEgePosY, &HEdgePosY);

			}

		} // for (i = 0; i < Param.m_iEdgeMeasureMultiPointNumber; i++)

		Hlong lNoFoundNumber;
		tuple_length(HEdgePosY, &lNoFoundNumber);

		Hobject HEdgePolygonContour;
		double dLineStartX, dLineStartY, dLineEndX, dLineEndY;

		POINT lLTPoint, lRTPoint, lLBPoint, lRBPoint;	// 4 Points(Polygon) of ROI
		double dIntersectPointX_1, dIntersectPointY_1, dIntersectPointX_2, dIntersectPointY_2;
		Hlong lParallel;

		Hobject HPolygonRgn;
		HTuple HRows, HCols;

		gen_empty_obj(&HPolygonRgn);

		lLTPoint.x = lROICol1;
		lLTPoint.y = lROIRow1;
		lRTPoint.x = lROICol2;
		lRTPoint.y = lROIRow1;
		lLBPoint.x = lROICol1;
		lLBPoint.y = lROIRow2;
		lRBPoint.x = lROICol2;
		lRBPoint.y = lROIRow2;

		if (lNoFoundNumber >= 2)
		{
			HTuple RowBegin, ColBegin, RowEnd, ColEnd, Nr, Nc, Dist;

			gen_contour_polygon_xld(&HEdgePolygonContour, HEdgePosY, HEdgePosX);

			fit_line_contour_xld(HEdgePolygonContour, "tukey", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

			tuple_length(RowBegin, &lNoFoundNumber);

			if (lNoFoundNumber > 0)
			{
				dLineStartX = ColBegin[0].D();
				dLineStartY = RowBegin[0].D();
				dLineEndX = ColEnd[0].D();
				dLineEndY = RowEnd[0].D();

				if (Param.m_iLineFitApplyPos == LINE_FIT_APPLY_POS_LEFT)
				{
					intersection_ll(lROIRow1, lROICol1, lROIRow1, lROICol2, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &dIntersectPointY_1, &dIntersectPointX_1, &lParallel);
					intersection_ll(lROIRow2, lROICol1, lROIRow2, lROICol2, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &dIntersectPointY_2, &dIntersectPointX_2, &lParallel);

					lLTPoint.x = (int)(dIntersectPointX_1+0.5) - Param.m_iRoiOffsetLeft;
					lLBPoint.x = (int)(dIntersectPointX_2+0.5) - Param.m_iRoiOffsetLeft;
					lRTPoint.x += Param.m_iRoiOffsetRight;
					lRBPoint.x += Param.m_iRoiOffsetRight;
				}
				else if (Param.m_iLineFitApplyPos == LINE_FIT_APPLY_POS_RIGHT)
				{
					intersection_ll(lROIRow1, lROICol1, lROIRow1, lROICol2, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &dIntersectPointY_1, &dIntersectPointX_1, &lParallel);
					intersection_ll(lROIRow2, lROICol1, lROIRow2, lROICol2, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &dIntersectPointY_2, &dIntersectPointX_2, &lParallel);

					lRTPoint.x = (int)(dIntersectPointX_1+0.5) + Param.m_iRoiOffsetRight;
					lRBPoint.x = (int)(dIntersectPointX_2+0.5) + Param.m_iRoiOffsetRight;
					lLTPoint.x -= Param.m_iRoiOffsetLeft;
					lLBPoint.x -= Param.m_iRoiOffsetLeft;
				}
				else if (Param.m_iLineFitApplyPos == LINE_FIT_APPLY_POS_BOTTOM)
				{
					intersection_ll(lROIRow1, lROICol1, lROIRow2, lROICol1, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &dIntersectPointY_1, &dIntersectPointX_1, &lParallel);
					intersection_ll(lROIRow1, lROICol2, lROIRow2, lROICol2, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &dIntersectPointY_2, &dIntersectPointX_2, &lParallel);

					lLTPoint.y -= Param.m_iRoiOffsetTop;
					lRTPoint.y -= Param.m_iRoiOffsetTop;
					lLBPoint.y = (int)(dIntersectPointY_1+0.5) + Param.m_iRoiOffsetBottom;
					lRBPoint.y = (int)(dIntersectPointY_2+0.5) + Param.m_iRoiOffsetBottom;
				}
				else if (Param.m_iLineFitApplyPos == LINE_FIT_APPLY_POS_TOP)
				{
					intersection_ll(lROIRow1, lROICol1, lROIRow2, lROICol1, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &dIntersectPointY_1, &dIntersectPointX_1, &lParallel);
					intersection_ll(lROIRow1, lROICol2, lROIRow2, lROICol2, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &dIntersectPointY_2, &dIntersectPointX_2, &lParallel);

					lLTPoint.y += Param.m_iRoiOffsetBottom;
					lRTPoint.y += Param.m_iRoiOffsetBottom;
					lLBPoint.y = (int)(dIntersectPointY_1+0.5) - Param.m_iRoiOffsetTop;
					lRBPoint.y = (int)(dIntersectPointY_2+0.5) - Param.m_iRoiOffsetTop;
				}
				

				tuple_gen_const(0, 0, &HRows);
				tuple_gen_const(0, 0, &HCols);

				tuple_concat(HRows, lLTPoint.y, &HRows);
				tuple_concat(HCols, lLTPoint.x, &HCols);
				tuple_concat(HRows, lRTPoint.y, &HRows);
				tuple_concat(HCols, lRTPoint.x, &HCols);
				tuple_concat(HRows, lRBPoint.y, &HRows);
				tuple_concat(HCols, lRBPoint.x, &HCols);
				tuple_concat(HRows, lLBPoint.y, &HRows);
				tuple_concat(HCols, lLBPoint.x, &HCols);

				gen_region_polygon_filled(&HPolygonRgn, HRows, HCols);

				if (THEAPP.m_pGFunction->ValidHRegion(HPolygonRgn))
					union1(HPolygonRgn, pHNewInspectAreaRgn);
				else
					return FALSE;
			}
			else
				return FALSE;
		}
		else
			return FALSE;

		return TRUE;
	}
	catch (HException& except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::EdgeMeasureAlgorithm] : %s", except.message); THEAPP.SaveLog(str);

		return FALSE;
	}
}

// 24.02.29 Local Align 추가 - LeeGW Start 
// 24.11.20 edge strength 추가 
BOOL Algorithm::EdgeMeasureAlgorithm(Hobject HImage, Hobject HInspectAreaRgn, CAlgorithmParam Param, Hobject* pHLineFitXLD, int* piFitLineXPos, int* piFitLineYPos, double* pdEdgeCenterX, double* pdEdgeCenterY, double* pdLineStartX, double* pdLineStartY, double* pdLineEndX, double* pdLineEndY, double* dEdgeMeanAmp)
{
	try
	{

		int iEdgeMeasureAlignPosX, iEdgeMeasureAlignPosY;
		double dEdgeCenterX, dEdgeCenterY;
		double dEdgeLineStartX, dEdgeLineStartY, dEdgeLineEndX, dEdgeLineEndY;

		iEdgeMeasureAlignPosX = iEdgeMeasureAlignPosY = INVALID_ALIGN_RESULT;
		dEdgeLineStartX = dEdgeLineStartY = dEdgeLineEndX = dEdgeLineEndY = INVALID_ALIGN_RESULT;

		Hobject HInspImage = HImage;

		if (Param.m_bROIEdgeMeasureUse)
		{

			dEdgeCenterX = dEdgeCenterY = -1;

			BYTE* pImageData;
			char type[30];
			Hlong lImageWidth, lImageHeight;

			get_image_pointer1(HInspImage, (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

			if (Param.m_iROIEdgeMeasureMultiPointNumber <= 0)
				Param.m_iROIEdgeMeasureMultiPointNumber = 2;

			if (Param.m_bROIEdgeMeasureEndPoint)
				Param.m_iROIEdgeMeasureMultiPointNumber = 2;

			// Edge Measure

			double dEdgeMeasureCenterX, dEdgeMeasureCenterY;
			int iROISizeX, iROISizeY;

			Hlong lEdgePosY, lEdgePosX;
			Hlong lROIRow1, lROIRow2, lROICol1, lROICol2;
			HTuple HlROIRow1, HlROIRow2, HlROICol1, HlROICol2;
			HTuple HROIRow, HROICol;
			tuple_gen_const(0, 0, &HROIRow);
			tuple_gen_const(0, 0, &HROICol);

			int iSubLTPointX, iSubLTPointY, iSubRBPointX, iSubRBPointY;

			HTuple  MeasureHandle;
			HTuple RowEdge, ColumnEdge, Amplitude, Distance, Indices, Inverted;;
			Hlong NoEdge, MaxAmpEdgeIndex;

			double dEgePosX, dEgePosY;
			int iEdgePosX, iEdgePosY;
			Hobject RegressContour;

			double dMeasureAngleRad = 0;
			HTuple HAngleRad;
			tuple_rad(Param.m_dROIEdgeMeasureRotateAngle, &HAngleRad);
			dMeasureAngleRad = HAngleRad[0].D();

			HTuple HEdgePosX, HEdgePosY;
			tuple_gen_const(0, 0, &HEdgePosX);
			tuple_gen_const(0, 0, &HEdgePosY);

			smallest_rectangle1(HInspectAreaRgn, &HlROIRow1, &HlROICol1, &HlROIRow2, &HlROICol2);

			lROIRow1 = HlROIRow1[0].L();
			lROICol1 = HlROICol1[0].L();
			lROIRow2 = HlROIRow2[0].L();
			lROICol2 = HlROICol2[0].L();

			Hobject HEdgeAmp, HEdgeImage; // PSW241120
			double dEdgeES, HdSdAmp;

			reduce_domain(HInspImage, HInspectAreaRgn, &HEdgeImage);
			sobel_amp(HEdgeImage, &HEdgeAmp, "sum_sqrt", 3);
			intensity(HInspectAreaRgn, HEdgeAmp, &dEdgeES, &HdSdAmp); // PSW241120

			if (Param.m_iROIEdgeMeasureMultiPointNumber == 1)
			{
				dEdgeMeasureCenterX = (double)(lROICol1 + lROICol2) * 0.5;
				dEdgeMeasureCenterY = (double)(lROIRow1 + lROIRow2) * 0.5;

				iROISizeX = lROICol2 - lROICol1;
				iROISizeY = lROIRow2 - lROIRow1;

				if (Param.m_iROIEdgeMeasureDir == 1)		// Y Dir
				{
					gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, PI / 2 + dMeasureAngleRad, iROISizeY / 2, iROISizeX / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (Param.m_iROIEdgeMeasurePos == 0)
					{
						if (Param.m_iROIEdgeMeasureGv == 0)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 1)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 2)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iROIEdgeMeasurePos == 1)
					{
						if (Param.m_iROIEdgeMeasureGv == 0)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 1)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 2)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iROIEdgeMeasurePos == 2)
					{
						if (Param.m_iROIEdgeMeasureGv == 0)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 1)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 2)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}
				else		// X Dir
				{
					gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, PI + dMeasureAngleRad, iROISizeX / 2, iROISizeY / 2, lImageWidth,
						lImageHeight, "nearest_neighbor", &MeasureHandle);

					if (Param.m_iROIEdgeMeasurePos == 0)
					{
						if (Param.m_iROIEdgeMeasureGv == 0)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 1)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 2)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
								"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iROIEdgeMeasurePos == 1)
					{
						if (Param.m_iROIEdgeMeasureGv == 0)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 1)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 2)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
								"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
					else if (Param.m_iROIEdgeMeasurePos == 2)
					{
						if (Param.m_iROIEdgeMeasureGv == 0)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 1)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						else if (Param.m_iROIEdgeMeasureGv == 2)
							measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
								"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
					}
				}

				close_measure(MeasureHandle);

				tuple_length(Amplitude, &NoEdge);

				if (NoEdge <= 0)
				{
					*pdEdgeCenterX = INVALID_ALIGN_RESULT;
					*pdEdgeCenterY = INVALID_ALIGN_RESULT;
					*pdLineStartX = INVALID_ALIGN_RESULT;
					*pdLineStartY = INVALID_ALIGN_RESULT;
					*pdLineEndX = INVALID_ALIGN_RESULT;
					*pdLineEndY = INVALID_ALIGN_RESULT;
					*dEdgeMeanAmp = INVALID_ALIGN_RESULT;

					return FALSE;
				}
				else
				{
					tuple_abs(Amplitude, &Amplitude);
					tuple_sort_index(Amplitude, &Indices);
					tuple_inverse(Indices, &Inverted);
					MaxAmpEdgeIndex = Inverted[0].L();

					if (Param.m_iROIEdgeMeasureDir == 1)		// Y Dir
					{
						dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
						lEdgePosY = (int)dEgePosY;

						dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
						lEdgePosX = (int)dEgePosX;

						//gen_contour_polygon_xld(&RegressContour, HEdgePosY, HROICol);

						*pdEdgeCenterX = dEgePosX;
						*pdEdgeCenterY = dEgePosY;
						*dEdgeMeanAmp = dEdgeES;
						*pdLineStartX = *pdEdgeCenterX - ONE_POINT_EDGE_EXTEND_PXL;
						*pdLineStartY = *pdEdgeCenterY;
						*pdLineEndX = *pdEdgeCenterX + ONE_POINT_EDGE_EXTEND_PXL;
						*pdLineEndY = *pdEdgeCenterY;


						gen_cross_contour_xld(&RegressContour, *pdEdgeCenterY, *pdEdgeCenterX, 30.0, 0);
					}
					else
					{
						dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
						lEdgePosY = (int)dEgePosY;

						dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
						lEdgePosX = (int)dEgePosX;

						//gen_contour_polygon_xld(&RegressContour, HROIRow, HEdgePosX);

						*pdEdgeCenterX = dEgePosX;
						*pdEdgeCenterY = dEgePosY;
						*dEdgeMeanAmp = dEdgeES;
						*pdLineStartX = *pdEdgeCenterX;
						*pdLineStartY = *pdEdgeCenterY - ONE_POINT_EDGE_EXTEND_PXL;
						*pdLineEndX = *pdEdgeCenterX;
						*pdLineEndY = *pdEdgeCenterY + ONE_POINT_EDGE_EXTEND_PXL;

						gen_cross_contour_xld(&RegressContour, *pdEdgeCenterY, *pdEdgeCenterX, 30.0, 0);

					}

					concat_obj(*pHLineFitXLD, RegressContour, pHLineFitXLD);

				}

			}
			else
			{

				int iPitch;
				if (Param.m_iROIEdgeMeasureDir == 1)		// Y Dir
					iPitch = (lROICol2 - lROICol1) / Param.m_iROIEdgeMeasureMultiPointNumber;
				else
					iPitch = (lROIRow2 - lROIRow1) / Param.m_iROIEdgeMeasureMultiPointNumber;


				for (int i = 0; i < Param.m_iROIEdgeMeasureMultiPointNumber; i++)
				{
					if (Param.m_bROIEdgeMeasureEndPoint)
					{
						if (i == 0)	// Start Position
						{
							if (Param.m_iROIEdgeMeasureDir == 1)		// Y Dir
							{
								iSubLTPointX = lROICol1;
								iSubRBPointX = lROICol1 + Param.m_iROIEdgeMeasureStartLength;
								iSubLTPointY = lROIRow1;
								iSubRBPointY = lROIRow2;
							}
							else
							{
								iSubLTPointX = lROICol1;
								iSubRBPointX = lROICol2;
								iSubLTPointY = lROIRow1;
								iSubRBPointY = lROIRow1 + Param.m_iROIEdgeMeasureStartLength;
							}
						}			// End Position
						else
						{
							if (Param.m_iROIEdgeMeasureDir == 1)		// Y Dir
							{
								iSubLTPointX = lROICol2 - 1 - Param.m_iROIEdgeMeasureEndLength;
								iSubRBPointX = lROICol2 - 1;
								iSubLTPointY = lROIRow1;
								iSubRBPointY = lROIRow2;
							}
							else
							{
								iSubLTPointX = lROICol1;
								iSubRBPointX = lROICol2;
								iSubLTPointY = lROIRow2 - 1 - Param.m_iROIEdgeMeasureEndLength;
								iSubRBPointY = lROIRow2 - 1;
							}
						}
					}
					else
					{
						if (Param.m_iROIEdgeMeasureDir == 1)		// Y Dir
						{
							iSubLTPointX = lROICol1 + i * iPitch;
							iSubRBPointX = lROICol1 + (i + 1) * iPitch;
							iSubLTPointY = lROIRow1;
							iSubRBPointY = lROIRow2;
						}
						else
						{
							iSubLTPointX = lROICol1;
							iSubRBPointX = lROICol2;
							iSubLTPointY = lROIRow1 + i * iPitch;
							iSubRBPointY = lROIRow1 + (i + 1) * iPitch;
						}
					}

					dEdgeMeasureCenterX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
					dEdgeMeasureCenterY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;

					iROISizeX = iSubRBPointX - iSubLTPointX;
					iROISizeY = iSubRBPointY - iSubLTPointY;

					if (Param.m_iROIEdgeMeasureDir == 1)		// Y Dir
					{
						gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, PI / 2 + dMeasureAngleRad, iROISizeY / 2, iROISizeX / 2, lImageWidth,
							lImageHeight, "nearest_neighbor", &MeasureHandle);

						if (Param.m_iROIEdgeMeasurePos == 0)
						{
							if (Param.m_iROIEdgeMeasureGv == 0)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 1)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 2)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (Param.m_iROIEdgeMeasurePos == 1)
						{
							if (Param.m_iROIEdgeMeasureGv == 0)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 1)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 2)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (Param.m_iROIEdgeMeasurePos == 2)
						{
							if (Param.m_iROIEdgeMeasureGv == 0)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 1)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 2)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
					}
					else		// X Dir
					{
						gen_measure_rectangle2(dEdgeMeasureCenterY, dEdgeMeasureCenterX, PI + dMeasureAngleRad, iROISizeX / 2, iROISizeY / 2, lImageWidth,
							lImageHeight, "nearest_neighbor", &MeasureHandle);

						if (Param.m_iROIEdgeMeasurePos == 0)
						{
							if (Param.m_iROIEdgeMeasureGv == 0)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 1)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 2)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
									"all", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (Param.m_iROIEdgeMeasurePos == 1)
						{
							if (Param.m_iROIEdgeMeasureGv == 0)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 1)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 2)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
									"first", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
						else if (Param.m_iROIEdgeMeasurePos == 2)
						{
							if (Param.m_iROIEdgeMeasureGv == 0)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "all",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 1)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "negative",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
							else if (Param.m_iROIEdgeMeasureGv == 2)
								measure_pos(HImage, MeasureHandle, Param.m_dROIEdgeMeasureSmFactor, (double)Param.m_iROIEdgeMeasureEdgeStr, "positive",
									"last", &RowEdge, &ColumnEdge, &Amplitude, &Distance);
						}
					}

					close_measure(MeasureHandle);

					tuple_length(Amplitude, &NoEdge);

					if (NoEdge <= 0)
					{
						continue;
					}
					else
					{
						tuple_abs(Amplitude, &Amplitude);
						tuple_sort_index(Amplitude, &Indices);
						tuple_inverse(Indices, &Inverted);
						MaxAmpEdgeIndex = Inverted[0].L();

						if (Param.m_iROIEdgeMeasureDir == 1)		// Y Dir
						{
							dEgePosY = RowEdge[MaxAmpEdgeIndex].D();
							dEgePosX = (double)(iSubLTPointX + iSubRBPointX) * 0.5;
						}
						else
						{
							dEgePosX = ColumnEdge[MaxAmpEdgeIndex].D();
							dEgePosY = (double)(iSubLTPointY + iSubRBPointY) * 0.5;
						}

						tuple_concat(HEdgePosX, dEgePosX, &HEdgePosX);
						tuple_concat(HEdgePosY, dEgePosY, &HEdgePosY);

					}

				} // for (i = 0; i < Param.m_iROIEdgeMeasureMultiPointNumber; i++)

				HTuple HlNoFoundNumber;
				Hlong lNoFoundNumber;
				tuple_length(HEdgePosY, &HlNoFoundNumber);
				lNoFoundNumber = HlNoFoundNumber[0].L();

				Hobject HEdgePolygonContour;

				if (lNoFoundNumber >= 2)
				{
					HTuple RowBegin, ColBegin, RowEnd, ColEnd, Nr, Nc, Dist;

					if (Param.m_bROIEdgeMeasureEndPoint)
					{
						if (*pdLineStartX != NULL)
						{
							*pdLineStartX = HEdgePosX[0].D();
							*pdLineStartY = HEdgePosY[0].D();
							*pdLineEndX = HEdgePosX[lNoFoundNumber - 1].D();
							*pdLineEndY = HEdgePosY[lNoFoundNumber - 1].D();
							*dEdgeMeanAmp = dEdgeES;
						}

						tuple_concat(HEdgePosY[0], HEdgePosY[lNoFoundNumber - 1], &HEdgePosX);
						tuple_concat(HEdgePosX[0], HEdgePosX[lNoFoundNumber - 1], &HEdgePosX);

						gen_contour_polygon_xld(&RegressContour, HEdgePosY, HEdgePosX);

						concat_obj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
					}
					else
					{
						gen_contour_polygon_xld(&HEdgePolygonContour, HEdgePosY, HEdgePosX);
						fit_line_contour_xld(HEdgePolygonContour, "regression", -1, 0, 5, 2, &RowBegin, &ColBegin, &RowEnd, &ColEnd, &Nr, &Nc, &Dist);

						tuple_length(RowBegin, &HlNoFoundNumber);
						lNoFoundNumber = HlNoFoundNumber[0].L();

						if (lNoFoundNumber > 0)
						{
							if (*pdLineStartX != NULL)
							{
								*pdLineStartX = ColBegin[0].D();
								*pdLineStartY = RowBegin[0].D();
								*pdLineEndX = ColEnd[0].D();
								*pdLineEndY = RowEnd[0].D();
								*dEdgeMeanAmp = dEdgeES;
							}

							tuple_concat(RowBegin[0], RowEnd[0], &RowBegin);
							tuple_concat(ColBegin[0], ColEnd[0], &ColBegin);

							gen_contour_polygon_xld(&RegressContour, RowBegin, ColBegin);

							concat_obj(*pHLineFitXLD, RegressContour, pHLineFitXLD);
						}
					}
				} // if (lNoFoundNumber >= 2)
				else
				{
					*pdEdgeCenterX = INVALID_ALIGN_RESULT;
					*pdEdgeCenterY = INVALID_ALIGN_RESULT;
					*pdLineStartX = INVALID_ALIGN_RESULT;
					*pdLineStartY = INVALID_ALIGN_RESULT;
					*pdLineEndX = INVALID_ALIGN_RESULT;
					*pdLineEndY = INVALID_ALIGN_RESULT;
					*dEdgeMeanAmp = INVALID_ALIGN_RESULT;

					return FALSE;
				}
			}

		}

		return TRUE;
	}
	catch (HException& except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::EdgeMeasureAlgorithm] : %s", except.message); THEAPP.SaveLog(str);
		return FALSE;
	}
}



BOOL Algorithm::ApplyLocalAlignResult(Hobject* pHROIHRegion, CAlgorithmParam AlgorithmParam, int iThreadIdx)
{
	try
	{
		BOOL bDebugSave = FALSE;

		int iNoInspectROI = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount();
		if (iNoInspectROI <= 0)
			return TRUE;

		GTRegion *pInspectROIRgn;

		BOOL bFound = FALSE;

		int iLocalAlignImageNo = AlgorithmParam.m_iLocalAlignImageIndex + 1;

		for (int iROIIndex = 0; iROIIndex < iNoInspectROI; iROIIndex++)
		{
			pInspectROIRgn = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(iROIIndex);

			if (pInspectROIRgn == NULL)
				continue;

			if (pInspectROIRgn->m_iInspectionType != INSPECTION_TYPE_LOCAL_ALIGN)
				continue;

			if (pInspectROIRgn->m_iTeachImageIndex == iLocalAlignImageNo)
			{
				if (pInspectROIRgn->m_iLocalAlignID == AlgorithmParam.m_iLocalAlignROINo)
				{
					bFound = TRUE;
					break;
				}
			}
		}
	
		if (bFound)
		{
			if (AlgorithmParam.m_iLocalAlignType == LOCAL_ALIGN_MATCHING_ROI_TYPE_MATCHING)
			{
				if (AlgorithmParam.m_iLocalAlignApplyMethod == LOCAL_ALIGN_MATCHING_ROI_POSITION)
				{
					if (AlgorithmParam.m_bLocalAlignPosX || AlgorithmParam.m_bLocalAlignPosY || AlgorithmParam.m_bLocalAlignPosAngle)	// Delta X/Y
					{

						HTuple HomMat2DLocal;
						double dDeltaX, dDeltaY, dDeltaAngle;
						dDeltaX = dDeltaY = dDeltaAngle = 0;

						if (AlgorithmParam.m_bLocalAlignPosX && (pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx] != INVALID_ALIGN_RESULT))
							dDeltaX = pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx] + pInspectROIRgn->m_iLocalAlignDeltaX[iThreadIdx];
						else
							dDeltaX = pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx];

						if (AlgorithmParam.m_bLocalAlignPosY && (pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx] != INVALID_ALIGN_RESULT))
							dDeltaY = pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx] + pInspectROIRgn->m_iLocalAlignDeltaY[iThreadIdx];
						else
							dDeltaY = pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx];

						if (AlgorithmParam.m_bLocalAlignPosAngle && (pInspectROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx] != INVALID_ALIGN_RESULT))
							dDeltaAngle = pInspectROIRgn->m_dLocalAlignDeltaAngle[iThreadIdx];

						vector_angle_to_rigid(pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointY[iThreadIdx], pInspectROIRgn->m_dLocalAlignDeltaAngleFixedPointX[iThreadIdx], 0, dDeltaY, dDeltaX, dDeltaAngle, &HomMat2DLocal);
						affine_trans_region(*pHROIHRegion, pHROIHRegion, HomMat2DLocal, "true");

					}
				}
				else		// LOCAL_ALIGN_MATCHING_ROI_SHAPE
				{
					if (THEAPP.m_pGFunction->ValidHRegion(pInspectROIRgn->m_HLocalAlignShapeRgn[iThreadIdx]))
						copy_obj(pInspectROIRgn->m_HLocalAlignShapeRgn[iThreadIdx], pHROIHRegion, 1, -1);
				}

			}
			//else if (AlgorithmParam.m_iLocalAlignROIType == LOCAL_ALIGN_MATCHING_ROI_TYPE_EDGE_MEASURE)
			//{
			//	Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
			//	HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

			//	smallest_rectangle1(*pHROIHRegion, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

			//	lLTPointY = HlLTPointY.L();
			//	lLTPointX = HlLTPointX.L();
			//	lRBPointY = HlRBPointY.L();
			//	lRBPointX = HlRBPointX.L();

			//	if (pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] == NOT_ANGLE_LINE && pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] == NOT_ANGLE_LINE)
			//	{
			//		Hlong lOffset = 0;

			//		if (AlgorithmParam.m_iLocalAlignFitPos == LINE_FIT_XDIR_OBJECT_TOP)
			//		{
			//			if (pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT)
			//			{
			//				lOffset = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] - lLTPointY;
			//				MoveRegion(*pHROIHRegion, pHROIHRegion, lOffset, 0);
			//			}
			//		}
			//		else if (AlgorithmParam.m_iLocalAlignFitPos == LINE_FIT_XDIR_OBJECT_BOTTOM)
			//		{
			//			if (pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT)
			//			{
			//				lOffset = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] - lRBPointY;
			//				MoveRegion(*pHROIHRegion, pHROIHRegion, lOffset, 0);
			//			}
			//		}
			//		else if (AlgorithmParam.m_iLocalAlignFitPos == LINE_FIT_YDIR_OBJECT_LEFT)
			//		{
			//			if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT)
			//			{
			//				lOffset = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] - lLTPointX;
			//				MoveRegion(*pHROIHRegion, pHROIHRegion, 0, lOffset);
			//			}
			//		}
			//		else if (AlgorithmParam.m_iLocalAlignFitPos == LINE_FIT_YDIR_OBJECT_RIGHT)
			//		{
			//			if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT)
			//			{
			//				lOffset = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] - lRBPointX;
			//				MoveRegion(*pHROIHRegion, pHROIHRegion, 0, lOffset);
			//			}
			//		}
			//	}	// Horizontal/Vertial Line
			//	else
			//	{
			//		if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT &&
			//			pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT &&
			//			pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] != INVALID_ALIGN_RESULT &&
			//			pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] != INVALID_ALIGN_RESULT)
			//		{
			//			double dLineStartX, dLineStartY, dLineEndX, dLineEndY;

			//			dLineStartX = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx];
			//			dLineStartY = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx];
			//			dLineEndX = pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx];
			//			dLineEndY = pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx];

			//			POINT lLTPoint, lRTPoint, lLBPoint, lRBPoint;	// 4 Points(Polygon) of ROI
			//			double dIntersectPointX_1, dIntersectPointY_1, dIntersectPointX_2, dIntersectPointY_2;
			//			HTuple HIntersectX, HIntersectY, HParallel;

			//			Hobject HPolygonRgn;
			//			HTuple HRows, HCols;

			//			gen_empty_obj(&HPolygonRgn);

			//			lLTPoint.x = lLTPointX;
			//			lLTPoint.y = lLTPointY;
			//			lRTPoint.x = lRBPointX;
			//			lRTPoint.y = lLTPointY;
			//			lLBPoint.x = lLTPointX;
			//			lLBPoint.y = lRBPointY;
			//			lRBPoint.x = lRBPointX;
			//			lRBPoint.y = lRBPointY;

			//			if (AlgorithmParam.m_iLocalAlignFitPos == LINE_FIT_XDIR_OBJECT_TOP)
			//			{
			//				IntersectionLl(lLTPoint.y, lLTPoint.x, lLBPoint.y, lLBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
			//				dIntersectPointY_1 = HIntersectY[0].D();
			//				dIntersectPointX_1 = HIntersectX[0].D();

			//				IntersectionLl(lRTPoint.y, lRTPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
			//				dIntersectPointY_2 = HIntersectY[0].D();
			//				dIntersectPointX_2 = HIntersectX[0].D();

			//				lLTPoint.x = (int)(dIntersectPointX_1 + 0.5);
			//				lLTPoint.y = (int)(dIntersectPointY_1 + 0.5);
			//				lRTPoint.x = (int)(dIntersectPointX_2 + 0.5);
			//				lRTPoint.y = (int)(dIntersectPointY_2 + 0.5);
			//			}
			//			else if (AlgorithmParam.m_iLocalAlignFitPos == LINE_FIT_XDIR_OBJECT_BOTTOM)
			//			{
			//				IntersectionLl(lLTPoint.y, lLTPoint.x, lLBPoint.y, lLBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
			//				dIntersectPointY_1 = HIntersectY[0].D();
			//				dIntersectPointX_1 = HIntersectX[0].D();

			//				IntersectionLl(lRTPoint.y, lRTPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
			//				dIntersectPointY_2 = HIntersectY[0].D();
			//				dIntersectPointX_2 = HIntersectX[0].D();

			//				lLBPoint.x = (int)(dIntersectPointX_1 + 0.5);
			//				lLBPoint.y = (int)(dIntersectPointY_1 + 0.5);
			//				lRBPoint.x = (int)(dIntersectPointX_2 + 0.5);
			//				lRBPoint.y = (int)(dIntersectPointY_2 + 0.5);
			//			}
			//			else if (AlgorithmParam.m_iLocalAlignFitPos == LINE_FIT_YDIR_OBJECT_LEFT)
			//			{
			//				IntersectionLl(lLTPoint.y, lLTPoint.x, lRTPoint.y, lRTPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
			//				dIntersectPointY_1 = HIntersectY[0].D();
			//				dIntersectPointX_1 = HIntersectX[0].D();

			//				IntersectionLl(lLBPoint.y, lLBPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
			//				dIntersectPointY_2 = HIntersectY[0].D();
			//				dIntersectPointX_2 = HIntersectX[0].D();

			//				lLTPoint.x = (int)(dIntersectPointX_1 + 0.5);
			//				lLTPoint.y = (int)(dIntersectPointY_1 + 0.5);
			//				lLBPoint.x = (int)(dIntersectPointX_2 + 0.5);
			//				lLBPoint.y = (int)(dIntersectPointY_2 + 0.5);
			//			}
			//			else if (AlgorithmParam.m_iLocalAlignFitPos == LINE_FIT_YDIR_OBJECT_RIGHT)
			//			{
			//				IntersectionLl(lLTPoint.y, lLTPoint.x, lRTPoint.y, lRTPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
			//				dIntersectPointY_1 = HIntersectY[0].D();
			//				dIntersectPointX_1 = HIntersectX[0].D();

			//				IntersectionLl(lLBPoint.y, lLBPoint.x, lRBPoint.y, lRBPoint.x, dLineStartY, dLineStartX, dLineEndY, dLineEndX, &HIntersectY, &HIntersectX, &HParallel);
			//				dIntersectPointY_2 = HIntersectY[0].D();
			//				dIntersectPointX_2 = HIntersectX[0].D();

			//				lRTPoint.x = (int)(dIntersectPointX_1 + 0.5);
			//				lRTPoint.y = (int)(dIntersectPointY_1 + 0.5);
			//				lRBPoint.x = (int)(dIntersectPointX_2 + 0.5);
			//				lRBPoint.y = (int)(dIntersectPointY_2 + 0.5);
			//			}

			//			tuple_gen_const(0, 0, &HRows);
			//			tuple_gen_const(0, 0, &HCols);

			//			tuple_concat(HRows, lLTPoint.y, &HRows);
			//			tuple_concat(HCols, lLTPoint.x, &HCols);
			//			tuple_concat(HRows, lRTPoint.y, &HRows);
			//			tuple_concat(HCols, lRTPoint.x, &HCols);
			//			tuple_concat(HRows, lRBPoint.y, &HRows);
			//			tuple_concat(HCols, lRBPoint.x, &HCols);
			//			tuple_concat(HRows, lLBPoint.y, &HRows);
			//			tuple_concat(HCols, lLBPoint.x, &HCols);

			//			GenRegionPolygonFilled(&HPolygonRgn, HRows, HCols);

			//			Intersection(HPolygonRgn, *pHROIHRegion, pHROIHRegion);
			//		}
			//	}
			//}


			//	if (bFoundNew)
			//	{
			//		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
			//		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;

			//		smallest_rectangle1(*pHROIHRegion, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);

			//		lLTPointY = HlLTPointY.L();
			//		lLTPointX = HlLTPointX.L();
			//		lRBPointY = HlRBPointY.L();
			//		lRBPointX = HlRBPointX.L();

			//		if (pInspectROIRgn->m_iLocalAlignLineFitXPos2[iThreadIdx] == NOT_ANGLE_LINE && pInspectROIRgn->m_iLocalAlignLineFitYPos2[iThreadIdx] == NOT_ANGLE_LINE)
			//		{
			//			Hlong lOffset = 0;

			//			if (AlgorithmParam.m_iLocalAlignFitPos_2 == LINE_FIT_XDIR_OBJECT_TOP)
			//			{
			//				if (pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT)
			//				{
			//					lOffset = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] - lLTPointY;
			//					MoveRegion(*pHROIHRegion, pHROIHRegion, lOffset, 0);
			//				}
			//			}
			//			else if (AlgorithmParam.m_iLocalAlignFitPos_2 == LINE_FIT_XDIR_OBJECT_BOTTOM)
			//			{
			//				if (pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] != INVALID_ALIGN_RESULT)
			//				{
			//					lOffset = pInspectROIRgn->m_iLocalAlignLineFitYPos[iThreadIdx] - lRBPointY;
			//					MoveRegion(*pHROIHRegion, pHROIHRegion, lOffset, 0);
			//				}
			//			}
			//			else if (AlgorithmParam.m_iLocalAlignFitPos_2 == LINE_FIT_YDIR_OBJECT_LEFT)
			//			{
			//				if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT)
			//				{
			//					lOffset = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] - lLTPointX;
			//					MoveRegion(*pHROIHRegion, pHROIHRegion, 0, lOffset);
			//				}
			//			}
			//			else if (AlgorithmParam.m_iLocalAlignFitPos_2 == LINE_FIT_YDIR_OBJECT_RIGHT)
			//			{
			//				if (pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] != INVALID_ALIGN_RESULT)
			//				{
			//					lOffset = pInspectROIRgn->m_iLocalAlignLineFitXPos[iThreadIdx] - lRBPointX;
			//					MoveRegion(*pHROIHRegion, pHROIHRegion, 0, lOffset);
			//				}
			//			}
			//		}	// Horizontal/Vertial Line
			//	}	// if (bFoundNew)

			//}	// if (AlgorithmParam.m_bUseLocalAlignAddLineFit)

			//if (bFoundPost) //VER 1.1.4.3 add
			//{
			//	HTuple HomMat2DLocal;
			//	double dDeltaX, dDeltaY, dDeltaAngle;
			//	dDeltaX = dDeltaY = dDeltaAngle = 0;
			//	HTuple htLTy, htLTx, htRBy, htRBx, htCenterX, htCenterY;
			//	smallest_rectangle1(*pHROIHRegion, &htLTy, &htLTx, &htRBy, &htRBx);

			//	if (pInspectROIRgn2->m_iLocalAlignDeltaX != INVALID_ALIGN_RESULT)
			//		dDeltaX = pInspectROIRgn2->m_dLocalAlignDeltaAngleFixedPointX + pInspectROIRgn2->m_iLocalAlignDeltaX;
			//	else
			//		dDeltaX = pInspectROIRgn2->m_dLocalAlignDeltaAngleFixedPointX;

			//	if (pInspectROIRgn2->m_iLocalAlignDeltaY != INVALID_ALIGN_RESULT)
			//		dDeltaY = pInspectROIRgn2->m_dLocalAlignDeltaAngleFixedPointY + pInspectROIRgn2->m_iLocalAlignDeltaY;
			//	else
			//		dDeltaY = pInspectROIRgn2->m_dLocalAlignDeltaAngleFixedPointY;

			//	if (pInspectROIRgn2->m_dLocalAlignDeltaAngle != INVALID_ALIGN_RESULT)
			//		dDeltaAngle = pInspectROIRgn2->m_dLocalAlignDeltaAngle;

			//	if(AlgorithmParam.m_iPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_MODELCENTER )
			//		VectorAngleToRigid(dDeltaY, dDeltaX, 0, dDeltaY, dDeltaX, dDeltaAngle, &HomMat2DLocal);
			//	else
			//	{
			//		if (AlgorithmParam.m_iPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_LEFTTOP)
			//		{
			//			htCenterY = htLTy;
			//			htCenterX = htLTx;
			//		}
			//		else if (AlgorithmParam.m_iPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_RIGHTTOP)
			//		{
			//			htCenterY = htLTy;
			//			htCenterX = htRBx;
			//		}
			//		else if (AlgorithmParam.m_iPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_LEFTBOTTOM)
			//		{
			//			htCenterY = htRBy;
			//			htCenterX = htLTx;
			//		}
			//		else if (AlgorithmParam.m_iPostLocalAlignFitPos == POST_LOCAL_ALIGN_ROTATION_CENTER_RIGHTBOTTOM)
			//		{
			//			htCenterY = htRBy;
			//			htCenterX = htRBx;
			//		}
			//		VectorAngleToRigid(htCenterY, htCenterX, 0, htLTy, htRBx, dDeltaAngle, &HomMat2DLocal);
			//	}
			//	
			//	AffineTransRegion(*pHROIHRegion, pHROIHRegion, HomMat2DLocal, "nearest_neighbor");
			//}

			//Hobject HResultXLD;

			//gen_contour_region_xld(pHROIHRegion, &HResultXLD, "border");

			//if (THEAPP.m_pGFunction->ValidHXLD(HResultXLD))
			//	concat_obj(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD, HResultXLD, &(THEAPP.m_pInspectAdminViewDlg->m_HParameterDefectXLD));

			//THEAPP.m_pInspectAdminViewDlg->UpdateView();

		}

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		CString str;
		str.Format("Halcon Exception [Algorithm::ApplyLocalAlignResult] : <%s>%s", (const char *)HOperatorName[0].S(), (const char *)HErrMsg[0].S());
		THEAPP.SaveLog(str);

		return FALSE;
	}
}

// 비등방 얼라인 알고리즘 추가 - LeeGW
Hobject Algorithm::AnisoAlignAlgorithm(HTuple pHModelID, Hobject HImage, Hobject HInspectAreaRgn, CAlgorithmParam Param, BOOL* pbShiftResult, int* piLocalAlignDeltaX, int* piLocalAlignDeltaY, double* pdLocalAlignDeltaAngle, double* pdLocalAlignDeltaAngleFixedPointX, double* pdLocalAlignDeltaAngleFixedPointY)
{
	try
	{
		BOOL bDebugSave = FALSE;

		Hobject HAlignRgn;
		gen_empty_obj(&HAlignRgn);

		*pbShiftResult = FALSE;

		if (pHModelID < 0)
		{
			return HAlignRgn;
		}

		if (bDebugSave)
		{
			write_image(HImage, "bmp", 0, "c:\\DualTest\\InspectImage");
			write_region(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn.reg");
		}

		HTuple dAngleRangeRad;
		tuple_rad((double)Param.m_iROIAnisoAlignMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad = dAngleRangeRad  * -1.0;

		Hlong lROIArea;
		double dROICenterX, dROICenterY;
		HTuple HdROICenterX, HdROICenterY, HlROIArea;
		Hobject HInspectAreaRectRgn;
		shape_trans(HInspectAreaRgn, &HInspectAreaRectRgn, "rectangle1");
		area_center(HInspectAreaRectRgn, &HlROIArea, &HdROICenterY, &HdROICenterX);
		lROIArea = HlROIArea[0].L();
		dROICenterY = HdROICenterY[0].D();
		dROICenterX = HdROICenterX[0].D();

		int iSearchMarginX = Param.m_iROIAnisoAlignXSearchMargin;
		if (iSearchMarginX < 0)
			iSearchMarginX = 0;
		int iSearchMarginY = Param.m_iROIAnisoAlignYSearchMargin;
		if (iSearchMarginY < 0)
			iSearchMarginY = 0;

		Hobject HInspectAreaDilatedRgn;
		dilation_rectangle1(HInspectAreaRgn, &HInspectAreaDilatedRgn, iSearchMarginX * 2 + 1, iSearchMarginY * 2 + 1);

		Hobject HMatchingImageReduced;
		reduce_domain(HImage, HInspectAreaDilatedRgn, &HMatchingImageReduced);

		Hlong lNoFoundNumber = 0;
		HTuple Row, Column, Angle, ScaleX, ScaleY, Score;
		HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate, HomMat2DScale;

		double dTransX, dTransY;
		Hobject HModelContour;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		ScaleX = HTuple(0.0);
		ScaleY = HTuple(0.0);
		Score = HTuple(0.0);

		find_aniso_shape_model(HMatchingImageReduced,
			pHModelID,							// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad * 2.0,
			Param.m_dROIAnisoAlignMatchingScaleMinY,
			Param.m_dROIAnisoAlignMatchingScaleMaxY,
			Param.m_dROIAnisoAlignMatchingScaleMinX,
			Param.m_dROIAnisoAlignMatchingScaleMaxX,
			Param.m_dROIAnisoAlignMatchingScore,		// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"interpolation",							// Subpixel accuracy if not equal to 'none'. 'none', 'interpolation', 'least_squares', 'least_squares_high', 'least_squares_very_high'
			0,											// Number of pyramid levels used in the matching
			0.9,										// Greediness'' of the search heuristic (0: safe but slow; 1: fast but matches may be missed).
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&ScaleY,
			&ScaleX,
			&Score);									// Score of the found instances of the model.

		gen_empty_obj(&HMatchingImageReduced);

		HTuple HlNoFoundNumber;
		tuple_length(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber[0].L();

		if (lNoFoundNumber <= 0)
		{
			*piLocalAlignDeltaX = INVALID_ALIGN_RESULT;
			*piLocalAlignDeltaY = INVALID_ALIGN_RESULT;
			*pdLocalAlignDeltaAngle = INVALID_ALIGN_RESULT;
			*pdLocalAlignDeltaAngleFixedPointX = dROICenterX;
			*pdLocalAlignDeltaAngleFixedPointY = dROICenterY;
			return HAlignRgn;
		}

		double dDeltaX, dDeltaY, dAngle, dScaleX, dScaleY, dScore;
		dDeltaX = Column[0].D();
		dDeltaY = Row[0].D();
		dAngle = Angle[0].D();
		dScore = Score[0].D();
		dScaleX = ScaleX[0].D();
		dScaleY = ScaleY[0].D();

		double dAbs;
		Hobject HResultXLD;

		if (Param.m_bROIAnisoAlignInspectShift)
		{
			double dShiftX, dShiftY;
			dShiftX = dDeltaX - dROICenterX;
			dShiftY = dDeltaY - dROICenterY;

			if (dShiftX >= 0)
			{
				if (dShiftX > Param.m_dROIAnisoAlignShiftRight)
					*pbShiftResult = TRUE;
			}
			else
			{
				if (fabs(dShiftX) > Param.m_dROIAnisoAlignShiftLeft)
					*pbShiftResult = TRUE;
			}

			if (dShiftY >= 0)
			{
				if (dShiftY > Param.m_dROIAnisoAlignShiftBottom)
					*pbShiftResult = TRUE;
			}
			else
			{
				if (fabs(dShiftY) > Param.m_dROIAnisoAlignShiftTop)
					*pbShiftResult = TRUE;
			}
		}

		long lMoveX, lMoveY;

		if (Param.m_iROIAnisoAlignMatchingAngleRange == 0 && Param.m_dROIAnisoAlignMatchingScaleMinY == 1 && Param.m_dROIAnisoAlignMatchingScaleMaxY == 1)
		{
			if (Param.m_bROIAnisoAlignUsePosX)
			{
				dTransX = dDeltaX - dROICenterX;
				dAbs = fabs(dTransX);
				if (dAbs > (double)iSearchMarginX)
					dTransX = 0;
			}
			else
			{
				dTransX = 0;
			}

			if (Param.m_bROIAnisoAlignUsePosY)
			{
				dTransY = dDeltaY - dROICenterY;
				dAbs = fabs(dTransY);
				if (dAbs > (double)iSearchMarginY)
					dTransY = 0;
			}
			else
			{
				dTransY = 0;
			}

			lMoveX = (long)(dTransX + 0.5);
			lMoveY = (long)(dTransY + 0.5);

			move_region(HInspectAreaRgn, &HAlignRgn, lMoveY, lMoveX);

			dAngle = 0;
		}
		else
		{
			hom_mat2d_identity(&HomMat2DIdentity);
			hom_mat2d_translate(HomMat2DIdentity, dDeltaY, dDeltaX, &HomMat2DTranslate);
			hom_mat2d_rotate(HomMat2DTranslate, dAngle, dDeltaY, dDeltaX, &HomMat2DRotate);
			hom_mat2d_scale(HomMat2DRotate, dScaleY, dScaleX, dDeltaY, dDeltaX, &HomMat2DScale);

			get_shape_model_contours(&HModelContour, pHModelID, 1);
			affine_trans_contour_xld(HModelContour, &HResultXLD, HomMat2DScale);
			gen_region_contour_xld(HResultXLD, &HAlignRgn, "filled");

			dTransX = dDeltaX - dROICenterX;
			dTransY = dDeltaY - dROICenterY;
			lMoveX = (long)(dTransX + 0.5);
			lMoveY = (long)(dTransY + 0.5);
		}

		if (Param.m_bROIAnisoAlignLocalAlignUse && piLocalAlignDeltaX != NULL && piLocalAlignDeltaY != NULL && pdLocalAlignDeltaAngle != NULL)
		{
			*piLocalAlignDeltaX = lMoveX;
			*piLocalAlignDeltaY = lMoveY;
			*pdLocalAlignDeltaAngle = dAngle;
			*pdLocalAlignDeltaAngleFixedPointX = dROICenterX;
			*pdLocalAlignDeltaAngleFixedPointY = dROICenterY;
		}

		if (THEAPP.m_pGFunction->ValidHRegion(HAlignRgn) == TRUE)
			union1(HAlignRgn, &HAlignRgn);

		return HAlignRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		CString str;
		str.Format("Halcon Exception [Algorithm::ROIAnisoAlignAlgorithm] : <%s>%s", (const char*)HOperatorName[0].S(), (const char*)HErrMsg[0].S());
		THEAPP.SaveLog(str);

		Hobject HAlignRgn;
		gen_empty_obj(&HAlignRgn);
		return HAlignRgn;
	}
}

// 24.03.05 - PartCheck 알고리즘 추가 - LeeGW Start
Hobject Algorithm::PartCheckAlgorithm(HTuple HPartModelID, Hobject HImage, Hobject HROIRgn, CAlgorithmParam Param, int *piAlignDeltaX, int *piAlignDeltaY, double *pdAlignDeltaAngle, double *pdAlignDeltaAngleFixedPointX, double *pdAlignDeltaAngleFixedPointY, Hobject *pHAlignRgn)
{
	try
	{

		BOOL bDebugSave = FALSE;

		Hobject HDefectRgn;
		gen_empty_obj(&HDefectRgn);

		gen_empty_obj(pHAlignRgn);

		if (HPartModelID < 0)
		{
			return HDefectRgn;
		}

		Hobject HInspectAreaRgn;
		gen_empty_obj(&HInspectAreaRgn);

		HInspectAreaRgn = HROIRgn;
		if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
			return HDefectRgn;

		// Model Ref: G-Center of Domain
		double dTeachAlignRefX, dTeachAlignRefY;
		HTuple HlArea, HdTeachAlignRefY, HdTeachAlignRefX;
		area_center(HInspectAreaRgn, &HlArea, &HdTeachAlignRefY, &HdTeachAlignRefX);
		dTeachAlignRefY = HdTeachAlignRefY[0].D();
		dTeachAlignRefX = HdTeachAlignRefX[0].D();

		int iSearchMarginLeft = Param.m_iROIPartCheckMatchingSearchMarginX;
		if (iSearchMarginLeft < 0)
			iSearchMarginLeft = 0;
		int iSearchMarginRight = Param.m_iROIPartCheckMatchingSearchMarginX2;
		if (iSearchMarginRight < 0)
			iSearchMarginRight = 0;
		int iSearchMarginTop = Param.m_iROIPartCheckMatchingSearchMarginY;
		if (iSearchMarginTop < 0)
			iSearchMarginTop = 0;
		int iSearchMarginBottom = Param.m_iROIPartCheckMatchingSearchMarginY2;
		if (iSearchMarginBottom < 0)
			iSearchMarginBottom = 0;

		Hobject HInspectAreaDilatedRgn;
		shape_trans(HInspectAreaRgn, &HInspectAreaDilatedRgn, "rectangle1");

		Hlong lLTPointY, lLTPointX, lRBPointY, lRBPointX;
		HTuple HlLTPointY, HlLTPointX, HlRBPointY, HlRBPointX;
		smallest_rectangle1(HInspectAreaDilatedRgn, &HlLTPointY, &HlLTPointX, &HlRBPointY, &HlRBPointX);
		lLTPointY = HlLTPointY[0].L();
		lLTPointX = HlLTPointX[0].L();
		lRBPointY = HlRBPointY[0].L();
		lRBPointX = HlRBPointX[0].L();

		BYTE *pImageData;
		char type[128];
		Hlong lImageWidth, lImageHeight;

		get_image_pointer1(HImage, (Hlong*)&pImageData, type, &lImageWidth, &lImageHeight);

		Hlong lCropLTPointX, lCropLTPointY, lCropRBPointX, lCropRBPointY;

		lCropLTPointX = lLTPointX - iSearchMarginLeft;
		if (lCropLTPointX < 0)
			lCropLTPointX = 0;
		lCropLTPointY = lLTPointY - iSearchMarginTop;
		if (lCropLTPointY < 0)
			lCropLTPointY = 0;
		lCropRBPointX = lRBPointX + iSearchMarginRight;
		if (lCropRBPointX >= lImageWidth)
			lCropRBPointX = lImageWidth - 1;
		lCropRBPointY = lRBPointY + iSearchMarginBottom;
		if (lCropRBPointY >= lImageHeight)
			lCropRBPointY = lImageHeight - 1;

		Hobject HMatchingImageReduced;
		crop_part(HImage, &HMatchingImageReduced, lCropLTPointY, lCropLTPointX, lCropRBPointY, lCropRBPointX);

		HTuple dAngleRangeRad;
		tuple_rad(Param.m_iROIPartCheckMatchingAngleRange, &dAngleRangeRad);

		if (dAngleRangeRad < 0)
			dAngleRangeRad = dAngleRangeRad * -1.0;

		Hlong lNoFoundNumber;
		HTuple Row, Column, Angle, Score;

		Row = HTuple(0.0);
		Column = HTuple(0.0);
		Angle = HTuple(0.0);
		Score = HTuple(0.0);

		HTuple NumLevels, AngleStart, AngleExtent, AngleStep, Metric;
		get_ncc_model_params(HPartModelID, &NumLevels, &AngleStart, &AngleExtent, &AngleStep, &Metric);

		int iNoTeachNumLevel = 0;
		iNoTeachNumLevel = NumLevels[0].L();
		if (iNoTeachNumLevel > 4)
			iNoTeachNumLevel = 4;
		else
			iNoTeachNumLevel = 0;

		find_ncc_model(HMatchingImageReduced,
			HPartModelID,								// Handle of the model
			-dAngleRangeRad,							// Smallest rotation of the model
			dAngleRangeRad*2.0,							// Extent of the rotation angles.
			0.1,										// Minumum score of the instances of the model to be found
			1,											// Number of instances of the model to be found
			0.5,										// Maximum overlap of the instances of the model to be found
			"true",										// Subpixel accuracy
			iNoTeachNumLevel,							// Number of pyramid levels used in the matching
			&Row,										// Row coordinate of the found instances of the model.
			&Column,									// Column coordinate of the found instances of the model.
			&Angle,										// Rotation angle of the found instances of the model. (radian)
			&Score);									// Score of the found instances of the model.

		
		HMatchingImageReduced.Reset();
		gen_empty_obj(&HMatchingImageReduced);

		HTuple HlNoFoundNumber;
		tuple_length(Score, &HlNoFoundNumber);
		lNoFoundNumber = HlNoFoundNumber[0].L();

		if (lNoFoundNumber <= 0)
		{
			HDefectRgn = HInspectAreaRgn;
			*piAlignDeltaX = INVALID_ALIGN_RESULT;
			*piAlignDeltaY = INVALID_ALIGN_RESULT;
			*pdAlignDeltaAngle = INVALID_ALIGN_RESULT;
			*pdAlignDeltaAngleFixedPointX = dTeachAlignRefX;
			*pdAlignDeltaAngleFixedPointY = dTeachAlignRefY; 

		}
		else
		{
			double dDeltaX, dDeltaY, dAngle, dScore, dXShift, dYShift;
			HTuple HAngleDeg;
			double dAngleDeg;
			HTuple HomMat2DRotate;
			double dMatchingScoreTolerance;
			Hobject HAffineTransRgn, HResultXLD;

			dDeltaX = Column[0].D();
			dDeltaY = Row[0].D();
			dAngle = Angle[0].D();
			dScore = Score[0].D();

			dDeltaX = dDeltaX + lCropLTPointX;
			dDeltaY = dDeltaY + lCropLTPointY;

			vector_angle_to_rigid(dTeachAlignRefY, dTeachAlignRefX, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);
			affine_trans_region(HInspectAreaRgn, &HAffineTransRgn, HomMat2DRotate, "true"); //nearest_neighbor
			gen_contour_region_xld(HAffineTransRgn, &HResultXLD, "border");
			
			if (Param.m_bROIPartCheckExistInspUse)
			{
				dMatchingScoreTolerance = (double)Param.m_iROIPartCheckMatchingScore / 100;

				if (dScore < dMatchingScoreTolerance)
					HDefectRgn = HAffineTransRgn;
			}

			dXShift = dDeltaX - dTeachAlignRefX;
			dYShift = dDeltaY - dTeachAlignRefY;
			tuple_deg(Angle, &HAngleDeg);
			dAngleDeg = HAngleDeg[0].D();
			
			if (Param.m_bROIPartCheckShiftInspUse)
			{
				if (fabs(dXShift) > (double)Param.m_iROIPartCheckShiftX || fabs(dYShift) > (double)Param.m_iROIPartCheckShiftY || fabs(dAngleDeg) > (double)Param.m_dROIPartCheckRotationAngle)
					HDefectRgn = HAffineTransRgn;
			}

			if (Param.m_bROIPartCheckLocalAlignUse)
			{
				*piAlignDeltaX = (long)(dXShift + 0.5);
				*piAlignDeltaY = (long)(dYShift + 0.5);
				*pdAlignDeltaAngle = dAngle;
				*pdAlignDeltaAngleFixedPointX = dTeachAlignRefX;
				*pdAlignDeltaAngleFixedPointY = dTeachAlignRefY;
				*pHAlignRgn = HAffineTransRgn;
			}

			HomMat2DRotate.Reset();
			HAffineTransRgn.Reset();
		}

		return HDefectRgn;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		CString str;
		str.Format("Halcon Exception [Algorithm::PartCheckAlgorithm] : <%s>%s", (const char *)HOperatorName[0].S(), (const char *)HErrMsg[0].S());
		THEAPP.SaveLog(str);

		return HROIRgn;
	}
}

// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW Start
void Algorithm::RemoveDontCare(Hobject *pNewHRegion, CAlgorithmParam Param, int iThreadIdx)
{
try{
	Hobject HPolygonRgn;
	HPolygonRgn = *pNewHRegion;

	Hobject HDontCareRgn;
	gen_empty_obj(&HDontCareRgn);

	Hobject HDefectRgn, HIntersectRgn, _HFoundInspectAreaRgn, _HResultXld;

	GTRegion *pRegion;
	for (int k=0; k<THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegionCount(); k++)
	{
		pRegion = THEAPP.m_pModelDataManager->m_pInspectionArea->GetChildTRegion(k);

		CAlgorithmParam AlgorithmParam;

		for (int i = 0; i < MAX_ROI_TEACHING_TAB; i++)
		{
			AlgorithmParam = pRegion->m_AlgorithmParam[i];
	
			if (Param.m_iImageType != pRegion->m_iTeachImageIndex - 1)
				continue;

			if (AlgorithmParam.m_bUseDontCare == TRUE)
			{
				if (THEAPP.m_pGFunction->ValidHImage(m_HAlgoInspectImage[pRegion->m_iTeachImageIndex - 1]) == FALSE)
				{
					HDefectRgn = CommonAlgorithmROI(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[pRegion->m_iTeachImageIndex - 1], pRegion, AlgorithmParam, &_HFoundInspectAreaRgn, &_HResultXld, iThreadIdx);
				}
				else
				{
					HDefectRgn = CommonAlgorithmROI(m_HAlgoInspectImage[pRegion->m_iTeachImageIndex - 1], pRegion, AlgorithmParam, &_HFoundInspectAreaRgn, &_HResultXld, iThreadIdx);
				}
				if (THEAPP.m_pGFunction->ValidHRegion(HDefectRgn))
				{
					intersection(HPolygonRgn, HDefectRgn, &HIntersectRgn);
					if (THEAPP.m_pGFunction->ValidHRegion(HIntersectRgn))
					{
						concat_obj(HDontCareRgn, HIntersectRgn, &HDontCareRgn);
					}
				}
			}
		}

	}

	if (THEAPP.m_pGFunction->ValidHRegion(HDontCareRgn))
	{
		union1(HDontCareRgn, &HDontCareRgn);
		difference(HPolygonRgn, HDontCareRgn, &HPolygonRgn);
	}

	copy_obj(HPolygonRgn, pNewHRegion, 1, -1);
}
catch (HException& except)
{
	CString str; str.Format("Halcon Exception [Algorithm::RemoveDontCare] : %s", except.message);	THEAPP.SaveLog(str);
}
}
// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW End


// 24.06.07 - v2647 - 매칭 정확도 향상을 위한 필터링 추가 - LeeGW Start
BOOL Algorithm::GetemphasizedImage(Hobject HImage, Hobject *pHScaledImage)
{
	//try
	//{

	//	Hobject HProcessImage;
	//	HTuple HFactor, HSize, HIter;
	//	//Hobject  SelectedChannel, LowerRegion, UpperRegion;
	//	//HTuple  Theta, Iterations, Sigma;
	//	//HTuple  LowerLimit, UpperLimit, Channels;
	//	//HTuple	Index, MinGray, MaxGray, ExpDefaultCtrlDummyVar;
	//	HTuple HAbsHisto, HRelHisto, HFunction, HSmoothFunction, Indices, Inverted;
	//	long lPeak, lHystUpperThreshold, lHystLowThreshold;


	//	// Factor value 제한
	//	if (THEAPP.m_pModelDataManager->m_dEmphasisFilterFactor > 20.0) 
	//	{
	//		HFactor = 20.0;
	//	}
	//	else if (THEAPP.m_pModelDataManager->m_dEmphasisFilterFactor < 0.0) 
	//	{
	//		HFactor = 0.0;
	//	}
	//	else
	//	{
	//		HFactor = THEAPP.m_pModelDataManager->m_dEmphasisFilterFactor;
	//	}

	//	// Size value 제한
	//	if (THEAPP.m_pModelDataManager->m_iEmphasisFilterSize > 201) 
	//	{
	//		HSize = 201;
	//	}
	//	else if (THEAPP.m_pModelDataManager->m_iEmphasisFilterSize < 3) 
	//	{
	//		HSize = 3;
	//	}
	//	else
	//	{
	//		HSize = THEAPP.m_pModelDataManager->m_iEmphasisFilterSize;
	//	}

	//	for (HIter=0; HIter < THEAPP.m_pModelDataManager->m_dEmphasisFilterIteration; HIter+=1)
	//	{
	//		emphasize(HImage, &HProcessImage, HSize, HSize, HFactor);
	//		binomial_filter(HProcessImage, &HProcessImage, 3, 3);
	//	}

	//	if(THEAPP.m_pModelDataManager->m_bUseEmphasisThresholdFilter)
	//	{
	//		int iScaleMin, iScaleMax;
	//		int iScalePeakUpperOffset = 20;
	//		int iScalePeakLowerOffset = 10;

	//		gray_histo(HProcessImage, HProcessImage, &HAbsHisto, &HRelHisto);
	//		create_funct_1d_array(HAbsHisto, &HFunction);
	//		smooth_funct_1d_gauss(HFunction, 10.0, &HSmoothFunction);
	//		tuple_last_n(HFunction, 3, &HAbsHisto);
	//		tuple_sort_index(HAbsHisto, &Indices);
	//		tuple_inverse(Indices, &Inverted);
	//		lPeak = Inverted[0].L();

	//		iScaleMax = lPeak + iScalePeakUpperOffset;
	//		if (iScaleMax>255)
	//			iScaleMax = 255;

	//		iScaleMin = lPeak - iScalePeakLowerOffset;
	//		if (iScaleMin<0)
	//			iScaleMin = 0;

	//		THEAPP.m_pGFunction->ScaleImageRange(HImage, &HProcessImage, HTuple(iScaleMin), HTuple(iScaleMax));
	//		binomial_filter(HProcessImage, &HProcessImage, 3, 3);
	//	}

	//	copy_obj(HProcessImage, pHScaledImage, 1, 1);

	//	gen_empty_obj(&HProcessImage);

	//	return TRUE;
	//}
	//catch (HException& except)
	//{
	//	CString str; str.Format("Halcon Exception [Algorithm::GetScaledImage3] : %s", except.message);	THEAPP.SaveLog(str);
	//	return FALSE;
	//}

	return FALSE;
}
// 24.06.07 - v2647 - 매칭 정확도 향상을 위한 필터링 추가 - LeeGW End


BOOL Algorithm::GetPreprocessImage(BOOL bUseDomain, Hobject HOrgImage, Hobject HROIRgn, CAlgorithmParam Param, Hobject *pHArithmeticImage)
{
	try
	{
		BOOL bDebugSave = FALSE;

		if (bDebugSave)
			write_image(HOrgImage, "bmp", 0, "c:\\DualTest\\ProcessingOrgImage");

		Hobject HImageReduced1, HImageReduced2, HImageReduced3, HImageReduced4;
		int iExpandSize;
		Hobject HExpandReduceImage;
		Hobject HInspectAreaRgn;
		Hobject HMaxImage, HMinImage;

		gen_empty_obj(&HInspectAreaRgn);
		gen_empty_obj(pHArithmeticImage);


		if (Param.m_bUseImageProcessFilter)
		{
			if (bUseDomain)
			{
				//GetInspectAreaROI(HOrgImage, HROIRgn, Param, &HInspectAreaRgn);
				copy_obj(HROIRgn, &HInspectAreaRgn, 1, -1);
				if (THEAPP.m_pGFunction->ValidHRegion(HInspectAreaRgn) == FALSE)
				{
					copy_image(HOrgImage, pHArithmeticImage);
					return FALSE;
				}

				if (bDebugSave)
					write_region(HInspectAreaRgn, "c:\\DualTest\\InspectAreaRgn");

				reduce_domain(HOrgImage, HInspectAreaRgn, &HImageReduced1);
			}
			else
			{
				HImageReduced1 = HOrgImage;
			}

			if ((Param.m_iImageProcessFilterType1 == FILTER_TYPE_NOT_USED || Param.m_iImageProcessFilterType1 == FILTER_TYPE_MEDIAN || Param.m_iImageProcessFilterType1 == FILTER_TYPE_SHARPENING) == FALSE)
			{
				if (Param.m_iImageProcessFilterType1X > Param.m_iImageProcessFilterType1Y)
					iExpandSize = Param.m_iImageProcessFilterType1X / 2 + 1;
				else
					iExpandSize = Param.m_iImageProcessFilterType1Y / 2 + 1;

				expand_domain_gray(HImageReduced1, &HExpandReduceImage, iExpandSize);
				reduce_domain(HExpandReduceImage, HInspectAreaRgn, &HImageReduced1);
			}

			switch (Param.m_iImageProcessFilterType1)
			{
			case FILTER_TYPE_NOT_USED:
				break;
			case FILTER_TYPE_MEAN:
				mean_image(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y);
				break;
#if H_VERSION > 9
			case FILTER_TYPE_MEDIAN:
				median_rect(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y);
				break;
#endif
			case FILTER_TYPE_GAUSS:
				binomial_filter(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y);
				break;
			case FILTER_TYPE_SCALE_MAX:
				scale_image_max(HImageReduced1, pHArithmeticImage);
				break;
			case FILTER_TYPE_emphasize:
				emphasize(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y, 1.0);
				break;
			case FILTER_TYPE_GRAY_OPENING:
				gray_opening_shape(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y, "octagon");
				break;
			case FILTER_TYPE_GRAY_CLOSING:
				gray_closing_shape(HImageReduced1, pHArithmeticImage, (int)Param.m_iImageProcessFilterType1X, (int)Param.m_iImageProcessFilterType1Y, "octagon");
				break;
#if H_VERSION > 10
			case FILTER_TYPE_EXPONENT:
				pow_image(HImageReduced1, pHArithmeticImage, Param.m_iImageProcessFilterType1X);
				scale_image_max(*pHArithmeticImage, pHArithmeticImage);
				break;
			case FILTER_TYPE_LOG:
				log_image(HImageReduced1, pHArithmeticImage, "e");
				scale_image_max(*pHArithmeticImage, pHArithmeticImage);
				break;
#endif
			case FILTER_TYPE_ANISO_DIFFUSION:
				anisotropic_diffusion(HImageReduced1, pHArithmeticImage, "perona-malik", (int)Param.m_iImageProcessFilterType1X, 1, (int)Param.m_iImageProcessFilterType1Y);
				break;
			case FILTER_TYPE_SHARPENING:
				shock_filter(HImageReduced1, pHArithmeticImage, 0.5, 10, "canny", 1.5);
				break;
			default:
				break;
			}

			if (Param.m_iImageProcessFilterType1 == FILTER_TYPE_NOT_USED)
				*pHArithmeticImage = HImageReduced1;

			if ((Param.m_iImageProcessFilterType2 == FILTER_TYPE_NOT_USED || Param.m_iImageProcessFilterType2 == FILTER_TYPE_MEDIAN || Param.m_iImageProcessFilterType2 == FILTER_TYPE_SHARPENING) == FALSE)
			{
				if (Param.m_iImageProcessFilterType2X > Param.m_iImageProcessFilterType2Y)
					iExpandSize = Param.m_iImageProcessFilterType2X / 2 + 1;
				else
					iExpandSize = Param.m_iImageProcessFilterType2Y / 2 + 1;

				expand_domain_gray(*pHArithmeticImage, &HExpandReduceImage, iExpandSize);
				reduce_domain(HExpandReduceImage, HInspectAreaRgn, pHArithmeticImage);
			}

			switch (Param.m_iImageProcessFilterType2)
			{
			case FILTER_TYPE_NOT_USED:
				break;
			case FILTER_TYPE_MEAN:
				mean_image(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y);
				break;
#if H_VERSION > 9
			case FILTER_TYPE_MEDIAN:
				median_rect(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y);
				break;
#endif
			case FILTER_TYPE_GAUSS:
				binomial_filter(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y);
				break;
			case FILTER_TYPE_SCALE_MAX:
				scale_image_max(*pHArithmeticImage, pHArithmeticImage);
				break;
			case FILTER_TYPE_emphasize:
				emphasize(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y, 1.0);
				break;
			case FILTER_TYPE_GRAY_OPENING:
				gray_opening_shape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y, "octagon");
				break;
			case FILTER_TYPE_GRAY_CLOSING:
				gray_closing_shape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType2X, (int)Param.m_iImageProcessFilterType2Y, "octagon");
				break;
#if H_VERSION > 10
			case FILTER_TYPE_EXPONENT:
				pow_image(*pHArithmeticImage, pHArithmeticImage, Param.m_iImageProcessFilterType2X);
				scale_image_max(*pHArithmeticImage, pHArithmeticImage);
				break;
			case FILTER_TYPE_LOG:
				log_image(*pHArithmeticImage, pHArithmeticImage, "e");
				scale_image_max(*pHArithmeticImage, pHArithmeticImage);
				break;
#endif
			case FILTER_TYPE_ANISO_DIFFUSION:
				anisotropic_diffusion(*pHArithmeticImage, pHArithmeticImage, "perona-malik", (int)Param.m_iImageProcessFilterType2X, 1, (int)Param.m_iImageProcessFilterType2Y);
				break;
			case FILTER_TYPE_SHARPENING:
				shock_filter(*pHArithmeticImage, pHArithmeticImage, 0.5, 10, "canny", 1.5);
				break;
			default:
				break;
			}

			if (Param.m_iImageProcessFilterType1 == FILTER_TYPE_NOT_USED && Param.m_iImageProcessFilterType2 == FILTER_TYPE_NOT_USED)
				*pHArithmeticImage = HImageReduced1;

			if ((Param.m_iImageProcessFilterType3 == FILTER_TYPE_NOT_USED || Param.m_iImageProcessFilterType3 == FILTER_TYPE_MEDIAN || Param.m_iImageProcessFilterType3 == FILTER_TYPE_SHARPENING) == FALSE)
			{
				if (Param.m_iImageProcessFilterType3X > Param.m_iImageProcessFilterType3Y)
					iExpandSize = Param.m_iImageProcessFilterType3X / 2 + 1;
				else
					iExpandSize = Param.m_iImageProcessFilterType3Y / 2 + 1;

				expand_domain_gray(*pHArithmeticImage, &HExpandReduceImage, iExpandSize);
				reduce_domain(HExpandReduceImage, HInspectAreaRgn, pHArithmeticImage);
			}

			switch (Param.m_iImageProcessFilterType3)
			{
			case FILTER_TYPE_NOT_USED:
				break;
			case FILTER_TYPE_MEAN:
				mean_image(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y);
				break;
#if H_VERSION > 9
			case FILTER_TYPE_MEDIAN:
				median_rect(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y);
				break;
#endif
			case FILTER_TYPE_GAUSS:
				binomial_filter(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y);
				break;
			case FILTER_TYPE_SCALE_MAX:
				scale_image_max(*pHArithmeticImage, pHArithmeticImage);
				break;
			case FILTER_TYPE_emphasize:
				emphasize(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y, 1.0);
				break;
			case FILTER_TYPE_GRAY_OPENING:
				gray_opening_shape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y, "octagon");
				break;
			case FILTER_TYPE_GRAY_CLOSING:
				gray_closing_shape(*pHArithmeticImage, pHArithmeticImage, (int)Param.m_iImageProcessFilterType3X, (int)Param.m_iImageProcessFilterType3Y, "octagon");
				break;
#if H_VERSION > 10
			case FILTER_TYPE_EXPONENT:
				pow_image(*pHArithmeticImage, pHArithmeticImage, Param.m_iImageProcessFilterType3X);
				scale_image_max(*pHArithmeticImage, pHArithmeticImage);
				break;
			case FILTER_TYPE_LOG:
				log_image(*pHArithmeticImage, pHArithmeticImage, "e");
				scale_image_max(*pHArithmeticImage, pHArithmeticImage);
				break;
#endif
			case FILTER_TYPE_ANISO_DIFFUSION:
				anisotropic_diffusion(*pHArithmeticImage, pHArithmeticImage, "perona-malik", (int)Param.m_iImageProcessFilterType3X, 1, (int)Param.m_iImageProcessFilterType3Y);
				break;
			case FILTER_TYPE_SHARPENING:
				shock_filter(*pHArithmeticImage, pHArithmeticImage, 0.5, 10, "canny", 1.5);
				break;
			default:
				break;
			}
		}

		if (bDebugSave)
			write_image(*pHArithmeticImage, "bmp", 0, "c:\\DualTest\\ProcessingAfterImage");

		return TRUE;
	}
	catch (HException& except)
	{
		HTuple HExp;
		HTuple HOperatorName, HErrMsg;
		except.ToHTuple(&HExp);
		except.GetExceptionData(HExp, "operator", &HOperatorName);
		except.GetExceptionData(HExp, "error_message", &HErrMsg);

		CString str;
		str.Format("Halcon Exception [Algorithm::GetPreprocessImage] : <%s>%s", (const char *)HOperatorName[0].S(), (const char *)HErrMsg[0].S());
		THEAPP.SaveLog(str);

		return FALSE;
	}
}

// 24.07.03 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
BOOL Algorithm::TemplateMatchingAlgorithm(Hobject HImage, Hobject *HContour, HTuple *ModelID)
{
try
{
	CString strLog;
	double dMatchingScore = THEAPP.m_pModelDataManager->m_dBLROIAlignMatchingScore;;
	int iSearchingMargin = THEAPP.m_pModelDataManager->m_iBLROIAlignSearchingMargin;

	if (*ModelID<0)
		return FALSE;

	Hlong lNoFoundNumber = 0;
	HTuple HRowRef, HColRef, HRadRef;
	HTuple HRow, HCol, HAngle, HScore;
	Hobject HMatchingRgn, HMatchingImage_Reduced;
		
	gen_region_contour_xld(*HContour, &HMatchingRgn, "filled");
	smallest_circle(HMatchingRgn ,&HRowRef, &HColRef, &HRadRef);
	union1(HMatchingRgn, &HMatchingRgn);
	shape_trans(HMatchingRgn, &HMatchingRgn, "rectangle1");
	dilation_rectangle1(HMatchingRgn, &HMatchingRgn, iSearchingMargin*2+1, iSearchingMargin*2+1);
	reduce_domain(HImage, HMatchingRgn, &HMatchingImage_Reduced);
	binomial_filter(HMatchingImage_Reduced, &HMatchingImage_Reduced, 5, 5);

	HRow = HTuple(0.0);
	HCol = HTuple(0.0);
	HAngle = HTuple(0.0);
	HScore = HTuple(0.0);
					
	find_ncc_model(HMatchingImage_Reduced,
		*ModelID,		// Handle of the model
		-0.39,														// Smallest rotation of the model
		0.79,														// Extent of the rotation angles.
		dMatchingScore,															// Minumum score of the instances of the model to be found
		1,																		// Number of instances of the model to be found
		0.5,																	// Maximum overlap of the instances of the model to be found
		"true",																	// Subpixel accuracy
		0,																		// Number of pyramid levels used in the matching
		&HRow,																	// Row coordinate of the found instances of the model.
		&HCol,																// Column coordinate of the found instances of the model.
		&HAngle,																	// Rotation angle of the found instances of the model. (radian)
		&HScore);																// Score of the found instances of the model.

		tuple_length(HScore, &lNoFoundNumber);

		if (lNoFoundNumber>0)
		{
			double dRowRef, dColRef;
			double dDeltaY, dDeltaX, dAngle, dScore;
			double dTransX, dTransY, dRotAngle;

			HTuple HomMat2DIdentity, HomMat2DTranslate, HomMat2DRotate;
			Hobject HResultXLD;
			
			dRowRef = HRowRef[0].D();
			dColRef = HColRef[0].D();
			dDeltaY = HRow[0].D();
			dDeltaX = HCol[0].D();
			dAngle = HAngle[0].D();
			dScore = HScore[0].D();

			gen_region_contour_xld(*HContour, &HMatchingRgn, "filled");
			vector_angle_to_rigid(dRowRef, dColRef, 0, dDeltaY, dDeltaX, dAngle, &HomMat2DRotate);
			affine_trans_region(HMatchingRgn, &HResultXLD, HomMat2DRotate, "false");
			gen_contour_region_xld(HResultXLD, HContour, "border");
		
			dTransX = dDeltaX - dColRef;
			dTransY = dDeltaY - dRowRef;
			tuple_deg(HAngle, &HAngle);
			dRotAngle = HAngle[0].D();

			HomMat2DIdentity.Reset();
			HomMat2DTranslate.Reset();
			HomMat2DRotate.Reset();

			strLog.Format("Template Matching Success [(X,Y)Delta(px)=(%d,%d), Rotation(Deg)=(%.2lf), Score(%%)=%d]", (int)dTransX, (int)dTransY, dRotAngle, (int)(dScore*100.0));
		}
		else
		{
			strLog.Format("Template Matching Failed");
		}

		HMatchingRgn.Reset();
		gen_empty_obj(&HMatchingImage_Reduced);

		THEAPP.SaveLog(strLog);

		return TRUE;
}
catch (HException &except)
{
	CString str;
	str.Format("Halcon Exception [TemplateMatchingAlgorithm] : %s",except.message);
	THEAPP.SaveLog(str);

	return FALSE;
}
}
// 24.07.03 - v2651 - ROI Templet Matching Align 추가 - LeeGW END

// FAI 측정
void Algorithm::InspectFAI(int iTrayNo, int iIndexNo, int iModuleNo)
{
	// ***************** FAI-51 *****************
	// FAI-51 변수
	double dDistance, dAngle;
	HTuple HDistance, HAngle;
	DPOINT dPoint1, dPoint2;
	DPOINT dLineStart, dLineEnd;
	DPOINT dLineStart2, dLineEnd2;
	HTuple HInnerEdgeConcatX, HInnerEdgeConcatY;
	HTuple HOuterEdgeConcatX, HOuterEdgeConcatY;
	HTuple HDatumEdgeConcatX, HDatumEdgeConcatY;
	HTuple HDatumEllipseEdgeConcatX, HDatumEllipseEdgeConcatY;
	double dInnerNormalRadius, dOuterNormalRadius, dDatumNormalRadius;
	int iNoInnerEdge, iNoOuterEdge, iNoDatumEdge, iNoDatumEllipseEdge;
	HTuple HNoCircleFitEdge;
	double dInnerCircleCenterX, dInnerCircleCenterY, dInnerCircleRadius;
	double dOuterCircleCenterX, dOuterCircleCenterY, dOuterCircleRadius;
	double dDatumCircleCenterX, dDatumCircleCenterY, dDatumCircleRadius;
	double dDatumEllipseCenterX, dDatumEllipseCenterY, dDatumEllipseMajor, dDatumEllipseMinor, dDatumEllipseAngle;
	double dShiftX, dShiftY;
	Hobject HInnerCircleXLD, HOuterCircleXLD, HDatumCircleXLD, HDatumEllipseXLD, HEllipseContour, HCrossXLD, HFAIMeasureRgn;
	HTuple HStartPhi, HEndPhi, HPointOrder;
	HTuple HDatumEllipseCenterX, HDatumEllipseCenterY, HDatumEllipseArea, HDatumCircleCenterX, HDatumCircleCenterY, HDatumCircleArea;

	dDatumCircleCenterY = dDatumCircleCenterX = -1.0;
	dDatumEllipseCenterY = dDatumEllipseCenterX = -1.0;
	dInnerCircleCenterY = dInnerCircleCenterX = -1.0;

	int iInnerCircleMaxFitPonit = 8;
	switch (THEAPP.m_pModelDataManager->m_iFAIInnerCircleFitType)
	{
	case 0:	// 8P
		iInnerCircleMaxFitPonit = 8;
		break;
	case 1:	// 6P
		iInnerCircleMaxFitPonit = 6;
		break;
	case 2:	// 4P
		iInnerCircleMaxFitPonit = 4;
		break;
	}

	int iOuterCircleMaxFitPonit = 8;
	switch (THEAPP.m_pModelDataManager->m_iFAIOuterCircleFitType)
	{
	case 0:	// 8P
		iOuterCircleMaxFitPonit = 8;
		break;
	case 1:	// 6P
		iOuterCircleMaxFitPonit = 6;
		break;
	case 2:	// 4P
		iOuterCircleMaxFitPonit = 4;
		break;
	}

	int iDatumCircleMaxFitPonit = 8;
	switch (THEAPP.m_pModelDataManager->m_iFAIDatumCircleFitType)
	{
	case 0:	// 8P
		iDatumCircleMaxFitPonit = 8;
		break;
	case 1:	// 6P
		iDatumCircleMaxFitPonit = 6;
		break;
	case 2:	// 4P
		iDatumCircleMaxFitPonit = 4;
		break;
	}
	
	double dFAIMultiple[MAX_FAI_ITEM] = { 1 };
	double dFAIOffset[MAX_FAI_ITEM] = { 0 };

#ifdef INLINE_MODE
	int iPocketNo = -1;
	if (iIndexNo > 0)
		iPocketNo = THEAPP.m_iModuleNumberOneLine * (iIndexNo - 1) + (iModuleNo - 1) % THEAPP.m_iModuleNumberOneLine + 1;
#else
	int iPocketNo = THEAPP.m_pModelDataManager->m_iFAISimulPocketNum;
#endif

	for (int kk = 0; kk < MAX_FAI_ITEM; kk++)
	{
		if (THEAPP.m_pModelDataManager->m_bFAIUniqueOffsetUse[kk] == TRUE && (iPocketNo > 0 && iPocketNo <= MAX_CIRCULAR_TABLE_POCKET))
		{
			dFAIMultiple[kk] = THEAPP.m_pModelDataManager->m_dFAIMultiple[kk] * THEAPP.m_pModelDataManager->m_dFAIUniqueMultiple[kk][iPocketNo - 1];
			dFAIOffset[kk] = THEAPP.m_pModelDataManager->m_dFAIMultiple[kk] * THEAPP.m_pModelDataManager->m_dFAIUniqueOffset[kk][iPocketNo - 1] + THEAPP.m_pModelDataManager->m_dFAIOffset[kk];

			// 임시 if 처리. 필요시 아래 if 문 삭제.
			if (dFAIMultiple[kk] != 1.0 || dFAIOffset[kk] != 0.0)
			{
				CString sLog;
				sLog.Format("FAI-%d Pocket-%d 보정 : Multiple=%.3f, Offset=%.3f", kk, iPocketNo, dFAIMultiple[kk], dFAIOffset[kk]);
				THEAPP.SaveLog(sLog);
			}
		}
		else
		{
			dFAIMultiple[kk] = THEAPP.m_pModelDataManager->m_dFAIMultiple[kk];
			dFAIOffset[kk] = THEAPP.m_pModelDataManager->m_dFAIOffset[kk];

			// 임시 if 처리. 필요시 아래 if 문 삭제.
			if (dFAIMultiple[kk] != 1.0 || dFAIOffset[kk] != 0.0)
			{
				CString sLog;
				sLog.Format("FAI-%d 보정 : Multiple=%.3f, Offset=%.3f", kk, dFAIMultiple[kk], dFAIOffset[kk]);
				THEAPP.SaveLog(sLog);
			}
		}
	}

	// ********** FAI-51 치수 측정 **********
	BOOL bIsMeasureFAI = TRUE;
	for (int ii = 0; ii < iInnerCircleMaxFitPonit; ii++)
	{
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo-1][iModuleNo - 1].m_dInnerChamferCircleX[51][ii] > 0);
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo-1][iModuleNo - 1].m_dInnerChamferCircleY[51][ii] > 0);
	}

	for (int ii = 0; ii < iOuterCircleMaxFitPonit; ii++)
	{
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleX[51][ii] > 0);
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleY[51][ii] > 0);
	}


	if (bIsMeasureFAI)
	{
		tuple_gen_const(0, 0, &HInnerEdgeConcatX);
		tuple_gen_const(0, 0, &HInnerEdgeConcatY);

		for (int iii = 0; iii < iInnerCircleMaxFitPonit; iii++)
		{
			dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[51][iii];
			dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[51][iii];

			if (dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				tuple_concat(HInnerEdgeConcatX, HTuple(dPoint1.x), &HInnerEdgeConcatX);
				tuple_concat(HInnerEdgeConcatY, HTuple(dPoint1.y), &HInnerEdgeConcatY);

				gen_cross_contour_xld(&HCrossXLD, dPoint1.y+1, dPoint1.x+1, 30.0, 0);
				concat_obj(m_HReviewXLD_FAI_Item[51][2], HCrossXLD, &(m_HReviewXLD_FAI_Item[51][2]));
			}
		}

		tuple_gen_const(0, 0, &HOuterEdgeConcatX);
		tuple_gen_const(0, 0, &HOuterEdgeConcatY);


		for (int iii = 0; iii < iOuterCircleMaxFitPonit; iii++)
		{
			dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleX[51][iii];
			dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleY[51][iii];

			if (dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				tuple_concat(HOuterEdgeConcatX, HTuple(dPoint1.x), &HOuterEdgeConcatX);
				tuple_concat(HOuterEdgeConcatY, HTuple(dPoint1.y), &HOuterEdgeConcatY);

				gen_cross_contour_xld(&HCrossXLD, dPoint1.y+1, dPoint1.x+1, 30.0, 0);
				concat_obj(m_HReviewXLD_FAI_Item[51][2], HCrossXLD, &(m_HReviewXLD_FAI_Item[51][2]));
			}
		}

		tuple_length(HInnerEdgeConcatX, &HNoCircleFitEdge); // 개수 측정
		iNoInnerEdge = HNoCircleFitEdge[0].L();

		tuple_length(HOuterEdgeConcatX, &HNoCircleFitEdge);
		iNoOuterEdge = HNoCircleFitEdge[0].L();

		if (iNoInnerEdge >= iInnerCircleMaxFitPonit && iNoOuterEdge >= iOuterCircleMaxFitPonit)
		{

			dInnerNormalRadius = 0;
			for (int iii = 0; iii < (iInnerCircleMaxFitPonit / 2); iii++)
			{
				dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[51][iii];
				dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[51][iii];

				dPoint2.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[51][iii + iInnerCircleMaxFitPonit / 2];
				dPoint2.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[51][iii + iInnerCircleMaxFitPonit / 2];

				distance_pp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance[0].D();
				dInnerNormalRadius += dDistance;
			}
			dInnerNormalRadius /= (iInnerCircleMaxFitPonit / 2);
			dInnerNormalRadius *= 0.5;

			dOuterNormalRadius = 0;
			for (int iii = 0; iii < (iOuterCircleMaxFitPonit / 2); iii++)
			{

				dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleX[51][iii];
				dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleY[51][iii];

				dPoint2.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleX[51][iii + iOuterCircleMaxFitPonit / 2];
				dPoint2.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterPrismCircleY[51][iii + iOuterCircleMaxFitPonit / 2];

				distance_pp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance[0].D();
				dOuterNormalRadius += dDistance;

			}
			dOuterNormalRadius /= (iOuterCircleMaxFitPonit / 2);
			dOuterNormalRadius *= 0.5;

			InspectCircleFitting(HInnerEdgeConcatX, HInnerEdgeConcatY, dInnerNormalRadius, dInnerCircleCenterX, dInnerCircleCenterY, dInnerCircleRadius);

			if (dInnerCircleCenterY > 0 && dInnerCircleCenterX > 0)
			{
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterX[51] = dInnerCircleCenterX;
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterY[51] = dInnerCircleCenterY;

				gen_circle_contour_xld(&HInnerCircleXLD, dInnerCircleCenterY+1, dInnerCircleCenterX+1, dInnerCircleRadius, 0, 6.28318, "positive", 1);
				gen_region_contour_xld(HInnerCircleXLD, &HFAIMeasureRgn, "filled");
				gen_cross_contour_xld(&HCrossXLD, dInnerCircleCenterY+1, dInnerCircleCenterX+1, 30.0, 0);

				concat_obj(m_HMeasureRgn_FAI_Item[51][0], HFAIMeasureRgn, &m_HMeasureRgn_FAI_Item[51][0]);
				concat_obj(m_HMeasureRgn_FAI_Item[52][0], HFAIMeasureRgn, &m_HMeasureRgn_FAI_Item[52][0]);
				concat_obj(m_HMeasureRgn_FAI_Item[53][0], HFAIMeasureRgn, &m_HMeasureRgn_FAI_Item[53][0]);

				concat_obj(m_HReviewXLD_FAI_Item[51][0], HInnerCircleXLD, &(m_HReviewXLD_FAI_Item[51][0]));
				concat_obj(m_HReviewXLD_FAI_Item[51][0], HCrossXLD, &(m_HReviewXLD_FAI_Item[51][0]));
				concat_obj(m_HReviewXLD_FAI_Item[51][2], HCrossXLD, &(m_HReviewXLD_FAI_Item[51][2]));
			}

			InspectCircleFitting(HOuterEdgeConcatX, HOuterEdgeConcatY, dOuterNormalRadius, dOuterCircleCenterX, dOuterCircleCenterY, dOuterCircleRadius);

			if (HOuterEdgeConcatY > 0 && HOuterEdgeConcatX > 0)
			{
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterCircleCenterX[51] = dOuterCircleCenterX;
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterCircleCenterY[51] = dOuterCircleCenterY;

				gen_circle_contour_xld(&HOuterCircleXLD, dOuterCircleCenterY+1, dOuterCircleCenterX+1, dOuterCircleRadius, 0, 6.28318, "positive", 1);
				gen_region_contour_xld(HOuterCircleXLD, &HFAIMeasureRgn, "filled");
				gen_cross_contour_xld(&HCrossXLD, dOuterCircleCenterY+1, dOuterCircleCenterX+1, 30.0, 0);

				concat_obj(m_HMeasureRgn_FAI_Item[51][0], HFAIMeasureRgn, &m_HMeasureRgn_FAI_Item[51][0]);
				concat_obj(m_HMeasureRgn_FAI_Item[52][0], HFAIMeasureRgn, &m_HMeasureRgn_FAI_Item[52][0]);
				concat_obj(m_HMeasureRgn_FAI_Item[53][0], HFAIMeasureRgn,&m_HMeasureRgn_FAI_Item[53][0]);

				concat_obj(m_HReviewXLD_FAI_Item[51][1], HOuterCircleXLD, &(m_HReviewXLD_FAI_Item[51][1]));
				concat_obj(m_HReviewXLD_FAI_Item[51][1], HCrossXLD, &(m_HReviewXLD_FAI_Item[51][1]));
				concat_obj(m_HReviewXLD_FAI_Item[51][2], HCrossXLD, &(m_HReviewXLD_FAI_Item[51][2]));
			}
		}

		// FAI-51
		dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterX[51];
		dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterY[51];

		dPoint2.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterCircleCenterX[51];
		dPoint2.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dOuterCircleCenterY[51];

		double dMulDistanceX = (dPoint1.x - dPoint2.x) * THEAPP.m_pCalDataService->GetPixelSize() * dFAIMultiple[52] + dFAIOffset[52];
		double dMulDistanceY = (dPoint1.y - dPoint2.y) * THEAPP.m_pCalDataService->GetPixelSize() * dFAIMultiple[53] + dFAIOffset[53]; 
		double dMulDistance = 2.0 * sqrt(dMulDistanceX*dMulDistanceX + dMulDistanceY*dMulDistanceY);

		
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[53][0] = dMulDistanceY;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[53][1] = (dPoint1.y - dPoint2.y) * THEAPP.m_pCalDataService->GetPixelSize();
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[52][0] = dMulDistanceX;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[52][1] = (dPoint1.x - dPoint2.x) * THEAPP.m_pCalDataService->GetPixelSize();
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[51][0] = dMulDistance * dFAIMultiple[51] + dFAIOffset[51];
		//THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[51][1] = dInnerNormalRadius;
		//THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[51][2] = dOuterNormalRadius;
		
	}
	////// FAI-51 끝

	// ********** FAI-26 치수 측정 **********
	bIsMeasureFAI = TRUE;
	for (int ii = 0; ii < iInnerCircleMaxFitPonit; ii++)
	{
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[26][ii] > 0);
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[26][ii] > 0);
	}
	for (int ii = 0; ii < iDatumCircleMaxFitPonit; ii++)
	{
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleX[26][ii] > 0);
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleY[26][ii] > 0);
	}
	bIsMeasureFAI *= THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[26][0]);

	if (bIsMeasureFAI)
	{
		tuple_gen_const(0, 0, &HInnerEdgeConcatX);
		tuple_gen_const(0, 0, &HInnerEdgeConcatY);
		for (int iii = 0; iii < iInnerCircleMaxFitPonit; iii++)
		{
			dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[26][iii];
			dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[26][iii];

			if (dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				tuple_concat(HInnerEdgeConcatX, HTuple(dPoint1.x), &HInnerEdgeConcatX);
				tuple_concat(HInnerEdgeConcatY, HTuple(dPoint1.y), &HInnerEdgeConcatY);

				gen_cross_contour_xld(&HCrossXLD, dPoint1.y + 1, dPoint1.x + 1, 30.0, 0);
				concat_obj(m_HReviewXLD_FAI_Item[26][0], HCrossXLD, &(m_HReviewXLD_FAI_Item[26][0]));
			}
		}

		tuple_gen_const(0, 0, &HDatumEdgeConcatX);
		tuple_gen_const(0, 0, &HDatumEdgeConcatY);
		for (int iii = 0; iii < iDatumCircleMaxFitPonit; iii++)
		{
			dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleX[26][iii];
			dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleY[26][iii];

			if (dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				tuple_concat(HDatumEdgeConcatX, HTuple(dPoint1.x), &HDatumEdgeConcatX);
				tuple_concat(HDatumEdgeConcatY, HTuple(dPoint1.y), &HDatumEdgeConcatY);

				gen_cross_contour_xld(&HCrossXLD, dPoint1.y + 1, dPoint1.x + 1, 30.0, 0);
				concat_obj(m_HReviewXLD_FAI_Item[26][0], HCrossXLD, &(m_HReviewXLD_FAI_Item[26][0]));
			}
		}

		tuple_length(HInnerEdgeConcatX, &HNoCircleFitEdge); // 개수 측정
		iNoInnerEdge = HNoCircleFitEdge[0].L();

		tuple_length(HDatumEdgeConcatX, &HNoCircleFitEdge);
		iNoDatumEdge = HNoCircleFitEdge[0].L();

		tuple_length(HDatumEllipseEdgeConcatX, &HNoCircleFitEdge);
		iNoDatumEllipseEdge = HNoCircleFitEdge[0].L();
		
		if (iNoInnerEdge >= iInnerCircleMaxFitPonit && iNoDatumEdge >= iDatumCircleMaxFitPonit)
		{

			dInnerNormalRadius = 0;
			for (int iii = 0; iii < (iInnerCircleMaxFitPonit / 2); iii++)
			{
				dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[26][iii];
				dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[26][iii];

				dPoint2.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[26][iii + iInnerCircleMaxFitPonit / 2];
				dPoint2.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[26][iii + iInnerCircleMaxFitPonit / 2];

				distance_pp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance[0].D();
				dInnerNormalRadius += dDistance;
			}
			dInnerNormalRadius /= (iInnerCircleMaxFitPonit / 2);
			dInnerNormalRadius *= 0.5;

			dDatumNormalRadius = 0;
			for (int iii = 0; iii < (iDatumCircleMaxFitPonit / 2); iii++)
			{

				dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleX[26][iii];
				dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleY[26][iii];

				dPoint2.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleX[26][iii + iDatumCircleMaxFitPonit / 2];
				dPoint2.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleY[26][iii + iDatumCircleMaxFitPonit / 2];

				distance_pp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance[0].D();
				dDatumNormalRadius += dDistance;

			}
			dDatumNormalRadius /= (iDatumCircleMaxFitPonit / 2);
			dDatumNormalRadius *= 0.5;

			InspectCircleFitting(HInnerEdgeConcatX, HInnerEdgeConcatY, dInnerNormalRadius, dInnerCircleCenterX, dInnerCircleCenterY, dInnerCircleRadius);

			if (dInnerCircleCenterY > 0 && dInnerCircleCenterX > 0)
			{
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterX[26] = dInnerCircleCenterX;
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterY[26] = dInnerCircleCenterY;

				gen_circle_contour_xld(&HInnerCircleXLD, dInnerCircleCenterY + 1, dInnerCircleCenterX + 1, dInnerCircleRadius, 0, 6.28318, "positive", 1);
				gen_region_contour_xld(HInnerCircleXLD, &HFAIMeasureRgn, "filled");
				gen_cross_contour_xld(&HCrossXLD, dInnerCircleCenterY + 1, dInnerCircleCenterX + 1, 30.0, 0);

				concat_obj(m_HReviewXLD_FAI_Item[26][1], HInnerCircleXLD, &(m_HReviewXLD_FAI_Item[26][1]));
				concat_obj(m_HReviewXLD_FAI_Item[26][0], HCrossXLD,	&(m_HReviewXLD_FAI_Item[26][0]));
				concat_obj(m_HReviewXLD_FAI_Item[26][1], HCrossXLD,	&(m_HReviewXLD_FAI_Item[26][1]));
			}

			InspectCircleFitting(HDatumEdgeConcatX, HDatumEdgeConcatY, dDatumNormalRadius, dDatumCircleCenterX, dDatumCircleCenterY, dDatumCircleRadius);

			if (dDatumCircleCenterX > 0 && dDatumCircleCenterY > 0)
			{
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleCenterX[26] = dDatumCircleCenterX;
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleCenterY[26] = dDatumCircleCenterY;

				gen_circle_contour_xld(&HDatumCircleXLD, dDatumCircleCenterY + 1, dDatumCircleCenterX + 1, dDatumCircleRadius, 0, 6.28318, "positive", 1);
				gen_region_contour_xld(HDatumCircleXLD, &HFAIMeasureRgn, "filled");
				gen_cross_contour_xld(&HCrossXLD, dDatumCircleCenterY + 1, dDatumCircleCenterX + 1, 30.0, 0);

				concat_obj(m_HReviewXLD_FAI_Item[26][2], HDatumCircleXLD, &(m_HReviewXLD_FAI_Item[26][2]));
				concat_obj(m_HReviewXLD_FAI_Item[26][0], HCrossXLD, &(m_HReviewXLD_FAI_Item[26][0]));
				concat_obj(m_HReviewXLD_FAI_Item[26][2], HCrossXLD, &(m_HReviewXLD_FAI_Item[26][2]));
			}

			dDatumEllipseCenterY = dDatumEllipseCenterX = 0.0;

			if (THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[26][0]) == TRUE)
			{
				copy_obj(m_HMeasureRgn_FAI_Item[26][0], &HFAIMeasureRgn, 1, 1);

				area_center(HFAIMeasureRgn, &HDatumEllipseArea, &HDatumEllipseCenterY, &HDatumEllipseCenterX);
				gen_contour_region_xld(HFAIMeasureRgn, &HDatumEllipseXLD, "border");
				dDatumEllipseCenterX = HDatumEllipseCenterX[0].D();
				dDatumEllipseCenterY = HDatumEllipseCenterY[0].D();
			
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumEllipseCenterX[26] = dDatumEllipseCenterX;
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumEllipseCenterY[26] = dDatumEllipseCenterY;
			
				gen_cross_contour_xld(&HCrossXLD, dDatumEllipseCenterY + 1, dDatumEllipseCenterX + 1, 30.0, 0);

				concat_obj(m_HReviewXLD_FAI_Item[26][3], HDatumEllipseXLD, &(m_HReviewXLD_FAI_Item[26][3]));
				concat_obj(m_HReviewXLD_FAI_Item[26][0], HCrossXLD,	&(m_HReviewXLD_FAI_Item[26][0]));
				concat_obj(m_HReviewXLD_FAI_Item[26][3], HCrossXLD,	&(m_HReviewXLD_FAI_Item[26][3]));
			}
		}

		// FAI-26
		InspectFAIConcent(26, -9.593, dDatumCircleCenterY, dDatumCircleCenterX, dDatumEllipseCenterY, dDatumEllipseCenterX, dInnerCircleCenterY, dInnerCircleCenterX, dShiftY, dShiftX);


		double dShiftX2, dShiftY2, dResult;
		dShiftX2 = dShiftX * dFAIMultiple[27] + dFAIOffset[27];
		dShiftY2 = dShiftY * dFAIMultiple[28] + dFAIOffset[28];
		dResult = 2 * sqrt(pow(dShiftX2 - 0.11, 2) + pow(dShiftY2 - 8.92, 2));

		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[26][0] = dResult * dFAIMultiple[26] + dFAIOffset[26];
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[27][0] = dShiftX2;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[27][1] = dShiftX;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[28][0] = dShiftY2;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[28][1] = dShiftY;

		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[26][2] = dInnerNormalRadius;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[27][2] = dInnerCircleCenterX;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[28][2] = dInnerCircleCenterY;
	}

	// ********** FAI-14 치수 측정 **********
	bIsMeasureFAI = TRUE;
	for (int ii = 0; ii < iInnerCircleMaxFitPonit; ii++)
	{
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[14][ii] > 0);
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[14][ii] > 0);
	}
	bIsMeasureFAI *= THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[14][0]);
	bIsMeasureFAI *= THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[14][1]);

	if (bIsMeasureFAI)
	{
		tuple_gen_const(0, 0, &HInnerEdgeConcatX);
		tuple_gen_const(0, 0, &HInnerEdgeConcatY);
		for (int iii = 0; iii < iInnerCircleMaxFitPonit; iii++)
		{
			dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[14][iii];
			dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[14][iii];

			if (dPoint1.x >= 0 && dPoint1.y >= 0)
			{
				tuple_concat(HInnerEdgeConcatX, HTuple(dPoint1.x), &HInnerEdgeConcatX);
				tuple_concat(HInnerEdgeConcatY, HTuple(dPoint1.y), &HInnerEdgeConcatY);

				gen_cross_contour_xld(&HCrossXLD, dPoint1.y + 1, dPoint1.x + 1, 30.0, 0);
				concat_obj(m_HReviewXLD_FAI_Item[14][0], HCrossXLD, &(m_HReviewXLD_FAI_Item[14][0]));
			}
		}

		tuple_length(HInnerEdgeConcatX, &HNoCircleFitEdge); // 개수 측정
		iNoInnerEdge = HNoCircleFitEdge[0].L();;

		if (iNoInnerEdge >= iInnerCircleMaxFitPonit)
		{

			dInnerNormalRadius = 0;
			for (int iii = 0; iii < (iInnerCircleMaxFitPonit / 2); iii++)
			{
				dPoint1.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[14][iii];
				dPoint1.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[14][iii];

				dPoint2.x = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleX[14][iii + iInnerCircleMaxFitPonit / 2];
				dPoint2.y = THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerChamferCircleY[14][iii + iInnerCircleMaxFitPonit / 2];

				distance_pp(dPoint1.y, dPoint1.x, dPoint2.y, dPoint2.x, &HDistance);
				dDistance = HDistance[0].D();
				dInnerNormalRadius += dDistance;
			}
			dInnerNormalRadius /= (iInnerCircleMaxFitPonit / 2);
			dInnerNormalRadius *= 0.5;

			InspectCircleFitting(HInnerEdgeConcatX, HInnerEdgeConcatY, dInnerNormalRadius, dInnerCircleCenterX, dInnerCircleCenterY, dInnerCircleRadius);

			if (dInnerCircleCenterY > 0 && dInnerCircleCenterX > 0)
			{
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterX[14] = dInnerCircleCenterX;
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dInnerCircleCenterY[14] = dInnerCircleCenterY;

				gen_circle_contour_xld(&HInnerCircleXLD, dInnerCircleCenterY + 1, dInnerCircleCenterX + 1, dInnerCircleRadius, 0, 6.28318, "positive", 1);
				gen_region_contour_xld(HInnerCircleXLD, &HFAIMeasureRgn, "filled");
				gen_cross_contour_xld(&HCrossXLD, dInnerCircleCenterY + 1, dInnerCircleCenterX + 1, 30.0, 0);

				concat_obj(m_HReviewXLD_FAI_Item[14][1], HInnerCircleXLD, &(m_HReviewXLD_FAI_Item[14][1]));
				concat_obj(m_HReviewXLD_FAI_Item[14][0], HCrossXLD, &(m_HReviewXLD_FAI_Item[14][0]));
				concat_obj(m_HReviewXLD_FAI_Item[14][1], HCrossXLD, &(m_HReviewXLD_FAI_Item[14][1]));
			}
		}

		if (THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[14][0]) == TRUE)
		{
			copy_obj(m_HMeasureRgn_FAI_Item[14][0], &HFAIMeasureRgn, 1, 1);

			area_center(HFAIMeasureRgn, &HDatumCircleArea, &HDatumCircleCenterY, &HDatumCircleCenterX);
			gen_contour_region_xld(HFAIMeasureRgn, &HDatumCircleXLD, "border");
			dDatumCircleCenterX = HDatumCircleCenterX[0].D();
			dDatumCircleCenterY = HDatumCircleCenterY[0].D();

			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleCenterX[14] = dDatumCircleCenterX;
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumCircleCenterY[14] = dDatumCircleCenterY;

			gen_cross_contour_xld(&HCrossXLD, dDatumCircleCenterY + 1, dDatumCircleCenterX + 1, 30.0, 0);

			concat_obj(m_HReviewXLD_FAI_Item[14][2], HDatumCircleXLD, &(m_HReviewXLD_FAI_Item[14][2]));
			concat_obj(m_HReviewXLD_FAI_Item[14][0], HCrossXLD, &(m_HReviewXLD_FAI_Item[14][0]));
			concat_obj(m_HReviewXLD_FAI_Item[14][2], HCrossXLD, &(m_HReviewXLD_FAI_Item[14][2]));
		}


		if (THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[14][1]) == TRUE)
		{
			copy_obj(m_HMeasureRgn_FAI_Item[14][1], &HFAIMeasureRgn, 1, 1);

			area_center(HFAIMeasureRgn, &HDatumEllipseArea, &HDatumEllipseCenterY, &HDatumEllipseCenterX);
			gen_contour_region_xld(HFAIMeasureRgn, &HDatumEllipseXLD, "border");
			dDatumEllipseCenterX = HDatumEllipseCenterX[0].D();
			dDatumEllipseCenterY = HDatumEllipseCenterY[0].D();

			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumEllipseCenterX[14] = dDatumEllipseCenterX;
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dDatumEllipseCenterY[14] = dDatumEllipseCenterY;

			gen_cross_contour_xld(&HCrossXLD, dDatumEllipseCenterY + 1, dDatumEllipseCenterX + 1, 30.0, 0);


			concat_obj(m_HReviewXLD_FAI_Item[14][3], HDatumEllipseXLD,
				&(m_HReviewXLD_FAI_Item[14][3]));
			concat_obj(m_HReviewXLD_FAI_Item[14][0], HCrossXLD,
				&(m_HReviewXLD_FAI_Item[14][0]));
			concat_obj(m_HReviewXLD_FAI_Item[14][3], HCrossXLD,
				&(m_HReviewXLD_FAI_Item[14][3]));
		}
		
		// FAI-14
		InspectFAIConcent(14, -68.636, dDatumCircleCenterY, dDatumCircleCenterX, dDatumEllipseCenterY, dDatumEllipseCenterX, dInnerCircleCenterY, dInnerCircleCenterX, dShiftY, dShiftX);

		double dShiftX2, dShiftY2, dResult;
		dShiftX2 = dShiftX * dFAIMultiple[15] + dFAIOffset[15];
		dShiftY2 = dShiftY * dFAIMultiple[16] + dFAIOffset[16];
		dResult = 2 * sqrt(pow(dShiftX2 - 12.014, 2) + pow(dShiftY2 - 2.109, 2));

		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[14][0] = dResult * dFAIMultiple[14] + dFAIOffset[14];
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[15][0] = dShiftX2;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[15][1] = dShiftX;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[16][0] = dShiftY2;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[16][1] = dShiftY;

		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[14][2] = dInnerNormalRadius;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[15][2] = dInnerCircleCenterX;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[16][2] = dInnerCircleCenterY;

	}

	// ********** FAI-Rotation 치수 측정 ********** 
	bIsMeasureFAI = TRUE;
	for (int ii = 0; ii < MAX_FAI_ROTATION_BRACKET_ROI; ii++)
	{
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dBracketLineStartX[123][ii] > 0);
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dBracketLineStartY[123][ii] > 0);
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dBracketLineEndX[123][ii] > 0);
		bIsMeasureFAI *= (THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dBracketLineEndY[123][ii] > 0);
	}

	double dBracketRoationDeg;

	if (bIsMeasureFAI == TRUE)
	{
		if (dDatumCircleCenterX < 0 || dDatumCircleCenterY < 0)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[14][0]) == TRUE)
			{
				copy_obj(m_HMeasureRgn_FAI_Item[14][0], &HFAIMeasureRgn, 1, 1);

				area_center(HFAIMeasureRgn, &HDatumCircleArea, &HDatumCircleCenterY, &HDatumCircleCenterX);
				gen_contour_region_xld(HFAIMeasureRgn, &HDatumCircleXLD, "border");
				dDatumCircleCenterX = HDatumCircleCenterX[0].D();
				dDatumCircleCenterY = HDatumCircleCenterY[0].D();
			}
		}

		if (dDatumEllipseCenterX < 0 || dDatumEllipseCenterY < 0)
		{
			if (THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[14][1]) == TRUE)
			{
				copy_obj(m_HMeasureRgn_FAI_Item[14][1], &HFAIMeasureRgn, 1, 1);

				area_center(HFAIMeasureRgn, &HDatumEllipseArea, &HDatumEllipseCenterY, &HDatumEllipseCenterX);
				gen_contour_region_xld(HFAIMeasureRgn, &HDatumEllipseXLD, "border");
				dDatumEllipseCenterX = HDatumEllipseCenterX[0].D();
				dDatumEllipseCenterY = HDatumEllipseCenterY[0].D();
			}
		}

		InspectFAIRotation(123, 21.364, dDatumCircleCenterY, dDatumCircleCenterX, dDatumEllipseCenterY, dDatumEllipseCenterX,
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dBracketLineStartX[123],
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dBracketLineStartY[123],
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dBracketLineEndX[123],
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dBracketLineEndY[123],
			&dBracketRoationDeg);


		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[123][0] = dBracketRoationDeg * dFAIMultiple[123] + dFAIOffset[123];;
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[123][1] = dBracketRoationDeg;
	}


	// ********** FAI-EA 치수 측정 **********
	HTuple lNoWeldingSpot;
	Hobject HRgnWeldingSpot, HAllSpotXld;
	if (THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[124][0]) == TRUE)
	{
		gen_contour_region_xld(m_HMeasureRgn_FAI_Item[124][0], &HAllSpotXld, "border");
		concat_obj(m_HReviewXLD_FAI_Item[124][0], HAllSpotXld, &m_HReviewXLD_FAI_Item[124][0]);

		connection(m_HMeasureRgn_FAI_Item[124][0], &HRgnWeldingSpot);
		count_obj(HRgnWeldingSpot, &lNoWeldingSpot);
		THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[124][0] = lNoWeldingSpot[0].D();
	}

	// ********** FAI-125-Dia 치수 측정 **********
	HTuple HSpotCenterY, HSpotCenterX, HSpotRadius, HSpotDiameter, HSpotMaxOffset, HSpotOffset;
	Hobject HSSpotXld;

	HSpotMaxOffset.Reset();
	for (int i = 0; i < 4; i++)
	{
		if (THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[125][i]) == TRUE)
		{
			gen_contour_region_xld(m_HMeasureRgn_FAI_Item[125][i], &HSSpotXld, "border");
			concat_obj(m_HReviewXLD_FAI_Item[125][0], HSSpotXld, &m_HReviewXLD_FAI_Item[125][0]);

			smallest_circle(m_HMeasureRgn_FAI_Item[125][i], &HSpotCenterY, &HSpotCenterX, &HSpotRadius);
			HSpotDiameter = (2 * HSpotRadius * THEAPP.m_pCalDataService->GetPixelSize()) * dFAIMultiple[125] + dFAIOffset[125];
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[125][1 + i] = HSpotDiameter[0].D();

			HSpotOffset = (HSpotDiameter - 0.5 * (THEAPP.m_pModelDataManager->m_dFAISpecMax[125] + THEAPP.m_pModelDataManager->m_dFAISpecMin[125])).Abs();
			if (HSpotMaxOffset < HSpotOffset)
			{
				HSpotMaxOffset = HSpotOffset;
				THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[125][0] = HSpotDiameter[0].D();

				copy_obj(HSSpotXld, &m_HReviewXLD_FAI_Item[125][1], 1, 1);
			}
		}
	}

	// ********** FAI-126-Pos 치수 측정 **********
	// FAI-126~130
	double dDistanceMin, dPointX_Welding_Spot, dPointY_Welding_Spot, dPointX_Welding_Pocket, dPointY_Welding_Pocket;
	Hobject HLineXld, HSpotXld, HPocketXld;
	gen_empty_obj(&HLineXld);

	for (int i = 0; i < 5; i++)
	{
		if ((THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[126+i][0]) == TRUE) && (THEAPP.m_pGFunction->ValidHRegion(m_HMeasureRgn_FAI_Item[126 + i][1]) == TRUE))
		{
			gen_contour_region_xld(m_HMeasureRgn_FAI_Item[126 + i][0], &HSpotXld, "border");
			gen_contour_region_xld(m_HMeasureRgn_FAI_Item[126 + i][1], &HPocketXld, "border");

			HLineXld = InspectFAIPosition(HSpotXld, HPocketXld, dPointX_Welding_Spot, dPointY_Welding_Spot, dPointX_Welding_Pocket, dPointY_Welding_Pocket, dDistanceMin);
			THEAPP.m_StructFaiMeasure[iTrayNo - 1][iModuleNo - 1].m_dFAIMeasureValue[126 + i][0] = dDistanceMin * THEAPP.m_pCalDataService->GetPixelSize() * dFAIMultiple[126 + i] + dFAIOffset[126 + i];

			concat_obj(m_HReviewXLD_FAI_Item[126+i][0], HLineXld, &m_HReviewXLD_FAI_Item[126+i][0]);	
		
		}
	}

}

///// FAI 동심도 측정  - LeeGW
BOOL Algorithm::InspectFAIConcent(int iFAINum, double dAxisAngle, HTuple D1CenterY, HTuple D1CenterX, HTuple D2CenterY, HTuple D2CenterX, HTuple LCenterY, HTuple LCenterX, double& dShiftY, double& dShiftX)
{
	try
	{
		dShiftY = dShiftX = 0.0;

		// Rad 값 변환 
		HTuple RotateAngleAxisY = dAxisAngle* HTuple(PI / 180);	// Y축
		HTuple RotateAngleAxisX = (dAxisAngle < 0 ? 90.0 + dAxisAngle : dAxisAngle - 90.0) * HTuple(PI / 180); // X축

		// 회전 행렬 생성
		HTuple HomMat2D, HomMat2D_AxisX, HomMat2D_AxisY;
		hom_mat2d_identity(&HomMat2D);
		hom_mat2d_rotate(HomMat2D, RotateAngleAxisY, D1CenterX, D1CenterY, &HomMat2D_AxisY);

		// Y축
		HTuple axisYEndY, axisYEndX;
		affine_trans_point_2d(HomMat2D_AxisY, D2CenterX, D2CenterY, &axisYEndX, &axisYEndY);

		// X축
		HTuple axisXEndY, axisXEndX;
		hom_mat2d_rotate(HomMat2D, RotateAngleAxisX, D1CenterX, D1CenterY, &HomMat2D_AxisX);
		affine_trans_point_2d(HomMat2D_AxisX, D2CenterX, D2CenterY, &axisXEndX, &axisXEndY);

		// 원의 중심 좌표
		HTuple LTransX, LTransY;
		distance_pl(LCenterY, LCenterX, D1CenterY, D1CenterX, axisYEndY, axisYEndX, &LTransX);
		distance_pl(LCenterY, LCenterX, D1CenterY, D1CenterX, axisXEndY, axisXEndX, &LTransY);
	
		dShiftX = LTransX[0].D() * THEAPP.m_pCalDataService->GetPixelSize();
		dShiftY = LTransY[0].D() * THEAPP.m_pCalDataService->GetPixelSize();

		Hobject Contour1, Contour2;
		gen_empty_obj(&Contour1);
		gen_empty_obj(&Contour2);

		gen_contour_polygon_xld(&Contour1, HTuple(axisYEndY[0].D()).Concat(HTuple(D1CenterY)), HTuple(axisYEndX[0].D()).Concat(HTuple(D1CenterX)));
		concat_obj(m_HReviewXLD_FAI_Item[iFAINum][0], Contour1, &m_HReviewXLD_FAI_Item[iFAINum][0]);

		gen_contour_polygon_xld(&Contour2, HTuple(axisXEndY[0].D()).Concat(HTuple(D1CenterY)), HTuple(axisXEndX[0].D()).Concat(HTuple(D1CenterX)));
		concat_obj(m_HReviewXLD_FAI_Item[iFAINum][0], Contour2, &m_HReviewXLD_FAI_Item[iFAINum][0]);

		return	TRUE;
	}
	catch (HException &except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::InspectFAIConcent] : %s", except.message); THEAPP.SaveLog(str);

		return FALSE;
}
}

BOOL Algorithm::InspectFAIRotation(int iFAINum, double dAxisAngle, HTuple D1CenterY, HTuple D1CenterX, HTuple D2CenterY, HTuple D2CenterX, double *pdLineStartX, double *pdLineStartY, double *pdLineEndX, double *pdLineEndY, double* pdRotationDegree)
{
	try
	{
		*pdRotationDegree = 0;

		// Rad 값 변환 
		HTuple RotateAngleAxisY = dAxisAngle* HTuple(PI / 180);	// Y축

		// 회전 행렬 생성
		HTuple HomMat2D, HomMat2D_AxisX, HomMat2D_AxisY;
		hom_mat2d_identity(&HomMat2D);
		hom_mat2d_rotate(HomMat2D, RotateAngleAxisY, D1CenterX, D1CenterY, &HomMat2D_AxisY);

		// Y축
		HTuple axisYEndY, axisYEndX;
		affine_trans_point_2d(HomMat2D_AxisY, D2CenterX, D2CenterY, &axisYEndX, &axisYEndY);

		// Bracket Lines ROI order (MSOP 기준)
		// L1(Upper Left)->L2(Upper Center)->L3(Upper Right)->L4(Lower Left)->L5(Lower Center)->L6(Lower Right)

		Hlong lParallel;
		double dUpperIntersectPoint1X, dUpperIntersectPoint1Y, dUpperIntersectPoint2X, dUpperIntersectPoint2Y;
		double dLowerIntersectPoint1X, dLowerIntersectPoint1Y, dLowerIntersectPoint2X, dLowerIntersectPoint2Y;

		intersection_ll(pdLineStartY[0], pdLineStartX[0], pdLineEndY[0], pdLineEndX[0], pdLineStartY[1], pdLineStartX[1], pdLineEndY[1], pdLineEndX[1], &dUpperIntersectPoint1Y, &dUpperIntersectPoint1X, &lParallel);
		intersection_ll(pdLineStartY[2], pdLineStartX[2], pdLineEndY[2], pdLineEndX[2], pdLineStartY[1], pdLineStartX[1], pdLineEndY[1], pdLineEndX[1], &dUpperIntersectPoint2Y, &dUpperIntersectPoint2X, &lParallel);

		intersection_ll(pdLineStartY[3], pdLineStartX[3], pdLineEndY[3], pdLineEndX[3], pdLineStartY[4], pdLineStartX[4], pdLineEndY[4], pdLineEndX[4], &dLowerIntersectPoint1Y, &dLowerIntersectPoint1X, &lParallel);
		intersection_ll(pdLineStartY[5], pdLineStartX[5], pdLineEndY[5], pdLineEndX[5], pdLineStartY[4], pdLineStartX[4], pdLineEndY[4], pdLineEndX[4], &dLowerIntersectPoint2Y, &dLowerIntersectPoint2X, &lParallel);

		double dUpperCenterPointX, dUpperCenterPointY;
		double dLowerCenterPointX, dLowerCenterPointY;

		dUpperCenterPointX = (dUpperIntersectPoint1X + dUpperIntersectPoint2X) * 0.5;
		dUpperCenterPointY = (dUpperIntersectPoint1Y + dUpperIntersectPoint2Y) * 0.5;
		dLowerCenterPointX = (dLowerIntersectPoint1X + dLowerIntersectPoint2X) * 0.5;
		dLowerCenterPointY = (dLowerIntersectPoint1Y + dLowerIntersectPoint2Y) * 0.5;

		HTuple HRotationAngleRad, HRotationAngleDeg;
		angle_ll(D1CenterY, D1CenterX, axisYEndY, axisYEndX, (HTuple)dUpperCenterPointY, (HTuple)dUpperCenterPointX, (HTuple)dLowerCenterPointY, (HTuple)dLowerCenterPointX, &HRotationAngleRad);

		tuple_deg(HRotationAngleRad, &HRotationAngleDeg);
		*pdRotationDegree = HRotationAngleDeg[0].D();

		Hobject Contour1, Contour2;
		gen_empty_obj(&Contour1);
		gen_empty_obj(&Contour2);

		// Datum
		gen_contour_polygon_xld(&Contour1, HTuple(axisYEndY[0].D()).Concat(HTuple(D1CenterY)), HTuple(axisYEndX[0].D()).Concat(HTuple(D1CenterX)));
		concat_obj(m_HReviewXLD_FAI_Item[iFAINum][0], Contour1, &m_HReviewXLD_FAI_Item[iFAINum][0]);

		// Bracket
		gen_contour_polygon_xld(&Contour2, HTuple(dUpperCenterPointY).Concat(HTuple(dLowerCenterPointY)), HTuple(dUpperCenterPointX).Concat(HTuple(dLowerCenterPointX)));
		concat_obj(m_HReviewXLD_FAI_Item[iFAINum][0], Contour2, &m_HReviewXLD_FAI_Item[iFAINum][0]);

		return TRUE;
	}
	catch (HException &except)
	{
		CString str; str.Format("Halcon Exception [Algorithm::InspectFAIRotation] : %s", except.message); THEAPP.SaveLog(str);

		return FALSE;
	}
}


Hobject Algorithm::InspectFAIPosition(Hobject Contour1, Hobject Contour2, double& dC1PointX, double& dC1PointY, double& dC2PointX, double& dC2PointY, double& dDistanceMin)
{

	// 초기화
	HTuple HDistanceMin, HMinIndex;
	HDistanceMin = 99999;
	HMinIndex = 0;

	// Contour1 포인트
	HTuple HPointX, HPointY;
	get_contour_xld(Contour1, &HPointY, &HPointX);

	// Contour1의 각 점에 대해 Contour2와의 최단거리 계산
	for (int i = 0; i < HPointX.Num(); i++)
	{
		HTuple HMin, HMax;
		distance_pc(Contour2, HPointY[i].D(), HPointX[i].D(), &HMin, &HMax);

		if (HMin < HDistanceMin)
		{
			HDistanceMin = HMin;
			HMinIndex = i;
		}
	}

	// Contour1 최단거리 좌표
	dC1PointX = HPointX[HMinIndex];
	dC1PointY = HPointY[HMinIndex];

	// 초기화
	HDistanceMin = 99999;
	HMinIndex = 0;

	// Contour2 포인트
	get_contour_xld(Contour2, &HPointY, &HPointX);

	// Contour2의 각 점에 대해 Contour1 최단거리 좌표와 최단거리 계산
	for (int i = 0; i < HPointX.Num(); i++)
	{
		HTuple HDistance;
		distance_pp(dC1PointY, dC1PointX, HPointY[i], HPointX[i], &HDistance);

		if (HDistance < HDistanceMin)
		{
			HDistanceMin = HDistance;
			HMinIndex = i;
		}
	}

	// Contour2 최단거리 좌표
	dC2PointX = HPointX[HMinIndex];
	dC2PointY = HPointY[HMinIndex];

	// 최단거리
	dDistanceMin = HDistanceMin[0].D();

	// 최단거리 XLD
	Hobject HXLDLine, HXLDCross1, HXLDCross2;
	gen_contour_polygon_xld(&HXLDLine, HTuple(dC1PointY).Concat(dC2PointY), HTuple(dC1PointX).Concat(dC2PointX));

	// START
	// 2025.03.24 - LeeGW
	gen_cross_contour_xld(&HXLDCross1, dC1PointY, dC1PointX, 30, 0);
	gen_cross_contour_xld(&HXLDCross2, dC2PointY, dC2PointX, 30, 0);
	concat_obj(HXLDLine, HXLDCross1, &HXLDLine);
	concat_obj(HXLDLine, HXLDCross2, &HXLDLine);
	// END

	return HXLDLine;

}