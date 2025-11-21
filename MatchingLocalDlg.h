#pragma once

// CMatchingLocalDlg 대화 상자입니다.

class CMatchingLocalDlg : public CDialog
{
	DECLARE_DYNAMIC(CMatchingLocalDlg)

public:
	CMatchingLocalDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMatchingLocalDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MATCHING_LOCAL_DLG };

	void LoadViewParam();
	void SetControlState(int iMatchingType);

	BOOL m_bCheckUseMatchingShape;
	BOOL m_bCheckUseMatchingNCC;

	int m_iEditMatchingPyramidLevel;
	double m_iEditMatchingAngleRange;
	double m_dEditMatchingScaleMin;
	double m_dEditMatchingScaleMax;
	int m_iEditMatchingTeachingContrast;
	int m_iEditMatchingTeachingContrastLow;
	int m_iEditMatchingTeachingContrastMinSize;
	double m_dEditMatchingScore;
	int m_iEditMatchingInspectionMinContrast;
	int m_iEditMatchingSearchMarginX;
	int m_iEditMatchingSearchMarginY;
	BOOL m_bCheckAlignImage[MAX_IMAGE_TAB];

protected:
	int m_iCurImageIndex;

	int m_iCurrentShapeModel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedButtonCreateMatchingModel();
	afx_msg void OnBnClickedButtonShowContrast();
		
	afx_msg void OnBnClickedButtonChangeScore();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonImageRotation();
	afx_msg void OnBnClickedButtonTestMatchingModel();
	afx_msg void OnBnClickedCheckUseShapeMatching();
	afx_msg void OnClickedCheckUseTemplateMatching();
	afx_msg void OnBnClickedButtonAffineImage();
	double m_dEditDeltaX;
	double m_dEditDeltaY;
	double m_dEditRotationAngle;

	BOOL m_bCheckUseMatchingAffineConstant;

};
