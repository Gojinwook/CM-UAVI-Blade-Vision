#pragma once

#include "IniFileCS.h"

#define PARAM_SCALE_MIN (UINT)1
#define PARAM_RATIO_MIN (UINT)1
#define PARAM_SIMILARITY_MIN 0.0

#define PARAM_SCALE_MAX (UINT)100000
#define PARAM_BRIGHT_MAX (UINT)255
#define PARAM_RATIO_MAX 1000.0
#define PARAM_SIMILARITY_MAX 1.0

class CAlgorithmParam
{
public:
	CAlgorithmParam(void);
	virtual ~CAlgorithmParam(void);

	/**
	 * RMS Data Parameters 모음
	 *
	 * Current_Recipe.txt 에 저장되는 파라미터.
	 * Boolean : 11개
	 * Integer : 12개
	 * Double  : 10개
	 * 이상 총 33개의 Parameter로 구성하기로 LGIT와 협의함.
	 */
	struct
	{
		// RMS Data Bollean Variables
		// Auto Parameter update - 250918, jhkim
		// NOTE : 아래 값 중 Min 변수는 파일에 쓰일 때는 "Min" 이 없다. (ex: m_iDefectConditionArea)
		//		: LGIT 측 요구. 이유는 LGIT 측에서 쓸 때는 변수명에 Min 이 없어서 인듯. (251103)
		BOOL m_bDefectConditionXLength;
		BOOL m_bDefectConditionYLength;
		BOOL m_bUseDefectConditionArea;
		BOOL m_bUseDefectConditionLength;
		BOOL m_bUseDefectConditionWidth;
		BOOL m_bUseDefectConditionMean;
		BOOL m_bUseDefectConditionStdev;
		BOOL m_bUseDefectConditionAnisometry;	// 길이/폭 비율 (anisometry : 비등방성)
		BOOL m_bUseDefectConditionCircularity;	// 원형유사도
		BOOL m_bUseDefectConditionConvexity;	// 컨벡스 유사도
		BOOL m_bUseDefectConditionEllipseRatio; // 장축/단축 비율 (ellipse ratio : 타원 비율)
		// RMS Data Integer Variables (Min)
		UINT m_iDefectConditionXLengthMin;
		UINT m_iDefectConditionYLengthMin;
		UINT m_iDefectConditionAreaMin;
		UINT m_iDefectConditionLengthMin;
		UINT m_iDefectConditionWidthMin;
		UINT m_iDefectConditionMeanMin;
		// RMS Data Integer Variables (Max)
		UINT m_iDefectConditionXLengthMax;
		UINT m_iDefectConditionYLengthMax;
		UINT m_iDefectConditionAreaMax;
		UINT m_iDefectConditionLengthMax;
		UINT m_iDefectConditionWidthMax;
		UINT m_iDefectConditionMeanMax;
		// RMS Data Double Variables (Min)
		double m_dDefectConditionStdevMin;
		double m_dDefectConditionAnisometryMin; // 길이/폭 비율 (anisometry : 비등방성)
		double m_dDefectConditionCircularityMin;
		double m_dDefectConditionConvexityMin;
		double m_dDefectConditionEllipseRatioMin; // 장축/단축 비율 (ellipse ratio : 타원 비율)
		// RMS Data Double Variables (Max)
		double m_dDefectConditionStdevMax;
		double m_dDefectConditionAnisometryMax; // 길이/폭 비율 (anisometry : 비등방성)
		double m_dDefectConditionCircularityMax;
		double m_dDefectConditionConvexityMax;
		double m_dDefectConditionEllipseRatioMax; // 장축/단축 비율 (ellipse ratio : 타원 비율)
	};

	// Member Variable
	struct
	{
		BOOL m_bInspect;
		int m_iDefectNameIdx;

		BOOL m_bSpecialNG; // 25.02.12 - v3008 - Special NG 분리배출 추가 - LeeGW

		BOOL m_bUseInspectArea[3];
		int m_iInspectAreaContour1Number[3];
		int m_iInspectAreaContour1Margin[3];
		int m_iInspectAreaContour2Number[3];
		int m_iInspectAreaContour2Margin[3];

		BOOL m_bUseCommonAlgorithm;

		BOOL m_bUseBrightFixedThres;
		int m_iBrightLowerThres;
		int m_iBrightUpperThres;
		BOOL m_bUseBrightDT;
		BOOL m_bUseBrightLineNormalization;
		int m_iBrightMedianFilterSize;
		int m_iBrightClosingFilterSize;
		int m_iBrightDTFilterSize;
		int m_iBrightDTValue;
		BOOL m_bUseBrightUniformityCheck;
		int m_iBrightUniformityOffset;
		int m_iBrightUniformityHystLength;
		int m_iBrightUniformityHystOffset;
		BOOL m_bUseBrightHystThres;
		int m_iBrightHystSecureThres;
		int m_iBrightHystPotentialThres;
		int m_iBrightHystPotentialLength;

		BOOL m_bUseDarkFixedThres;
		int m_iDarkLowerThres;
		int m_iDarkUpperThres;
		BOOL m_bUseDarkDT;
		BOOL m_bUseDarkLineNormalization;
		int m_iDarkMedianFilterSize;
		int m_iDarkClosingFilterSize;
		int m_iDarkDTFilterSize;
		int m_iDarkDTValue;
		BOOL m_bUseDarkUniformityCheck;
		int m_iDarkUniformityOffset;
		int m_iDarkUniformityHystLength;
		int m_iDarkUniformityHystOffset;
		BOOL m_bUseDarkHystThres;
		int m_iDarkHystSecureThres;
		int m_iDarkHystPotentialThres;
		int m_iDarkHystPotentialLength;

		BOOL m_bCombineBrightDarkBlob;
		int m_iOpeningSize;
		int m_iClosingSize;
		BOOL m_bFillup;
		BOOL m_bUseConnection;
		int m_iConnectionMinSize;
		int m_iConnectionLength;
		BOOL m_bUseAngle;
		int m_iConnectionMaxWidth;

		BOOL m_bUseEdge;
		double m_dEdgeZoomRatio;
		int m_iEdgeStrength;
		int m_iEdgeConnectionDistance;
		int m_iEdgeMaxEndpointDistance;
		int m_iEdgeMinContourLength;

		BOOL m_bUseDefectConditionOuterDist;
		BOOL m_bUseDefectConditionInnerDist;
		BOOL m_bUseDefectConditionEpoxyDist;
		int m_iDefectConditionOuterDist;
		int m_iDefectConditionInnerDist;
		int m_iDefectConditionEpoxyDist;

		int m_iEpoxySEThres;
		int m_iEpoxyESThres;
		int m_iEpoxyClosingSize;
		int m_iEpoxyOpeningSize;
		int m_iEpoxyNearDistance;
		int m_iEpoxyNearMinSize;
		BOOL m_bEpoxyConditionHole;
		BOOL m_bEpoxyConditionNear;

		BOOL m_bUseImageScaling;
		int m_iImageScaleMethod;
		int m_iImageScaleMin;
		int m_iImageScaleMax;

		BOOL m_bDefectConditionContrast;
		int m_iDefectConditionContrastMin;
		int m_iDefectConditionContrastMax;

		BOOL m_bUseFTConnected;
		BOOL m_bUseFTConnectedArea;
		BOOL m_bUseFTConnectedLength;
		BOOL m_bUseFTConnectedWidth;
		int m_iFTConnectedAreaMin;
		int m_iFTConnectedLengthMin;
		int m_iFTConnectedWidthMin;

		BOOL m_bUseLineFit;
		int m_iLineFitApplyPos;
		int m_iRoiOffsetTop;
		int m_iRoiOffsetBottom;
		int m_iRoiOffsetLeft;
		int m_iRoiOffsetRight;

		int m_iEdgeMeasureMultiPointNumber;
		BOOL m_bEdgeMeasureEndPoint;
		int m_iEdgeMeasureStartLength;
		int m_iEdgeMeasureEndLength;
		int m_iEdgeMeasureDir;
		int m_iEdgeMeasureGv;
		int m_iEdgeMeasurePos;
		int m_dEdgeMeasureSmFactor;
		int m_iEdgeMeasureEdgeStr;

		BOOL m_bUseApplyDontCare;
		BOOL m_bUseDontCare; // 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW

		int m_iImageType; // 24.07.08 - v2652 - 검사제외영역 영상 번호 파라미터 추가 - LeeGW

		//// Apply
		BOOL m_bUseLocalAlign;
		int m_iLocalAlignImageIndex;
		int m_iLocalAlignROINo;
		int m_iLocalAlignType;
		int m_iLocalAlignApplyMethod;
		BOOL m_bLocalAlignPosX;
		BOOL m_bLocalAlignPosY;
		BOOL m_bLocalAlignPosAngle;
		int m_iLocalAlignFitPos;

		// Aniso Align
		BOOL m_bUseROIAnisoAlign;
		int m_iROIAnisoAlignXSearchMargin;
		int m_iROIAnisoAlignYSearchMargin;
		int m_iROIAnisoAlignMatchingAngleRange;
		double m_dROIAnisoAlignMatchingScaleMinY;
		double m_dROIAnisoAlignMatchingScaleMaxY;
		double m_dROIAnisoAlignMatchingScaleMinX;
		double m_dROIAnisoAlignMatchingScaleMaxX;
		int m_iROIAnisoAlignMatchingMinContrast;
		BOOL m_bROIAnisoAlignUsePosX;
		BOOL m_bROIAnisoAlignUsePosY;
		BOOL m_bROIAnisoAlignInspectShift;
		double m_dROIAnisoAlignShiftTop;
		double m_dROIAnisoAlignShiftBottom;
		double m_dROIAnisoAlignShiftLeft;
		double m_dROIAnisoAlignShiftRight;
		BOOL m_bROIAnisoAlignLocalAlignUse;
		double m_dROIAnisoAlignMatchingScore;
		BOOL m_bROIAnisoAlignModelExist;

		//// Part Check
		BOOL m_bROIPartCheckUse;
		BOOL m_bROIPartCheckLocalAlignUse;
		int m_iROIPartCheckMatchingAngleRange;
		int m_iROIPartCheckMatchingSearchMarginX;
		int m_iROIPartCheckMatchingSearchMarginY;
		int m_iROIPartCheckMatchingSearchMarginX2;
		int m_iROIPartCheckMatchingSearchMarginY2;

		BOOL m_bROIPartCheckExistInspUse;
		int m_iROIPartCheckMatchingScore;
		BOOL m_bROIPartCheckShiftInspUse;
		int m_iROIPartCheckShiftX;
		int m_iROIPartCheckShiftY;
		double m_dROIPartCheckRotationAngle;

		//// Edge Measure
		BOOL m_bROIEdgeMeasureUse;
		BOOL m_bROIEdgeMeasureLocalAlignUse;
		int m_iROIEdgeMeasureMultiPointNumber;
		BOOL m_bROIEdgeMeasureEndPoint;
		int m_iROIEdgeMeasureStartLength;
		int m_iROIEdgeMeasureEndLength;
		int m_iROIEdgeMeasureDir;
		int m_iROIEdgeMeasureGv;
		int m_iROIEdgeMeasurePos;
		double m_dROIEdgeMeasureSmFactor;
		int m_iROIEdgeMeasureEdgeStr;
		double m_dROIEdgeMeasureRotateAngle;

		// Image Filter
		BOOL m_bUseImageProcessFilter;
		int m_iImageProcessFilterType1;
		int m_iImageProcessFilterType2;
		int m_iImageProcessFilterType3;
		double m_iImageProcessFilterType1X;
		double m_iImageProcessFilterType2X;
		double m_iImageProcessFilterType3X;
		double m_iImageProcessFilterType1Y;
		double m_iImageProcessFilterType2Y;
		double m_iImageProcessFilterType3Y;
	};

	// 240227 Local Align 추가 - LeeGW End

public:
	// Member Function
	void Save(CIniFileCS INI, CString strSection, CString sCommonName);
	void Load(CIniFileCS INI, CString strSection, CString sCommonName);
	void Reset();

	// 240227 Local Align 추가 - LeeGW Start
	void Save(HANDLE hFile);
	BOOL Load(HANDLE hFile, int iModelVersion);
	// Local Align Param

	void CopyFrom(const CAlgorithmParam &src);

	// 두 객체를 비교하여 다른 변수명을 문자열로 반환
	CString GetDifferentVariables(const CAlgorithmParam &other) const;
	
	CAlgorithmParam &operator=(CAlgorithmParam &data);
	BOOL operator==(const CAlgorithmParam &other) const
	{
		// GetDifferentVariables를 활용하여 차이가 있는지 확인
		return GetDifferentVariables(other).IsEmpty();
	}
	// 비동등 비교 연산자 (!=) 오버로딩 (== 연산자 재활용)
	bool operator!=(const CAlgorithmParam &other) const	{return !(*this == other);}
};
