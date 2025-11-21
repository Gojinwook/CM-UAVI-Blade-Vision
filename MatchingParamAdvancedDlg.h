#pragma once


// CMatchingParamAdvancedDlg 대화 상자입니다.

class CMatchingParamAdvancedDlg : public CDialog
{
	DECLARE_DYNAMIC(CMatchingParamAdvancedDlg)

public:
	CMatchingParamAdvancedDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMatchingParamAdvancedDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MATCHING_PARAM_ADVANCED };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBnOk();
	afx_msg void OnBnClickedBnCancle();
	int m_iEditMatchingContrastC1;
	int m_iEditMatchingContrastC2;
	int m_iEditMatchingContrastC3;
	int m_iEditMatchingContrastC4;

	void LoadImageContours(int iImageNo);
	void SetInspectParam(int iImageNo);
	void GetInspectParam(int iImageNo);

	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW START
	BOOL m_bCheckUseBLROIAlign;
	BOOL m_bCheckUseBLROIAlignContour[MAX_CONTOUR_NUM];
	int m_iEditBLROIAlignXOffset;
	int m_iEditBLROIAlignYOffset;
	int m_iEditBLROIAlignInnerMargin;
	int m_iEditBLROIAlignOuterMargin;
	double m_dEditBLROIAlignMatchingScore;
	int m_iEditBLROIAlignSearchingMargin;
	CComboBox m_cbBLROIAlignImage;

	BOOL m_bUseBLROIAlign[BARREL_LENS_IMAGE_TAB];
	BOOL m_bUseBLROIAlignContour[BARREL_LENS_IMAGE_TAB][MAX_CONTOUR_NUM];
	int m_iBLROIAlignInnerMargin[BARREL_LENS_IMAGE_TAB];
	int m_iBLROIAlignOuterMargin[BARREL_LENS_IMAGE_TAB];

	int m_iBeforeBLROIAlignImage;
	// 24.07.01 - v2651 - ROI Templet Matching Align 추가 - LeeGW END

	afx_msg void OnBnClickedButtonRoiAlignCreateModel();
	afx_msg void OnBnClickedButtonRoiAlignResultCheck();
	afx_msg void OnBnClickedButtonRoiAlignAreaCheck();
	afx_msg void OnSelchangeComboRoiAlignImage();
	afx_msg void OnBnClickedButtonRoiAlignDeleteModel();
	afx_msg void OnBnClickedBnOk2();
};
