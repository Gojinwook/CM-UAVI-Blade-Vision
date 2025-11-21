#pragma once


// CLocalAlignApplyDlg 대화 상자입니다.

class CLocalAlignApplyDlg : public CDialog
{
	DECLARE_DYNAMIC(CLocalAlignApplyDlg)

public:
	CLocalAlignApplyDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLocalAlignApplyDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iInspectionType);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

	Hobject m_HLocalAlignImage[MAX_IMAGE_TAB];

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOCAL_ALIGN_APPLY_DLG };

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
	
	afx_msg void OnBnClickedButtonLocalAlignApplyTest();
	afx_msg void OnBnClickedRadioUsePartCheck();
	afx_msg void OnBnClickedRadioUseEdgeMeasure();
	afx_msg void OnBnClickedRadioUseMatchingShape();
	afx_msg void OnBnClickedRadioUseMatchingPos();
	afx_msg void OnBnClickedRadioUseLineFit();
};
