#pragma once

#include "FileBase.h"
#include "GTRegion.h"
#include "ThreadManager.hpp"

class CModelDataManager : public CFileBase
{
public:

	static CModelDataManager	*m_pInstance;
	static CModelDataManager* GetInstance(BOOL bShowFlag=FALSE);
	void DeleteInstance();
	CModelDataManager(void);
	~CModelDataManager(void);
	
public:
	CThreadManager thmUpdateRecipe;

	void InitModelData();

	std::vector<CString> m_vHeaders = {
        _T("Date"), _T("Time"), _T("Station"), _T("Machine_Code"),
        _T("Login_Mode"), _T("Recipe"), _T("SW_Version"), _T("LotNum"),
        _T("NG Area"), _T("Image Type"), _T("NG Defect Name"), _T("Image No"),
        _T("Inspection Tab No"), _T("Parameter"), _T("Before"), _T("After")
    };

	CString m_sModelName;
	int m_iModelVersion;

	int m_iTrayArrayX, m_iTrayArrayY;
	double m_dModulePitchX, m_dModulePitchY;
	int m_iTrayModuleMax;			// 트레이 용량
	int m_iTrayModuleAmt;			// 트레이 상의 시료 개수 = 트레이 용량 - 트레이 bottom right에서 비어있는 시료의 수
	int m_iTrayHESizeX, m_iTrayHESizeY;
	BOOL m_bOneBarrel;
	BOOL m_bBlackCoating;

	// Global Align
	BOOL m_bUseLocalAlignMatching[MAX_IMAGE_TAB];
	HTuple m_lLAlignModelID[MAX_IMAGE_TAB];
	int m_iLocalMatchingMethod[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingRectLTX[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingRectLTY[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingRectRBX[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingRectRBY[MAX_IMAGE_TAB];
	int m_iLocalMatchingPyramidLevel[MAX_IMAGE_TAB];
	double m_iLocalMatchingAngleRange[MAX_IMAGE_TAB];
	double m_dLocalMatchingScaleMin[MAX_IMAGE_TAB];
	double m_dLocalMatchingScaleMax[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingContrast[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingContrastLow[MAX_IMAGE_TAB];
	int m_iLocalMatchingTeachingContrastMinSize[MAX_IMAGE_TAB];
	double m_dLocalMatchingScore[MAX_IMAGE_TAB];
	int m_iLocalMatchingInspectionMinContrast[MAX_IMAGE_TAB];
	int m_iLocalMatchingSearchMarginX[MAX_IMAGE_TAB];
	int m_iLocalMatchingSearchMarginY[MAX_IMAGE_TAB];
	BOOL m_bUseMatchingAffineConstant[MAX_IMAGE_TAB];
	double m_dLocalTeachAlignRefX[MAX_IMAGE_TAB], m_dLocalTeachAlignRefY[MAX_IMAGE_TAB];
	BOOL m_bLocalAlignImage[MAX_IMAGE_TAB][MAX_IMAGE_TAB];

	// 24.02.29 Local Align 추가 - LeeGW Start
	// Local Align	
	int GetLastLocalAlignID(int iTeachingImageIdx);
	BOOL CheckLocalAlignID(int iTeachingImageIdx, int iCheckID);
	BOOL m_bLocalAlignROI[MAX_IMAGE_TAB];

	// FAI 추가 - LeeGW
	int GetLastFAIOuterCircleID(int iTeachingImageIdx);
	BOOL CheckFAIOuterCircleID(int iTeachingImageIdx, int iCheckID);
	BOOL m_bFAIOuterCircleROI[MAX_IMAGE_TAB];

	int GetLastFAIInnerCircleID(int iTeachingImageIdx);
	BOOL CheckFAIInnerCircleID(int iTeachingImageIdx, int iCheckID);
	BOOL m_bFAIInnerCircleROI[MAX_IMAGE_TAB];

	int GetLastFAIDatumCircleID(int iTeachingImageIdx);
	BOOL CheckFAIDatumCircleID(int iTeachingImageIdx, int iCheckID);
	BOOL m_bFAIDatumCircleROI[MAX_IMAGE_TAB];

	int GetLastFAIDatumEllipseID(int iTeachingImageIdx);
	BOOL CheckFAIDatumEllipseID(int iTeachingImageIdx, int iCheckID);
	BOOL m_bFAIDatumEllipseROI[MAX_IMAGE_TAB];

	int GetLastFAIWeldingSpotID(int iTeachingImageIdx);
	BOOL CheckFAIWeldingSpotID(int iTeachingImageIdx, int iCheckID);
	BOOL m_bFAIWeldingSpotROI[MAX_IMAGE_TAB];

	int GetLastFAIWeldingPocketID(int iTeachingImageIdx);
	BOOL CheckFAIWeldingPocketID(int iTeachingImageIdx, int iCheckID);
	BOOL m_bFAIWeldingPocketROI[MAX_IMAGE_TAB];

	int GetLastFAIBracketID(int iTeachingImageIdx);
	BOOL CheckFAIBracketID(int iTeachingImageIdx, int iCheckID);
	BOOL m_FAIBracketROI[MAX_IMAGE_TAB];

	//Matching Info
	int m_iRefContRow;
	int m_iRefContCol;
	int m_iRefContRad1;
	int m_iRefContRad2;
	int m_iRefContRad3;
	int m_iRefContRad4;
	double m_dContRatio23;			// 하단베럴 내곽라인 (contour2) 를 정하기 위한 contour3으로부터의 비례값
	double m_dContRatio45;			// 렌즈 외곽라인 (contour5) 를 정하기 위한 contour4로부터의 비례값
	int m_iBlackMatchingImageType;	// Black Barrel 매칭 영상 선택
	BOOL m_bUseMatchingAngle;
	int m_iMatchingAngleRange;
	BOOL m_bUseMatchingScale;
	int m_iMatchingScaleMax;
	int m_iMatchingScaleMin;
	double m_dOutmostContourCenterX, m_dOutmostContourCenterY;

	int m_iMatchingContrastC1;
	int m_iMatchingContrastC2;
	int m_iMatchingContrastC3;
	int m_iMatchingContrastC4;

	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
	HTuple m_iBLROIAlignMatchingModel[BARREL_LENS_IMAGE_TAB][MAX_CONTOUR_NUM];

	BOOL m_bUseBLROIAlign[BARREL_LENS_IMAGE_TAB];
	BOOL m_bUseBLROIAlignContour[BARREL_LENS_IMAGE_TAB][MAX_CONTOUR_NUM];

	int m_iBLROIAlignInnerMargin[BARREL_LENS_IMAGE_TAB];
	int m_iBLROIAlignOuterMargin[BARREL_LENS_IMAGE_TAB];
	double m_dBLROIAlignMatchingScore;
	int m_iBLROIAlignSearchingMargin;
	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW END

	int m_iBarcodeLTPointX;
	int m_iBarcodeLTPointY;
	int m_iBarcodeRBPointX;
	int m_iBarcodeRBPointY;

	std::vector<int> m_viModuleNo;
	std::vector<int> m_viPosInspNo;
	std::vector<std::vector<double> > m_mdInspectPosXY;
	std::vector<std::vector<double> > m_mdBarcodeScanPosXY;
	std::vector<int> m_viInspectModuleNo;
	double m_mdInspPosZ[MAX_IMAGE_TAB];

public:
	HTuple ModelID[MAX_MATCHING_MODEL_NUM];
	Hobject ContoursOri[MAX_CONTOUR_NUM];
	Hobject Contours[MAX_CONTOUR_NUM];
	HTuple ModelID1_Uniformity;
	Hobject ContoursAffinTrans1_Uniformity_Ori;

public:
	void UpdateParamChange();	//0531
	void SaveModelData();
	void SaveChangeParamHistory(BOOL bSave);
	// Save Current_Recipe.txt (RMS Parameter data) - 250905, jhkim
	void SaveRmsParamData();
	void LoadRmsParamData(CString sPath);
	//////////////////////////////////// Initialize Model Data
	void LoadModel();
	void LoadModelBaseData(); // 모션 위치 & 모듈 넘버 얻어옴
	void LoadModelData(); 
	void LoadModelContourData();
	//////////////////////////////////////////////////////////
	void ModelPreSet();
	void ModelPreSet(int iLineNo);
	void CompareParameter(CFile* file, SYSTEMTIME time, BOOL bSave);

	void InitLightInfo();
	int LoadLightInfo (CString sLinfoPath);
	void SaveLightInfo (CString sLinfoPath);

	void InitLightAverageValue();
	int LoadLightAverageValue (CString sLinfoPath);
	void SaveLightAverageValue (CString sLinfoPath);

	void SetControllerLightInfo();

	int GetLastPadID();

	Hobject GetInspectAreaRgn(int iGrabImageIdx, int iTabIdx);

	void UpdatePolygonROIBoundary();

public: 
	
	////  Register Info 
	CString m_strLogin_Mode;
	CString m_strOperator;
	CString m_strPwd;

	// Go (2016/06/02)
	CAlgorithmParam m_AlgorithmParam[MAX_IMAGE_TAB][MAX_TEACHING_TAB];

	CAlgorithmParam GetInspROIAlgorithmParam(int iGrabImageIdx, int iTabIdx);	// 24.03.18 Local Align - LeeGW

//	CAlgorithmParam m_ROIAlgorithmParam[MAX_IMAGE_TAB][MAX_ROI_TEACHING_TAB];
	//// Inspect parameter

	// FAI
	double m_dFAIMultiple[MAX_FAI_ITEM];
	double m_dFAIOffset[MAX_FAI_ITEM];
	double m_dFAISpecMin[MAX_FAI_ITEM];
	double m_dFAISpecMax[MAX_FAI_ITEM];
	BOOL m_bFAINGUse[MAX_FAI_ITEM];
	BOOL m_bFAISpecialNGUse[MAX_FAI_ITEM];
	BOOL m_bFAIUniqueOffsetUse[MAX_FAI_ITEM];
	double m_dFAIUniqueMultiple[MAX_FAI_ITEM][MAX_CIRCULAR_TABLE_POCKET];
	double m_dFAIUniqueOffset[MAX_FAI_ITEM][MAX_CIRCULAR_TABLE_POCKET];
	

	int m_iFAIInnerCircleFitType;
	int m_iFAIOuterCircleFitType;
	int m_iFAIDatumCircleFitType;

	int m_iFAISimulPocketNum;

	// Lens
	BOOL m_bLensDirtLensInspect;
	int m_iLensDirtLensFThresLow;
	int m_iLensDirtLensFThresHigh;
	int m_iLensDirtLensRoiOuterMargin;
	BOOL m_bLensDirtLensUseConnection;
	int m_iLensDirtLensConnectionLength;
	int m_iLensDirtLensDefectMinSize;

	BOOL m_bLensScratchLensInspect;
	int m_iLensScratchLensDThresMediF;
	int m_iLensScratchLensDThresMeanF;
	int m_iLensScratchLensDThresVal;
	int m_iLensScratchLensRoiOuterMargin;
	BOOL m_bLensScratchLensUseConnection;
	int m_iLensScratchLensConnectionLength;
	int m_iLensScratchLensDefectMinSize;
	int m_iLensScratchLensScratchMinLength;

	BOOL m_bLensStainLensInspect;
	BOOL m_bLensStainLensUseDThres;
	int m_iLensStainScaleFactor;
	int m_iLensStainLensDThresMediF;
	int m_iLensStainLensDThresMeanF;
	int m_iLensStainLensDThresVal;
	BOOL m_bLensStainLensUseFThres;
	int m_iLensStainLensFThresLow;
	int m_iLensStainLensFThresHigh;
	int m_iLensStainLensRoiOuterMargin;
	int m_iLensStainLensRoiEnforceErosion;
	int m_iLensStainLensRoiCastThres;
	int m_iLensStainLensRoiCastMargin;
	int m_iLensStainLensDefectMinSize;
	BOOL m_bLensStainUseLength;
	int m_iLensStainDefectMinLength;
	int m_iLensStainDefectMaxLength;
	BOOL m_bLensStainUseNoScale;
	int m_iLensStainNoScaleThres;

	///////////////////////////// Top Berrel Inner for Inspection Condition ///////////
	int m_iDecisonTopBarrelInner_RoiOuterMargin;
	int m_iDecisonTopBarrelInner_RoiInnerMargin;

	///////////////////////////// Barcode Shift Inspection ///////////

	BOOL m_bInspectBarcodeShift;
	double m_dBarcodeShiftRefX;
	double m_dBarcodeShiftRefY;
	double m_dBarcodeShiftToleranceX;
	double m_dBarcodeShiftToleranceY;

	///////////////////////////// //Barcode Rotation Inspection ///////////

	BOOL m_bInspectBarcodeRotation;
	double m_dBarcodeShiftToleranceRotation;
	double m_dBarcodeShiftRefRotation;

	//Barcode Hole Center Start
	BOOL m_bUseBarcodeShiftHoleCenter;
	int m_iBarcodeShiftHoleCenterGvMin;
	int m_iBarcodeShiftHoleCenterGvMax;
	int m_iBarcodeShiftHoleCenterGvClosing;
	int m_iBarcodeShiftHoleCenterDilation;
	//Barcode Hole Center End

	///////////////////////////// Black Coating Inspection Inspection ///////////

	BOOL m_bInspectBlackCoatingDiameter;
	double m_dBlackCoatingRef;
	double m_dBlackCoatingTolerance;
	double m_dBlackCoatingOffset;

	int m_iBlackCoatingImage;
	int m_iBlackCoatingContour;
	int m_iBlackCoatingOuterMargin;
	int m_iBlackCoatingInnerMargin;
	int m_iBlackCoatingMinGray;
	int m_iBlackCoatingMaxGray;

	int m_iDatumImage;
	int m_iDatumContour;
	int m_iDatumOuterMargin;
	int m_iDatumInnerMargin;
	int m_iDatumMinGray;
	int m_iDatumMaxGray;

	BOOL m_bUseImageScaling_BC;
	BOOL m_bUseImageScaling_DT;
	int m_iImageScaleMethod_BC;
	int m_iImageScaleMethod_DT;
	int m_iImageScaleMax_BC;
	int m_iImageScaleMax_DT;
	int m_iImageScaleMin_BC;
	int m_iImageScaleMin_DT;
	int m_iInspectionType;

	int m_iBlackCoatingWidth;
	int m_iBlackCoatingHeight;
	int m_iBlackCoatingDiffGray;
	int m_iBlackCoatingTransition;
	int m_iDatumWidth;
	int m_iDatumHeight;
	int m_iDatumDiffGray;
	int m_iDatumTransition;
	//////////////////////////////// Inspect Condition ////////////////////////////////////////////

	// New

	// Lens
	double m_dLensMinArea, m_dLensMidArea, m_dLensMaxArea;
	double m_dLensScratchMinArea, m_dLensScratchMidArea, m_dLensScratchMaxArea;
	double m_dLensDirtMinArea, m_dLensDirtMidArea, m_dLensDirtMaxArea;
	double m_dLensWhiteDotMinArea, m_dLensWhiteDotMidArea, m_dLensWhiteDotMaxArea; // 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW
	int m_iLensMinNumAccept, m_iLensMidNumAccept, m_iLensMaxNumAccept;
	int m_iLensScratchMinNumAccept, m_iLensScratchMidNumAccept, m_iLensScratchMaxNumAccept;
	int m_iLensDirtMinNumAccept, m_iLensDirtMidNumAccept, m_iLensDirtMaxNumAccept;
	int m_iLensWhiteDotMinNumAccept, m_iLensWhiteDotMidNumAccept, m_iLensWhiteDotMaxNumAccept;	 // 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW

	// Top Barrel
	double m_dDirtMinArea_Top, m_dDirtMidArea_Top, m_dDirtMaxArea_Top;
	double m_dScratchMinArea_Top, m_dScratchMidArea_Top, m_dScratchMaxArea_Top;
	double m_dStainMinArea_Top, m_dStainMidArea_Top, m_dStainMaxArea_Top;
	double m_dDentMinArea_Top, m_dDentMidArea_Top, m_dDentMaxArea_Top;
	double m_dChipMinArea_Top, m_dChipMidArea_Top, m_dChipMaxArea_Top;
	double m_dEpoxyMinArea_Top, m_dEpoxyMidArea_Top, m_dEpoxyMaxArea_Top;
	double m_dRes1MinArea_Top, m_dRes1MidArea_Top, m_dRes1MaxArea_Top;
	double m_dRes2MinArea_Top, m_dRes2MidArea_Top, m_dRes2MaxArea_Top;
	double m_dWhiteDotMinArea_Top, m_dWhiteDotMidArea_Top, m_dWhiteDotMaxArea_Top;	// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW
	int m_iDirtMinNumAccept_Top, m_iDirtMidNumAccept_Top, m_iDirtMaxNumAccept_Top;
	int m_iScratchMinNumAccept_Top, m_iScratchMidNumAccept_Top, m_iScratchMaxNumAccept_Top;
	int m_iStainMinNumAccept_Top, m_iStainMidNumAccept_Top, m_iStainMaxNumAccept_Top;
	int m_iDentMinNumAccept_Top, m_iDentMidNumAccept_Top, m_iDentMaxNumAccept_Top;
	int m_iChipMinNumAccept_Top, m_iChipMidNumAccept_Top, m_iChipMaxNumAccept_Top;
	int m_iEpoxyMinNumAccept_Top, m_iEpoxyMidNumAccept_Top, m_iEpoxyMaxNumAccept_Top;
	int m_iRes1MinNumAccept_Top, m_iRes1MidNumAccept_Top, m_iRes1MaxNumAccept_Top;
	int m_iRes2MinNumAccept_Top, m_iRes2MidNumAccept_Top, m_iRes2MaxNumAccept_Top;
	int m_iWhiteDotMinNumAccept_Top, m_iWhiteDotMidNumAccept_Top, m_iWhiteDotMaxNumAccept_Top;	// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW

	// Bottom Barrel
	double m_dDirtMinArea_Btm, m_dDirtMidArea_Btm, m_dDirtMaxArea_Btm;
	double m_dScratchMinArea_Btm, m_dScratchMidArea_Btm, m_dScratchMaxArea_Btm;
	double m_dStainMinArea_Btm, m_dStainMidArea_Btm, m_dStainMaxArea_Btm;
	double m_dDentMinArea_Btm, m_dDentMidArea_Btm, m_dDentMaxArea_Btm;
	double m_dChipMinArea_Btm, m_dChipMidArea_Btm, m_dChipMaxArea_Btm;
	double m_dEpoxyMinArea_Btm, m_dEpoxyMidArea_Btm, m_dEpoxyMaxArea_Btm;
	double m_dRes1MinArea_Btm, m_dRes1MidArea_Btm, m_dRes1MaxArea_Btm;
	double m_dRes2MinArea_Btm, m_dRes2MidArea_Btm, m_dRes2MaxArea_Btm;
	double m_dWhiteDotMinArea_Btm, m_dWhiteDotMidArea_Btm, m_dWhiteDotMaxArea_Btm;	// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW
	int m_iDirtMinNumAccept_Btm, m_iDirtMidNumAccept_Btm, m_iDirtMaxNumAccept_Btm;
	int m_iScratchMinNumAccept_Btm, m_iScratchMidNumAccept_Btm, m_iScratchMaxNumAccept_Btm;
	int m_iStainMinNumAccept_Btm, m_iStainMidNumAccept_Btm, m_iStainMaxNumAccept_Btm;
	int m_iDentMinNumAccept_Btm, m_iDentMidNumAccept_Btm, m_iDentMaxNumAccept_Btm;
	int m_iChipMinNumAccept_Btm, m_iChipMidNumAccept_Btm, m_iChipMaxNumAccept_Btm;
	int m_iEpoxyMinNumAccept_Btm, m_iEpoxyMidNumAccept_Btm, m_iEpoxyMaxNumAccept_Btm;
	int m_iRes1MinNumAccept_Btm, m_iRes1MidNumAccept_Btm, m_iRes1MaxNumAccept_Btm;
	int m_iRes2MinNumAccept_Btm, m_iRes2MidNumAccept_Btm, m_iRes2MaxNumAccept_Btm;
	int m_iWhiteDotMinNumAccept_Btm, m_iWhiteDotMidNumAccept_Btm, m_iWhiteDotMaxNumAccept_Btm;	// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW

	// Inner Barrel
	double m_dDirtMinArea_Inner, m_dDirtMidArea_Inner, m_dDirtMaxArea_Inner;
	double m_dScratchMinArea_Inner, m_dScratchMidArea_Inner, m_dScratchMaxArea_Inner;
	double m_dStainMinArea_Inner, m_dStainMidArea_Inner, m_dStainMaxArea_Inner;
	double m_dDentMinArea_Inner, m_dDentMidArea_Inner, m_dDentMaxArea_Inner;
	double m_dChipMinArea_Inner, m_dChipMidArea_Inner, m_dChipMaxArea_Inner;
	double m_dEpoxyMinArea_Inner, m_dEpoxyMidArea_Inner, m_dEpoxyMaxArea_Inner;
	double m_dRes1MinArea_Inner, m_dRes1MidArea_Inner, m_dRes1MaxArea_Inner;
	double m_dRes2MinArea_Inner, m_dRes2MidArea_Inner, m_dRes2MaxArea_Inner;
	double m_dWhiteDotMinArea_Inner, m_dWhiteDotMidArea_Inner, m_dWhiteDotMaxArea_Inner;	// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW
	int m_iDirtMinNumAccept_Inner, m_iDirtMidNumAccept_Inner, m_iDirtMaxNumAccept_Inner;
	int m_iScratchMinNumAccept_Inner, m_iScratchMidNumAccept_Inner, m_iScratchMaxNumAccept_Inner;
	int m_iStainMinNumAccept_Inner, m_iStainMidNumAccept_Inner, m_iStainMaxNumAccept_Inner;
	int m_iDentMinNumAccept_Inner, m_iDentMidNumAccept_Inner, m_iDentMaxNumAccept_Inner;
	int m_iChipMinNumAccept_Inner, m_iChipMidNumAccept_Inner, m_iChipMaxNumAccept_Inner;
	int m_iEpoxyMinNumAccept_Inner, m_iEpoxyMidNumAccept_Inner, m_iEpoxyMaxNumAccept_Inner;
	int m_iRes1MinNumAccept_Inner, m_iRes1MidNumAccept_Inner, m_iRes1MaxNumAccept_Inner;
	int m_iRes2MinNumAccept_Inner, m_iRes2MidNumAccept_Inner, m_iRes2MaxNumAccept_Inner;
	int m_iWhiteDotMinNumAccept_Inner, m_iWhiteDotMidNumAccept_Inner, m_iWhiteDotMaxNumAccept_Inner;	// 2024.09.30 - v2660 - WhiteDot 추가 - LEEGW
		
	// Inspection Light 값
	int m_iInspLightBright[MAX_IMAGE_TAB][MAX_LIGHT_CHANNEL];
	int m_iLightPageNo[MAX_IMAGE_TAB][MAX_LIGHT_PAGE];
	int m_iBarcodeLightBright;

	// Uniformity Light 값
	int Surface_UniformityLight1,Surface_UniformityLight2,Surface_UniformityLight3,Surface_UniformityLight4,Surface_UniformityLight5;
	int Edge_UniformityLight1,Edge_UniformityLight2,Edge_UniformityLight3,Edge_UniformityLight4,Edge_UniformityLight5;
	int Lens1_UniformityLight1,Lens1_UniformityLight2,Lens1_UniformityLight3,Lens1_UniformityLight4,Lens1_UniformityLight5;
	int Lens2_UniformityLight1,Lens2_UniformityLight2,Lens2_UniformityLight3,Lens2_UniformityLight4,Lens2_UniformityLight5;
	int Uniformity_Range;

	int m_iLightAverageValue[MAX_IMAGE_TAB][MAX_LIGHT_CHANNEL];
	int m_iLightAverageValueTotal[MAX_IMAGE_TAB];

	int m_iLightValueStart[MAX_IMAGE_TAB];
	int m_iLightValueEnd[MAX_IMAGE_TAB];
	int m_iLightValueInterval[MAX_IMAGE_TAB];
	int m_iLightValueInTol[MAX_IMAGE_TAB];
	int m_iTotalImageValueTol[MAX_IMAGE_TAB];

	//0531
	void INI_CompareNRecord_BOOL(BOOL bNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile* file);
	void INI_CompareNRecord(int iNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile* file);
	void INI_CompareNRecord(double dNow, CString sParam, CString sKey, CString sSection, CString sFixVal, CIniFileCS INI, CFile* file);

	void CompareNRecord(BOOL bPrev, BOOL bNow, CString sParam, CString sFixVal, CFile* file);
	void CompareNRecord(UINT iPrev, UINT iNow, CString sParam, CString sFixVal, CFile* file);
	void CompareNRecord(double dPrev, double dNow, CString sParam, CString sFixVal, CFile* file);

	void CopyFrom(const CModelDataManager& src);
public:
	GTRegion	*m_pInspectionArea;		// 1개 비전에 티칭된 모든 ROI 포함


protected:
	CFile	m_File;

};

