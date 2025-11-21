#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmParam.h"

CAlgorithmParam::CAlgorithmParam(void)
{
	Reset();
}

CAlgorithmParam::~CAlgorithmParam(void)
{
}

void CAlgorithmParam::Reset()
{
	m_bInspect = FALSE;
	m_iDefectNameIdx = DEFECT_NAME_DIRT;

	m_bSpecialNG = FALSE; // 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

	m_bUseInspectArea[0] = TRUE;
	m_iInspectAreaContour1Number[0] = MATCHING_CONTOUR_3;
	m_iInspectAreaContour1Margin[0] = 0;
	m_iInspectAreaContour2Number[0] = MATCHING_CONTOUR_3;
	m_iInspectAreaContour2Margin[0] = -9999;
	m_bUseInspectArea[1] = FALSE;
	m_iInspectAreaContour1Number[1] = MATCHING_CONTOUR_4;
	m_iInspectAreaContour1Margin[1] = 0;
	m_iInspectAreaContour2Number[1] = MATCHING_CONTOUR_4;
	m_iInspectAreaContour2Margin[1] = -9999;
	m_bUseInspectArea[2] = FALSE;
	m_iInspectAreaContour1Number[2] = MATCHING_CONTOUR_3;
	m_iInspectAreaContour1Margin[2] = 0;
	m_iInspectAreaContour2Number[2] = MATCHING_CONTOUR_4;
	m_iInspectAreaContour2Margin[2] = -9999;

	m_bUseCommonAlgorithm = FALSE;

	m_bUseBrightFixedThres = TRUE;
	m_iBrightLowerThres = 200;
	m_iBrightUpperThres = 255;
	m_bUseBrightDT = FALSE;
	m_bUseBrightLineNormalization = FALSE;
	m_iBrightMedianFilterSize = 1;
	m_iBrightClosingFilterSize = 0;
	m_iBrightDTFilterSize = 51;
	m_iBrightDTValue = 20;
	m_bUseBrightUniformityCheck = FALSE;
	m_iBrightUniformityOffset = 40;
	m_iBrightUniformityHystLength = -1;
	m_iBrightUniformityHystOffset = 10;
	m_bUseBrightHystThres = FALSE;
	m_iBrightHystSecureThres = 190;
	m_iBrightHystPotentialThres = 175;
	m_iBrightHystPotentialLength = 5;

	m_bUseDarkFixedThres = FALSE;
	m_iDarkLowerThres = 0;
	m_iDarkUpperThres = 55;
	m_bUseDarkDT = FALSE;
	m_bUseDarkLineNormalization = FALSE;
	m_iDarkMedianFilterSize = 1;
	m_iDarkClosingFilterSize = 0;
	m_iDarkDTFilterSize = 51;
	m_iDarkDTValue = 20;
	m_bUseDarkUniformityCheck = FALSE;
	m_iDarkUniformityOffset = 40;
	m_iDarkUniformityHystLength = -1;
	m_iDarkUniformityHystOffset = 10;
	m_bUseDarkHystThres = FALSE;
	m_iDarkHystSecureThres = 50;
	m_iDarkHystPotentialThres = 65;
	m_iDarkHystPotentialLength = 5;

	m_bCombineBrightDarkBlob = FALSE;
	m_iOpeningSize = 0;
	m_iClosingSize = 1;
	m_bFillup = FALSE;
	m_bUseConnection = FALSE;
	m_iConnectionMinSize = 5;
	m_iConnectionLength = 10;
	m_bUseAngle = FALSE;
	m_iConnectionMaxWidth = 6;

	m_bUseEdge = FALSE;
	m_dEdgeZoomRatio = 0.2;
	m_iEdgeStrength = 10;
	m_iEdgeConnectionDistance = 10;
	m_iEdgeMaxEndpointDistance = 10;
	m_iEdgeMinContourLength = 100;

	m_bUseDefectConditionArea = TRUE;
	m_bUseDefectConditionLength = FALSE;
	m_bUseDefectConditionWidth = FALSE;
	m_bUseDefectConditionMean = FALSE;
	m_bUseDefectConditionStdev = FALSE;
	m_bUseDefectConditionAnisometry = FALSE;
	m_bUseDefectConditionCircularity = FALSE;
	m_bUseDefectConditionConvexity = FALSE;
	m_bUseDefectConditionEllipseRatio = FALSE;
	m_bUseDefectConditionOuterDist = FALSE;
	m_bUseDefectConditionInnerDist = FALSE;
	m_bUseDefectConditionEpoxyDist = FALSE;

	m_iDefectConditionAreaMin = 100;
	m_iDefectConditionLengthMin = 10;
	m_iDefectConditionWidthMin = 2;
	m_iDefectConditionMeanMin = 60;
	m_dDefectConditionStdevMin = 1.0;
	m_dDefectConditionAnisometryMin = 1.0;
	m_dDefectConditionCircularityMin = 0.7;
	m_dDefectConditionConvexityMin = 0.8;
	m_dDefectConditionEllipseRatioMin = 1.0;

	m_iDefectConditionAreaMax = 100000;
	m_iDefectConditionLengthMax = 100000;
	m_iDefectConditionWidthMax = 100000;
	m_iDefectConditionMeanMax = 255;
	m_dDefectConditionStdevMax = 50.0;
	m_dDefectConditionAnisometryMax = 5.0;
	m_dDefectConditionCircularityMax = 1.0;
	m_dDefectConditionConvexityMax = 1.0;
	m_dDefectConditionEllipseRatioMax = 5.0;

	m_iDefectConditionOuterDist = 0;
	m_iDefectConditionInnerDist = 0;
	m_iDefectConditionEpoxyDist = 0;

	m_iEpoxySEThres = 25;
	m_iEpoxyESThres = 50;
	m_iEpoxyClosingSize = 5;
	m_iEpoxyOpeningSize = 2;
	m_iEpoxyNearDistance = 1;
	m_iEpoxyNearMinSize = 20;
	m_bEpoxyConditionHole = FALSE;
	m_bEpoxyConditionNear = FALSE;

	m_bUseImageScaling = FALSE;
	m_iImageScaleMethod = IMAGE_SCALE_AUTO;
	m_iImageScaleMin = 0;
	m_iImageScaleMax = 50;

	m_bDefectConditionXLength = FALSE;
	m_bDefectConditionYLength = FALSE;
	m_bDefectConditionContrast = FALSE;
	m_iDefectConditionXLengthMin = 10;
	m_iDefectConditionYLengthMin = 10;
	m_iDefectConditionContrastMin = 10;
	m_iDefectConditionXLengthMax = 100000;
	m_iDefectConditionYLengthMax = 100000;
	m_iDefectConditionContrastMax = 255;

	m_bUseFTConnected = FALSE;
	m_bUseFTConnectedArea = TRUE;
	m_bUseFTConnectedLength = FALSE;
	m_bUseFTConnectedWidth = FALSE;
	m_iFTConnectedAreaMin = 100;
	m_iFTConnectedLengthMin = 10;
	m_iFTConnectedWidthMin = 5;

	m_bUseLineFit = FALSE;
	m_iLineFitApplyPos = LINE_FIT_APPLY_POS_LEFT;
	m_iRoiOffsetTop = 0;
	m_iRoiOffsetBottom = 0;
	m_iRoiOffsetLeft = 0;
	m_iRoiOffsetRight = 0;

	m_iEdgeMeasureMultiPointNumber = 8;
	m_bEdgeMeasureEndPoint = FALSE;
	m_iEdgeMeasureStartLength = 100;
	m_iEdgeMeasureEndLength = 500;
	m_iEdgeMeasureDir = 0;
	m_iEdgeMeasureGv = 0;
	m_iEdgeMeasurePos = 0;
	m_dEdgeMeasureSmFactor = 1.0;
	m_iEdgeMeasureEdgeStr = 10;

	m_bUseApplyDontCare = FALSE;
	m_bUseDontCare = FALSE; // 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW

	m_iImageType = 0;

	// 240227 Local Align 추가 - LeeGW Start
	// Local Align Param
	//// Apply
	m_bUseLocalAlign = FALSE;
	m_iLocalAlignImageIndex = 0;
	m_iLocalAlignROINo = 0;
	m_iLocalAlignType = 0;
	m_iLocalAlignApplyMethod = 0;
	m_bLocalAlignPosX = FALSE;
	m_bLocalAlignPosY = FALSE;
	m_bLocalAlignPosAngle = FALSE;
	m_iLocalAlignFitPos = 0;

	// Aniso Align
	m_bUseROIAnisoAlign = FALSE;
	m_iROIAnisoAlignXSearchMargin = 30;
	m_iROIAnisoAlignYSearchMargin = 30;
	m_iROIAnisoAlignMatchingAngleRange = 0;
	m_dROIAnisoAlignMatchingScaleMinY = 0.8;
	m_dROIAnisoAlignMatchingScaleMaxY = 1.2;
	m_dROIAnisoAlignMatchingScaleMinX = 0.8;
	m_dROIAnisoAlignMatchingScaleMaxX = 1.2;
	m_iROIAnisoAlignMatchingMinContrast = 15;
	m_bROIAnisoAlignUsePosX = FALSE;
	m_bROIAnisoAlignUsePosY = FALSE;
	m_dROIAnisoAlignMatchingScore = 0.3;
	m_bROIAnisoAlignInspectShift = FALSE;
	m_dROIAnisoAlignShiftTop = 10;
	m_dROIAnisoAlignShiftBottom = 10;
	m_dROIAnisoAlignShiftLeft = 10;
	m_dROIAnisoAlignShiftRight = 10;
	m_bROIAnisoAlignLocalAlignUse = FALSE;
	m_bROIAnisoAlignModelExist = FALSE;

	//// Part Check
	m_bROIPartCheckUse = FALSE;
	m_bROIPartCheckLocalAlignUse = FALSE;
	m_iROIPartCheckMatchingAngleRange = 0;
	m_iROIPartCheckMatchingSearchMarginX = 100;
	m_iROIPartCheckMatchingSearchMarginY = 100;
	m_iROIPartCheckMatchingSearchMarginX2 = 100;
	m_iROIPartCheckMatchingSearchMarginY2 = 100;
	m_bROIPartCheckExistInspUse = FALSE;
	m_iROIPartCheckMatchingScore = 0;
	m_bROIPartCheckShiftInspUse = FALSE;
	m_iROIPartCheckShiftX = 0;
	m_iROIPartCheckShiftY = 0;
	m_dROIPartCheckRotationAngle = 0.0;

	//// Edge Measure
	m_bROIEdgeMeasureUse = FALSE;
	m_bROIEdgeMeasureLocalAlignUse = FALSE;
	m_iROIEdgeMeasureMultiPointNumber = 8;
	m_bROIEdgeMeasureEndPoint = FALSE;
	m_iROIEdgeMeasureStartLength = 100;
	m_iROIEdgeMeasureEndLength = 500;
	m_iROIEdgeMeasureDir = 0;
	m_iROIEdgeMeasureGv = 0;
	m_iROIEdgeMeasurePos = 0;
	m_dROIEdgeMeasureSmFactor = 1.0;
	m_iROIEdgeMeasureEdgeStr = 10;
	m_dROIEdgeMeasureRotateAngle = 0.0;

	// Image Filter
	m_bUseImageProcessFilter = FALSE;
	m_iImageProcessFilterType1 = FILTER_TYPE_NOT_USED;
	m_iImageProcessFilterType2 = FILTER_TYPE_NOT_USED;
	m_iImageProcessFilterType3 = FILTER_TYPE_NOT_USED;
	m_iImageProcessFilterType1X = 7;
	m_iImageProcessFilterType2X = 7;
	m_iImageProcessFilterType3X = 7;
	m_iImageProcessFilterType1Y = 7;
	m_iImageProcessFilterType2Y = 7;
	m_iImageProcessFilterType3Y = 7;

	// 240227 Local Align 추가 - LeeGW End
}

void CAlgorithmParam::Save(CIniFileCS INI, CString strSection, CString sCommonName)
{
	CString sParamName;

	sParamName.Format("%s_m_bInspect", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bInspect);
	sParamName.Format("%s_m_iDefectType", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectNameIdx);

	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW START
	sParamName.Format("%s_m_bSpecialNG", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bSpecialNG);
	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW END

	for (int i = 0; i < 3; i++)
	{
		sParamName.Format("%s_m_bUseInspectArea_%d", sCommonName, i + 1);
		INI.Set_Bool(strSection, sParamName, m_bUseInspectArea[i]);
		sParamName.Format("%s_m_iInspectAreaContour1Number_%d", sCommonName, i + 1);
		INI.Set_Integer(strSection, sParamName, m_iInspectAreaContour1Number[i]);
		sParamName.Format("%s_m_iInspectAreaContour1Margin_%d", sCommonName, i + 1);
		INI.Set_Integer(strSection, sParamName, m_iInspectAreaContour1Margin[i]);
		sParamName.Format("%s_m_iInspectAreaContour2Number_%d", sCommonName, i + 1);
		INI.Set_Integer(strSection, sParamName, m_iInspectAreaContour2Number[i]);
		sParamName.Format("%s_m_iInspectAreaContour2Margin_%d", sCommonName, i + 1);
		INI.Set_Integer(strSection, sParamName, m_iInspectAreaContour2Margin[i]);
	}

	sParamName.Format("%s_m_bUseBrightFixedThres", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseBrightFixedThres);
	sParamName.Format("%s_m_iBrightLowerThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightLowerThres);
	sParamName.Format("%s_m_iBrightUpperThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightUpperThres);

	sParamName.Format("%s_m_bUseBrightDT", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseBrightDT);
	sParamName.Format("%s_m_bUseBrightLineNormalization", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseBrightLineNormalization);
	sParamName.Format("%s_m_iBrightMedianFilterSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightMedianFilterSize);
	sParamName.Format("%s_m_iBrightClosingFilterSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightClosingFilterSize);
	sParamName.Format("%s_m_iBrightDTFilterSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightDTFilterSize);
	sParamName.Format("%s_m_iBrightDTValue", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightDTValue);

	sParamName.Format("%s_m_bUseBrightUniformityCheck", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseBrightUniformityCheck);
	sParamName.Format("%s_m_iBrightUniformityOffset", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightUniformityOffset);
	sParamName.Format("%s_m_iBrightUniformityHystLength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightUniformityHystLength);
	sParamName.Format("%s_m_iBrightUniformityHystOffset", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightUniformityHystOffset);

	sParamName.Format("%s_m_bUseBrightHystThres", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseBrightHystThres);
	sParamName.Format("%s_m_iBrightHystSecureThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightHystSecureThres);
	sParamName.Format("%s_m_iBrightHystPotentialThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightHystPotentialThres);
	sParamName.Format("%s_m_iBrightHystPotentialLength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iBrightHystPotentialLength);

	sParamName.Format("%s_m_bUseDarkFixedThres", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDarkFixedThres);
	sParamName.Format("%s_m_iDarkLowerThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkLowerThres);
	sParamName.Format("%s_m_iDarkUpperThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkUpperThres);

	sParamName.Format("%s_m_bUseDarkDT", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDarkDT);
	sParamName.Format("%s_m_bUseDarkLineNormalization", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDarkLineNormalization);
	sParamName.Format("%s_m_iDarkMedianFilterSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkMedianFilterSize);
	sParamName.Format("%s_m_iDarkClosingFilterSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkClosingFilterSize);
	sParamName.Format("%s_m_iDarkDTFilterSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkDTFilterSize);
	sParamName.Format("%s_m_iDarkDTValue", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkDTValue);

	sParamName.Format("%s_m_bUseDarkUniformityCheck", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDarkUniformityCheck);
	sParamName.Format("%s_m_iDarkUniformityOffset", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkUniformityOffset);
	sParamName.Format("%s_m_iDarkUniformityHystLength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkUniformityHystLength);
	sParamName.Format("%s_m_iDarkUniformityHystOffset", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkUniformityHystOffset);

	sParamName.Format("%s_m_bUseDarkHystThres", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDarkHystThres);
	sParamName.Format("%s_m_iDarkHystSecureThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkHystSecureThres);
	sParamName.Format("%s_m_iDarkHystPotentialThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkHystPotentialThres);
	sParamName.Format("%s_m_iDarkHystPotentialLength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDarkHystPotentialLength);

	sParamName.Format("%s_m_bCombineBrightDarkBlob", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bCombineBrightDarkBlob);
	sParamName.Format("%s_m_iOpeningSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iOpeningSize);
	sParamName.Format("%s_m_iClosingSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iClosingSize);
	sParamName.Format("%s_m_bFillup", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bFillup);
	sParamName.Format("%s_m_bUseConnection", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseConnection);
	sParamName.Format("%s_m_iConnectionMinSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iConnectionMinSize);
	sParamName.Format("%s_m_iConnectionLength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iConnectionLength);
	sParamName.Format("%s_m_bUseAngle", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseAngle);
	sParamName.Format("%s_m_iConnectionMaxWidth", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iConnectionMaxWidth);

	sParamName.Format("%s_m_bUseEdge", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseEdge);
	sParamName.Format("%s_m_dEdgeZoomRatio", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dEdgeZoomRatio);
	sParamName.Format("%s_m_iEdgeStrength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeStrength);
	sParamName.Format("%s_m_iEdgeConnectionDistance", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeConnectionDistance);
	sParamName.Format("%s_m_iEdgeMaxEndpointDistance", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeMaxEndpointDistance);
	sParamName.Format("%s_m_iEdgeMinContourLength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeMinContourLength);

	sParamName.Format("%s_m_bUseDefectConditionArea", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionArea);
	sParamName.Format("%s_m_bUseDefectConditionLength", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionLength);
	sParamName.Format("%s_m_bUseDefectConditionWidth", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionWidth);
	sParamName.Format("%s_m_bUseDefectConditionMean", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionMean);
	sParamName.Format("%s_m_bUseDefectConditionStdev", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionStdev);
	sParamName.Format("%s_m_bUseDefectConditionAnisometry", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionAnisometry);
	sParamName.Format("%s_m_bUseDefectConditionCircularity", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionCircularity);
	sParamName.Format("%s_m_bUseDefectConditionConvexity", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionConvexity);
	sParamName.Format("%s_m_bUseDefectConditionEllipseRatio", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionEllipseRatio);
	sParamName.Format("%s_m_bUseDefectConditionOuterDist", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionOuterDist);
	sParamName.Format("%s_m_bUseDefectConditionInnerDist", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionInnerDist);
	sParamName.Format("%s_m_bUseDefectConditionEpoxyDist", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDefectConditionEpoxyDist);

	sParamName.Format("%s_m_iDefectConditionArea", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionAreaMin);
	sParamName.Format("%s_m_iDefectConditionLength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionLengthMin);
	sParamName.Format("%s_m_iDefectConditionWidth", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionWidthMin);
	sParamName.Format("%s_m_iDefectConditionMean", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionMeanMin);
	sParamName.Format("%s_m_dDefectConditionStdevMin", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionStdevMin);
	sParamName.Format("%s_m_dDefectConditionAnisometryMin", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionAnisometryMin);
	sParamName.Format("%s_m_dDefectConditionCircularityMin", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionCircularityMin);
	sParamName.Format("%s_m_dDefectConditionConvexityMin", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionConvexityMin);
	sParamName.Format("%s_m_dDefectConditionEllipseRatioMin", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionEllipseRatioMin);

	sParamName.Format("%s_m_iDefectConditionAreaMax", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionAreaMax);
	sParamName.Format("%s_m_iDefectConditionLengthMax", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionLengthMax);
	sParamName.Format("%s_m_iDefectConditionWidthMax", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionWidthMax);
	sParamName.Format("%s_m_iDefectConditionMeanMax", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionMeanMax);
	sParamName.Format("%s_m_dDefectConditionStdev", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionStdevMax);
	sParamName.Format("%s_m_dDefectConditionAnisometry", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionAnisometryMax);
	sParamName.Format("%s_m_dDefectConditionCircularityMax", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionCircularityMax);
	sParamName.Format("%s_m_dDefectConditionConvexityMax", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionConvexityMax);
	sParamName.Format("%s_m_dDefectConditionEllipseRatioMax", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dDefectConditionEllipseRatioMax);

	sParamName.Format("%s_m_iDefectConditionOuterDist", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionOuterDist);
	sParamName.Format("%s_m_iDefectConditionInnerDist", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionInnerDist);
	sParamName.Format("%s_m_iDefectConditionEpoxyDist", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionEpoxyDist);

	sParamName.Format("%s_m_iEpoxySEThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEpoxySEThres);
	sParamName.Format("%s_m_iEpoxyESThres", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEpoxyESThres);
	sParamName.Format("%s_m_iEpoxyClosingSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEpoxyClosingSize);
	sParamName.Format("%s_m_iEpoxyOpeningSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEpoxyOpeningSize);
	sParamName.Format("%s_m_iEpoxyNearDistance", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEpoxyNearDistance);
	sParamName.Format("%s_m_iEpoxyNearMinSize", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEpoxyNearMinSize);
	sParamName.Format("%s_m_bEpoxyConditionHole", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bEpoxyConditionHole);
	sParamName.Format("%s_m_bEpoxyConditionNear", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bEpoxyConditionNear);

	sParamName.Format("%s_m_bUseImageScaling", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseImageScaling);
	sParamName.Format("%s_m_iImageScaleMethod", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iImageScaleMethod);
	sParamName.Format("%s_m_iImageScaleMin", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iImageScaleMin);
	sParamName.Format("%s_m_iImageScaleMax", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iImageScaleMax);

	sParamName.Format("%s_m_bDefectConditionXLength", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bDefectConditionXLength);
	sParamName.Format("%s_m_bDefectConditionYLength", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bDefectConditionYLength);
	sParamName.Format("%s_m_bDefectConditionContrast", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bDefectConditionContrast);
	sParamName.Format("%s_m_iDefectConditionXLengthMin", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionXLengthMin);
	sParamName.Format("%s_m_iDefectConditionYLengthMin", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionYLengthMin);
	sParamName.Format("%s_m_iDefectConditionContrastMin", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionContrastMin);
	sParamName.Format("%s_m_iDefectConditionXLengthMax", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionXLengthMax);
	sParamName.Format("%s_m_iDefectConditionYLengthMax", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionYLengthMax);
	sParamName.Format("%s_m_iDefectConditionContrastMax", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iDefectConditionContrastMax);

	sParamName.Format("%s_m_bUseFTConnected", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseFTConnected);
	sParamName.Format("%s_m_bUseFTConnectedArea", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseFTConnectedArea);
	sParamName.Format("%s_m_bUseFTConnectedLength", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseFTConnectedLength);
	sParamName.Format("%s_m_bUseFTConnectedWidth", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseFTConnectedWidth);
	sParamName.Format("%s_m_iFTConnectedAreaMin", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iFTConnectedAreaMin);
	sParamName.Format("%s_m_iFTConnectedLengthMin", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iFTConnectedLengthMin);
	sParamName.Format("%s_m_iFTConnectedWidthMin", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iFTConnectedWidthMin);

	sParamName.Format("%s_m_bUseLineFit", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseLineFit);
	sParamName.Format("%s_m_iLineFitApplyPos", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iLineFitApplyPos);
	sParamName.Format("%s_m_iRoiOffsetTop", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iRoiOffsetTop);
	sParamName.Format("%s_m_iRoiOffsetBottom", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iRoiOffsetBottom);
	sParamName.Format("%s_m_iRoiOffsetLeft", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iRoiOffsetLeft);
	sParamName.Format("%s_m_iRoiOffsetRight", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iRoiOffsetRight);
	sParamName.Format("%s_m_iEdgeMeasureMultiPointNumber", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeMeasureMultiPointNumber);
	sParamName.Format("%s_m_bEdgeMeasureEndPoint", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bEdgeMeasureEndPoint);
	sParamName.Format("%s_m_iEdgeMeasureStartLength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeMeasureStartLength);
	sParamName.Format("%s_m_iEdgeMeasureEndLength", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeMeasureEndLength);
	sParamName.Format("%s_m_iEdgeMeasureDir", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeMeasureDir);
	sParamName.Format("%s_m_iEdgeMeasureGv", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeMeasureGv);
	sParamName.Format("%s_m_iEdgeMeasurePos", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeMeasurePos);
	sParamName.Format("%s_m_dEdgeMeasureSmFactor", sCommonName);
	INI.Set_Double(strSection, sParamName, m_dEdgeMeasureSmFactor);
	sParamName.Format("%s_m_iEdgeMeasureEdgeStr", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iEdgeMeasureEdgeStr);

	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW
	sParamName.Format("%s_m_bUseApplyDontCare", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseApplyDontCare);
	sParamName.Format("%s_m_bUseDontCare", sCommonName);
	INI.Set_Bool(strSection, sParamName, m_bUseDontCare);
	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW

	// 24.07.08 - v2652 - 검사제외영역 영상 번호 파라미터 추가 - LeeGW
	sParamName.Format("%s_m_iImageType", sCommonName);
	INI.Set_Integer(strSection, sParamName, m_iImageType);
	// 24.07.08 - v2652 - 검사제외영역 영상 번호 파라미터 추가 - LeeGW
}

void CAlgorithmParam::Load(CIniFileCS INI, CString strSection, CString sCommonName)
{
	CString sParamName;

	sParamName.Format("%s_m_bInspect", sCommonName);
	m_bInspect = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iDefectType", sCommonName);
	m_iDefectNameIdx = INI.Get_Integer(strSection, sParamName, DEFECT_NAME_DIRT);

	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW START
	sParamName.Format("%s_m_bSpecialNG", sCommonName);
	m_bSpecialNG = INI.Get_Bool(strSection, sParamName, FALSE);
	// 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW END

	sParamName.Format("%s_m_bUseInspectArea_%d", sCommonName, 1);
	m_bUseInspectArea[0] = INI.Get_Bool(strSection, sParamName, TRUE);
	sParamName.Format("%s_m_bUseInspectArea_%d", sCommonName, 2);
	m_bUseInspectArea[1] = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseInspectArea_%d", sCommonName, 3);
	m_bUseInspectArea[2] = INI.Get_Bool(strSection, sParamName, FALSE);

	for (int i = 0; i < 3; i++)
	{
		sParamName.Format("%s_m_iInspectAreaContour1Number_%d", sCommonName, i + 1);
		m_iInspectAreaContour1Number[i] = INI.Get_Integer(strSection, sParamName, MATCHING_CONTOUR_4);
		sParamName.Format("%s_m_iInspectAreaContour1Margin_%d", sCommonName, i + 1);
		m_iInspectAreaContour1Margin[i] = INI.Get_Integer(strSection, sParamName, 2);
		sParamName.Format("%s_m_iInspectAreaContour2Number_%d", sCommonName, i + 1);
		m_iInspectAreaContour2Number[i] = INI.Get_Integer(strSection, sParamName, MATCHING_CONTOUR_3);
		sParamName.Format("%s_m_iInspectAreaContour2Margin_%d", sCommonName, i + 1);
		m_iInspectAreaContour2Margin[i] = INI.Get_Integer(strSection, sParamName, -2);
	}

	sParamName.Format("%s_m_bUseBrightFixedThres", sCommonName);
	m_bUseBrightFixedThres = INI.Get_Bool(strSection, sParamName, TRUE);
	sParamName.Format("%s_m_iBrightLowerThres", sCommonName);
	m_iBrightLowerThres = INI.Get_Integer(strSection, sParamName, 200);
	sParamName.Format("%s_m_iBrightUpperThres", sCommonName);
	m_iBrightUpperThres = INI.Get_Integer(strSection, sParamName, 255);

	sParamName.Format("%s_m_bUseBrightDT", sCommonName);
	m_bUseBrightDT = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseBrightLineNormalization", sCommonName);
	m_bUseBrightLineNormalization = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iBrightMedianFilterSize", sCommonName);
	m_iBrightMedianFilterSize = INI.Get_Integer(strSection, sParamName, 1);
	sParamName.Format("%s_m_iBrightClosingFilterSize", sCommonName);
	m_iBrightClosingFilterSize = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iBrightDTFilterSize", sCommonName);
	m_iBrightDTFilterSize = INI.Get_Integer(strSection, sParamName, 51);
	sParamName.Format("%s_m_iBrightDTValue", sCommonName);
	m_iBrightDTValue = INI.Get_Integer(strSection, sParamName, 20);

	sParamName.Format("%s_m_bUseBrightUniformityCheck", sCommonName);
	m_bUseBrightUniformityCheck = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iBrightUniformityOffset", sCommonName);
	m_iBrightUniformityOffset = INI.Get_Integer(strSection, sParamName, 40);
	sParamName.Format("%s_m_iBrightUniformityHystLength", sCommonName);
	m_iBrightUniformityHystLength = INI.Get_Integer(strSection, sParamName, -1);
	sParamName.Format("%s_m_iBrightUniformityHystOffset", sCommonName);
	m_iBrightUniformityHystOffset = INI.Get_Integer(strSection, sParamName, 10);

	sParamName.Format("%s_m_bUseBrightHystThres", sCommonName);
	m_bUseBrightHystThres = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iBrightHystSecureThres", sCommonName);
	m_iBrightHystSecureThres = INI.Get_Integer(strSection, sParamName, 190);
	sParamName.Format("%s_m_iBrightHystPotentialThres", sCommonName);
	m_iBrightHystPotentialThres = INI.Get_Integer(strSection, sParamName, 175);
	sParamName.Format("%s_m_iBrightHystPotentialLength", sCommonName);
	m_iBrightHystPotentialLength = INI.Get_Integer(strSection, sParamName, 5);

	sParamName.Format("%s_m_bUseDarkFixedThres", sCommonName);
	m_bUseDarkFixedThres = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iDarkLowerThres", sCommonName);
	m_iDarkLowerThres = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iDarkUpperThres", sCommonName);
	m_iDarkUpperThres = INI.Get_Integer(strSection, sParamName, 55);

	sParamName.Format("%s_m_bUseDarkDT", sCommonName);
	m_bUseDarkDT = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDarkLineNormalization", sCommonName);
	m_bUseDarkLineNormalization = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iDarkMedianFilterSize", sCommonName);
	m_iDarkMedianFilterSize = INI.Get_Integer(strSection, sParamName, 1);
	sParamName.Format("%s_m_iDarkClosingFilterSize", sCommonName);
	m_iDarkClosingFilterSize = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iDarkDTFilterSize", sCommonName);
	m_iDarkDTFilterSize = INI.Get_Integer(strSection, sParamName, 51);
	sParamName.Format("%s_m_iDarkDTValue", sCommonName);
	m_iDarkDTValue = INI.Get_Integer(strSection, sParamName, 20);

	sParamName.Format("%s_m_bUseDarkUniformityCheck", sCommonName);
	m_bUseDarkUniformityCheck = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iDarkUniformityOffset", sCommonName);
	m_iDarkUniformityOffset = INI.Get_Integer(strSection, sParamName, 40);
	sParamName.Format("%s_m_iDarkUniformityHystLength", sCommonName);
	m_iDarkUniformityHystLength = INI.Get_Integer(strSection, sParamName, -1);
	sParamName.Format("%s_m_iDarkUniformityHystOffset", sCommonName);
	m_iDarkUniformityHystOffset = INI.Get_Integer(strSection, sParamName, 10);

	sParamName.Format("%s_m_bUseDarkHystThres", sCommonName);
	m_bUseDarkHystThres = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iDarkHystSecureThres", sCommonName);
	m_iDarkHystSecureThres = INI.Get_Integer(strSection, sParamName, 50);
	sParamName.Format("%s_m_iDarkHystPotentialThres", sCommonName);
	m_iDarkHystPotentialThres = INI.Get_Integer(strSection, sParamName, 65);
	sParamName.Format("%s_m_iDarkHystPotentialLength", sCommonName);
	m_iDarkHystPotentialLength = INI.Get_Integer(strSection, sParamName, 5);

	sParamName.Format("%s_m_bCombineBrightDarkBlob", sCommonName);
	m_bCombineBrightDarkBlob = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iOpeningSize", sCommonName);
	m_iOpeningSize = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iClosingSize", sCommonName);
	m_iClosingSize = INI.Get_Integer(strSection, sParamName, 1);
	sParamName.Format("%s_m_bFillup", sCommonName);
	m_bFillup = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseConnection", sCommonName);
	m_bUseConnection = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iConnectionMinSize", sCommonName);
	m_iConnectionMinSize = INI.Get_Integer(strSection, sParamName, 5);
	sParamName.Format("%s_m_iConnectionLength", sCommonName);
	m_iConnectionLength = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_bUseAngle", sCommonName);
	m_bUseAngle = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iConnectionMaxWidth", sCommonName);
	m_iConnectionMaxWidth = INI.Get_Integer(strSection, sParamName, 6);

	sParamName.Format("%s_m_bUseEdge", sCommonName);
	m_bUseEdge = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_dEdgeZoomRatio", sCommonName);
	m_dEdgeZoomRatio = INI.Get_Double(strSection, sParamName, 0.2);
	sParamName.Format("%s_m_iEdgeStrength", sCommonName);
	m_iEdgeStrength = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_iEdgeConnectionDistance", sCommonName);
	m_iEdgeConnectionDistance = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_iEdgeMaxEndpointDistance", sCommonName);
	m_iEdgeMaxEndpointDistance = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_iEdgeMinContourLength", sCommonName);
	m_iEdgeMinContourLength = INI.Get_Integer(strSection, sParamName, 100);

	sParamName.Format("%s_m_bUseDefectConditionArea", sCommonName);
	m_bUseDefectConditionArea = INI.Get_Bool(strSection, sParamName, TRUE);
	sParamName.Format("%s_m_bUseDefectConditionLength", sCommonName);
	m_bUseDefectConditionLength = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionWidth", sCommonName);
	m_bUseDefectConditionWidth = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionMean", sCommonName);
	m_bUseDefectConditionMean = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionStdev", sCommonName);
	m_bUseDefectConditionStdev = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionAnisometry", sCommonName);
	m_bUseDefectConditionAnisometry = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionCircularity", sCommonName);
	m_bUseDefectConditionCircularity = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionConvexity", sCommonName);
	m_bUseDefectConditionConvexity = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionEllipseRatio", sCommonName);
	m_bUseDefectConditionEllipseRatio = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionOuterDist", sCommonName);
	m_bUseDefectConditionOuterDist = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionInnerDist", sCommonName);
	m_bUseDefectConditionInnerDist = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDefectConditionEpoxyDist", sCommonName);
	m_bUseDefectConditionEpoxyDist = INI.Get_Bool(strSection, sParamName, FALSE);

	sParamName.Format("%s_m_iDefectConditionArea", sCommonName);
	m_iDefectConditionAreaMin = INI.Get_Integer(strSection, sParamName, 100);
	sParamName.Format("%s_m_iDefectConditionLength", sCommonName);
	m_iDefectConditionLengthMin = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_iDefectConditionWidth", sCommonName);
	m_iDefectConditionWidthMin = INI.Get_Integer(strSection, sParamName, 2);
	sParamName.Format("%s_m_iDefectConditionMean", sCommonName);
	m_iDefectConditionMeanMin = INI.Get_Integer(strSection, sParamName, 60);
	sParamName.Format("%s_m_dDefectConditionStdevMin", sCommonName);
	m_dDefectConditionStdevMin = INI.Get_Double(strSection, sParamName, 1.0);
	sParamName.Format("%s_m_dDefectConditionAnisometryMin", sCommonName);
	m_dDefectConditionAnisometryMin = INI.Get_Double(strSection, sParamName, 1.0);
	sParamName.Format("%s_m_dDefectConditionCircularityMin", sCommonName);
	m_dDefectConditionCircularityMin = INI.Get_Double(strSection, sParamName, 0.7);
	sParamName.Format("%s_m_dDefectConditionConvexityMin", sCommonName);
	m_dDefectConditionConvexityMin = INI.Get_Double(strSection, sParamName, 0.8);
	sParamName.Format("%s_m_dDefectConditionEllipseRatioMin", sCommonName);
	m_dDefectConditionEllipseRatioMin = INI.Get_Double(strSection, sParamName, 1.0);

	sParamName.Format("%s_m_iDefectConditionAreaMax", sCommonName);
	m_iDefectConditionAreaMax = INI.Get_Integer(strSection, sParamName, 100000);
	sParamName.Format("%s_m_iDefectConditionLengthMax", sCommonName);
	m_iDefectConditionLengthMax = INI.Get_Integer(strSection, sParamName, 100000);
	sParamName.Format("%s_m_iDefectConditionWidthMax", sCommonName);
	m_iDefectConditionWidthMax = INI.Get_Integer(strSection, sParamName, 100000);
	sParamName.Format("%s_m_iDefectConditionMeanMax", sCommonName);
	m_iDefectConditionMeanMax = INI.Get_Integer(strSection, sParamName, 255);
	sParamName.Format("%s_m_dDefectConditionStdev", sCommonName);
	m_dDefectConditionStdevMax = INI.Get_Double(strSection, sParamName, 50.0);
	sParamName.Format("%s_m_dDefectConditionAnisometry", sCommonName);
	m_dDefectConditionAnisometryMax = INI.Get_Double(strSection, sParamName, 5.0);
	sParamName.Format("%s_m_dDefectConditionCircularityMax", sCommonName);
	m_dDefectConditionCircularityMax = INI.Get_Double(strSection, sParamName, 1.0);
	sParamName.Format("%s_m_dDefectConditionConvexityMax", sCommonName);
	m_dDefectConditionConvexityMax = INI.Get_Double(strSection, sParamName, 1.0);
	sParamName.Format("%s_m_dDefectConditionEllipseRatioMax", sCommonName);
	m_dDefectConditionEllipseRatioMax = INI.Get_Double(strSection, sParamName, 5.0);

	sParamName.Format("%s_m_iDefectConditionOuterDist", sCommonName);
	m_iDefectConditionOuterDist = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iDefectConditionInnerDist", sCommonName);
	m_iDefectConditionInnerDist = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iDefectConditionEpoxyDist", sCommonName);
	m_iDefectConditionEpoxyDist = INI.Get_Integer(strSection, sParamName, 0);

	sParamName.Format("%s_m_iEpoxySEThres", sCommonName);
	m_iEpoxySEThres = INI.Get_Integer(strSection, sParamName, 25);
	sParamName.Format("%s_m_iEpoxyESThres", sCommonName);
	m_iEpoxyESThres = INI.Get_Integer(strSection, sParamName, 50);
	sParamName.Format("%s_m_iEpoxyClosingSize", sCommonName);
	m_iEpoxyClosingSize = INI.Get_Integer(strSection, sParamName, 5);
	sParamName.Format("%s_m_iEpoxyOpeningSize", sCommonName);
	m_iEpoxyOpeningSize = INI.Get_Integer(strSection, sParamName, 2);
	sParamName.Format("%s_m_iEpoxyNearDistance", sCommonName);
	m_iEpoxyNearDistance = INI.Get_Integer(strSection, sParamName, 1);
	sParamName.Format("%s_m_iEpoxyNearMinSize", sCommonName);
	m_iEpoxyNearMinSize = INI.Get_Integer(strSection, sParamName, 20);
	sParamName.Format("%s_m_bEpoxyConditionHole", sCommonName);
	m_bEpoxyConditionHole = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bEpoxyConditionNear", sCommonName);
	m_bEpoxyConditionNear = INI.Get_Bool(strSection, sParamName, FALSE);

	sParamName.Format("%s_m_bUseImageScaling", sCommonName);
	m_bUseImageScaling = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iImageScaleMethod", sCommonName);
	m_iImageScaleMethod = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iImageScaleMin", sCommonName);
	m_iImageScaleMin = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iImageScaleMax", sCommonName);
	m_iImageScaleMax = INI.Get_Integer(strSection, sParamName, 50);

	sParamName.Format("%s_m_bDefectConditionXLength", sCommonName);
	m_bDefectConditionXLength = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bDefectConditionYLength", sCommonName);
	m_bDefectConditionYLength = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bDefectConditionContrast", sCommonName);
	m_bDefectConditionContrast = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iDefectConditionXLengthMin", sCommonName);
	m_iDefectConditionXLengthMin = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_iDefectConditionYLengthMin", sCommonName);
	m_iDefectConditionYLengthMin = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_iDefectConditionContrastMin", sCommonName);
	m_iDefectConditionContrastMin = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_iDefectConditionXLengthMax", sCommonName);
	m_iDefectConditionXLengthMax = INI.Get_Integer(strSection, sParamName, 100000);
	sParamName.Format("%s_m_iDefectConditionYLengthMax", sCommonName);
	m_iDefectConditionYLengthMax = INI.Get_Integer(strSection, sParamName, 100000);
	sParamName.Format("%s_m_iDefectConditionContrastMax", sCommonName);
	m_iDefectConditionContrastMax = INI.Get_Integer(strSection, sParamName, 255);

	sParamName.Format("%s_m_bUseFTConnected", sCommonName);
	m_bUseFTConnected = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseFTConnectedArea", sCommonName);
	m_bUseFTConnectedArea = INI.Get_Bool(strSection, sParamName, TRUE);
	sParamName.Format("%s_m_bUseFTConnectedLength", sCommonName);
	m_bUseFTConnectedLength = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseFTConnectedWidth", sCommonName);
	m_bUseFTConnectedWidth = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iFTConnectedAreaMin", sCommonName);
	m_iFTConnectedAreaMin = INI.Get_Integer(strSection, sParamName, 100);
	sParamName.Format("%s_m_iFTConnectedLengthMin", sCommonName);
	m_iFTConnectedLengthMin = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_iFTConnectedWidthMin", sCommonName);
	m_iFTConnectedWidthMin = INI.Get_Integer(strSection, sParamName, 5);

	sParamName.Format("%s_m_bUseLineFit", sCommonName);
	m_bUseLineFit = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iLineFitApplyPos", sCommonName);
	m_iLineFitApplyPos = INI.Get_Integer(strSection, sParamName, LINE_FIT_APPLY_POS_LEFT);
	sParamName.Format("%s_m_iRoiOffsetTop", sCommonName);
	m_iRoiOffsetTop = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iRoiOffsetBottom", sCommonName);
	m_iRoiOffsetBottom = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iRoiOffsetLeft", sCommonName);
	m_iRoiOffsetLeft = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iRoiOffsetRight", sCommonName);
	m_iRoiOffsetRight = INI.Get_Integer(strSection, sParamName, 0);

	sParamName.Format("%s_m_iEdgeMeasureMultiPointNumber", sCommonName);
	m_iEdgeMeasureMultiPointNumber = INI.Get_Integer(strSection, sParamName, 10);
	sParamName.Format("%s_m_bEdgeMeasureEndPoint", sCommonName);
	m_bEdgeMeasureEndPoint = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_iEdgeMeasureStartLength", sCommonName);
	m_iEdgeMeasureStartLength = INI.Get_Integer(strSection, sParamName, 100);
	sParamName.Format("%s_m_iEdgeMeasureEndLength", sCommonName);
	m_iEdgeMeasureEndLength = INI.Get_Integer(strSection, sParamName, 500);
	sParamName.Format("%s_m_iEdgeMeasureDir", sCommonName);
	m_iEdgeMeasureDir = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iEdgeMeasureGv", sCommonName);
	m_iEdgeMeasureGv = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_iEdgeMeasurePos", sCommonName);
	m_iEdgeMeasurePos = INI.Get_Integer(strSection, sParamName, 0);
	sParamName.Format("%s_m_dEdgeMeasureSmFactor", sCommonName);
	m_dEdgeMeasureSmFactor = INI.Get_Double(strSection, sParamName, 1.0);
	sParamName.Format("%s_m_iEdgeMeasureEdgeStr", sCommonName);
	m_iEdgeMeasureEdgeStr = INI.Get_Integer(strSection, sParamName, 10);

	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW
	sParamName.Format("%s_m_bUseApplyDontCare", sCommonName);
	m_bUseApplyDontCare = INI.Get_Bool(strSection, sParamName, FALSE);
	sParamName.Format("%s_m_bUseDontCare", sCommonName);
	m_bUseDontCare = INI.Get_Bool(strSection, sParamName, FALSE);
	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW

	// 24.07.08 - v2652 - 검사제외영역 영상 번호 파라미터 추가 - LeeGW
	sParamName.Format("%s_m_iImageType", sCommonName);
	m_iImageType = INI.Get_Integer(strSection, sParamName, 0);
	// 24.07.08 - v2652 - 검사제외영역 영상 번호 파라미터 추가 - LeeGW

	// 24.03.18 Local Align Endt - LeeGW
}

CAlgorithmParam &CAlgorithmParam::operator=(CAlgorithmParam &data)
{
	if (this == &data)
		return *this;

	m_bInspect = data.m_bInspect;
	m_iDefectNameIdx = data.m_iDefectNameIdx;

	m_bSpecialNG = data.m_bSpecialNG; // 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

	for (int i = 0; i < 3; i++)
	{
		m_bUseInspectArea[i] = data.m_bUseInspectArea[i];
		m_iInspectAreaContour1Number[i] = data.m_iInspectAreaContour1Number[i];
		m_iInspectAreaContour1Margin[i] = data.m_iInspectAreaContour1Margin[i];
		m_iInspectAreaContour2Number[i] = data.m_iInspectAreaContour2Number[i];
		m_iInspectAreaContour2Margin[i] = data.m_iInspectAreaContour2Margin[i];
	}

	m_bUseCommonAlgorithm = data.m_bUseCommonAlgorithm;

	m_bUseBrightFixedThres = data.m_bUseBrightFixedThres;
	m_iBrightLowerThres = data.m_iBrightLowerThres;
	m_iBrightUpperThres = data.m_iBrightUpperThres;
	m_bUseBrightDT = data.m_bUseBrightDT;
	m_bUseBrightLineNormalization = data.m_bUseBrightLineNormalization;
	m_iBrightMedianFilterSize = data.m_iBrightMedianFilterSize;
	m_iBrightClosingFilterSize = data.m_iBrightClosingFilterSize;
	m_iBrightDTFilterSize = data.m_iBrightDTFilterSize;
	m_iBrightDTValue = data.m_iBrightDTValue;
	m_bUseBrightUniformityCheck = data.m_bUseBrightUniformityCheck;
	m_iBrightUniformityOffset = data.m_iBrightUniformityOffset;
	m_iBrightUniformityHystLength = data.m_iBrightUniformityHystLength;
	m_iBrightUniformityHystOffset = data.m_iBrightUniformityHystOffset;
	m_bUseBrightHystThres = data.m_bUseBrightHystThres;
	m_iBrightHystSecureThres = data.m_iBrightHystSecureThres;
	m_iBrightHystPotentialThres = data.m_iBrightHystPotentialThres;
	m_iBrightHystPotentialLength = data.m_iBrightHystPotentialLength;

	m_bUseDarkFixedThres = data.m_bUseDarkFixedThres;
	m_iDarkLowerThres = data.m_iDarkLowerThres;
	m_iDarkUpperThres = data.m_iDarkUpperThres;
	m_bUseDarkDT = data.m_bUseDarkDT;
	m_bUseDarkLineNormalization = data.m_bUseDarkLineNormalization;
	m_iDarkMedianFilterSize = data.m_iDarkMedianFilterSize;
	m_iDarkClosingFilterSize = data.m_iDarkClosingFilterSize;
	m_iDarkDTFilterSize = data.m_iDarkDTFilterSize;
	m_iDarkDTValue = data.m_iDarkDTValue;
	m_bUseDarkUniformityCheck = data.m_bUseDarkUniformityCheck;
	m_iDarkUniformityOffset = data.m_iDarkUniformityOffset;
	m_iDarkUniformityHystLength = data.m_iDarkUniformityHystLength;
	m_iDarkUniformityHystOffset = data.m_iDarkUniformityHystOffset;
	m_bUseDarkHystThres = data.m_bUseDarkHystThres;
	m_iDarkHystSecureThres = data.m_iDarkHystSecureThres;
	m_iDarkHystPotentialThres = data.m_iDarkHystPotentialThres;
	m_iDarkHystPotentialLength = data.m_iDarkHystPotentialLength;

	m_bCombineBrightDarkBlob = data.m_bCombineBrightDarkBlob;
	m_iOpeningSize = data.m_iOpeningSize;
	m_iClosingSize = data.m_iClosingSize;
	m_bFillup = data.m_bFillup;
	m_bUseConnection = data.m_bUseConnection;
	m_iConnectionMinSize = data.m_iConnectionMinSize;
	m_iConnectionLength = data.m_iConnectionLength;
	m_bUseAngle = data.m_bUseAngle;
	m_iConnectionMaxWidth = data.m_iConnectionMaxWidth;

	m_bUseEdge = data.m_bUseEdge;
	m_dEdgeZoomRatio = data.m_dEdgeZoomRatio;
	m_iEdgeStrength = data.m_iEdgeStrength;
	m_iEdgeConnectionDistance = data.m_iEdgeConnectionDistance;
	m_iEdgeMaxEndpointDistance = data.m_iEdgeMaxEndpointDistance;
	m_iEdgeMinContourLength = data.m_iEdgeMinContourLength;

	m_bUseDefectConditionArea = data.m_bUseDefectConditionArea;
	m_bUseDefectConditionLength = data.m_bUseDefectConditionLength;
	m_bUseDefectConditionWidth = data.m_bUseDefectConditionWidth;
	m_bUseDefectConditionMean = data.m_bUseDefectConditionMean;
	m_bUseDefectConditionStdev = data.m_bUseDefectConditionStdev;
	m_bUseDefectConditionAnisometry = data.m_bUseDefectConditionAnisometry;
	m_bUseDefectConditionCircularity = data.m_bUseDefectConditionCircularity;
	m_bUseDefectConditionConvexity = data.m_bUseDefectConditionConvexity;
	m_bUseDefectConditionEllipseRatio = data.m_bUseDefectConditionEllipseRatio;
	m_bUseDefectConditionOuterDist = data.m_bUseDefectConditionOuterDist;
	m_bUseDefectConditionInnerDist = data.m_bUseDefectConditionInnerDist;
	m_bUseDefectConditionEpoxyDist = data.m_bUseDefectConditionEpoxyDist;

	m_iDefectConditionAreaMin = data.m_iDefectConditionAreaMin;
	m_iDefectConditionLengthMin = data.m_iDefectConditionLengthMin;
	m_iDefectConditionWidthMin = data.m_iDefectConditionWidthMin;
	m_iDefectConditionMeanMin = data.m_iDefectConditionMeanMin;
	m_dDefectConditionStdevMin = data.m_dDefectConditionStdevMin;
	m_dDefectConditionAnisometryMin = data.m_dDefectConditionAnisometryMin;
	m_dDefectConditionCircularityMin = data.m_dDefectConditionCircularityMin;
	m_dDefectConditionConvexityMin = data.m_dDefectConditionConvexityMin;
	m_dDefectConditionEllipseRatioMin = data.m_dDefectConditionEllipseRatioMin;

	m_iDefectConditionAreaMax = data.m_iDefectConditionAreaMax;
	m_iDefectConditionLengthMax = data.m_iDefectConditionLengthMax;
	m_iDefectConditionWidthMax = data.m_iDefectConditionWidthMax;
	m_iDefectConditionMeanMax = data.m_iDefectConditionMeanMax;
	m_dDefectConditionStdevMax = data.m_dDefectConditionStdevMax;
	m_dDefectConditionAnisometryMax = data.m_dDefectConditionAnisometryMax;
	m_dDefectConditionCircularityMax = data.m_dDefectConditionCircularityMax;
	m_dDefectConditionConvexityMax = data.m_dDefectConditionConvexityMax;
	m_dDefectConditionEllipseRatioMax = data.m_dDefectConditionEllipseRatioMax;

	m_iDefectConditionOuterDist = data.m_iDefectConditionOuterDist;
	m_iDefectConditionInnerDist = data.m_iDefectConditionInnerDist;
	m_iDefectConditionEpoxyDist = data.m_iDefectConditionEpoxyDist;

	m_iEpoxySEThres = data.m_iEpoxySEThres;
	m_iEpoxyESThres = data.m_iEpoxyESThres;
	m_iEpoxyClosingSize = data.m_iEpoxyClosingSize;
	m_iEpoxyOpeningSize = data.m_iEpoxyOpeningSize;
	m_iEpoxyNearDistance = data.m_iEpoxyNearDistance;
	m_iEpoxyNearMinSize = data.m_iEpoxyNearMinSize;
	m_bEpoxyConditionHole = data.m_bEpoxyConditionHole;
	m_bEpoxyConditionNear = data.m_bEpoxyConditionNear;

	m_bUseImageScaling = data.m_bUseImageScaling;
	m_iImageScaleMethod = data.m_iImageScaleMethod;
	m_iImageScaleMin = data.m_iImageScaleMin;
	m_iImageScaleMax = data.m_iImageScaleMax;

	m_bDefectConditionXLength = data.m_bDefectConditionXLength;
	m_bDefectConditionYLength = data.m_bDefectConditionYLength;
	m_bDefectConditionContrast = data.m_bDefectConditionContrast;
	m_iDefectConditionXLengthMin = data.m_iDefectConditionXLengthMin;
	m_iDefectConditionYLengthMin = data.m_iDefectConditionYLengthMin;
	m_iDefectConditionContrastMin = data.m_iDefectConditionContrastMin;
	m_iDefectConditionXLengthMax = data.m_iDefectConditionXLengthMax;
	m_iDefectConditionYLengthMax = data.m_iDefectConditionYLengthMax;
	m_iDefectConditionContrastMax = data.m_iDefectConditionContrastMax;

	m_bUseFTConnected = data.m_bUseFTConnected;
	m_bUseFTConnectedArea = data.m_bUseFTConnectedArea;
	m_bUseFTConnectedLength = data.m_bUseFTConnectedLength;
	m_bUseFTConnectedWidth = data.m_bUseFTConnectedWidth;
	m_iFTConnectedAreaMin = data.m_iFTConnectedAreaMin;
	m_iFTConnectedLengthMin = data.m_iFTConnectedLengthMin;
	m_iFTConnectedWidthMin = data.m_iFTConnectedWidthMin;

	m_bUseLineFit = data.m_bUseLineFit;
	m_iLineFitApplyPos = data.m_iLineFitApplyPos;
	m_iRoiOffsetTop = data.m_iRoiOffsetTop;
	m_iRoiOffsetBottom = data.m_iRoiOffsetBottom;
	m_iRoiOffsetLeft = data.m_iRoiOffsetLeft;
	m_iRoiOffsetRight = data.m_iRoiOffsetRight;

	m_iEdgeMeasureMultiPointNumber = data.m_iEdgeMeasureMultiPointNumber;
	m_bEdgeMeasureEndPoint = data.m_bEdgeMeasureEndPoint;
	m_iEdgeMeasureStartLength = data.m_iEdgeMeasureStartLength;
	m_iEdgeMeasureEndLength = data.m_iEdgeMeasureEndLength;
	m_iEdgeMeasureDir = data.m_iEdgeMeasureDir;
	m_iEdgeMeasureGv = data.m_iEdgeMeasureGv;
	m_iEdgeMeasurePos = data.m_iEdgeMeasurePos;
	m_dEdgeMeasureSmFactor = data.m_dEdgeMeasureSmFactor;
	m_iEdgeMeasureEdgeStr = data.m_iEdgeMeasureEdgeStr;

	m_bUseApplyDontCare = data.m_bUseApplyDontCare; // 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW
	m_bUseDontCare = data.m_bUseDontCare;			// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW

	m_iImageType = data.m_iImageType; // 24.07.08 - v2652 - 검사제외영역 영상 번호 파라미터 추가 - LeeGW

	// 240227 Local Align 추가 - LeeGW Start
	// Local Align Param
	//// Apply
	m_bUseLocalAlign = data.m_bUseLocalAlign;
	m_iLocalAlignImageIndex = data.m_iLocalAlignImageIndex;
	m_iLocalAlignROINo = data.m_iLocalAlignROINo;
	m_iLocalAlignType = data.m_iLocalAlignType;
	m_iLocalAlignApplyMethod = data.m_iLocalAlignApplyMethod;
	m_bLocalAlignPosX = data.m_bLocalAlignPosX;
	m_bLocalAlignPosY = data.m_bLocalAlignPosY;
	m_bLocalAlignPosAngle = data.m_bLocalAlignPosAngle;
	m_iLocalAlignFitPos = data.m_iLocalAlignFitPos;

	// Aniso Align
	m_bUseROIAnisoAlign = data.m_bUseROIAnisoAlign;
	m_iROIAnisoAlignXSearchMargin = data.m_iROIAnisoAlignXSearchMargin;
	m_iROIAnisoAlignYSearchMargin = data.m_iROIAnisoAlignYSearchMargin;
	m_iROIAnisoAlignMatchingAngleRange = data.m_iROIAnisoAlignMatchingAngleRange;
	m_dROIAnisoAlignMatchingScaleMinY = data.m_dROIAnisoAlignMatchingScaleMinY;
	m_dROIAnisoAlignMatchingScaleMaxY = data.m_dROIAnisoAlignMatchingScaleMaxY;
	m_dROIAnisoAlignMatchingScaleMinX = data.m_dROIAnisoAlignMatchingScaleMinX;
	m_dROIAnisoAlignMatchingScaleMaxX = data.m_dROIAnisoAlignMatchingScaleMaxX;
	m_iROIAnisoAlignMatchingMinContrast = data.m_iROIAnisoAlignMatchingMinContrast;
	m_bROIAnisoAlignUsePosX = data.m_bROIAnisoAlignUsePosX;
	m_bROIAnisoAlignUsePosY = data.m_bROIAnisoAlignUsePosY;
	m_bROIAnisoAlignInspectShift = data.m_bROIAnisoAlignInspectShift;
	m_dROIAnisoAlignShiftTop = data.m_dROIAnisoAlignShiftTop;
	m_dROIAnisoAlignShiftBottom = data.m_dROIAnisoAlignShiftBottom;
	m_dROIAnisoAlignShiftLeft = data.m_dROIAnisoAlignShiftLeft;
	m_dROIAnisoAlignShiftRight = data.m_dROIAnisoAlignShiftRight;
	m_bROIAnisoAlignLocalAlignUse = data.m_bROIAnisoAlignLocalAlignUse;
	m_dROIAnisoAlignMatchingScore = data.m_dROIAnisoAlignMatchingScore;
	m_bROIAnisoAlignModelExist = data.m_bROIAnisoAlignModelExist;

	//// Part Check
	m_bROIPartCheckUse = data.m_bROIPartCheckUse;
	m_bROIPartCheckLocalAlignUse = data.m_bROIPartCheckLocalAlignUse;
	m_iROIPartCheckMatchingAngleRange = data.m_iROIPartCheckMatchingAngleRange;
	m_iROIPartCheckMatchingSearchMarginX = data.m_iROIPartCheckMatchingSearchMarginX;
	m_iROIPartCheckMatchingSearchMarginY = data.m_iROIPartCheckMatchingSearchMarginY;
	m_iROIPartCheckMatchingSearchMarginX2 = data.m_iROIPartCheckMatchingSearchMarginX2;
	m_iROIPartCheckMatchingSearchMarginY2 = data.m_iROIPartCheckMatchingSearchMarginY2;

	m_bROIPartCheckExistInspUse = data.m_bROIPartCheckExistInspUse;
	m_iROIPartCheckMatchingScore = data.m_iROIPartCheckMatchingScore;
	m_bROIPartCheckShiftInspUse = data.m_bROIPartCheckShiftInspUse;
	m_iROIPartCheckShiftX = data.m_iROIPartCheckShiftX;
	m_iROIPartCheckShiftY = data.m_iROIPartCheckShiftY;
	m_dROIPartCheckRotationAngle = data.m_dROIPartCheckRotationAngle;

	//// Edge Measure
	m_bROIEdgeMeasureUse = data.m_bROIEdgeMeasureUse;
	m_bROIEdgeMeasureLocalAlignUse = data.m_bROIEdgeMeasureLocalAlignUse;
	m_iROIEdgeMeasureMultiPointNumber = data.m_iROIEdgeMeasureMultiPointNumber;
	m_bROIEdgeMeasureEndPoint = data.m_bROIEdgeMeasureEndPoint;
	m_iROIEdgeMeasureStartLength = data.m_iROIEdgeMeasureStartLength;
	m_iROIEdgeMeasureEndLength = data.m_iROIEdgeMeasureEndLength;
	m_iROIEdgeMeasureDir = data.m_iROIEdgeMeasureDir;
	m_iROIEdgeMeasureGv = data.m_iROIEdgeMeasureGv;
	m_iROIEdgeMeasurePos = data.m_iROIEdgeMeasurePos;
	m_dROIEdgeMeasureSmFactor = data.m_dROIEdgeMeasureSmFactor;
	m_iROIEdgeMeasureEdgeStr = data.m_iROIEdgeMeasureEdgeStr;
	m_dROIEdgeMeasureRotateAngle = data.m_dROIEdgeMeasureRotateAngle;

	m_bUseImageProcessFilter = data.m_bUseImageProcessFilter;
	m_iImageProcessFilterType1 = data.m_iImageProcessFilterType1;
	m_iImageProcessFilterType2 = data.m_iImageProcessFilterType2;
	m_iImageProcessFilterType3 = data.m_iImageProcessFilterType3;
	m_iImageProcessFilterType1X = data.m_iImageProcessFilterType1X;
	m_iImageProcessFilterType2X = data.m_iImageProcessFilterType2X;
	m_iImageProcessFilterType3X = data.m_iImageProcessFilterType3X;
	m_iImageProcessFilterType1Y = data.m_iImageProcessFilterType1Y;
	m_iImageProcessFilterType2Y = data.m_iImageProcessFilterType2Y;
	m_iImageProcessFilterType3Y = data.m_iImageProcessFilterType3Y;
	// 240227 Local Align 추가 - LeeGW End

	return *this;
}

// 24.03.04 Local Align 추가 - LeeGW Start
void CAlgorithmParam::Save(HANDLE hFile)
{
	DWORD dwBytesWritten;

	// Inspection
	WriteFile(hFile, &m_bInspect, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectNameIdx, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bSpecialNG, sizeof(BOOL), &dwBytesWritten, NULL);

	for (int i = 0; i < 3; i++)
	{
		WriteFile(hFile, &m_bUseInspectArea[i], sizeof(BOOL), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_iInspectAreaContour1Number[i], sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_iInspectAreaContour1Margin[i], sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_iInspectAreaContour2Number[i], sizeof(int), &dwBytesWritten, NULL);
		WriteFile(hFile, &m_iInspectAreaContour2Margin[i], sizeof(int), &dwBytesWritten, NULL);
	}

	WriteFile(hFile, &m_bUseCommonAlgorithm, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseBrightFixedThres, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightLowerThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUpperThres, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseBrightDT, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseBrightLineNormalization, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightMedianFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightClosingFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightDTValue, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseBrightUniformityCheck, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityHystLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightUniformityHystOffset, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseBrightHystThres, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystSecureThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystPotentialThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iBrightHystPotentialLength, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDarkFixedThres, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkLowerThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUpperThres, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDarkDT, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDarkLineNormalization, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkMedianFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkClosingFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTFilterSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkDTValue, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDarkUniformityCheck, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityOffset, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityHystLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkUniformityHystOffset, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDarkHystThres, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkHystSecureThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkHystPotentialThres, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDarkHystPotentialLength, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bCombineBrightDarkBlob, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iOpeningSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iClosingSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bFillup, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseConnection, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionMinSize, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseAngle, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iConnectionMaxWidth, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseEdge, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dEdgeZoomRatio, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeStrength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeConnectionDistance, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMaxEndpointDistance, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMinContourLength, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseDefectConditionArea, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionWidth, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionMean, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionStdev, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionAnisometry, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionCircularity, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionConvexity, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionEllipseRatio, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionOuterDist, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDefectConditionInnerDist, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDefectConditionAreaMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionLengthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionWidthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionMeanMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionStdevMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionAnisometryMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionCircularityMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionConvexityMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionEllipseRatioMin, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDefectConditionAreaMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionLengthMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionWidthMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionMeanMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionStdevMin, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionAnisometryMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionCircularityMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionConvexityMax, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dDefectConditionEllipseRatioMax, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDefectConditionOuterDist, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionInnerDist, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseImageScaling, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageScaleMethod, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageScaleMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageScaleMax, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bDefectConditionXLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionYLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bDefectConditionContrast, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iDefectConditionXLengthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionYLengthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionContrastMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionXLengthMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionYLengthMax, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iDefectConditionContrastMax, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseFTConnected, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseFTConnectedArea, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseFTConnectedLength, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseFTConnectedWidth, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFTConnectedAreaMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFTConnectedLengthMin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iFTConnectedWidthMin, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseLineFit, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLineFitApplyPos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iRoiOffsetTop, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iRoiOffsetBottom, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iRoiOffsetLeft, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iRoiOffsetRight, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iEdgeMeasureMultiPointNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bEdgeMeasureEndPoint, sizeof(BOOL), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_iEdgeMeasureStartLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureEndLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureDir, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureGv, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dEdgeMeasureSmFactor, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iEdgeMeasureEdgeStr, sizeof(int), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseApplyDontCare, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bUseDontCare, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageType, sizeof(int), &dwBytesWritten, NULL);

	// Apply
	WriteFile(hFile, &m_bUseLocalAlign, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLocalAlignImageIndex, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLocalAlignROINo, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLocalAlignType, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLocalAlignApplyMethod, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bLocalAlignPosX, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bLocalAlignPosY, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bLocalAlignPosAngle, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iLocalAlignFitPos, sizeof(int), &dwBytesWritten, NULL);

	// Aniso Align
	WriteFile(hFile, &m_bUseROIAnisoAlign, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignXSearchMargin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignYSearchMargin, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMatchingAngleRange, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScaleMinY, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScaleMaxY, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScaleMinX, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScaleMaxX, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIAnisoAlignMatchingMinContrast, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAnisoAlignUsePosX, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAnisoAlignUsePosY, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignMatchingScore, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAnisoAlignInspectShift, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignShiftTop, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignShiftBottom, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignShiftLeft, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIAnisoAlignShiftRight, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIAnisoAlignLocalAlignUse, sizeof(BOOL), &dwBytesWritten, NULL);

	// Part Check
	WriteFile(hFile, &m_bROIPartCheckUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIPartCheckLocalAlignUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIPartCheckMatchingAngleRange, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIPartCheckMatchingSearchMarginX, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIPartCheckMatchingSearchMarginY, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIPartCheckMatchingSearchMarginX2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIPartCheckMatchingSearchMarginY2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIPartCheckExistInspUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIPartCheckMatchingScore, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIPartCheckShiftInspUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIPartCheckShiftX, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIPartCheckShiftY, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIPartCheckRotationAngle, sizeof(double), &dwBytesWritten, NULL);

	//// Edge Measure
	WriteFile(hFile, &m_bROIEdgeMeasureUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIEdgeMeasureLocalAlignUse, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIEdgeMeasureMultiPointNumber, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_bROIEdgeMeasureEndPoint, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIEdgeMeasureStartLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIEdgeMeasureEndLength, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIEdgeMeasureDir, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIEdgeMeasureGv, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIEdgeMeasurePos, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIEdgeMeasureSmFactor, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iROIEdgeMeasureEdgeStr, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_dROIEdgeMeasureRotateAngle, sizeof(double), &dwBytesWritten, NULL);

	WriteFile(hFile, &m_bUseImageProcessFilter, sizeof(BOOL), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType1, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType2, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType3, sizeof(int), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType1X, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType2X, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType3X, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType1Y, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType2Y, sizeof(double), &dwBytesWritten, NULL);
	WriteFile(hFile, &m_iImageProcessFilterType3Y, sizeof(double), &dwBytesWritten, NULL);

	// 240227 Local Align 추가 - LeeGW End
}

BOOL CAlgorithmParam::Load(HANDLE hFile, int iModelVersion)
{
	DWORD dwBytesRead;

	// Inspection
	ReadFile(hFile, &m_bInspect, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectNameIdx, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bSpecialNG, sizeof(BOOL), &dwBytesRead, NULL);

	for (int i = 0; i < 3; i++)
	{
		ReadFile(hFile, &m_bUseInspectArea[i], sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iInspectAreaContour1Number[i], sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iInspectAreaContour1Margin[i], sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iInspectAreaContour2Number[i], sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iInspectAreaContour2Margin[i], sizeof(int), &dwBytesRead, NULL);
	}

	ReadFile(hFile, &m_bUseCommonAlgorithm, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseBrightFixedThres, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightLowerThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUpperThres, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseBrightDT, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseBrightLineNormalization, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightMedianFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightClosingFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightDTValue, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseBrightUniformityCheck, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityHystLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightUniformityHystOffset, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseBrightHystThres, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystSecureThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystPotentialThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iBrightHystPotentialLength, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDarkFixedThres, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkLowerThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUpperThres, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDarkDT, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDarkLineNormalization, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkMedianFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkClosingFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkDTFilterSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkDTValue, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDarkUniformityCheck, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityOffset, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityHystLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkUniformityHystOffset, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDarkHystThres, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkHystSecureThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkHystPotentialThres, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDarkHystPotentialLength, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bCombineBrightDarkBlob, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iOpeningSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iClosingSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bFillup, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseConnection, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionMinSize, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseAngle, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iConnectionMaxWidth, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseEdge, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dEdgeZoomRatio, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeStrength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeConnectionDistance, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMaxEndpointDistance, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMinContourLength, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseDefectConditionArea, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionWidth, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionMean, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionStdev, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionAnisometry, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionCircularity, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionConvexity, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionEllipseRatio, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionOuterDist, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDefectConditionInnerDist, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iDefectConditionAreaMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionLengthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionWidthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionMeanMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionStdevMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionAnisometryMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionCircularityMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionConvexityMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionEllipseRatioMin, sizeof(double), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iDefectConditionAreaMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionLengthMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionWidthMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionMeanMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionStdevMin, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionAnisometryMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionCircularityMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionConvexityMax, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dDefectConditionEllipseRatioMax, sizeof(double), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iDefectConditionOuterDist, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionInnerDist, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseImageScaling, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageScaleMethod, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageScaleMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageScaleMax, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bDefectConditionXLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionYLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bDefectConditionContrast, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iDefectConditionXLengthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionYLengthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionContrastMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionXLengthMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionYLengthMax, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iDefectConditionContrastMax, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseFTConnected, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseFTConnectedArea, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseFTConnectedLength, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseFTConnectedWidth, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iFTConnectedAreaMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iFTConnectedLengthMin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iFTConnectedWidthMin, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseLineFit, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLineFitApplyPos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iRoiOffsetTop, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iRoiOffsetBottom, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iRoiOffsetLeft, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iRoiOffsetRight, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iEdgeMeasureMultiPointNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bEdgeMeasureEndPoint, sizeof(BOOL), &dwBytesRead, NULL);

	ReadFile(hFile, &m_iEdgeMeasureStartLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureEndLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureDir, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureGv, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dEdgeMeasureSmFactor, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iEdgeMeasureEdgeStr, sizeof(int), &dwBytesRead, NULL);

	ReadFile(hFile, &m_bUseApplyDontCare, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bUseDontCare, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iImageType, sizeof(int), &dwBytesRead, NULL);

	// Apply
	ReadFile(hFile, &m_bUseLocalAlign, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLocalAlignImageIndex, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLocalAlignROINo, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLocalAlignType, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLocalAlignApplyMethod, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bLocalAlignPosX, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bLocalAlignPosY, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bLocalAlignPosAngle, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iLocalAlignFitPos, sizeof(BOOL), &dwBytesRead, NULL);

	// Aniso Align
	ReadFile(hFile, &m_bUseROIAnisoAlign, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignXSearchMargin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignYSearchMargin, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignMatchingAngleRange, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScaleMinY, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScaleMaxY, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScaleMinX, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScaleMaxX, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIAnisoAlignMatchingMinContrast, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAnisoAlignUsePosX, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAnisoAlignUsePosY, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignMatchingScore, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAnisoAlignInspectShift, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignShiftTop, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignShiftBottom, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignShiftLeft, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIAnisoAlignShiftRight, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIAnisoAlignLocalAlignUse, sizeof(BOOL), &dwBytesRead, NULL);

	// Part Check
	ReadFile(hFile, &m_bROIPartCheckUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIPartCheckLocalAlignUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIPartCheckMatchingAngleRange, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIPartCheckMatchingSearchMarginX, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIPartCheckMatchingSearchMarginY, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIPartCheckMatchingSearchMarginX2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIPartCheckMatchingSearchMarginY2, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIPartCheckExistInspUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIPartCheckMatchingScore, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIPartCheckShiftInspUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIPartCheckShiftX, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIPartCheckShiftY, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIPartCheckRotationAngle, sizeof(double), &dwBytesRead, NULL);

	//// Edge Measure
	ReadFile(hFile, &m_bROIEdgeMeasureUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIEdgeMeasureLocalAlignUse, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIEdgeMeasureMultiPointNumber, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_bROIEdgeMeasureEndPoint, sizeof(BOOL), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIEdgeMeasureStartLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIEdgeMeasureEndLength, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIEdgeMeasureDir, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIEdgeMeasureGv, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIEdgeMeasurePos, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIEdgeMeasureSmFactor, sizeof(double), &dwBytesRead, NULL);
	ReadFile(hFile, &m_iROIEdgeMeasureEdgeStr, sizeof(int), &dwBytesRead, NULL);
	ReadFile(hFile, &m_dROIEdgeMeasureRotateAngle, sizeof(double), &dwBytesRead, NULL);

	if (iModelVersion >= 3013)
	{
		ReadFile(hFile, &m_bUseImageProcessFilter, sizeof(BOOL), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessFilterType1, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessFilterType2, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessFilterType3, sizeof(int), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessFilterType1X, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessFilterType2X, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessFilterType3X, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessFilterType1Y, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessFilterType2Y, sizeof(double), &dwBytesRead, NULL);
		ReadFile(hFile, &m_iImageProcessFilterType3Y, sizeof(double), &dwBytesRead, NULL);
	}

	return TRUE;
}
// 24.03.04 Local Align 추가 - LeeGW End

void CAlgorithmParam::CopyFrom(const CAlgorithmParam &src)
{
	m_bInspect = src.m_bInspect;
	m_iDefectNameIdx = src.m_iDefectNameIdx;
	m_bSpecialNG = src.m_bSpecialNG;
	for (int i = 0; i < 3; i++)
		m_bUseInspectArea[i] = src.m_bUseInspectArea[i];
	for (int i = 0; i < 3; i++)
		m_iInspectAreaContour1Number[i] = src.m_iInspectAreaContour1Number[i];
	for (int i = 0; i < 3; i++)
		m_iInspectAreaContour1Margin[i] = src.m_iInspectAreaContour1Margin[i];
	for (int i = 0; i < 3; i++)
		m_iInspectAreaContour2Number[i] = src.m_iInspectAreaContour2Number[i];
	for (int i = 0; i < 3; i++)
		m_iInspectAreaContour2Margin[i] = src.m_iInspectAreaContour2Margin[i];
	m_bUseCommonAlgorithm = src.m_bUseCommonAlgorithm;
	m_bUseBrightFixedThres = src.m_bUseBrightFixedThres;
	m_iBrightLowerThres = src.m_iBrightLowerThres;
	m_iBrightUpperThres = src.m_iBrightUpperThres;
	m_bUseBrightDT = src.m_bUseBrightDT;
	m_bUseBrightLineNormalization = src.m_bUseBrightLineNormalization;
	m_iBrightMedianFilterSize = src.m_iBrightMedianFilterSize;
	m_iBrightClosingFilterSize = src.m_iBrightClosingFilterSize;
	m_iBrightDTFilterSize = src.m_iBrightDTFilterSize;
	m_iBrightDTValue = src.m_iBrightDTValue;
	m_bUseBrightUniformityCheck = src.m_bUseBrightUniformityCheck;
	m_iBrightUniformityOffset = src.m_iBrightUniformityOffset;
	m_iBrightUniformityHystLength = src.m_iBrightUniformityHystLength;
	m_iBrightUniformityHystOffset = src.m_iBrightUniformityHystOffset;
	m_bUseBrightHystThres = src.m_bUseBrightHystThres;
	m_iBrightHystSecureThres = src.m_iBrightHystSecureThres;
	m_iBrightHystPotentialThres = src.m_iBrightHystPotentialThres;
	m_iBrightHystPotentialLength = src.m_iBrightHystPotentialLength;
	m_bUseDarkFixedThres = src.m_bUseDarkFixedThres;
	m_iDarkLowerThres = src.m_iDarkLowerThres;
	m_iDarkUpperThres = src.m_iDarkUpperThres;
	m_bUseDarkDT = src.m_bUseDarkDT;
	m_bUseDarkLineNormalization = src.m_bUseDarkLineNormalization;
	m_iDarkMedianFilterSize = src.m_iDarkMedianFilterSize;
	m_iDarkClosingFilterSize = src.m_iDarkClosingFilterSize;
	m_iDarkDTFilterSize = src.m_iDarkDTFilterSize;
	m_iDarkDTValue = src.m_iDarkDTValue;
	m_bUseDarkUniformityCheck = src.m_bUseDarkUniformityCheck;
	m_iDarkUniformityOffset = src.m_iDarkUniformityOffset;
	m_iDarkUniformityHystLength = src.m_iDarkUniformityHystLength;
	m_iDarkUniformityHystOffset = src.m_iDarkUniformityHystOffset;
	m_bUseDarkHystThres = src.m_bUseDarkHystThres;
	m_iDarkHystSecureThres = src.m_iDarkHystSecureThres;
	m_iDarkHystPotentialThres = src.m_iDarkHystPotentialThres;
	m_iDarkHystPotentialLength = src.m_iDarkHystPotentialLength;
	m_bCombineBrightDarkBlob = src.m_bCombineBrightDarkBlob;
	m_iOpeningSize = src.m_iOpeningSize;
	m_iClosingSize = src.m_iClosingSize;
	m_bFillup = src.m_bFillup;
	m_bUseConnection = src.m_bUseConnection;
	m_iConnectionMinSize = src.m_iConnectionMinSize;
	m_iConnectionLength = src.m_iConnectionLength;
	m_bUseAngle = src.m_bUseAngle;
	m_iConnectionMaxWidth = src.m_iConnectionMaxWidth;
	m_bUseEdge = src.m_bUseEdge;
	m_dEdgeZoomRatio = src.m_dEdgeZoomRatio;
	m_iEdgeStrength = src.m_iEdgeStrength;
	m_iEdgeConnectionDistance = src.m_iEdgeConnectionDistance;
	m_iEdgeMaxEndpointDistance = src.m_iEdgeMaxEndpointDistance;
	m_iEdgeMinContourLength = src.m_iEdgeMinContourLength;
	m_bUseDefectConditionArea = src.m_bUseDefectConditionArea;
	m_bUseDefectConditionLength = src.m_bUseDefectConditionLength;
	m_bUseDefectConditionWidth = src.m_bUseDefectConditionWidth;
	m_bUseDefectConditionMean = src.m_bUseDefectConditionMean;
	m_bUseDefectConditionStdev = src.m_bUseDefectConditionStdev;
	m_bUseDefectConditionAnisometry = src.m_bUseDefectConditionAnisometry;
	m_bUseDefectConditionCircularity = src.m_bUseDefectConditionCircularity;
	m_bUseDefectConditionConvexity = src.m_bUseDefectConditionConvexity;
	m_bUseDefectConditionEllipseRatio = src.m_bUseDefectConditionEllipseRatio;
	m_bUseDefectConditionOuterDist = src.m_bUseDefectConditionOuterDist;
	m_bUseDefectConditionInnerDist = src.m_bUseDefectConditionInnerDist;
	m_bUseDefectConditionEpoxyDist = src.m_bUseDefectConditionEpoxyDist;
	m_iDefectConditionAreaMin = src.m_iDefectConditionAreaMin;
	m_iDefectConditionLengthMin = src.m_iDefectConditionLengthMin;
	m_iDefectConditionWidthMin = src.m_iDefectConditionWidthMin;
	m_iDefectConditionMeanMin = src.m_iDefectConditionMeanMin;
	m_dDefectConditionStdevMin = src.m_dDefectConditionStdevMin;
	m_dDefectConditionAnisometryMin = src.m_dDefectConditionAnisometryMin;
	m_dDefectConditionCircularityMin = src.m_dDefectConditionCircularityMin;
	m_dDefectConditionConvexityMin = src.m_dDefectConditionConvexityMin;
	m_dDefectConditionEllipseRatioMin = src.m_dDefectConditionEllipseRatioMin;
	m_iDefectConditionAreaMax = src.m_iDefectConditionAreaMax;
	m_iDefectConditionLengthMax = src.m_iDefectConditionLengthMax;
	m_iDefectConditionWidthMax = src.m_iDefectConditionWidthMax;
	m_iDefectConditionMeanMax = src.m_iDefectConditionMeanMax;
	m_dDefectConditionStdevMax = src.m_dDefectConditionStdevMax;
	m_dDefectConditionAnisometryMax = src.m_dDefectConditionAnisometryMax;
	m_dDefectConditionCircularityMax = src.m_dDefectConditionCircularityMax;
	m_dDefectConditionConvexityMax = src.m_dDefectConditionConvexityMax;
	m_dDefectConditionEllipseRatioMax = src.m_dDefectConditionEllipseRatioMax;
	m_iDefectConditionOuterDist = src.m_iDefectConditionOuterDist;
	m_iDefectConditionInnerDist = src.m_iDefectConditionInnerDist;
	m_iDefectConditionEpoxyDist = src.m_iDefectConditionEpoxyDist;
	m_iEpoxySEThres = src.m_iEpoxySEThres;
	m_iEpoxyESThres = src.m_iEpoxyESThres;
	m_iEpoxyClosingSize = src.m_iEpoxyClosingSize;
	m_iEpoxyOpeningSize = src.m_iEpoxyOpeningSize;
	m_iEpoxyNearDistance = src.m_iEpoxyNearDistance;
	m_iEpoxyNearMinSize = src.m_iEpoxyNearMinSize;
	m_bEpoxyConditionHole = src.m_bEpoxyConditionHole;
	m_bEpoxyConditionNear = src.m_bEpoxyConditionNear;
	m_bUseImageScaling = src.m_bUseImageScaling;
	m_iImageScaleMethod = src.m_iImageScaleMethod;
	m_iImageScaleMin = src.m_iImageScaleMin;
	m_iImageScaleMax = src.m_iImageScaleMax;
	m_bDefectConditionXLength = src.m_bDefectConditionXLength;
	m_bDefectConditionYLength = src.m_bDefectConditionYLength;
	m_bDefectConditionContrast = src.m_bDefectConditionContrast;
	m_iDefectConditionXLengthMin = src.m_iDefectConditionXLengthMin;
	m_iDefectConditionYLengthMin = src.m_iDefectConditionYLengthMin;
	m_iDefectConditionContrastMin = src.m_iDefectConditionContrastMin;
	m_iDefectConditionXLengthMax = src.m_iDefectConditionXLengthMax;
	m_iDefectConditionYLengthMax = src.m_iDefectConditionYLengthMax;
	m_iDefectConditionContrastMax = src.m_iDefectConditionContrastMax;
	m_bUseFTConnected = src.m_bUseFTConnected;
	m_bUseFTConnectedArea = src.m_bUseFTConnectedArea;
	m_bUseFTConnectedLength = src.m_bUseFTConnectedLength;
	m_bUseFTConnectedWidth = src.m_bUseFTConnectedWidth;
	m_iFTConnectedAreaMin = src.m_iFTConnectedAreaMin;
	m_iFTConnectedLengthMin = src.m_iFTConnectedLengthMin;
	m_iFTConnectedWidthMin = src.m_iFTConnectedWidthMin;
	m_bUseLineFit = src.m_bUseLineFit;
	m_iLineFitApplyPos = src.m_iLineFitApplyPos;
	m_iRoiOffsetTop = src.m_iRoiOffsetTop;
	m_iRoiOffsetBottom = src.m_iRoiOffsetBottom;
	m_iRoiOffsetLeft = src.m_iRoiOffsetLeft;
	m_iRoiOffsetRight = src.m_iRoiOffsetRight;
	m_iEdgeMeasureMultiPointNumber = src.m_iEdgeMeasureMultiPointNumber;
	m_bEdgeMeasureEndPoint = src.m_bEdgeMeasureEndPoint;
	m_iEdgeMeasureStartLength = src.m_iEdgeMeasureStartLength;
	m_iEdgeMeasureEndLength = src.m_iEdgeMeasureEndLength;
	m_iEdgeMeasureDir = src.m_iEdgeMeasureDir;
	m_iEdgeMeasureGv = src.m_iEdgeMeasureGv;
	m_iEdgeMeasurePos = src.m_iEdgeMeasurePos;
	m_dEdgeMeasureSmFactor = src.m_dEdgeMeasureSmFactor;
	m_iEdgeMeasureEdgeStr = src.m_iEdgeMeasureEdgeStr;
	m_bUseApplyDontCare = src.m_bUseApplyDontCare;
	m_bUseDontCare = src.m_bUseDontCare;
	m_iImageType = src.m_iImageType;
	m_bUseLocalAlign = src.m_bUseLocalAlign;
	m_iLocalAlignImageIndex = src.m_iLocalAlignImageIndex;
	m_iLocalAlignROINo = src.m_iLocalAlignROINo;
	m_iLocalAlignType = src.m_iLocalAlignType;
	m_iLocalAlignApplyMethod = src.m_iLocalAlignApplyMethod;
	m_bLocalAlignPosX = src.m_bLocalAlignPosX;
	m_bLocalAlignPosY = src.m_bLocalAlignPosY;
	m_bLocalAlignPosAngle = src.m_bLocalAlignPosAngle;
	m_iLocalAlignFitPos = src.m_iLocalAlignFitPos;
	m_bUseROIAnisoAlign = src.m_bUseROIAnisoAlign;
	m_iROIAnisoAlignXSearchMargin = src.m_iROIAnisoAlignXSearchMargin;
	m_iROIAnisoAlignYSearchMargin = src.m_iROIAnisoAlignYSearchMargin;
	m_iROIAnisoAlignMatchingAngleRange = src.m_iROIAnisoAlignMatchingAngleRange;
	m_dROIAnisoAlignMatchingScaleMinY = src.m_dROIAnisoAlignMatchingScaleMinY;
	m_dROIAnisoAlignMatchingScaleMaxY = src.m_dROIAnisoAlignMatchingScaleMaxY;
	m_dROIAnisoAlignMatchingScaleMinX = src.m_dROIAnisoAlignMatchingScaleMinX;
	m_dROIAnisoAlignMatchingScaleMaxX = src.m_dROIAnisoAlignMatchingScaleMaxX;
	m_iROIAnisoAlignMatchingMinContrast = src.m_iROIAnisoAlignMatchingMinContrast;
	m_bROIAnisoAlignUsePosX = src.m_bROIAnisoAlignUsePosX;
	m_bROIAnisoAlignUsePosY = src.m_bROIAnisoAlignUsePosY;
	m_bROIAnisoAlignInspectShift = src.m_bROIAnisoAlignInspectShift;
	m_dROIAnisoAlignShiftTop = src.m_dROIAnisoAlignShiftTop;
	m_dROIAnisoAlignShiftBottom = src.m_dROIAnisoAlignShiftBottom;
	m_dROIAnisoAlignShiftLeft = src.m_dROIAnisoAlignShiftLeft;
	m_dROIAnisoAlignShiftRight = src.m_dROIAnisoAlignShiftRight;
	m_bROIAnisoAlignLocalAlignUse = src.m_bROIAnisoAlignLocalAlignUse;
	m_dROIAnisoAlignMatchingScore = src.m_dROIAnisoAlignMatchingScore;
	m_bROIAnisoAlignModelExist = src.m_bROIAnisoAlignModelExist;
	m_bROIPartCheckUse = src.m_bROIPartCheckUse;
	m_bROIPartCheckLocalAlignUse = src.m_bROIPartCheckLocalAlignUse;
	m_iROIPartCheckMatchingAngleRange = src.m_iROIPartCheckMatchingAngleRange;
	m_iROIPartCheckMatchingSearchMarginX = src.m_iROIPartCheckMatchingSearchMarginX;
	m_iROIPartCheckMatchingSearchMarginY = src.m_iROIPartCheckMatchingSearchMarginY;
	m_iROIPartCheckMatchingSearchMarginX2 = src.m_iROIPartCheckMatchingSearchMarginX2;
	m_iROIPartCheckMatchingSearchMarginY2 = src.m_iROIPartCheckMatchingSearchMarginY2;
	m_bROIPartCheckExistInspUse = src.m_bROIPartCheckExistInspUse;
	m_iROIPartCheckMatchingScore = src.m_iROIPartCheckMatchingScore;
	m_bROIPartCheckShiftInspUse = src.m_bROIPartCheckShiftInspUse;
	m_iROIPartCheckShiftX = src.m_iROIPartCheckShiftX;
	m_iROIPartCheckShiftY = src.m_iROIPartCheckShiftY;
	m_dROIPartCheckRotationAngle = src.m_dROIPartCheckRotationAngle;
	m_bROIEdgeMeasureUse = src.m_bROIEdgeMeasureUse;
	m_bROIEdgeMeasureLocalAlignUse = src.m_bROIEdgeMeasureLocalAlignUse;
	m_iROIEdgeMeasureMultiPointNumber = src.m_iROIEdgeMeasureMultiPointNumber;
	m_bROIEdgeMeasureEndPoint = src.m_bROIEdgeMeasureEndPoint;
	m_iROIEdgeMeasureStartLength = src.m_iROIEdgeMeasureStartLength;
	m_iROIEdgeMeasureEndLength = src.m_iROIEdgeMeasureEndLength;
	m_iROIEdgeMeasureDir = src.m_iROIEdgeMeasureDir;
	m_iROIEdgeMeasureGv = src.m_iROIEdgeMeasureGv;
	m_iROIEdgeMeasurePos = src.m_iROIEdgeMeasurePos;
	m_dROIEdgeMeasureSmFactor = src.m_dROIEdgeMeasureSmFactor;
	m_iROIEdgeMeasureEdgeStr = src.m_iROIEdgeMeasureEdgeStr;
	m_dROIEdgeMeasureRotateAngle = src.m_dROIEdgeMeasureRotateAngle;
	m_bUseImageProcessFilter = src.m_bUseImageProcessFilter;
	m_iImageProcessFilterType1 = src.m_iImageProcessFilterType1;
	m_iImageProcessFilterType2 = src.m_iImageProcessFilterType2;
	m_iImageProcessFilterType3 = src.m_iImageProcessFilterType3;
	m_iImageProcessFilterType1X = src.m_iImageProcessFilterType1X;
	m_iImageProcessFilterType2X = src.m_iImageProcessFilterType2X;
	m_iImageProcessFilterType3X = src.m_iImageProcessFilterType3X;
	m_iImageProcessFilterType1Y = src.m_iImageProcessFilterType1Y;
	m_iImageProcessFilterType2Y = src.m_iImageProcessFilterType2Y;
	m_iImageProcessFilterType3Y = src.m_iImageProcessFilterType3Y;
}

CString CAlgorithmParam::GetDifferentVariables(const CAlgorithmParam &newParam) const
{
	CString strDiff = _T("");
	const double TOLERANCE = DBL_EPSILON * 100;

	// 면적 관련
	if (m_bUseDefectConditionArea != newParam.m_bUseDefectConditionArea)
	{
		BOOL bOldValue = m_bUseDefectConditionArea;
		BOOL bNewValue = newParam.m_bUseDefectConditionArea;
		strDiff.AppendFormat(_T("면적 사용여부:\t\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (m_iDefectConditionAreaMin != newParam.m_iDefectConditionAreaMin)
	{
		UINT iOldValue = m_iDefectConditionAreaMin;
		UINT iNewValue = newParam.m_iDefectConditionAreaMin;
		strDiff.AppendFormat(_T("면적 최소값:\t\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}
	if (m_iDefectConditionAreaMax != newParam.m_iDefectConditionAreaMax)
	{
		UINT iOldValue = m_iDefectConditionAreaMax;
		UINT iNewValue = newParam.m_iDefectConditionAreaMax;
		strDiff.AppendFormat(_T("면적 최대값:\t\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}

	// X 방향 길이 관련
	if (m_bDefectConditionXLength != newParam.m_bDefectConditionXLength)
	{
		BOOL bOldValue = m_bDefectConditionXLength;
		BOOL bNewValue = newParam.m_bDefectConditionXLength;
		strDiff.AppendFormat(_T("X 방향 길이 사용여부:\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (m_iDefectConditionXLengthMin != newParam.m_iDefectConditionXLengthMin)
	{
		UINT iOldValue = m_iDefectConditionXLengthMin;
		UINT iNewValue = newParam.m_iDefectConditionXLengthMin;
		strDiff.AppendFormat(_T("X 방향 길이 최소값:\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}
	if (m_iDefectConditionXLengthMax != newParam.m_iDefectConditionXLengthMax)
	{
		UINT iOldValue = m_iDefectConditionXLengthMax;
		UINT iNewValue = newParam.m_iDefectConditionXLengthMax;
		strDiff.AppendFormat(_T("X 방향 길이 최대값:\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}

	// Y 방향 길이 관련
	if (m_bDefectConditionYLength != newParam.m_bDefectConditionYLength)
	{
		BOOL bOldValue = m_bDefectConditionYLength;
		BOOL bNewValue = newParam.m_bDefectConditionYLength;
		strDiff.AppendFormat(_T("Y 방향 길이 사용여부:\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (m_iDefectConditionYLengthMin != newParam.m_iDefectConditionYLengthMin)
	{
		UINT iOldValue = m_iDefectConditionYLengthMin;
		UINT iNewValue = newParam.m_iDefectConditionYLengthMin;
		strDiff.AppendFormat(_T("Y 방향 길이 최소값:\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}
	if (m_iDefectConditionYLengthMax != newParam.m_iDefectConditionYLengthMax)
	{
		UINT iOldValue = m_iDefectConditionYLengthMax;
		UINT iNewValue = newParam.m_iDefectConditionYLengthMax;
		strDiff.AppendFormat(_T("Y 방향 길이 최대값:\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}

	// 길이 관련
	if (m_bUseDefectConditionLength != newParam.m_bUseDefectConditionLength)
	{
		BOOL bOldValue = m_bUseDefectConditionLength;
		BOOL bNewValue = newParam.m_bUseDefectConditionLength;
		strDiff.AppendFormat(_T("길이 사용여부:\t\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (m_iDefectConditionLengthMin != newParam.m_iDefectConditionLengthMin)
	{
		UINT iOldValue = m_iDefectConditionLengthMin;
		UINT iNewValue = newParam.m_iDefectConditionLengthMin;
		strDiff.AppendFormat(_T("길이 최소값:\t\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}
	if (m_iDefectConditionLengthMax != newParam.m_iDefectConditionLengthMax)
	{
		UINT iOldValue = m_iDefectConditionLengthMax;
		UINT iNewValue = newParam.m_iDefectConditionLengthMax;
		strDiff.AppendFormat(_T("길이 최대값:\t\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}

	// 너비 관련
	if (m_bUseDefectConditionWidth != newParam.m_bUseDefectConditionWidth)
	{
		BOOL bOldValue = m_bUseDefectConditionWidth;
		BOOL bNewValue = newParam.m_bUseDefectConditionWidth;
		strDiff.AppendFormat(_T("너비 사용여부:\t\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (m_iDefectConditionWidthMin != newParam.m_iDefectConditionWidthMin)
	{
		UINT iOldValue = m_iDefectConditionWidthMin;
		UINT iNewValue = newParam.m_iDefectConditionWidthMin;
		strDiff.AppendFormat(_T("너비 최소값:\t\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}
	if (m_iDefectConditionWidthMax != newParam.m_iDefectConditionWidthMax)
	{
		UINT iOldValue = m_iDefectConditionWidthMax;
		UINT iNewValue = newParam.m_iDefectConditionWidthMax;
		strDiff.AppendFormat(_T("너비 최대값:\t\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}

	// 밝기평균 관련
	if (m_bUseDefectConditionMean != newParam.m_bUseDefectConditionMean)
	{
		BOOL bOldValue = m_bUseDefectConditionMean;
		BOOL bNewValue = newParam.m_bUseDefectConditionMean;
		strDiff.AppendFormat(_T("밝기평균 사용여부:\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (m_iDefectConditionMeanMin != newParam.m_iDefectConditionMeanMin)
	{
		UINT iOldValue = m_iDefectConditionMeanMin;
		UINT iNewValue = newParam.m_iDefectConditionMeanMin;
		strDiff.AppendFormat(_T("밝기평균 최소값:\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}
	if (m_iDefectConditionMeanMax != newParam.m_iDefectConditionMeanMax)
	{
		UINT iOldValue = m_iDefectConditionMeanMax;
		UINT iNewValue = newParam.m_iDefectConditionMeanMax;
		strDiff.AppendFormat(_T("밝기평균 최대값:\t\t\t%d → %d\n"), iOldValue, iNewValue);
	}

	// 밝기 표준편차 관련
	if (m_bUseDefectConditionStdev != newParam.m_bUseDefectConditionStdev)
	{
		BOOL bOldValue = m_bUseDefectConditionStdev;
		BOOL bNewValue = newParam.m_bUseDefectConditionStdev;
		strDiff.AppendFormat(_T("밝기 표준편차 사용여부:\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (std::fabs(m_dDefectConditionStdevMin - newParam.m_dDefectConditionStdevMin) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionStdevMin;
		double dNewValue = newParam.m_dDefectConditionStdevMin;
		strDiff.AppendFormat(_T("밝기 표준편차 최소값:\t\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}
	if (std::fabs(m_dDefectConditionStdevMax - newParam.m_dDefectConditionStdevMax) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionStdevMax;
		double dNewValue = newParam.m_dDefectConditionStdevMax;
		strDiff.AppendFormat(_T("밝기 표준편차 최대값:\t\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}

	// 길이/폭 비율 관련
	if (m_bUseDefectConditionAnisometry != newParam.m_bUseDefectConditionAnisometry)
	{
		BOOL bOldValue = m_bUseDefectConditionAnisometry;
		BOOL bNewValue = newParam.m_bUseDefectConditionAnisometry;
		strDiff.AppendFormat(_T("길이/폭 비율 사용여부:\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (std::fabs(m_dDefectConditionAnisometryMin - newParam.m_dDefectConditionAnisometryMin) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionAnisometryMin;
		double dNewValue = newParam.m_dDefectConditionAnisometryMin;
		strDiff.AppendFormat(_T("길이/폭 비율 최소값:\t\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}
	if (std::fabs(m_dDefectConditionAnisometryMax - newParam.m_dDefectConditionAnisometryMax) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionAnisometryMax;
		double dNewValue = newParam.m_dDefectConditionAnisometryMax;
		strDiff.AppendFormat(_T("길이/폭 비율 최대값:\t\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}

	// 원형유사도 관련
	if (m_bUseDefectConditionCircularity != newParam.m_bUseDefectConditionCircularity)
	{
		BOOL bOldValue = m_bUseDefectConditionCircularity;
		BOOL bNewValue = newParam.m_bUseDefectConditionCircularity;
		strDiff.AppendFormat(_T("원형유사도 사용여부:\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (std::fabs(m_dDefectConditionCircularityMin - newParam.m_dDefectConditionCircularityMin) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionCircularityMin;
		double dNewValue = newParam.m_dDefectConditionCircularityMin;
		strDiff.AppendFormat(_T("원형유사도 최소값:\t\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}
	if (std::fabs(m_dDefectConditionCircularityMax - newParam.m_dDefectConditionCircularityMax) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionCircularityMax;
		double dNewValue = newParam.m_dDefectConditionCircularityMax;
		strDiff.AppendFormat(_T("원형유사도 최대값:\t\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}

	// 컨벡스 유사도(볼록도) 관련
	if (m_bUseDefectConditionConvexity != newParam.m_bUseDefectConditionConvexity)
	{
		BOOL bOldValue = m_bUseDefectConditionConvexity;
		BOOL bNewValue = newParam.m_bUseDefectConditionConvexity;
		strDiff.AppendFormat(_T("컨벡스 유사도(볼록도) 사용여부:\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (std::fabs(m_dDefectConditionConvexityMin - newParam.m_dDefectConditionConvexityMin) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionConvexityMin;
		double dNewValue = newParam.m_dDefectConditionConvexityMin;
		strDiff.AppendFormat(_T("컨벡스 유사도(볼록도) 최소값:\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}
	if (std::fabs(m_dDefectConditionConvexityMax - newParam.m_dDefectConditionConvexityMax) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionConvexityMax;
		double dNewValue = newParam.m_dDefectConditionConvexityMax;
		strDiff.AppendFormat(_T("컨벡스 유사도(볼록도) 최대값:\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}

	// 장축/단축 비율 관련
	if (m_bUseDefectConditionEllipseRatio != newParam.m_bUseDefectConditionEllipseRatio)
	{
		BOOL bOldValue = m_bUseDefectConditionEllipseRatio;
		BOOL bNewValue = newParam.m_bUseDefectConditionEllipseRatio;
		strDiff.AppendFormat(_T("장축/단축 비율 사용여부:\t\t\t%s → %s\n"),
							 bOldValue ? _T("TRUE") : _T("FALSE"),
							 bNewValue ? _T("TRUE") : _T("FALSE"));
	}
	if (std::fabs(m_dDefectConditionEllipseRatioMin - newParam.m_dDefectConditionEllipseRatioMin) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionEllipseRatioMin;
		double dNewValue = newParam.m_dDefectConditionEllipseRatioMin;
		strDiff.AppendFormat(_T("장축/단축 비율 최소값:\t\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}
	if (std::fabs(m_dDefectConditionEllipseRatioMax - newParam.m_dDefectConditionEllipseRatioMax) > TOLERANCE)
	{
		double dOldValue = m_dDefectConditionEllipseRatioMax;
		double dNewValue = newParam.m_dDefectConditionEllipseRatioMax;
		strDiff.AppendFormat(_T("장축/단축 비율 최대값:\t\t\t%.3f → %.3f\n"), dOldValue, dNewValue);
	}

	// 마지막 개행 제거
	if (!strDiff.IsEmpty() && strDiff.Right(1) == _T("\n"))
		strDiff = strDiff.Left(strDiff.GetLength() - 1);

	return strDiff;
}