#pragma once

#include "GTRegion.h"

class Algorithm
{
public:
	static Algorithm *m_pInstance;
	static Algorithm *GetInstance(BOOL bShowFlag = FALSE);
	void DeleteInstance();
	Algorithm(void);
	~Algorithm(void);

public:
	BOOL m_pCheckFinishConnection;
	HTuple MaxDefectSize;
	HTuple DefectSize;
	HTuple WhiteChppingDefectSensitive;
	HTuple BlackChppingDefectSensitive;
	HTuple Distance_referValue;

	Hobject m_HAlgoInspectImage[MAX_IMAGE_TAB];
	Hobject InspectBarcodeImage;
	Hobject InspectContour[BARREL_LENS_IMAGE_TAB][MAX_CONTOUR_NUM]; // 24.03.27 - �̹����� Shape ��Ī - LeeGW Start
	HTuple InspectModelID[MAX_MATCHING_MODEL_NUM];
	HTuple m_HInspectLAlignModelID[MAX_IMAGE_TAB];

	int ThreadFinishFlag;
	int m_nModuleNo;

	CString m_sBarcodeResult;
	double m_dBarcodePosOffsetX;
	double m_dBarcodePosOffsetY;
	BOOL m_bBarcodeShiftNG;
	BOOL m_bBarcodeModuleMixNG;

	// Barcode Rotation Log
	BOOL m_bBarcodeRotationNG;
	double m_dBarcodePosOffsetRotation;

	BOOL m_bBlackCoatingDiameterNG;
	double m_dBlackCoatingOuterDiameter;
	double m_dBlackCoatingPosX, m_dBlackCoatingPosY;
	double m_dO1O2Distance, m_dDiameterMin;

	////////////////////////////////// added for CMI3000 2000 ====> ///////////////////////////////////////////////////
	BOOL m_bMatchingSuccess;
	BOOL m_bModuleEmpty;
	BOOL m_bLightDisorder;
	BOOL m_bBarcodeError;

	// 24.07.01 - v2650 - ROI Shift �� ROI ��ü�ϱ� ���� ��Ī Score �߰� - LeeGW Start
	BOOL m_bMatchingContourSuccess[BARREL_LENS_IMAGE_TAB][MAX_CONTOUR_NUM];
	BOOL m_bTotalMatchingSuccess; // 24.07.01 - v2650 - ��ü �̹��� ��Ī ��� Ȯ�� ���� �߰� - LeeGW
	int m_iBlackMatchingImageType;
	// 24.07.01 - v2650 - ROI Shift �� ROI ��ü�ϱ� ���� ��Ī Score �߰� - LeeGW End
	BOOL m_bSpecialNGSortFlag;	 // 25.02.12 - v3008 - Special NG �и����� �߰� - LeeGW
	BOOL m_bAISpecialNGSortFlag; // 25.02.12 - v3008 - Special NG �и����� �߰� - LeeGW

	////////////////////////////////// <==== added for CMI3000 2000 ///////////////////////////////////////////////////
	BOOL m_bDisplayed;
	Hobject m_HdefectBarrel; // Total Barrel Defect Rgn
	Hobject m_HdefectLens;	 // Total Lens Defect Rgn

	Hobject m_HROIInspectAreaRgn; // Sidefill Inspect Area

	Hobject m_HdefectBarrelLens[MAX_IMAGE_TAB]; // 24.06.07 - v2647 - �̹����� �跲/���� ���� �ҷ� ���� �߰� - LeeGW
	Hobject m_HdefectExtra[MAX_IMAGE_TAB];

	// Go
	Hobject m_HDefectDirtRgn;
	Hobject m_HDefectScratchRgn;
	Hobject m_HDefectStainRgn;
	Hobject m_HDefectDentRgn;
	Hobject m_HDefectChippingRgn;
	Hobject m_HDefectEpoxyRgn;
	Hobject m_HDefectOutsideCTRgn;
	Hobject m_HDefectPeelOffRgn;
	Hobject m_HDefectWhiteDotRgn; // 2024.09.30 - v2660 - WhiteDot �߰� - LEEGW
	Hobject m_HDefectLensCTRgn;
	Hobject m_HDefectLensScratchRgn;
	Hobject m_HDefectLensDirtRgn;
	Hobject m_HDefectLensWhiteDotRgn;

	Hobject m_HDefectEpoxyHole[MAX_IMAGE_TAB];	   // LeeGW �߰�
	Hobject m_HDefectNorthSpringNG[MAX_IMAGE_TAB]; // LeeGW �߰�
	Hobject m_HDefectEastSpringNG[MAX_IMAGE_TAB];  // LeeGW �߰�

	Hobject m_HDefectDirtRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectScratchRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectStainRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectDentRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectChippingRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectEpoxyRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectOutsideCTRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectPeelOffRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectWhiteDotRgn_EachImage[MAX_IMAGE_TAB]; // 2024.09.30 - v2660 - WhiteDot �߰� - LEEGW

	// 24.06.07 - v2647 - ���� �ҷ� ����ȭ�� �߰� - LeeGW Start
	Hobject m_HDefectLensDirtRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectLensCTRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectLensScratchRgn_EachImage[MAX_IMAGE_TAB];
	Hobject m_HDefectLensWhiteDotRgn_EachImage[MAX_IMAGE_TAB]; // 2024.09.30 - v2660 - WhiteDot �߰� - LEEGW
	// 24.06.07 - v2647 - ���� �ҷ� ����ȭ�� �߰� - LeeGW End

	Hobject m_HTypeAIDefectRgn[MAX_IMAGE_TAB][MAX_DEFECT_NAME]; // AI �˻� �߰� - LeeGW
	Hobject m_HImageAIDefectRgn[MAX_IMAGE_TAB];					// AI �˻� �߰� - LeeGW

	Hobject m_HMeasureRgn_FAI_Item[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];   // FAI ���� ����
	Hobject m_HDefectRgn_FAI;												   // ������ FAI NG ����
	Hobject m_HReviewXLD_FAI_Item[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE];	   // FAI Review Image ���� ����
	POINT m_iViewportCenter_FAI_Item[MAX_FAI_ITEM][MAX_ONE_FAI_MEASURE_VALUE]; // FAI Review Image Viewport Center Point

	double m_tInspectStartTime;
	double m_tInspectEndTime;

	void Init_AlgorithmInspectParam();
	BOOL MatchingAlgorithm(Hobject HImg1, Hobject HImgRef, Hobject HImg2, Hobject HImg4,
						   Hobject *Contour1, Hobject *Contour2, Hobject *Contour3, Hobject *Contour4, Hobject *Contour5,
						   HTuple *ModelID1, HTuple *ModelID2, HTuple *ModelID3, HTuple *ModelID4, double *pdOutmostContourCenterX, double *pdOutmostContourCenterY);
	BOOL AllignImages(HTuple *ModelID);

	BOOL ImageAlignShapeMatching(Hobject HMatchingImage, Hobject *pHAlignImage, HTuple *plAlignModelID, BOOL *pbAlignImageFlag, double dMatchingAngleRange, double dMatchingScaleMin, double dMatchingScaleMax,
								 double dMatchingScore, double dTeachAlignRefX, double dTeachAlignRefY, int iMatchingMethod, int iMatchingLTX, int iMatchingLTY, int iMatchingRBX, int iMatchingRBY, int iMatchingSearchMarginX, int iMatchingSearchMarginY, BOOL bMatchingApplyAffineConstant,
								 double *pdRotationAngleDeg, double *pdDeltaX, double *pdDeltaY, HTuple *pHMatchingHomMat = NULL);

	BOOL EdgeMeasureAlgorithm(Hobject HImage, Hobject HInspectAreaRgn, CAlgorithmParam Param, Hobject *pHNewInspectAreaRgn);
	BOOL EdgeMeasureAlgorithm(Hobject HImage, Hobject HInspectAreaRgn, CAlgorithmParam Param, Hobject *pHLineFitXLD, int *piFitLineXPos, int *piFitLineYPos, double *pdEdgeCenterX, double *pdEdgeCenterY, double *pdLineStartX, double *pdLineStartY, double *pdLineEndX, double *pdLineEndY, double *dEdgeMeanAmp);

	Hobject Connection_nearRgn(Hobject HConnecedRgn);
	Hobject CutDefectRegionToImage(Hobject HImage, Hobject HRegion);

	BOOL InspectBarcode(Hobject *pHBarcodeImage, CString *psBarcodeResult, double *pdOffsetX, double *pdOffsetY, double *pdOffsetRotation, double dBarcodeShiftRefX, double dBarcodeShiftRefY); // Barcode Roation Inspection Modify

public:
	/// Barrel Surfface Inspection

	BOOL GetInspectArea_Circle(Hobject HContFit, int iOuterMargin, Hobject *pHRgnInspect);
	BOOL GetInspectArea_Lens(Hobject HRgnInspectPre, int iOuterMargin, Hobject *pHRgnInspect);
	BOOL GetInspectArea_Barrel(Hobject HInnerFitLine, Hobject HOuterFitLine, int iInnerMargin, int iOuterMargin, Hobject *pHInspectArea);
	BOOL GetInspectArea_Edge(Hobject HInnerFitLine, Hobject HOuterFitLine, BOOL bInspectInnerEdge, int iInnerMargin_Inside, int iInnerMargin_Outside, BOOL bInspectOuterEdge, int iOuterMargin_Inside, int iOuterMargin_Outside, Hobject *pHInspectArea);
	BOOL GetInspectArea_Inner(Hobject HFitLine, int iInnerMargin, int iOuterMargin, Hobject *pHInspectArea);
	BOOL GetInspectArea_Trans(Hobject HFitLine5, Hobject HFitLine4, Hobject HFitLine3, Hobject HFitLine2,
							  BOOL bInspectTransTop, int iTopInnerMargin, int iTopOuterMargin, BOOL bInspectTransBtm, int iBtmInnerMargin, int iBtmOuterMargin, Hobject *pHInspectArea);

	void InspectCircleFitting(HTuple X, HTuple Y, double dNormalRadius, double &dCenterX, double &dCenterY, double &dRadius);
	void InspectEllipseFitting(HTuple X, HTuple Y, double dNormalRadius, double &dCenterX, double &dCenterY, double &dSemiMajor, double &dSemiMinor, double &dAngle); // LeeGW

	int ApplyInspectionCondition(Hobject Hdefect, Hobject HRgnInspectTop, Hobject HRgnInspectBtm, Hobject HRgnInspectInner, Hobject *HConcatDefect,
								 double dAreaMin_Top, double dAreaMid_Top, double dAreaMax_Top, int iAcceptMin_Top, int iAcceptMid_Top, int iAcceptMax_Top,
								 double dAreaMin_Btm, double dAreaMid_Btm, double dAreaMax_Btm, int iAcceptMin_Btm, int iAcceptMid_Btm, int iAcceptMax_Btm,
								 double dAreaMin_Inner, double dAreaMid_Inner, double dAreaMax_Inner, int iAcceptMin_Inner, int iAcceptMid_Inner, int iAcceptMax_Inner);

public:
	Hobject Make_ROI_LensInner(Hobject HImage, Hobject Contour4, double dLensInner_InnerErosion, double dLensInner_OuterInnerErosion, int iThreshold, int iMargin);
	Hobject Make_ROI_LensCast(Hobject HImage, Hobject Contour4, int dLensInner_InnerErosion, int dLensInner_OuterInnerErosion, int iThreshold, int iMargin);

	int SortingDefectRegion(Hobject HRgnOrg, Hobject *HRgnMin, Hobject *HRgnMid, double dAreaMin, double dAreaMid, double dAreaMax, int iAcceptMin, int iAcceptMid, int iAcceptMax);
	int SortingDefectRegion(Hobject HRgnOrg, Hobject HImgRef, Hobject *HRgnMin, Hobject *HRgnMid, double dAreaMin, double dAreaMid, double dAreaMax, int iAcceptMin, int iAcceptMid, int iAcceptMax, int iAvrBright);
	int SortingDefectRegion(Hobject HRgnOrg, Hobject HImgRef1, Hobject HImgRef2, BOOL bAnd, Hobject *HRgnMin, Hobject *HRgnMid, double dAreaMin, double dAreaMid, double dAreaMax, int iAcceptMin, int iAcceptMid, int iAcceptMax, int iAvrBright);
	int SortingDefectRegion_AreaSum(Hobject Hdefect, Hobject *HDefectRgn, double dAreaMin, double dAreaMax);
	int ApplyLensInspectionCondition(Hobject HDefectRgn, Hobject *pHStainDefectRgn, Hobject *pHScratchDefectRgn, Hobject *pHTinyDefectRgn, Hobject *pHWhiteDotDefectRgn, Hobject LensImageReduced, Hobject *pHLensMinDefectRgn);

	BOOL BlobUnion(Hobject *pInputRgn, long lMergeLength);
	BOOL BlobUnionAngle(Hobject *pInputRgn, long lMergeLength, long lOrientationMinDiameter);
	BOOL BlobLengthWidthCondition(Hobject *pInputRgn, BOOL bUseLengthCondition, BOOL bUseWidthCondition, int iLengthValue, int iWidthValue);

	void CopyInspectInfomation(Hobject *HImageList, Hobject *pHBarcodeImage, Hobject *HContourList, HTuple *HModleIDList, HTuple *pHInspectAlignModelID);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	///
	///   Go �߰� (2016/06/02)
	///
	//////////////////////////////////////////////////////////////////////////
	BOOL GetInspectArea(Hobject HContour1, Hobject HContour2, Hobject HContour3, Hobject HContour4, Hobject HContour5, CAlgorithmParam Param, Hobject *pHInspectArea);
	BOOL GetInspectAreaROI(Hobject HImage, Hobject HInspectAreaRgn, CAlgorithmParam Param, Hobject *pHInspectArea, int iThreadIdx);
	void GetCircleFitInspectArea(Hobject HContour, int iOuterMargin, int iInnerMargin, Hobject *pHInspectArea);
	Hobject CommonAlgorithm(Hobject HImage, Hobject HContour1, Hobject HContour2, Hobject HContour3, Hobject HContour4, Hobject HContour5, CAlgorithmParam Param, Hobject *pHEpoxyDontCareRgn, Hobject *pHImage2 = NULL);
	template <typename T>
	BOOL ValidParam(BOOL bUseParam, T lower, T upper, T max = PARAM_SCALE_MAX) const
	{
		T min = 0;
		return bUseParam &&
			   lower >= min &&
			   upper <= max &&
			   lower <= upper;
	}
	Hobject CommonAlgorithmROI(Hobject HImage, GTRegion *pInspectRgn, CAlgorithmParam Param, Hobject *pHFoundInspectAreaRgn, Hobject *pHResultXld, int iThreadIdx);
	BOOL GetScaledImage(Hobject HImage, Hobject HContour1, Hobject HContour2, Hobject HContour3, Hobject HContour4, Hobject HContour5, CAlgorithmParam Param, Hobject *pHScaledImage);
	BOOL GetScaledImage(Hobject HImage, Hobject HInspectAreaRgn, int iImageScaleMethod, int iImageScaleMax, int iImageScaleMin, Hobject *pHScaledImage);
	BOOL GetScaledImage(Hobject HImage, Hobject HROIRgn, CAlgorithmParam Param, Hobject *pHScaledImage, int iThreadIdx);
	BOOL GetemphasizedImage(Hobject HImage, Hobject *pHShockedImage);
	BOOL GetPreprocessImage(BOOL bUseDomain, Hobject HOrgImage, Hobject HROIRgn, CAlgorithmParam Param, Hobject *pHArithmeticImage);

	// 24.02.29 Local Align �߰� - LeeGW Start
	BOOL ApplyLocalAlignResult(Hobject *pHROIHRegion, CAlgorithmParam AlgorithmParam, int iThreadIdx);
	Hobject AnisoAlignAlgorithm(HTuple pHModelID, Hobject HImage, Hobject HInspectAreaRgn, CAlgorithmParam Param, BOOL *pbShiftResult, int *piLocalAlignDeltaX, int *piLocalAlignDeltaY, double *pdLocalAlignDeltaAngle, double *pdLocalAlignDeltaAngleFixedPointX, double *pdLocalAlignDeltaAngleFixedPointY);
	Hobject PartCheckAlgorithm(HTuple HPartModelID, Hobject HImage, Hobject HROIRgn, CAlgorithmParam Param, int *piLocalAlignDeltaX, int *piLocalAlignDeltaY, double *pdLocalAlignDeltaAngle, double *pdLocalAlignDeltaAngleFixedPointX, double *pdLocalAlignDeltaAngleFixedPointY, Hobject *pHAlignRgn); // LeeGW

	void RemoveDontCare(Hobject *pNewTRegion, CAlgorithmParam Param, int iThreadIdx); // 24.03.29 - v2645 - �˻����ܿ��� �߰� - LeeGW

	BOOL TemplateMatchingAlgorithm(Hobject HImage, Hobject *HContour, HTuple *ModelID); // 24.07.03 - v2651 - ROI Templet Matching Align �߰� - LeeGW

	void InspectFAI(int iTrayNo, int iIndexNo, int iModuleNo);
	BOOL InspectFAIConcent(int iFAINum, double dAxisAngle, HTuple D1CenterY, HTuple D1CenterX, HTuple D2CenterY, HTuple D2CenterX, HTuple LCenterY, HTuple LCenterX, double &dShiftY, double &dShiftX);
	BOOL InspectFAIRotation(int iFAINum, double dAxisAngle, HTuple D1CenterY, HTuple D1CenterX, HTuple D2CenterY, HTuple D2CenterX, double *pdLineStartX, double *pdLineStartY, double *pdLineEndX, double *pdLineEndY, double *pdRotationDegree);
	Hobject InspectFAIPosition(Hobject Contour1, Hobject Contour2, double &dC1PointX, double &dC1PointY, double &dC2PointX, double &dC2PointY, double &dDistanceMin);

	double InspectFAI26(HTuple D1CenterY, HTuple D1CenterX, HTuple D2CenterY, HTuple D2CenterX, HTuple LCenterY, HTuple LCenterX, double &dShiftY, double &dShiftX);
};