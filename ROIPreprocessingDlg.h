#pragma once


// CROIPreprocessingDlg 대화 상자입니다.

class CROIPreprocessingDlg : public CDialog
{
	DECLARE_DYNAMIC(CROIPreprocessingDlg)

public:
	CROIPreprocessingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CROIPreprocessingDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iInspectionType);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	Hobject m_HROIAlgorithmImage[MAX_IMAGE_TAB];

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ROI_PREPROCESSING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	int			m_iSelectedImageType;
	int			m_iInspectionType;
	GTRegion	*m_pSelectedROI;	

public:
	virtual BOOL OnInitDialog();

	BOOL m_bCheckUseLocalAlign;
	CComboBox m_cbLocalAlignImageIndex;
	int m_iEditLocalAlignROINo;
	int m_iRadioLocalAlignType;
	int m_iRadioMatchingApplyMethod;
	BOOL m_bCheckLocalAlignPosX;
	BOOL m_bCheckLocalAlignPosY;
	BOOL m_bCheckLocalAlignPosAngle;
	int m_iRadioLocalAlignFitPos;

	BOOL m_bCheckUseImageProcessFilter;
	CComboBox m_cbImageProcessFilterType1;
	CComboBox m_cbImageProcessFilterType2;
	CComboBox m_cbImageProcessFilterType3;
	double m_iEditImageProcessFilterType1X;
	double m_iEditImageProcessFilterType2X;
	double m_iEditImageProcessFilterType3X;
	double m_iEditImageProcessFilterType1Y;
	double m_iEditImageProcessFilterType2Y;
	double m_iEditImageProcessFilterType3Y;
		
	afx_msg void OnBnClickedButtonLocalAlignApplyTest();
	afx_msg void OnBnClickedRadioUsePartCheck();
	afx_msg void OnBnClickedRadioUseEdgeMeasure();
	afx_msg void OnBnClickedRadioUseMatchingShape();
	afx_msg void OnBnClickedRadioUseMatchingPos();
	afx_msg void OnBnClickedRadioUseLineFit();
	afx_msg void OnBnClickedButtonDisplayProcessImage();


};
