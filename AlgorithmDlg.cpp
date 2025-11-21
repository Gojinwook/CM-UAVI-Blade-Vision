// AlgorithmDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "uScan.h"
#include "AlgorithmDlg.h"
#include "afxdialogex.h"


// CAlgorithmDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CAlgorithmDlg, CDialog)

CAlgorithmDlg* CAlgorithmDlg::m_pInstance = NULL;

CAlgorithmDlg* CAlgorithmDlg::GetInstance(BOOL bShowFlag)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CAlgorithmDlg;

		if(!m_pInstance->m_hWnd)
		{
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

			m_pInstance->Create(IDD_TEACH_ALGORITHM_DLG, pFrame->GetActiveView());
		}
	}

	return m_pInstance;
}

void CAlgorithmDlg::DeleteInstance()
{
	if(m_pInstance->GetSafeHwnd())
		m_pInstance->DestroyWindow();

	SAFE_DELETE(m_pInstance);
}

void CAlgorithmDlg::Show()
{
	ShowWindow(SW_SHOW);
}

void CAlgorithmDlg::Hide()
{
	ShowWindow(SW_HIDE);
}

CAlgorithmDlg::CAlgorithmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithmDlg::IDD, pParent)
{
	m_iSelectedImageType = 0;
	m_iSelectedInspection = 0;

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

	m_bCheckDefectConditionArea = FALSE;
	m_bCheckDefectConditionLength = FALSE;
	m_bCheckDefectConditionWidth = FALSE;
	m_bCheckDefectConditionMean = FALSE;
	m_bCheckDefectConditionStdev = FALSE;
	m_bCheckDefectConditionAnisometry = FALSE;
	m_bCheckDefectConditionCircularity = FALSE;
	m_bCheckDefectConditionConvexity = FALSE;
	m_bCheckDefectConditionEllipseRatio = FALSE;
	m_bCheckDefectConditionOuterDist = FALSE;
	m_bCheckDefectConditionInnerDist = FALSE;
	m_bCheckDefectConditionEpoxyDist = FALSE;

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
	m_iEditDefectConditionEpoxyDist = 0;

	m_bCheckUseBrightLineNormalization = FALSE;
	m_bCheckUseDarkLineNormalization = FALSE;

	m_iEditEpoxySEThres = 0;
	m_iEditEpoxyESThres = 0;
	m_iEditEpoxyClosingSize = 0;
	m_iEditEpoxyOpeningSize = 0;
	m_iEditEpoxyNearDistance = 0;
	m_iEditEpoxyNearMinSize = 0;
	m_bCheckEpoxyConditionHole = FALSE;
	m_bCheckEpoxyConditionNear = FALSE;

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

CAlgorithmDlg::~CAlgorithmDlg()
{
}

void CAlgorithmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_bnSave);
	DDX_Control(pDX, IDC_BUTTON_SAVE_AND_CLOSE, m_bnSaveClose);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_bnClose);
	DDX_Control(pDX, IDC_BUTTON_DISPLAY_SCALED_IMAGE, m_bnDisplayScaledImage);
	DDX_Control(pDX, IDC_BUTTON_SET_LINE_FIT, m_bnSetLineFit);

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
	DDX_Check(pDX, IDC_CHECK_DEFECT_CONDITION_EPOXY_DIST, m_bCheckDefectConditionEpoxyDist);

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
	DDX_Text(pDX, IDC_EDIT_DEFECT_CONDITION_EPOXY_DIST, m_iEditDefectConditionEpoxyDist);

	DDX_Check(pDX, IDC_CHECK_USE_BRIGHT_LINE_GRAY_NORMALIZATION, m_bCheckUseBrightLineNormalization);
	DDX_Check(pDX, IDC_CHECK_USE_DARK_LINE_GRAY_NORMALIZATION, m_bCheckUseDarkLineNormalization);

	DDX_Text(pDX, IDC_EDIT_EPOXY_SE_THRES, m_iEditEpoxySEThres);
	DDX_Text(pDX, IDC_EDIT_EPOXY_ES_THRES, m_iEditEpoxyESThres);
	DDX_Text(pDX, IDC_EDIT_EPOXY_CLOSING_SIZE, m_iEditEpoxyClosingSize);
	DDX_Text(pDX, IDC_EDIT_EPOXY_OPENING_SIZE, m_iEditEpoxyOpeningSize);
	DDX_Text(pDX, IDC_EDIT_EPOXY_NEAR_DISTANCE, m_iEditEpoxyNearDistance);
	DDX_Text(pDX, IDC_EDIT_EPOXY_NEAR_MIN_SIZE, m_iEditEpoxyNearMinSize);
	DDX_Check(pDX, IDC_CHECK_EPOXY_CONDITION_HOLE, m_bCheckEpoxyConditionHole);
	DDX_Check(pDX, IDC_CHECK_EPOXY_CONDITION_NEAR, m_bCheckEpoxyConditionNear);

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

}


BEGIN_MESSAGE_MAP(CAlgorithmDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CAlgorithmDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_AND_CLOSE, &CAlgorithmDlg::OnBnClickedButtonSaveAndClose)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CAlgorithmDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_AREA_MIN, &CAlgorithmDlg::OnBnClickedButtonAreaMin)
	ON_BN_CLICKED(IDC_BUTTON_AREA_MAX, &CAlgorithmDlg::OnBnClickedButtonAreaMax)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH_MIN, &CAlgorithmDlg::OnBnClickedButtonLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH_MAX, &CAlgorithmDlg::OnBnClickedButtonLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_MIN, &CAlgorithmDlg::OnBnClickedButtonWidthMin)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_MAX, &CAlgorithmDlg::OnBnClickedButtonWidthMax)
	ON_BN_CLICKED(IDC_BUTTON_MEAN_MIN, &CAlgorithmDlg::OnBnClickedButtonMeanMin)
	ON_BN_CLICKED(IDC_BUTTON_MEAN_MAX, &CAlgorithmDlg::OnBnClickedButtonMeanMax)
	ON_BN_CLICKED(IDC_BUTTON_STDEV_MIN, &CAlgorithmDlg::OnBnClickedButtonStdevMin)
	ON_BN_CLICKED(IDC_BUTTON_STDEV_MAX, &CAlgorithmDlg::OnBnClickedButtonStdevMax)
	ON_BN_CLICKED(IDC_BUTTON_ANISOMETRY_MIN, &CAlgorithmDlg::OnBnClickedButtonAnisometryMin)
	ON_BN_CLICKED(IDC_BUTTON_ANISOMETRY_MAX, &CAlgorithmDlg::OnBnClickedButtonAnisometryMax)
	ON_BN_CLICKED(IDC_BUTTON_CIRCULARITY_MIN, &CAlgorithmDlg::OnBnClickedButtonCircularityMin)
	ON_BN_CLICKED(IDC_BUTTON_CIRCULARITY_MAX, &CAlgorithmDlg::OnBnClickedButtonCircularityMax)
	ON_BN_CLICKED(IDC_BUTTON_CONVEXITY_MIN, &CAlgorithmDlg::OnBnClickedButtonConvexityMin)
	ON_BN_CLICKED(IDC_BUTTON_CONVEXITY_MAX, &CAlgorithmDlg::OnBnClickedButtonConvexityMax)
	ON_BN_CLICKED(IDC_BUTTON_ELLIPSE_RATIO_MIN, &CAlgorithmDlg::OnBnClickedButtonEllipseRatioMin)
	ON_BN_CLICKED(IDC_BUTTON_ELLIPSE_RATIO_MAX, &CAlgorithmDlg::OnBnClickedButtonEllipseRatioMax)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY_SCALED_IMAGE, &CAlgorithmDlg::OnBnClickedButtonDisplayScaledImage)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_MIN, &CAlgorithmDlg::OnBnClickedButtonXLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_X_LENGTH_MAX, &CAlgorithmDlg::OnBnClickedButtonXLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_MIN, &CAlgorithmDlg::OnBnClickedButtonYLengthMin)
	ON_BN_CLICKED(IDC_BUTTON_Y_LENGTH_MAX, &CAlgorithmDlg::OnBnClickedButtonYLengthMax)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_MIN, &CAlgorithmDlg::OnBnClickedButtonContrastMin)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_MAX, &CAlgorithmDlg::OnBnClickedButtonContrastMax)
	ON_BN_CLICKED(IDC_BUTTON_SET_LINE_FIT, &CAlgorithmDlg::OnBnClickedButtonSetLineFit)
END_MESSAGE_MAP()


// CAlgorithmDlg 메시지 처리기입니다.


BOOL CAlgorithmDlg::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message) {		
	case WM_NCLBUTTONDOWN :
		SetActiveWindow();
		return FALSE;

	case WM_KEYDOWN:
		if((int)pMsg->wParam == VK_ESCAPE || (int)pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}

	case WM_SYSKEYDOWN:
		if(pMsg->wParam == VK_F4)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CAlgorithmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTopMost, 1000, 30, 0, 0, SWP_NOSIZE);

	m_bnSave.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnSaveClose.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnClose.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);
	m_bnDisplayScaledImage.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, ROYAL_BLUE, 0, 0);

	m_bnDisplayScaledImage.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, OLIVE_GREEN, 0, 0);
	m_bnSetLineFit.Init_Ctrl(_T("Arial"), 9, FALSE, BLACK, OLIVE_GREEN, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAlgorithmDlg::OnBnClickedButtonSave()
{
	GetParam();
}


void CAlgorithmDlg::OnBnClickedButtonSaveAndClose()
{
	copy_image(m_HOrgScanImage, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage+m_iSelectedImageType);

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	GetParam();
	Hide();
}

void CAlgorithmDlg::SetSelectedInspection(int iImageType, int iTabIndex)
{ 
	m_iSelectedImageType = iImageType;
	m_iSelectedInspection = iTabIndex; 

	copy_image(THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage[m_iSelectedImageType], &m_HOrgScanImage);
}

void CAlgorithmDlg::SetParam()
{
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iSelectedImageType][m_iSelectedInspection];

	m_bCheckUseBrightFixedThres = AlgorithmParam.m_bUseBrightFixedThres;
	m_iEditBrightLowerThres = AlgorithmParam.m_iBrightLowerThres;
	m_iEditBrightUpperThres = AlgorithmParam.m_iBrightUpperThres;
	m_bCheckUseBrightDT = AlgorithmParam.m_bUseBrightDT;
	m_bCheckUseBrightLineNormalization = AlgorithmParam.m_bUseBrightLineNormalization;
	m_iEditBrightMedianFilterSize = AlgorithmParam.m_iBrightMedianFilterSize;
	m_iEditBrightClosingFilterSize = AlgorithmParam.m_iBrightClosingFilterSize;
	m_iEditBrightDTFilterSize = AlgorithmParam.m_iBrightDTFilterSize;
	m_iEditBrightDTValue = AlgorithmParam.m_iBrightDTValue;
	m_bCheckUseBrightUniformityCheck = AlgorithmParam.m_bUseBrightUniformityCheck;
	m_iEditBrightUniformityOffset = AlgorithmParam.m_iBrightUniformityOffset;
	m_iEditBrightUniformityHystLength = AlgorithmParam.m_iBrightUniformityHystLength;
	m_iEditBrightUniformityHystOffset = AlgorithmParam.m_iBrightUniformityHystOffset;
	m_bCheckUseBrightHystThres = AlgorithmParam.m_bUseBrightHystThres;
	m_iEditBrightHystSecureThres = AlgorithmParam.m_iBrightHystSecureThres;
	m_iEditBrightHystPotentialThres = AlgorithmParam.m_iBrightHystPotentialThres;
	m_iEditBrightHystPotentialLength = AlgorithmParam.m_iBrightHystPotentialLength;

	m_bCheckUseDarkFixedThres = AlgorithmParam.m_bUseDarkFixedThres;
	m_iEditDarkLowerThres = AlgorithmParam.m_iDarkLowerThres;
	m_iEditDarkUpperThres = AlgorithmParam.m_iDarkUpperThres;
	m_bCheckUseDarkDT = AlgorithmParam.m_bUseDarkDT;
	m_bCheckUseDarkLineNormalization = AlgorithmParam.m_bUseDarkLineNormalization;
	m_iEditDarkMedianFilterSize = AlgorithmParam.m_iDarkMedianFilterSize;
	m_iEditDarkClosingFilterSize = AlgorithmParam.m_iDarkClosingFilterSize;
	m_iEditDarkDTFilterSize = AlgorithmParam.m_iDarkDTFilterSize;
	m_iEditDarkDTValue = AlgorithmParam.m_iDarkDTValue;
	m_bCheckUseDarkUniformityCheck = AlgorithmParam.m_bUseDarkUniformityCheck;
	m_iEditDarkUniformityOffset = AlgorithmParam.m_iDarkUniformityOffset;
	m_iEditDarkUniformityHystLength = AlgorithmParam.m_iDarkUniformityHystLength;
	m_iEditDarkUniformityHystOffset = AlgorithmParam.m_iDarkUniformityHystOffset;
	m_bCheckUseDarkHystThres = AlgorithmParam.m_bUseDarkHystThres;
	m_iEditDarkHystSecureThres = AlgorithmParam.m_iDarkHystSecureThres;
	m_iEditDarkHystPotentialThres = AlgorithmParam.m_iDarkHystPotentialThres;
	m_iEditDarkHystPotentialLength = AlgorithmParam.m_iDarkHystPotentialLength;

	m_bCheckCombineBrightDarkBlob = AlgorithmParam.m_bCombineBrightDarkBlob;
	m_iEditOpeningSize = AlgorithmParam.m_iOpeningSize;
	m_iEditClosingSize = AlgorithmParam.m_iClosingSize;
	m_bCheckFillup = AlgorithmParam.m_bFillup;
	m_bCheckUseConnection = AlgorithmParam.m_bUseConnection;
	m_iEditConnectionMinSize = AlgorithmParam.m_iConnectionMinSize;
	m_iEditConnectionLength = AlgorithmParam.m_iConnectionLength;
	m_bCheckUseAngle = AlgorithmParam.m_bUseAngle;
	m_iEditConnectionMaxWidth = AlgorithmParam.m_iConnectionMaxWidth;

	m_bCheckUseEdge = AlgorithmParam.m_bUseEdge;
	m_dEditEdgeZoomRatio = AlgorithmParam.m_dEdgeZoomRatio;
	m_iEditEdgeStrength = AlgorithmParam.m_iEdgeStrength;
	m_iEditEdgeConnectionDistance = AlgorithmParam.m_iEdgeConnectionDistance;
	m_iEditEdgeMaxEndpointDistance = AlgorithmParam.m_iEdgeMaxEndpointDistance;
	m_iEditEdgeMinContourLength = AlgorithmParam.m_iEdgeMinContourLength;

	m_bCheckDefectConditionArea = AlgorithmParam.m_bUseDefectConditionArea;
	m_bCheckDefectConditionLength = AlgorithmParam.m_bUseDefectConditionLength;
	m_bCheckDefectConditionWidth = AlgorithmParam.m_bUseDefectConditionWidth;
	m_bCheckDefectConditionMean = AlgorithmParam.m_bUseDefectConditionMean;
	m_bCheckDefectConditionStdev = AlgorithmParam.m_bUseDefectConditionStdev;
	m_bCheckDefectConditionAnisometry = AlgorithmParam.m_bUseDefectConditionAnisometry;
	m_bCheckDefectConditionCircularity = AlgorithmParam.m_bUseDefectConditionCircularity;
	m_bCheckDefectConditionConvexity = AlgorithmParam.m_bUseDefectConditionConvexity;
	m_bCheckDefectConditionEllipseRatio = AlgorithmParam.m_bUseDefectConditionEllipseRatio;
	m_bCheckDefectConditionOuterDist = AlgorithmParam.m_bUseDefectConditionOuterDist;
	m_bCheckDefectConditionInnerDist = AlgorithmParam.m_bUseDefectConditionInnerDist;
	m_bCheckDefectConditionEpoxyDist = AlgorithmParam.m_bUseDefectConditionEpoxyDist;

	m_iEditDefectConditionAreaMin = AlgorithmParam.m_iDefectConditionAreaMin;
	m_iEditDefectConditionLengthMin = AlgorithmParam.m_iDefectConditionLengthMin;
	m_iEditDefectConditionWidthMin = AlgorithmParam.m_iDefectConditionWidthMin;
	m_iEditDefectConditionMeanMin = AlgorithmParam.m_iDefectConditionMeanMin;
	m_dEditDefectConditionStdevMin = AlgorithmParam.m_dDefectConditionStdevMin;
	m_dEditDefectConditionAnisometryMin = AlgorithmParam.m_dDefectConditionAnisometryMin;
	m_dEditDefectConditionCircularityMin = AlgorithmParam.m_dDefectConditionCircularityMin;
	m_dEditDefectConditionConvexityMin = AlgorithmParam.m_dDefectConditionConvexityMin;
	m_dEditDefectConditionEllipseRatioMin = AlgorithmParam.m_dDefectConditionEllipseRatioMin;

	m_iEditDefectConditionAreaMax = AlgorithmParam.m_iDefectConditionAreaMax;
	m_iEditDefectConditionLengthMax = AlgorithmParam.m_iDefectConditionLengthMax;
	m_iEditDefectConditionWidthMax = AlgorithmParam.m_iDefectConditionWidthMax;
	m_iEditDefectConditionMeanMax = AlgorithmParam.m_iDefectConditionMeanMax;
	m_dEditDefectConditionStdevMax = AlgorithmParam.m_dDefectConditionStdevMax;
	m_dEditDefectConditionAnisometryMax = AlgorithmParam.m_dDefectConditionAnisometryMax;
	m_dEditDefectConditionCircularityMax = AlgorithmParam.m_dDefectConditionCircularityMax;
	m_dEditDefectConditionConvexityMax = AlgorithmParam.m_dDefectConditionConvexityMax;
	m_dEditDefectConditionEllipseRatioMax = AlgorithmParam.m_dDefectConditionEllipseRatioMax;

	m_iEditDefectConditionOuterDist = AlgorithmParam.m_iDefectConditionOuterDist;
	m_iEditDefectConditionInnerDist = AlgorithmParam.m_iDefectConditionInnerDist;
	m_iEditDefectConditionEpoxyDist = AlgorithmParam.m_iDefectConditionEpoxyDist;

	m_iEditEpoxySEThres = AlgorithmParam.m_iEpoxySEThres;
	m_iEditEpoxyESThres = AlgorithmParam.m_iEpoxyESThres;
	m_iEditEpoxyClosingSize = AlgorithmParam.m_iEpoxyClosingSize;
	m_iEditEpoxyOpeningSize = AlgorithmParam.m_iEpoxyOpeningSize;
	m_iEditEpoxyNearDistance = AlgorithmParam.m_iEpoxyNearDistance;
	m_iEditEpoxyNearMinSize = AlgorithmParam.m_iEpoxyNearMinSize;
	m_bCheckEpoxyConditionHole = AlgorithmParam.m_bEpoxyConditionHole;
	m_bCheckEpoxyConditionNear = AlgorithmParam.m_bEpoxyConditionNear;

	m_bCheckUseImageScaling = AlgorithmParam.m_bUseImageScaling;
	m_iRadioImageScaleMethod = AlgorithmParam.m_iImageScaleMethod;
	m_iEditImageScaleMin = AlgorithmParam.m_iImageScaleMin;
	m_iEditImageScaleMax = AlgorithmParam.m_iImageScaleMax;

	m_bCheckDefectConditionXLength = AlgorithmParam.m_bDefectConditionXLength;
	m_bCheckDefectConditionYLength = AlgorithmParam.m_bDefectConditionYLength;
	m_bCheckDefectConditionContrast = AlgorithmParam.m_bDefectConditionContrast;
	m_iEditDefectConditionXLengthMin = AlgorithmParam.m_iDefectConditionXLengthMin;
	m_iEditDefectConditionYLengthMin = AlgorithmParam.m_iDefectConditionYLengthMin;
	m_iEditDefectConditionContrastMin = AlgorithmParam.m_iDefectConditionContrastMin;
	m_iEditDefectConditionXLengthMax = AlgorithmParam.m_iDefectConditionXLengthMax;
	m_iEditDefectConditionYLengthMax = AlgorithmParam.m_iDefectConditionYLengthMax;
	m_iEditDefectConditionContrastMax = AlgorithmParam.m_iDefectConditionContrastMax;

	m_bCheckUseFTConnected = AlgorithmParam.m_bUseFTConnected;
	m_bCheckUseFTConnectedArea = AlgorithmParam.m_bUseFTConnectedArea;
	m_bCheckUseFTConnectedLength = AlgorithmParam.m_bUseFTConnectedLength;
	m_bCheckUseFTConnectedWidth = AlgorithmParam.m_bUseFTConnectedWidth;
	m_iEditFTConnectedAreaMin = AlgorithmParam.m_iFTConnectedAreaMin;
	m_iEditFTConnectedLengthMin = AlgorithmParam.m_iFTConnectedLengthMin;
	m_iEditFTConnectedWidthMin = AlgorithmParam.m_iFTConnectedWidthMin;

	m_bCheckUseLineFit = AlgorithmParam.m_bUseLineFit;
	m_iRadioLineFitApplyPos = AlgorithmParam.m_iLineFitApplyPos;
	m_iEditRoiOffsetTop = AlgorithmParam.m_iRoiOffsetTop;
	m_iEditRoiOffsetBottom = AlgorithmParam.m_iRoiOffsetBottom;
	m_iEditRoiOffsetLeft = AlgorithmParam.m_iRoiOffsetLeft;
	m_iEditRoiOffsetRight = AlgorithmParam.m_iRoiOffsetRight;

	m_bCheckUseApplyDontCare = AlgorithmParam.m_bUseApplyDontCare;
	m_bCheckUseDontCare = AlgorithmParam.m_bUseDontCare;	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW

	UpdateData(FALSE);
}

//0531
void CAlgorithmDlg::GetParam()
{
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iSelectedImageType][m_iSelectedInspection];

	UpdateData();
	
	AlgorithmParam.m_bUseBrightFixedThres = m_bCheckUseBrightFixedThres;
	AlgorithmParam.m_iBrightLowerThres = m_iEditBrightLowerThres;
	AlgorithmParam.m_iBrightUpperThres = m_iEditBrightUpperThres;
	AlgorithmParam.m_bUseBrightDT = m_bCheckUseBrightDT;
	AlgorithmParam.m_bUseBrightLineNormalization = m_bCheckUseBrightLineNormalization;
	AlgorithmParam.m_iBrightMedianFilterSize = m_iEditBrightMedianFilterSize;
	AlgorithmParam.m_iBrightClosingFilterSize = m_iEditBrightClosingFilterSize;
	AlgorithmParam.m_iBrightDTFilterSize = m_iEditBrightDTFilterSize;
	AlgorithmParam.m_iBrightDTValue = m_iEditBrightDTValue;
	AlgorithmParam.m_bUseBrightUniformityCheck = m_bCheckUseBrightUniformityCheck;
	AlgorithmParam.m_iBrightUniformityOffset = m_iEditBrightUniformityOffset;
	AlgorithmParam.m_iBrightUniformityHystLength = m_iEditBrightUniformityHystLength;
	AlgorithmParam.m_iBrightUniformityHystOffset = m_iEditBrightUniformityHystOffset;
	AlgorithmParam.m_bUseBrightHystThres = m_bCheckUseBrightHystThres;
	AlgorithmParam.m_iBrightHystSecureThres = m_iEditBrightHystSecureThres;
	AlgorithmParam.m_iBrightHystPotentialThres = m_iEditBrightHystPotentialThres;
	AlgorithmParam.m_iBrightHystPotentialLength = m_iEditBrightHystPotentialLength;

	AlgorithmParam.m_bUseDarkFixedThres = m_bCheckUseDarkFixedThres;
	AlgorithmParam.m_iDarkLowerThres = m_iEditDarkLowerThres;
	AlgorithmParam.m_iDarkUpperThres = m_iEditDarkUpperThres;
	AlgorithmParam.m_bUseDarkDT = m_bCheckUseDarkDT;
	AlgorithmParam.m_bUseDarkLineNormalization = m_bCheckUseDarkLineNormalization;
	AlgorithmParam.m_iDarkMedianFilterSize = m_iEditDarkMedianFilterSize;
	AlgorithmParam.m_iDarkClosingFilterSize = m_iEditDarkClosingFilterSize;
	AlgorithmParam.m_iDarkDTFilterSize = m_iEditDarkDTFilterSize;
	AlgorithmParam.m_iDarkDTValue = m_iEditDarkDTValue;
	AlgorithmParam.m_bUseDarkUniformityCheck = m_bCheckUseDarkUniformityCheck;
	AlgorithmParam.m_iDarkUniformityOffset = m_iEditDarkUniformityOffset;
	AlgorithmParam.m_iDarkUniformityHystLength = m_iEditDarkUniformityHystLength;
	AlgorithmParam.m_iDarkUniformityHystOffset = m_iEditDarkUniformityHystOffset;
	AlgorithmParam.m_bUseDarkHystThres = m_bCheckUseDarkHystThres;
	AlgorithmParam.m_iDarkHystSecureThres = m_iEditDarkHystSecureThres;
	AlgorithmParam.m_iDarkHystPotentialThres = m_iEditDarkHystPotentialThres;
	AlgorithmParam.m_iDarkHystPotentialLength = m_iEditDarkHystPotentialLength;

	AlgorithmParam.m_bCombineBrightDarkBlob = m_bCheckCombineBrightDarkBlob;
	AlgorithmParam.m_iOpeningSize = m_iEditOpeningSize;
	AlgorithmParam.m_iClosingSize = m_iEditClosingSize;
	AlgorithmParam.m_bFillup = m_bCheckFillup;
	AlgorithmParam.m_bUseConnection = m_bCheckUseConnection;
	AlgorithmParam.m_iConnectionMinSize = m_iEditConnectionMinSize;
	AlgorithmParam.m_iConnectionLength = m_iEditConnectionLength;
	AlgorithmParam.m_bUseAngle = m_bCheckUseAngle;
	AlgorithmParam.m_iConnectionMaxWidth = m_iEditConnectionMaxWidth; 

	AlgorithmParam.m_bUseEdge = m_bCheckUseEdge;
	AlgorithmParam.m_dEdgeZoomRatio = m_dEditEdgeZoomRatio;
	AlgorithmParam.m_iEdgeStrength = m_iEditEdgeStrength;
	AlgorithmParam.m_iEdgeConnectionDistance = m_iEditEdgeConnectionDistance;
	AlgorithmParam.m_iEdgeMaxEndpointDistance = m_iEditEdgeMaxEndpointDistance;
	AlgorithmParam.m_iEdgeMinContourLength = m_iEditEdgeMinContourLength;

	AlgorithmParam.m_bUseDefectConditionArea = m_bCheckDefectConditionArea;
	AlgorithmParam.m_bUseDefectConditionLength = m_bCheckDefectConditionLength;
	AlgorithmParam.m_bUseDefectConditionWidth = m_bCheckDefectConditionWidth;
	AlgorithmParam.m_bUseDefectConditionMean = m_bCheckDefectConditionMean;
	AlgorithmParam.m_bUseDefectConditionStdev = m_bCheckDefectConditionStdev;
	AlgorithmParam.m_bUseDefectConditionAnisometry = m_bCheckDefectConditionAnisometry;
	AlgorithmParam.m_bUseDefectConditionCircularity = m_bCheckDefectConditionCircularity;
	AlgorithmParam.m_bUseDefectConditionConvexity = m_bCheckDefectConditionConvexity;
	AlgorithmParam.m_bUseDefectConditionEllipseRatio = m_bCheckDefectConditionEllipseRatio;
	AlgorithmParam.m_bUseDefectConditionOuterDist = m_bCheckDefectConditionOuterDist;
	AlgorithmParam.m_bUseDefectConditionInnerDist = m_bCheckDefectConditionInnerDist;
	AlgorithmParam.m_bUseDefectConditionEpoxyDist = m_bCheckDefectConditionEpoxyDist;

	AlgorithmParam.m_iDefectConditionAreaMin = m_iEditDefectConditionAreaMin;
	AlgorithmParam.m_iDefectConditionLengthMin = m_iEditDefectConditionLengthMin;
	AlgorithmParam.m_iDefectConditionWidthMin = m_iEditDefectConditionWidthMin;	
	AlgorithmParam.m_iDefectConditionMeanMin = m_iEditDefectConditionMeanMin;
	AlgorithmParam.m_dDefectConditionStdevMin = m_dEditDefectConditionStdevMin;
	AlgorithmParam.m_dDefectConditionAnisometryMin = m_dEditDefectConditionAnisometryMin;
	AlgorithmParam.m_dDefectConditionCircularityMin = m_dEditDefectConditionCircularityMin;
	AlgorithmParam.m_dDefectConditionConvexityMin = m_dEditDefectConditionConvexityMin;
	AlgorithmParam.m_dDefectConditionEllipseRatioMin = m_dEditDefectConditionEllipseRatioMin;

	AlgorithmParam.m_iDefectConditionAreaMax = m_iEditDefectConditionAreaMax;
	AlgorithmParam.m_iDefectConditionLengthMax = m_iEditDefectConditionLengthMax;
	AlgorithmParam.m_iDefectConditionWidthMax = m_iEditDefectConditionWidthMax;	
	AlgorithmParam.m_iDefectConditionMeanMax = m_iEditDefectConditionMeanMax;
	AlgorithmParam.m_dDefectConditionStdevMax = m_dEditDefectConditionStdevMax;
	AlgorithmParam.m_dDefectConditionAnisometryMax = m_dEditDefectConditionAnisometryMax;
	AlgorithmParam.m_dDefectConditionCircularityMax = m_dEditDefectConditionCircularityMax;
	AlgorithmParam.m_dDefectConditionConvexityMax = m_dEditDefectConditionConvexityMax;
	AlgorithmParam.m_dDefectConditionEllipseRatioMax = m_dEditDefectConditionEllipseRatioMax;

	AlgorithmParam.m_iDefectConditionOuterDist = m_iEditDefectConditionOuterDist;
	AlgorithmParam.m_iDefectConditionInnerDist = m_iEditDefectConditionInnerDist;
	AlgorithmParam.m_iDefectConditionEpoxyDist = m_iEditDefectConditionEpoxyDist;

	AlgorithmParam.m_iEpoxySEThres = m_iEditEpoxySEThres;
	AlgorithmParam.m_iEpoxyESThres = m_iEditEpoxyESThres;
	AlgorithmParam.m_iEpoxyClosingSize = m_iEditEpoxyClosingSize;
	AlgorithmParam.m_iEpoxyOpeningSize = m_iEditEpoxyOpeningSize;
	AlgorithmParam.m_iEpoxyNearDistance = m_iEditEpoxyNearDistance;
	AlgorithmParam.m_iEpoxyNearMinSize = m_iEditEpoxyNearMinSize;
	AlgorithmParam.m_bEpoxyConditionHole = m_bCheckEpoxyConditionHole;
	AlgorithmParam.m_bEpoxyConditionNear = m_bCheckEpoxyConditionNear;

	AlgorithmParam.m_bUseImageScaling = m_bCheckUseImageScaling;
	AlgorithmParam.m_iImageScaleMethod = m_iRadioImageScaleMethod;
	AlgorithmParam.m_iImageScaleMin = m_iEditImageScaleMin;
	AlgorithmParam.m_iImageScaleMax = m_iEditImageScaleMax;

	AlgorithmParam.m_bDefectConditionXLength = m_bCheckDefectConditionXLength;
	AlgorithmParam.m_bDefectConditionYLength = m_bCheckDefectConditionYLength;
	AlgorithmParam.m_bDefectConditionContrast = m_bCheckDefectConditionContrast;
	AlgorithmParam.m_iDefectConditionXLengthMin = m_iEditDefectConditionXLengthMin;
	AlgorithmParam.m_iDefectConditionYLengthMin = m_iEditDefectConditionYLengthMin;
	AlgorithmParam.m_iDefectConditionContrastMin = m_iEditDefectConditionContrastMin;
	AlgorithmParam.m_iDefectConditionXLengthMax = m_iEditDefectConditionXLengthMax;
	AlgorithmParam.m_iDefectConditionYLengthMax = m_iEditDefectConditionYLengthMax;
	AlgorithmParam.m_iDefectConditionContrastMax = m_iEditDefectConditionContrastMax;

	AlgorithmParam.m_bUseFTConnected = m_bCheckUseFTConnected;
	AlgorithmParam.m_bUseFTConnectedArea = m_bCheckUseFTConnectedArea;
	AlgorithmParam.m_bUseFTConnectedLength = m_bCheckUseFTConnectedLength;
	AlgorithmParam.m_bUseFTConnectedWidth = m_bCheckUseFTConnectedWidth;
	AlgorithmParam.m_iFTConnectedAreaMin = m_iEditFTConnectedAreaMin;
	AlgorithmParam.m_iFTConnectedLengthMin = m_iEditFTConnectedLengthMin;
	AlgorithmParam.m_iFTConnectedWidthMin = m_iEditFTConnectedWidthMin;

	AlgorithmParam.m_bUseLineFit = m_bCheckUseLineFit;
	AlgorithmParam.m_iLineFitApplyPos = m_iRadioLineFitApplyPos;
	AlgorithmParam.m_iRoiOffsetTop = m_iEditRoiOffsetTop;
	AlgorithmParam.m_iRoiOffsetBottom = m_iEditRoiOffsetBottom;
	AlgorithmParam.m_iRoiOffsetLeft = m_iEditRoiOffsetLeft;
	AlgorithmParam.m_iRoiOffsetRight = m_iEditRoiOffsetRight;

	AlgorithmParam.m_bUseApplyDontCare = m_bCheckUseApplyDontCare;	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW
	AlgorithmParam.m_bUseDontCare = m_bCheckUseDontCare;	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW

	AlgorithmParam.m_iImageType = m_iSelectedImageType;	// 24.03.29 - v2645 - 검사제외영역 영상 번호 파라미터 추가 - LeeGW

	THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iSelectedImageType][m_iSelectedInspection] = AlgorithmParam;
	THEAPP.m_pTabControlDlg->UpdateList(m_iSelectedImageType, m_iSelectedInspection);
}



void CAlgorithmDlg::OnBnClickedButtonClose()
{
	copy_image(m_HOrgScanImage, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage+m_iSelectedImageType);

	THEAPP.m_pInspectAdminViewDlg->UpdateView();

	Hide();
}


void CAlgorithmDlg::OnBnClickedButtonAreaMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonAreaMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionAreaMax = 100000;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionLengthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionLengthMax = 100000;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonWidthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionWidthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonWidthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionWidthMax = 100000;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonMeanMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionMeanMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonMeanMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionMeanMax = 255;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonStdevMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionStdevMin = 0.1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonStdevMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionStdevMax = 1000;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonAnisometryMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionAnisometryMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonAnisometryMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionAnisometryMax = 1000;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonCircularityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionCircularityMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonCircularityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionCircularityMax = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonConvexityMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionConvexityMin = 0;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonConvexityMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionConvexityMax = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonEllipseRatioMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionEllipseRatioMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonEllipseRatioMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_dEditDefectConditionEllipseRatioMax = 1000;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonDisplayScaledImage()
{
	CAlgorithmParam AlgorithmParam;

	AlgorithmParam = THEAPP.m_pModelDataManager->m_AlgorithmParam[m_iSelectedImageType][m_iSelectedInspection];

	BOOL bRet;
	Hobject HScaledImage;

	bRet = THEAPP.m_pAlgorithm->GetScaledImage(m_HOrgScanImage, THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_1], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_2],
		THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_3], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_4], THEAPP.m_pInspectAdminViewDlg->ContoursAffinTrans[MATCHING_CONTOUR_5],	AlgorithmParam, &HScaledImage);

	if (bRet)
	{
		copy_image(HScaledImage, THEAPP.m_pInspectAdminViewDlg->m_HTeachingImage+m_iSelectedImageType);

		THEAPP.m_pInspectAdminViewDlg->UpdateView();
	}
}


void CAlgorithmDlg::OnBnClickedButtonXLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonXLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionXLengthMax = 100000;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonYLengthMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonYLengthMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionYLengthMax = 100000;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonContrastMin()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionContrastMin = 1;
	UpdateData(FALSE);
}


void CAlgorithmDlg::OnBnClickedButtonContrastMax()
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	m_iEditDefectConditionContrastMax = 255;
	UpdateData(FALSE);
}

#include "TeachAlgorithmLinefitDlg.h"

void CAlgorithmDlg::OnBnClickedButtonSetLineFit()
{
	CTeachAlgorithmLinefitDlg dlg;

	dlg.SetImageType(m_iSelectedImageType);
	dlg.SetInspectionType(m_iSelectedInspection);

	dlg.DoModal();
}
