#pragma once


// CROIInspAlgorithmDlg 대화 상자입니다.

class CROIInspAlgorithmDlg : public CDialog
{
	DECLARE_DYNAMIC(CROIInspAlgorithmDlg)

public:
	CROIInspAlgorithmDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CROIInspAlgorithmDlg();

	void	Show();
	void	Hide();

	void SetSelectedInspection(int iImageType, GTRegion* pSelectedROI, int iTabIndex);
	void SetParam(CAlgorithmParam pROIAlgorithmParam);
	void GetParam(CAlgorithmParam* pROIAlgorithmParam);

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_ROI_INSP_ALGORITHM_DLG};

private:
	void	SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
	CRect	m_ScreenRect;

	static CROIInspAlgorithmDlg* m_pInstance;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUseBrightFixedThres;
	int m_iEditBrightLowerThres;
	int m_iEditBrightUpperThres;
	BOOL m_bCheckUseBrightDT;
	int m_iEditBrightMedianFilterSize;
	int m_iEditBrightClosingFilterSize;
	int m_iEditBrightDTFilterSize;
	int m_iEditBrightDTValue;
	BOOL m_bCheckUseBrightUniformityCheck;
	int m_iEditBrightUniformityOffset;
	int m_iEditBrightUniformityHystLength;
	int m_iEditBrightUniformityHystOffset;
	BOOL m_bCheckUseBrightHystThres;
	int m_iEditBrightHystSecureThres;
	int m_iEditBrightHystPotentialThres;
	int m_iEditBrightHystPotentialLength;

	BOOL m_bCheckUseDarkFixedThres;
	int m_iEditDarkLowerThres;
	int m_iEditDarkUpperThres;
	BOOL m_bCheckUseDarkDT;
	int m_iEditDarkMedianFilterSize;
	int m_iEditDarkClosingFilterSize;
	int m_iEditDarkDTFilterSize;
	int m_iEditDarkDTValue;
	BOOL m_bCheckUseDarkUniformityCheck;
	int m_iEditDarkUniformityOffset;
	int m_iEditDarkUniformityHystLength;
	int m_iEditDarkUniformityHystOffset;
	BOOL m_bCheckUseDarkHystThres;
	int m_iEditDarkHystSecureThres;
	int m_iEditDarkHystPotentialThres;
	int m_iEditDarkHystPotentialLength;

	BOOL m_bCheckCombineBrightDarkBlob;
	int m_iEditOpeningSize;
	int m_iEditClosingSize;
	BOOL m_bCheckFillup;
	BOOL m_bCheckUseConnection;
	int m_iEditConnectionMinSize;
	int m_iEditConnectionLength;
	BOOL m_bCheckUseAngle;
	int m_iEditConnectionMaxWidth;

	BOOL m_bCheckUseEdge;
	double m_dEditEdgeZoomRatio;
	int m_iEditEdgeStrength;
	int m_iEditEdgeConnectionDistance;
	int m_iEditEdgeMaxEndpointDistance;
	int m_iEditEdgeMinContourLength;

	BOOL m_bCheckDefectConditionArea;
	BOOL m_bCheckDefectConditionLength;
	BOOL m_bCheckDefectConditionWidth;
	BOOL m_bCheckDefectConditionMean;
	BOOL m_bCheckDefectConditionStdev;
	BOOL m_bCheckDefectConditionAnisometry;
	BOOL m_bCheckDefectConditionCircularity;
	BOOL m_bCheckDefectConditionConvexity;
	BOOL m_bCheckDefectConditionEllipseRatio;
	BOOL m_bCheckDefectConditionOuterDist;
	BOOL m_bCheckDefectConditionInnerDist;
	BOOL m_bCheckDefectConditionEpoxyDist;

	int m_iEditDefectConditionAreaMin;
	int m_iEditDefectConditionLengthMin;
	int m_iEditDefectConditionWidthMin;
	int m_iEditDefectConditionMeanMin;
	double m_dEditDefectConditionStdevMin;
	double m_dEditDefectConditionAnisometryMin;
	double m_dEditDefectConditionCircularityMin;
	double m_dEditDefectConditionConvexityMin;
	double m_dEditDefectConditionEllipseRatioMin;

	int m_iEditDefectConditionAreaMax;
	int m_iEditDefectConditionLengthMax;
	int m_iEditDefectConditionWidthMax;
	int m_iEditDefectConditionMeanMax;
	double m_dEditDefectConditionStdevMax;
	double m_dEditDefectConditionAnisometryMax;
	double m_dEditDefectConditionCircularityMax;
	double m_dEditDefectConditionConvexityMax;
	double m_dEditDefectConditionEllipseRatioMax;

	int m_iEditDefectConditionOuterDist;
	int m_iEditDefectConditionInnerDist;

protected:
	CButtonCS m_bnSave;
	CButtonCS m_bnClose;
	CButtonCS m_bnSaveClose;
	CButtonCS m_bnDisplayScaledImage;
	CButtonCS m_bnSetLineFit;

protected:

	int			m_iSelectedImageType;
	int			m_iInspectionType;
	GTRegion*	m_pSelectedROI;

	Hobject m_HOrgScanImage;

public:
	afx_msg void OnBnClickedButtonAreaMin();
	afx_msg void OnBnClickedButtonAreaMax();
	afx_msg void OnBnClickedButtonLengthMin();
	afx_msg void OnBnClickedButtonLengthMax();
	afx_msg void OnBnClickedButtonWidthMin();
	afx_msg void OnBnClickedButtonWidthMax();
	afx_msg void OnBnClickedButtonMeanMin();
	afx_msg void OnBnClickedButtonMeanMax();
	afx_msg void OnBnClickedButtonStdevMin();
	afx_msg void OnBnClickedButtonStdevMax();
	afx_msg void OnBnClickedButtonAnisometryMin();
	afx_msg void OnBnClickedButtonAnisometryMax();
	afx_msg void OnBnClickedButtonCircularityMin();
	afx_msg void OnBnClickedButtonCircularityMax();
	afx_msg void OnBnClickedButtonConvexityMin();
	afx_msg void OnBnClickedButtonConvexityMax();
	afx_msg void OnBnClickedButtonEllipseRatioMin();
	afx_msg void OnBnClickedButtonEllipseRatioMax();
	BOOL m_bCheckUseBrightLineNormalization;
	BOOL m_bCheckUseDarkLineNormalization;

	BOOL m_bCheckUseImageScaling;
	int m_iRadioImageScaleMethod;
	int m_iEditImageScaleMin;
	int m_iEditImageScaleMax;

	BOOL m_bCheckDefectConditionXLength;
	BOOL m_bCheckDefectConditionYLength;
	BOOL m_bCheckDefectConditionContrast;
	int m_iEditDefectConditionXLengthMin;
	int m_iEditDefectConditionYLengthMin;
	int m_iEditDefectConditionContrastMin;
	int m_iEditDefectConditionXLengthMax;
	int m_iEditDefectConditionYLengthMax;
	int m_iEditDefectConditionContrastMax;

	BOOL m_bCheckUseFTConnected;
	BOOL m_bCheckUseFTConnectedArea;
	BOOL m_bCheckUseFTConnectedLength;
	BOOL m_bCheckUseFTConnectedWidth;
	int m_iEditFTConnectedAreaMin;
	int m_iEditFTConnectedLengthMin;
	int m_iEditFTConnectedWidthMin;

	BOOL m_bCheckUseLineFit;
	int m_iRadioLineFitApplyPos;
	int m_iEditRoiOffsetTop;
	int m_iEditRoiOffsetBottom;
	int m_iEditRoiOffsetLeft;
	int m_iEditRoiOffsetRight;

	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW Start
	BOOL m_bCheckUseApplyDontCare;
	BOOL m_bCheckUseDontCare;
	// 24.03.29 - v2645 - 검사제외영역 추가 - LeeGW End

	afx_msg void OnBnClickedButtonDisplayScaledImage();
	afx_msg void OnBnClickedButtonXLengthMin();
	afx_msg void OnBnClickedButtonXLengthMax();
	afx_msg void OnBnClickedButtonYLengthMin();
	afx_msg void OnBnClickedButtonYLengthMax();
	afx_msg void OnBnClickedButtonContrastMin();
	afx_msg void OnBnClickedButtonContrastMax();
	afx_msg void OnBnClickedButtonSetLineFit();
};
