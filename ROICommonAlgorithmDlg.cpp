// ROIInspAlgorithmDlg.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "uScan.h"
#include "ROICommonAlgorithmDlg.h"
#include "afxdialogex.h"


// CROIInspAlgorithmDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CROICommonAlgorithmDlg, CDialog)

void CROICommonAlgorithmDlg::Show()
{
	ShowWindow(SW_SHOW);
}

void CROICommonAlgorithmDlg::Hide()
{
	ShowWindow(SW_HIDE);
}

CROICommonAlgorithmDlg::CROICommonAlgorithmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CROICommonAlgorithmDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_iInspectionType = 0;
	m_pSelectedROI = NULL;

	m_bCheckUseCommonAlgorithm = FALSE;
	m_bCheckUseBrightFixedThres = FALSE;
	m_iEditBrightLowerThres = 0;
	m_iEditBrightUpperThres = 0;
	m_bCheckUseBrightDT = FALSE;
	m_iEditBrightMedianFilterSize = 0;
	m_iEditBrightClosingFilterSize = 0;
	m_iEditBrightDTFilterSize = 0;
	m_iEditBrightDTValue = 0;
	m_bCheckUseBrightUniformityCheck = FALSE;
	m_iEditBrightUniformityOffset = 0;
	m_iEditBrightUniformityHystLength = 0;
	m_iEditBrightUniformityHystOffset = 0;
	m_bCheckUseBrightHystThres = FALSE;
	m_iEditBrightHystSecureThres = 0;
	m_iEditBrightHystPotentialThres = 0;
	m_iEditBrightHystPotentialLength = 0;

	m_bCheckUseDarkFixedThres = FALSE;
	m_iEditDarkLowerThres = 0;
	m_iEditDarkUpperThres = 0;
	m_bCheckUseDarkDT = FALSE;
	m_iEditDarkMedianFilterSize = 0;
	m_iEditDarkClosingFilterSize = 0;
	m_iEditDarkDTFilterSize = 0;
	m_iEditDarkDTValue = 0;
	m_bCheckUseDarkUniformityCheck = FALSE;
	m_iEditDarkUniformityOffset = 0;
	m_iEditDarkUniformityHystLength = 0;
	m_iEditDarkUniformityHystOffset = 0;
	m_bCheckUseDarkHystThres = FALSE;
	m_iEditDarkHystSecureThres = 0;
	m_iEditDarkHystPotentialThres = 0;
	m_iEditDarkHystPotentialLength = 0;

	m_bCheckCombineBrightDarkBlob = FALSE;
	m_iEditOpeningSize = 0;
	m_iEditClosingSize = 0;
	m_bCheckFillup = FALSE;
	m_bCheckUseConnection = FALSE;
	m_iEditConnectionMinSize = 0;
	m_iEditConnectionLength = 0;
	m_bCheckUseAngle = FALSE;
	m_iEditConnectionMaxWidth = 0;

	m_bCheckUseEdge = FALSE;
	m_dEditEdgeZoomRatio = 0.0;
	m_iEditEdgeStrength = 0;
	m_iEditEdgeConnectionDistance = 0;
	m_iEditEdgeMaxEndpointDistance = 0;
	m_iEditEdgeMinContourLength = 0;

	m_iEditBrightMedianFilterSize = FALSE;
	m_bCheckDefectConditionWidth = FALSE;
	m_bCheckDefectConditionMean = FALSE;
	m_bCheckDefectConditionStdev = FALSE;
	m_bCheckDefectConditionAnisometry = FALSE;
	m_bCheckDefectConditionCircularity = FALSE;
	m_bCheckDefectConditionConvexity = FALSE;
	m_bCheckDefectConditionEllipseRatio = FALSE;
	m_bCheckDefectConditionOuterDist = FALSE;
	m_bCheckDefectConditionInnerDist = FALSE;

	m_iEditDefectConditionAreaMin = 0;
	m_iEditDefectConditionLengthMin = 0;
	m_iEditDefectConditionWidthMin = 0;
	m_iEditDefectConditionMeanMin = 0;
	m_dEditDefectConditionStdevMin = 0;
	m_dEditDefectConditionAnisometryMin = 0;
	m_dEditDefectConditionCircularityMin = 0;
	m_dEditDefectConditionConvexityMin = 0;
	m_dEditDefectConditionEllipseRatioMin = 0;

	m_iEditDefectConditionAreaMax = 0;
	m_iEditDefectConditionLengthMax = 0;
	m_iEditDefectConditionWidthMax = 0;
	m_iEditDefectConditionMeanMax = 0;
	m_dEditDefectConditionStdevMax = 0;
	m_dEditDefectConditionAnisometryMax = 0;
	m_dEditDefectConditionCircularityMax = 0;
	m_dEditDefectConditionConvexityMax = 0;
	m_dEditDefectConditionEllipseRatioMax = 0;

	m_iEditDefectConditionOuterDist = 0;
	m_iEditDefectConditionInnerDist = 0;

	m_bCheckUseBrightLineNormalization = FALSE;
	m_bCheckUseDarkLineNormalization = FALSE;

	m_bCheckUseImageScaling = FALSE;
	m_iRadioImageScaleMethod = 0;
	m_iEditImageScaleMin = 0;
	m_iEditImageScaleMax = 0;

	m_bCheckDefectConditionXLength = FALSE;
	m_bCheckDefectConditionYLength = FALSE;
	m_bCheckDefectConditionContrast = FALSE;
	m_iEditDefectConditionXLengthMin = 0;
	m_iEditDefectConditionYLengthMin = 0;
	m_iEditDefectConditionContrastMin = 10;
	m_iEditDefectConditionXLengthMax = 100;
	m_iEditDefectConditionYLengthMax = 100;
	m_iEditDefectConditionContrastMax = 255;

	m_bCheckUseFTConnected = FALSE;
	m_bCheckUseFTConnectedArea = TRUE;
	m_bCheckUseFTConnectedLength = FALSE;
	m_bCheckUseFTConnectedWidth = FALSE;
	m_iEditFTConnectedAreaMin = 100;
	m_iEditFTConnectedLengthMin = 10;
	m_iEditFTConnectedWidthMin = 5;

	m_bCheckUseLineFit = FALSE;
	m_iRadioLineFitApplyPos = 0;
	m_iEditRoiOffsetTop = 0;
	m_iEditRoiOffsetBottom = 0;
	m_iEditRoiOffsetLeft = 0;
	m_iEditRoiOffsetRight = 0;

	m_bCheckUseApplyDontCare = FALSE;	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW
	m_bCheckUseDontCare = FALSE;	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW
}

CROICommonAlgorithmDlg::~CROICommonAlgorithmDlg()
{
}

void CROICommonAlgorithmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_COMMON_ALGORITHM, m_bCheckUseCommonAlgorithm);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_FIXED_THRESHOLD, m_bCheckUseBrightFixedThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_LOWER_THRES, m_iEditBrightLowerThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UPPER_THRES, m_iEditBrightUpperThres);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_DT, m_bCheckUseBrightDT);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_MED_FILTER_SIZE, m_iEditBrightMedianFilterSize);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_CLOSING_FILTER_SIZE, m_iEditBrightClosingFilterSize);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_FILTER_SIZE, m_iEditBrightDTFilterSize);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_DT_VALUE, m_iEditBrightDTValue);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_UNIFORMITY_CHECK, m_bCheckUseBrightUniformityCheck);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_OFFSET, m_iEditBrightUniformityOffset);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_HYST_LENGTH, m_iEditBrightUniformityHystLength);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_UNIFORMITY_HYST_OFFSET, m_iEditBrightUniformityHystOffset);
	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_HYST, m_bCheckUseBrightHystThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_SECURE_THRES, m_iEditBrightHystSecureThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_POTENTIAL_THRES, m_iEditBrightHystPotentialThres);
	DDX_Text(pDX, IDC_EDIT_BRIGHT_HYST_POTENTIAL_LENGTH, m_iEditBrightHystPotentialLength);

	DDX_Check(pDX, IDC_CHECK_USE_DARK_FIXED_THRESHOLD, m_bCheckUseDarkFixedThres);
	DDX_Text(pDX, IDC_EDIT_DARK_LOWER_THRES, m_iEditDarkLowerThres);
	DDX_Text(pDX, IDC_EDIT_DARK_UPPER_THRES, m_iEditDarkUpperThres);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_DT, m_bCheckUseDarkDT);
	DDX_Text(pDX, IDC_EDIT_DARK_MED_FILTER_SIZE, m_iEditDarkMedianFilterSize);
	DDX_Text(pDX, IDC_EDIT_DARK_CLOSING_FILTER_SIZE, m_iEditDarkClosingFilterSize);
	DDX_Text(pDX, IDC_EDIT_DARK_DT_FILTER_SIZE, m_iEditDarkDTFilterSize);
	DDX_Text(pDX, IDC_EDIT_DARK_DT_VALUE, m_iEditDarkDTValue);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_UNIFORMITY_CHECK, m_bCheckUseDarkUniformityCheck);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_OFFSET, m_iEditDarkUniformityOffset);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_HYST_LENGTH, m_iEditDarkUniformityHystLength);
	DDX_Text(pDX, IDC_EDIT_DARK_UNIFORMITY_HYST_OFFSET, m_iEditDarkUniformityHystOffset);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_HYST, m_bCheckUseDarkHystThres);
	DDX_Text(pDX, IDC_EDIT_DARK_HYST_SECURE_THRES, m_iEditDarkHystSecureThres);
	DDX_Text(pDX, IDC_EDIT_DARK_HYST_POTENTIAL_THRES, m_iEditDarkHystPotentialThres);
	DDX_Text(pDX, IDC_EDIT_DARK_HYST_POTENTIAL_LENGTH, m_iEditDarkHystPotentialLength);

	DDX_Check(pDX, IDC_CHECK_COMBINE_BLOB, m_bCheckCombineBrightDarkBlob);
	DDX_Text(pDX, IDC_EDIT_OPENING_SIZE, m_iEditOpeningSize);
	DDX_Text(pDX, IDC_EDIT_CLOSING_SIZE, m_iEditClosingSize);
	DDX_Check(pDX, IDC_CHECK_FILL_UP, m_bCheckFillup);
	DDX_Check(pDX, IDC_CHECK_USE_CONNECTION, m_bCheckUseConnection);
	DDX_Text(pDX, IDC_EDIT_CONNECT_MIN_SIZE, m_iEditConnectionMinSize);
	DDX_Text(pDX, IDC_EDIT_CONNECT_LENGTH, m_iEditConnectionLength);
	DDX_Check(pDX, IDC_CHECK_USE_ANGLE, m_bCheckUseAngle);
	DDX_Text(pDX, IDC_EDIT_CONNECT_MAX_WIDTH, m_iEditConnectionMaxWidth);

	DDX_Check(pDX, IDC_CHECK_USE_EDGE, m_bCheckUseEdge);
	DDX_Text(pDX, IDC_EDIT_EDGE_ZOOM_RATIO, m_dEditEdgeZoomRatio);
	DDX_Text(pDX, IDC_EDIT_EDGE_STRENGTH, m_iEditEdgeStrength);
	DDX_Text(pDX, IDC_EDIT_EDGE_CONNECTION_DISTANCE, m_iEditEdgeConnectionDistance);
	DDX_Text(pDX, IDC_EDIT_EDGE_MAX_ENDPOINT_DISTANCE, m_iEditEdgeMaxEndpointDistance);
	DDX_Text(pDX, IDC_EDIT_EDGE_MIN_CONTOUR_LENGTH, m_iEditEdgeMinContourLength);

	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_AREA, m_bCheckDefectConditionArea);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_LENGTH, m_bCheckDefectConditionLength);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_WIDTH, m_bCheckDefectConditionWidth);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_MEAN, m_bCheckDefectConditionMean);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_STDEV, m_bCheckDefectConditionStdev);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_ANISOMETRY, m_bCheckDefectConditionAnisometry);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_CIRCULARITY, m_bCheckDefectConditionCircularity);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_CONVEXITY, m_bCheckDefectConditionConvexity);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_ELLIPSE_RATIO, m_bCheckDefectConditionEllipseRatio);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_OUTER_DIST, m_bCheckDefectConditionOuterDist);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_INNER_DIST, m_bCheckDefectConditionInnerDist);

	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_AREA_MIN, m_iEditDefectConditionAreaMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_LENGTH_MIN, m_iEditDefectConditionLengthMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_WIDTH_MIN, m_iEditDefectConditionWidthMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_MEAN_MIN, m_iEditDefectConditionMeanMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_STDEV_MIN, m_dEditDefectConditionStdevMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_ANISOMETRY_MIN, m_dEditDefectConditionAnisometryMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CIRCULARITY_MIN, m_dEditDefectConditionCircularityMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CONVEXITY_MIN, m_dEditDefectConditionConvexityMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_ELLIPSE_RATIO_MIN, m_dEditDefectConditionEllipseRatioMin);

	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_AREA_MAX, m_iEditDefectConditionAreaMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_LENGTH_MAX, m_iEditDefectConditionLengthMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_WIDTH_MAX, m_iEditDefectConditionWidthMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_MEAN_MAX, m_iEditDefectConditionMeanMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_STDEV_MAX, m_dEditDefectConditionStdevMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_ANISOMETRY_MAX, m_dEditDefectConditionAnisometryMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CIRCULARITY_MAX, m_dEditDefectConditionCircularityMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CONVEXITY_MAX, m_dEditDefectConditionConvexityMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_ELLIPSE_RATIO_MAX, m_dEditDefectConditionEllipseRatioMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_OUTER_DIST, m_iEditDefectConditionOuterDist);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_INNER_DIST, m_iEditDefectConditionInnerDist);

	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_LINE_GRAY_NORMALIZATION, m_bCheckUseBrightLineNormalization);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_LINE_GRAY_NORMALIZATION, m_bCheckUseDarkLineNormalization);

	DDX_Check(pDX, IDC_CHECK_USE_IMAGE_SCALING, m_bCheckUseImageScaling);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_SCALE_AUTO, m_iRadioImageScaleMethod);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SCAN_MIN, m_iEditImageScaleMin);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SCAN_MAX, m_iEditImageScaleMax);

	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_X_LENGTH, m_bCheckDefectConditionXLength);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_Y_LENGTH, m_bCheckDefectConditionYLength);
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_CONTRAST, m_bCheckDefectConditionContrast);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_X_LENGTH_MIN, m_iEditDefectConditionXLengthMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_Y_LENGTH_MIN, m_iEditDefectConditionYLengthMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CONTRAST_MIN, m_iEditDefectConditionContrastMin);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_X_LENGTH_MAX, m_iEditDefectConditionXLengthMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_Y_LENGTH_MAX, m_iEditDefectConditionYLengthMax);
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_CONTRAST_MAX, m_iEditDefectConditionContrastMax);

	DDX_Check(pDX, IDC_CHECK_USE_FT_CONNECTED, m_bCheckUseFTConnected);
	DDX_Check(pDX, IDC_CHECK_FT_CONNECTED_AREA, m_bCheckUseFTConnectedArea);
	DDX_Check(pDX, IDC_CHECK_FT_CONNECTED_LENGTH, m_bCheckUseFTConnectedLength);
	DDX_Check(pDX, IDC_CHECK_FT_CONNECTED_WIDTH, m_bCheckUseFTConnectedWidth);
	DDX_Text(pDX, IDC_EDIT_FT_CONNECTED_AREA_MIN, m_iEditFTConnectedAreaMin);
	DDX_Text(pDX, IDC_EDIT_FT_CONNECTED_LENGTH_MIN, m_iEditFTConnectedLengthMin);
	DDX_Text(pDX, IDC_EDIT_FT_CONNECTED_WIDTH_MIN, m_iEditFTConnectedWidthMin);

	DDX_Check(pDX, IDC_CHECK_USE_LINE_FIT, m_bCheckUseLineFit);
	DDX_Radio(pDX, IDC_RADIO_LINE_FIT_APPLY_TOP, m_iRadioLineFitApplyPos);
	DDX_Text(pDX, IDC_EDIT_ROI_OFFSET_TOP, m_iEditRoiOffsetTop);
	DDX_Text(pDX, IDC_EDIT_ROI_OFFSET_BOTTOM, m_iEditRoiOffsetBottom);
	DDX_Text(pDX, IDC_EDIT_ROI_OFFSET_LEFT, m_iEditRoiOffsetLeft);
	DDX_Text(pDX, IDC_EDIT_ROI_OFFSET_RIGHT, m_iEditRoiOffsetRight);

	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW Strat
	DDX_Check(pDX, IDC_CHECK_USE_APPLY_DONT_CARE, m_bCheckUseApplyDontCare);
	DDX_Check(pDX, IDC_CHECK_USE_DONT_CARE, m_bCheckUseDontCare);
	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW End

	DDX_Control(pDX, IDC_BUTTON_DISPLAY_SCALED_IMAGE, m_bnDisplayScaledImage);
	DDX_Control(pDX, IDC_BUTTON_SET_LINE_FIT, m_bnSetLineFit);
}


BEGIN_MESSAGE_MAP(CROICommonAlgorithmDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_AREA_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonAreaMin)
	ON_BN_CLICKED(IDC_BUTTON_AREA_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonAreaMax)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonWidthMin)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonWidthMax)
	ON_BN_CLICKED(IDC_BUTTON_MEAN_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonMeanMin)
	ON_BN_CLICKED(IDC_BUTTON_MEAN_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonMeanMax)
	ON_BN_CLICKED(IDC_BUTTON_STDEV_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonStdevMin)
	ON_BN_CLICKED(IDC_BUTTON_STDEV_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonStdevMax)
	ON_BN_CLICKED(IDC_BUTTON_ANISOMETRY_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonAnisometryMin)
	ON_BN_CLICKED(IDC_BUTTON_ANISOMETRY_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonAnisometryMax)
	ON_BN_CLICKED(IDC_BUTTON_CIRCULARITY_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonCircularityMin)
	ON_BN_CLICKED(IDC_BUTTON_CIRCULARITY_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonCircularityMax)
	ON_BN_CLICKED(IDC_BUTTON_CONVEXITY_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonConvexityMin)
	ON_BN_CLICKED(IDC_BUTTON_CONVEXITY_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonConvexityMax)
	ON_BN_CLICKED(IDC_BUTTON_ELLIPSE_RATIO_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonEllipseRatioMin)
	ON_BN_CLICKED(IDC_BUTTON_ELLIPSE_RATIO_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonEllipseRatioMax)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY_SCALED_IMAGE, &CROICommonAlgorithmDlg::OnBnClickedButtonDisplayScaledImage)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonXLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonXLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonYLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonYLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_MIN, &CROICommonAlgorithmDlg::OnBnClickedButtonContrastMin)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_MAX, &CROICommonAlgorithmDlg::OnBnClickedButtonContrastMax)
	ON_BN_CLICKED(IDC_BUTTON_SET_LINE_FIT, &CROICommonAlgorithmDlg::OnBnClickedButtonSetLineFit)
END_MESSAGE_MAP()


// CROIInspAlgorithmDlg 메시지 처리기입니다.


BOOL CROICommonAlgorithmDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_NCLBUTTONDOWN:
		SetActiveWindow();
		return FALSE;

	case WM_KEYDOWN:
		if ((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}

	case WM_SYSKEYDOWN:
		if (pMsg->wParam == VK_F4)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CROICommonAlgorithmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bnDisplayScaledImage.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnDisplayScaledImage.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, OLIVE_GREEN, 0, 0);
	m_bnSetLineFit.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, OLIVE_GREEN, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CROICommonAlgorithmDlg::SetSelectedInspection(int iImageType, GTRegion* pSelectedROI, int iTabIndex)
{
	m_iSelectedImageType = iImageType;
	m_iInspectionType = iTabIndex;
	m_pSelectedROI = pSelectedROI;

	copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType], &m_HOrgScanImage);
}

void CROICommonAlgorithmDlg::SetParam(CAlgorithmParam pROIAlgorithmParam)
{
	m_bCheckUseCommonAlgorithm = pROIAlgorithmParam.m_bUseCommonAlgorithm;

	m_bCheckUseBrightFixedThres = pROIAlgorithmParam.m_bUseBrightFixedThres;
	m_iEditBrightLowerThres =pROIAlgorithmParam.m_iBrightLowerThres;
	m_iEditBrightUpperThres =pROIAlgorithmParam.m_iBrightUpperThres;
	m_bCheckUseBrightDT =pROIAlgorithmParam.m_bUseBrightDT;
	m_bCheckUseBrightLineNormalization =pROIAlgorithmParam.m_bUseBrightLineNormalization;
	m_iEditBrightMedianFilterSize =pROIAlgorithmParam.m_iBrightMedianFilterSize;
	m_iEditBrightClosingFilterSize =pROIAlgorithmParam.m_iBrightClosingFilterSize;
	m_iEditBrightDTFilterSize =pROIAlgorithmParam.m_iBrightDTFilterSize;
	m_iEditBrightDTValue =pROIAlgorithmParam.m_iBrightDTValue;
	m_bCheckUseBrightUniformityCheck =pROIAlgorithmParam.m_bUseBrightUniformityCheck;
	m_iEditBrightUniformityOffset =pROIAlgorithmParam.m_iBrightUniformityOffset;
	m_iEditBrightUniformityHystLength =pROIAlgorithmParam.m_iBrightUniformityHystLength;
	m_iEditBrightUniformityHystOffset =pROIAlgorithmParam.m_iBrightUniformityHystOffset;
	m_bCheckUseBrightHystThres =pROIAlgorithmParam.m_bUseBrightHystThres;
	m_iEditBrightHystSecureThres =pROIAlgorithmParam.m_iBrightHystSecureThres;
	m_iEditBrightHystPotentialThres =pROIAlgorithmParam.m_iBrightHystPotentialThres;
	m_iEditBrightHystPotentialLength =pROIAlgorithmParam.m_iBrightHystPotentialLength;

	m_bCheckUseDarkFixedThres =pROIAlgorithmParam.m_bUseDarkFixedThres;
	m_iEditDarkLowerThres =pROIAlgorithmParam.m_iDarkLowerThres;
	m_iEditDarkUpperThres =pROIAlgorithmParam.m_iDarkUpperThres;
	m_bCheckUseDarkDT =pROIAlgorithmParam.m_bUseDarkDT;
	m_bCheckUseDarkLineNormalization =pROIAlgorithmParam.m_bUseDarkLineNormalization;
	m_iEditDarkMedianFilterSize =pROIAlgorithmParam.m_iDarkMedianFilterSize;
	m_iEditDarkClosingFilterSize =pROIAlgorithmParam.m_iDarkClosingFilterSize;
	m_iEditDarkDTFilterSize =pROIAlgorithmParam.m_iDarkDTFilterSize;
	m_iEditDarkDTValue =pROIAlgorithmParam.m_iDarkDTValue;
	m_bCheckUseDarkUniformityCheck =pROIAlgorithmParam.m_bUseDarkUniformityCheck;
	m_iEditDarkUniformityOffset =pROIAlgorithmParam.m_iDarkUniformityOffset;
	m_iEditDarkUniformityHystLength =pROIAlgorithmParam.m_iDarkUniformityHystLength;
	m_iEditDarkUniformityHystOffset =pROIAlgorithmParam.m_iDarkUniformityHystOffset;
	m_bCheckUseDarkHystThres =pROIAlgorithmParam.m_bUseDarkHystThres;
	m_iEditDarkHystSecureThres =pROIAlgorithmParam.m_iDarkHystSecureThres;
	m_iEditDarkHystPotentialThres =pROIAlgorithmParam.m_iDarkHystPotentialThres;
	m_iEditDarkHystPotentialLength =pROIAlgorithmParam.m_iDarkHystPotentialLength;

	m_bCheckCombineBrightDarkBlob =pROIAlgorithmParam.m_bCombineBrightDarkBlob;
	m_iEditOpeningSize =pROIAlgorithmParam.m_iOpeningSize;
	m_iEditClosingSize =pROIAlgorithmParam.m_iClosingSize;
	m_bCheckFillup =pROIAlgorithmParam.m_bFillup;
	m_bCheckUseConnection =pROIAlgorithmParam.m_bUseConnection;
	m_iEditConnectionMinSize =pROIAlgorithmParam.m_iConnectionMinSize;
	m_iEditConnectionLength =pROIAlgorithmParam.m_iConnectionLength;
	m_bCheckUseAngle =pROIAlgorithmParam.m_bUseAngle;
	m_iEditConnectionMaxWidth =pROIAlgorithmParam.m_iConnectionMaxWidth;

	m_bCheckUseEdge =pROIAlgorithmParam.m_bUseEdge;
	m_dEditEdgeZoomRatio =pROIAlgorithmParam.m_dEdgeZoomRatio;
	m_iEditEdgeStrength =pROIAlgorithmParam.m_iEdgeStrength;
	m_iEditEdgeConnectionDistance =pROIAlgorithmParam.m_iEdgeConnectionDistance;
	m_iEditEdgeMaxEndpointDistance =pROIAlgorithmParam.m_iEdgeMaxEndpointDistance;
	m_iEditEdgeMinContourLength =pROIAlgorithmParam.m_iEdgeMinContourLength;

	m_bCheckDefectConditionArea =pROIAlgorithmParam.m_bUseDefectConditionArea;
	m_bCheckDefectConditionLength =pROIAlgorithmParam.m_bUseDefectConditionLength;
	m_bCheckDefectConditionWidth =pROIAlgorithmParam.m_bUseDefectConditionWidth;
	m_bCheckDefectConditionMean =pROIAlgorithmParam.m_bUseDefectConditionMean;
	m_bCheckDefectConditionStdev =pROIAlgorithmParam.m_bUseDefectConditionStdev;
	m_bCheckDefectConditionAnisometry =pROIAlgorithmParam.m_bUseDefectConditionAnisometry;
	m_bCheckDefectConditionCircularity =pROIAlgorithmParam.m_bUseDefectConditionCircularity;
	m_bCheckDefectConditionConvexity =pROIAlgorithmParam.m_bUseDefectConditionConvexity;
	m_bCheckDefectConditionEllipseRatio =pROIAlgorithmParam.m_bUseDefectConditionEllipseRatio;
	m_bCheckDefectConditionOuterDist =pROIAlgorithmParam.m_bUseDefectConditionOuterDist;
	m_bCheckDefectConditionInnerDist =pROIAlgorithmParam.m_bUseDefectConditionInnerDist;
	m_bCheckDefectConditionEpoxyDist =pROIAlgorithmParam.m_bUseDefectConditionEpoxyDist;

	m_iEditDefectConditionAreaMin =pROIAlgorithmParam.m_iDefectConditionAreaMin;
	m_iEditDefectConditionLengthMin =pROIAlgorithmParam.m_iDefectConditionLengthMin;
	m_iEditDefectConditionWidthMin =pROIAlgorithmParam.m_iDefectConditionWidthMin;
	m_iEditDefectConditionMeanMin =pROIAlgorithmParam.m_iDefectConditionMeanMin;
	m_dEditDefectConditionStdevMin =pROIAlgorithmParam.m_dDefectConditionStdevMin;
	m_dEditDefectConditionAnisometryMin =pROIAlgorithmParam.m_dDefectConditionAnisometryMin;
	m_dEditDefectConditionCircularityMin =pROIAlgorithmParam.m_dDefectConditionCircularityMin;
	m_dEditDefectConditionConvexityMin =pROIAlgorithmParam.m_dDefectConditionConvexityMin;
	m_dEditDefectConditionEllipseRatioMin =pROIAlgorithmParam.m_dDefectConditionEllipseRatioMin;

	m_iEditDefectConditionAreaMax =pROIAlgorithmParam.m_iDefectConditionAreaMax;
	m_iEditDefectConditionLengthMax =pROIAlgorithmParam.m_iDefectConditionLengthMax;
	m_iEditDefectConditionWidthMax =pROIAlgorithmParam.m_iDefectConditionWidthMax;
	m_iEditDefectConditionMeanMax =pROIAlgorithmParam.m_iDefectConditionMeanMax;
	m_dEditDefectConditionStdevMax =pROIAlgorithmParam.m_dDefectConditionStdevMax;
	m_dEditDefectConditionAnisometryMax =pROIAlgorithmParam.m_dDefectConditionAnisometryMax;
	m_dEditDefectConditionCircularityMax =pROIAlgorithmParam.m_dDefectConditionCircularityMax;
	m_dEditDefectConditionConvexityMax =pROIAlgorithmParam.m_dDefectConditionConvexityMax;
	m_dEditDefectConditionEllipseRatioMax =pROIAlgorithmParam.m_dDefectConditionEllipseRatioMax;

	m_iEditDefectConditionOuterDist =pROIAlgorithmParam.m_iDefectConditionOuterDist;
	m_iEditDefectConditionInnerDist =pROIAlgorithmParam.m_iDefectConditionInnerDist;

	m_bCheckUseImageScaling =pROIAlgorithmParam.m_bUseImageScaling;
	m_iRadioImageScaleMethod =pROIAlgorithmParam.m_iImageScaleMethod;
	m_iEditImageScaleMin =pROIAlgorithmParam.m_iImageScaleMin;
	m_iEditImageScaleMax =pROIAlgorithmParam.m_iImageScaleMax;

	m_bCheckDefectConditionXLength =pROIAlgorithmParam.m_bDefectConditionXLength;
	m_bCheckDefectConditionYLength =pROIAlgorithmParam.m_bDefectConditionYLength;
	m_bCheckDefectConditionContrast =pROIAlgorithmParam.m_bDefectConditionContrast;
	m_iEditDefectConditionXLengthMin =pROIAlgorithmParam.m_iDefectConditionXLengthMin;
	m_iEditDefectConditionYLengthMin =pROIAlgorithmParam.m_iDefectConditionYLengthMin;
	m_iEditDefectConditionContrastMin =pROIAlgorithmParam.m_iDefectConditionContrastMin;
	m_iEditDefectConditionXLengthMax =pROIAlgorithmParam.m_iDefectConditionXLengthMax;
	m_iEditDefectConditionYLengthMax =pROIAlgorithmParam.m_iDefectConditionYLengthMax;
	m_iEditDefectConditionContrastMax =pROIAlgorithmParam.m_iDefectConditionContrastMax;

	m_bCheckUseFTConnected =pROIAlgorithmParam.m_bUseFTConnected;
	m_bCheckUseFTConnectedArea =pROIAlgorithmParam.m_bUseFTConnectedArea;
	m_bCheckUseFTConnectedLength =pROIAlgorithmParam.m_bUseFTConnectedLength;
	m_bCheckUseFTConnectedWidth =pROIAlgorithmParam.m_bUseFTConnectedWidth;
	m_iEditFTConnectedAreaMin =pROIAlgorithmParam.m_iFTConnectedAreaMin;
	m_iEditFTConnectedLengthMin =pROIAlgorithmParam.m_iFTConnectedLengthMin;
	m_iEditFTConnectedWidthMin =pROIAlgorithmParam.m_iFTConnectedWidthMin;

	m_bCheckUseLineFit =pROIAlgorithmParam.m_bUseLineFit;
	m_iRadioLineFitApplyPos =pROIAlgorithmParam.m_iLineFitApplyPos;
	m_iEditRoiOffsetTop =pROIAlgorithmParam.m_iRoiOffsetTop;
	m_iEditRoiOffsetBottom =pROIAlgorithmParam.m_iRoiOffsetBottom;
	m_iEditRoiOffsetLeft =pROIAlgorithmParam.m_iRoiOffsetLeft;
	m_iEditRoiOffsetRight =pROIAlgorithmParam.m_iRoiOffsetRight;

	m_bCheckUseApplyDontCare =pROIAlgorithmParam.m_bUseApplyDontCare;
	m_bCheckUseDontCare =pROIAlgorithmParam.m_bUseDontCare;	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW

	UpdateData(FALSE);
}

//0531
void CROICommonAlgorithmDlg::GetParam(CAlgorithmParam* pROIAlgorithmParam)
{
	UpdateData();

	pROIAlgorithmParam->m_bUseCommonAlgorithm = m_bCheckUseCommonAlgorithm;
	pROIAlgorithmParam->m_bUseBrightFixedThres = m_bCheckUseBrightFixedThres;
	pROIAlgorithmParam->m_iBrightLowerThres = m_iEditBrightLowerThres;
	pROIAlgorithmParam->m_iBrightUpperThres = m_iEditBrightUpperThres;
	pROIAlgorithmParam->m_bUseBrightDT = m_bCheckUseBrightDT;
	pROIAlgorithmParam->m_bUseBrightLineNormalization = m_bCheckUseBrightLineNormalization;
	pROIAlgorithmParam->m_iBrightMedianFilterSize = m_iEditBrightMedianFilterSize;
	pROIAlgorithmParam->m_iBrightClosingFilterSize = m_iEditBrightClosingFilterSize;
	pROIAlgorithmParam->m_iBrightDTFilterSize = m_iEditBrightDTFilterSize;
	pROIAlgorithmParam->m_iBrightDTValue = m_iEditBrightDTValue;
	pROIAlgorithmParam->m_bUseBrightUniformityCheck = m_bCheckUseBrightUniformityCheck;
	pROIAlgorithmParam->m_iBrightUniformityOffset = m_iEditBrightUniformityOffset;
	pROIAlgorithmParam->m_iBrightUniformityHystLength = m_iEditBrightUniformityHystLength;
	pROIAlgorithmParam->m_iBrightUniformityHystOffset = m_iEditBrightUniformityHystOffset;
	pROIAlgorithmParam->m_bUseBrightHystThres = m_bCheckUseBrightHystThres;
	pROIAlgorithmParam->m_iBrightHystSecureThres = m_iEditBrightHystSecureThres;
	pROIAlgorithmParam->m_iBrightHystPotentialThres = m_iEditBrightHystPotentialThres;
	pROIAlgorithmParam->m_iBrightHystPotentialLength = m_iEditBrightHystPotentialLength;

	pROIAlgorithmParam->m_bUseDarkFixedThres = m_bCheckUseDarkFixedThres;
	pROIAlgorithmParam->m_iDarkLowerThres = m_iEditDarkLowerThres;
	pROIAlgorithmParam->m_iDarkUpperThres = m_iEditDarkUpperThres;
	pROIAlgorithmParam->m_bUseDarkDT = m_bCheckUseDarkDT;
	pROIAlgorithmParam->m_bUseDarkLineNormalization = m_bCheckUseDarkLineNormalization;
	pROIAlgorithmParam->m_iDarkMedianFilterSize = m_iEditDarkMedianFilterSize;
	pROIAlgorithmParam->m_iDarkClosingFilterSize = m_iEditDarkClosingFilterSize;
	pROIAlgorithmParam->m_iDarkDTFilterSize = m_iEditDarkDTFilterSize;
	pROIAlgorithmParam->m_iDarkDTValue = m_iEditDarkDTValue;
	pROIAlgorithmParam->m_bUseDarkUniformityCheck = m_bCheckUseDarkUniformityCheck;
	pROIAlgorithmParam->m_iDarkUniformityOffset = m_iEditDarkUniformityOffset;
	pROIAlgorithmParam->m_iDarkUniformityHystLength = m_iEditDarkUniformityHystLength;
	pROIAlgorithmParam->m_iDarkUniformityHystOffset = m_iEditDarkUniformityHystOffset;
	pROIAlgorithmParam->m_bUseDarkHystThres = m_bCheckUseDarkHystThres;
	pROIAlgorithmParam->m_iDarkHystSecureThres = m_iEditDarkHystSecureThres;
	pROIAlgorithmParam->m_iDarkHystPotentialThres = m_iEditDarkHystPotentialThres;
	pROIAlgorithmParam->m_iDarkHystPotentialLength = m_iEditDarkHystPotentialLength;

	pROIAlgorithmParam->m_bCombineBrightDarkBlob = m_bCheckCombineBrightDarkBlob;
	pROIAlgorithmParam->m_iOpeningSize = m_iEditOpeningSize;
	pROIAlgorithmParam->m_iClosingSize = m_iEditClosingSize;
	pROIAlgorithmParam->m_bFillup = m_bCheckFillup;
	pROIAlgorithmParam->m_bUseConnection = m_bCheckUseConnection;
	pROIAlgorithmParam->m_iConnectionMinSize = m_iEditConnectionMinSize;
	pROIAlgorithmParam->m_iConnectionLength = m_iEditConnectionLength;
	pROIAlgorithmParam->m_bUseAngle = m_bCheckUseAngle;
	pROIAlgorithmParam->m_iConnectionMaxWidth = m_iEditConnectionMaxWidth;

	pROIAlgorithmParam->m_bUseEdge = m_bCheckUseEdge;
	pROIAlgorithmParam->m_dEdgeZoomRatio = m_dEditEdgeZoomRatio;
	pROIAlgorithmParam->m_iEdgeStrength = m_iEditEdgeStrength;
	pROIAlgorithmParam->m_iEdgeConnectionDistance = m_iEditEdgeConnectionDistance;
	pROIAlgorithmParam->m_iEdgeMaxEndpointDistance = m_iEditEdgeMaxEndpointDistance;
	pROIAlgorithmParam->m_iEdgeMinContourLength = m_iEditEdgeMinContourLength;

	pROIAlgorithmParam->m_bUseDefectConditionArea = m_bCheckDefectConditionArea;
	pROIAlgorithmParam->m_bUseDefectConditionLength = m_bCheckDefectConditionLength;
	pROIAlgorithmParam->m_bUseDefectConditionWidth = m_bCheckDefectConditionWidth;
	pROIAlgorithmParam->m_bUseDefectConditionMean = m_bCheckDefectConditionMean;
	pROIAlgorithmParam->m_bUseDefectConditionStdev = m_bCheckDefectConditionStdev;
	pROIAlgorithmParam->m_bUseDefectConditionAnisometry = m_bCheckDefectConditionAnisometry;
	pROIAlgorithmParam->m_bUseDefectConditionCircularity = m_bCheckDefectConditionCircularity;
	pROIAlgorithmParam->m_bUseDefectConditionConvexity = m_bCheckDefectConditionConvexity;
	pROIAlgorithmParam->m_bUseDefectConditionEllipseRatio = m_bCheckDefectConditionEllipseRatio;
	pROIAlgorithmParam->m_bUseDefectConditionOuterDist = m_bCheckDefectConditionOuterDist;
	pROIAlgorithmParam->m_bUseDefectConditionInnerDist = m_bCheckDefectConditionInnerDist;
	pROIAlgorithmParam->m_bUseDefectConditionEpoxyDist = m_bCheckDefectConditionEpoxyDist;

	pROIAlgorithmParam->m_iDefectConditionAreaMin = m_iEditDefectConditionAreaMin;
	pROIAlgorithmParam->m_iDefectConditionLengthMin = m_iEditDefectConditionLengthMin;
	pROIAlgorithmParam->m_iDefectConditionWidthMin = m_iEditDefectConditionWidthMin;
	pROIAlgorithmParam->m_iDefectConditionMeanMin = m_iEditDefectConditionMeanMin;
	pROIAlgorithmParam->m_dDefectConditionStdevMin = m_dEditDefectConditionStdevMin;
	pROIAlgorithmParam->m_dDefectConditionAnisometryMin = m_dEditDefectConditionAnisometryMin;
	pROIAlgorithmParam->m_dDefectConditionCircularityMin = m_dEditDefectConditionCircularityMin;
	pROIAlgorithmParam->m_dDefectConditionConvexityMin = m_dEditDefectConditionConvexityMin;
	pROIAlgorithmParam->m_dDefectConditionEllipseRatioMin = m_dEditDefectConditionEllipseRatioMin;

	pROIAlgorithmParam->m_iDefectConditionAreaMax = m_iEditDefectConditionAreaMax;
	pROIAlgorithmParam->m_iDefectConditionLengthMax = m_iEditDefectConditionLengthMax;
	pROIAlgorithmParam->m_iDefectConditionWidthMax = m_iEditDefectConditionWidthMax;
	pROIAlgorithmParam->m_iDefectConditionMeanMax = m_iEditDefectConditionMeanMax;
	pROIAlgorithmParam->m_dDefectConditionStdevMax = m_dEditDefectConditionStdevMax;
	pROIAlgorithmParam->m_dDefectConditionAnisometryMax = m_dEditDefectConditionAnisometryMax;
	pROIAlgorithmParam->m_dDefectConditionCircularityMax = m_dEditDefectConditionCircularityMax;
	pROIAlgorithmParam->m_dDefectConditionConvexityMax = m_dEditDefectConditionConvexityMax;
	pROIAlgorithmParam->m_dDefectConditionEllipseRatioMax = m_dEditDefectConditionEllipseRatioMax;

	pROIAlgorithmParam->m_iDefectConditionOuterDist = m_iEditDefectConditionOuterDist;
	pROIAlgorithmParam->m_iDefectConditionInnerDist = m_iEditDefectConditionInnerDist;

	pROIAlgorithmParam->m_bUseImageScaling = m_bCheckUseImageScaling;
	pROIAlgorithmParam->m_iImageScaleMethod = m_iRadioImageScaleMethod;
	pROIAlgorithmParam->m_iImageScaleMin = m_iEditImageScaleMin;
	pROIAlgorithmParam->m_iImageScaleMax = m_iEditImageScaleMax;

	pROIAlgorithmParam->m_bDefectConditionXLength = m_bCheckDefectConditionXLength;
	pROIAlgorithmParam->m_bDefectConditionYLength = m_bCheckDefectConditionYLength;
	pROIAlgorithmParam->m_bDefectConditionContrast = m_bCheckDefectConditionContrast;
	pROIAlgorithmParam->m_iDefectConditionXLengthMin = m_iEditDefectConditionXLengthMin;
	pROIAlgorithmParam->m_iDefectConditionYLengthMin = m_iEditDefectConditionYLengthMin;
	pROIAlgorithmParam->m_iDefectConditionContrastMin = m_iEditDefectConditionContrastMin;
	pROIAlgorithmParam->m_iDefectConditionXLengthMax = m_iEditDefectConditionXLengthMax;
	pROIAlgorithmParam->m_iDefectConditionYLengthMax = m_iEditDefectConditionYLengthMax;
	pROIAlgorithmParam->m_iDefectConditionContrastMax = m_iEditDefectConditionContrastMax;

	pROIAlgorithmParam->m_bUseFTConnected = m_bCheckUseFTConnected;
	pROIAlgorithmParam->m_bUseFTConnectedArea = m_bCheckUseFTConnectedArea;
	pROIAlgorithmParam->m_bUseFTConnectedLength = m_bCheckUseFTConnectedLength;
	pROIAlgorithmParam->m_bUseFTConnectedWidth = m_bCheckUseFTConnectedWidth;
	pROIAlgorithmParam->m_iFTConnectedAreaMin = m_iEditFTConnectedAreaMin;
	pROIAlgorithmParam->m_iFTConnectedLengthMin = m_iEditFTConnectedLengthMin;
	pROIAlgorithmParam->m_iFTConnectedWidthMin = m_iEditFTConnectedWidthMin;

	pROIAlgorithmParam->m_bUseLineFit = m_bCheckUseLineFit;
	pROIAlgorithmParam->m_iLineFitApplyPos = m_iRadioLineFitApplyPos;
	pROIAlgorithmParam->m_iRoiOffsetTop = m_iEditRoiOffsetTop;
	pROIAlgorithmParam->m_iRoiOffsetBottom = m_iEditRoiOffsetBottom;
	pROIAlgorithmParam->m_iRoiOffsetLeft = m_iEditRoiOffsetLeft;
	pROIAlgorithmParam->m_iRoiOffsetRight = m_iEditRoiOffsetRight;

	pROIAlgorithmParam->m_bUseApplyDontCare = m_bCheckUseApplyDontCare;	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW
	pROIAlgorithmParam->m_bUseDontCare = m_bCheckUseDontCare;	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW

	pROIAlgorithmParam->m_iImageType = m_iSelectedImageType;	// 24.03.29 - v2645 - 검사제외영역 영상 번호 파라미터 추가 - LeeGW

	THEAPP.m_pTabControlDlg->UpdateROIList(m_iSelectedImageType, m_iInspectionType);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonAreaMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaMin = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonAreaMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaMax = 100000;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionLengthMin = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionLengthMax = 100000;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonWidthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionWidthMin = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonWidthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionWidthMax = 100000;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonMeanMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionMeanMin = 0;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonMeanMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionMeanMax = 255;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonStdevMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionStdevMin = 0.1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonStdevMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionStdevMax = 1000;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonAnisometryMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionAnisometryMin = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonAnisometryMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionAnisometryMax = 1000;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonCircularityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionCircularityMin = 0;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonCircularityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionCircularityMax = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonConvexityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionConvexityMin = 0;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonConvexityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionConvexityMax = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonEllipseRatioMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionEllipseRatioMin = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonEllipseRatioMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionEllipseRatioMax = 1000;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonDisplayScaledImage()
{
	if (m_pSelectedROI == NULL)
		return;

	CAlgorithmParam AlgorithmParam = m_pSelectedROI->m_AlgorithmParam[m_iInspectionType];;

	UpdateData(TRUE);

	AlgorithmParam.m_iImageScaleMethod = m_iRadioImageScaleMethod;
	AlgorithmParam.m_iImageScaleMin = m_iEditImageScaleMin;
	AlgorithmParam.m_iImageScaleMax = m_iEditImageScaleMax;

	BOOL bRet;
	Hobject HScaledImage;

	Hobject HROIHRegion;
	HROIHRegion = m_pSelectedROI->GetROIHRegion(THEAPP.m_pCalDataService);

	bRet = THEAPP.m_pAlgorithm->GetScaledImage(m_HOrgScanImage, HROIHRegion, AlgorithmParam, &HScaledImage, MAX_MODULE_ONE_TRAY);

	if (bRet)
	{
		copy_image(HScaledImage, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage + m_iSelectedImageType);

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
}


void CROICommonAlgorithmDlg::OnBnClickedButtonXLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthMin = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonXLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthMax = 100000;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonYLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthMin = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonYLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthMax = 100000;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonContrastMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionContrastMin = 1;
	UpdateData(FALSE);
}


void CROICommonAlgorithmDlg::OnBnClickedButtonContrastMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionContrastMax = 255;
	UpdateData(FALSE);
}

#include "TeachAlgorithmLinefitDlg.h"

void CROICommonAlgorithmDlg::OnBnClickedButtonSetLineFit()
{
	CTeachAlgorithmLinefitDlg dlg;

	dlg.SetImageType(m_iSelectedImageType);
	dlg.SetInspectionType(m_iInspectionType);
	dlg.SetSelectedROI(m_pSelectedROI);

	dlg.DoModal();
}
