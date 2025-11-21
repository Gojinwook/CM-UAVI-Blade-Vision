// 24.02.27 Local Align 추가 - LeeGW Create
#pragma once


// CROIPartCheckDlg 대화 상자입니다.

class CROIPartCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CROIPartCheckDlg)

public:
	CROIPartCheckDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CROIPartCheckDlg();

	void SetSelectedInspection(int iImageType,GTRegion *pSelectedROI, int iInspectionType);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);


// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ROI_PART_CHECK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	int			m_iSelectedImageType;
	int			m_iInspectionType;
	GTRegion	*m_pSelectedROI;

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonCreateMatchingModel();
	afx_msg void OnBnClickedButtonDeleteMatchingModel();
	afx_msg void OnBnClickedButtonTestMatchingModel();
	int m_iEditPartCheckMatchingAngleRange;
	int m_iEditPartCheckMatchingSearchMarginX;
	int m_iEditPartCheckMatchingSearchMarginY;
	int m_iEditPartCheckMatchingSearchMarginX2;
	int m_iEditPartCheckMatchingSearchMarginY2;
	BOOL m_bCheckPartCheckModelExist;
	BOOL m_bCheckPartCheckLocalAlignUse;
	BOOL m_bCheckPartCheckUse;

	BOOL m_bCheckPartCheckExistInspUse;
	int m_iEditPartCheckMatchingScore;

	BOOL m_bCheckPartCheckShiftInspUse;
	int m_iEditPartCheckShiftX;
	int m_iEditPartCheckShiftY;
	double m_dEditPartCheckRotationAngle;
};
