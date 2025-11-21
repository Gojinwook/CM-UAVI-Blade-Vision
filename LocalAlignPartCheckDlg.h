// 24.02.27 Local Align 추가 - LeeGW Create
#pragma once


// CLocalAlignPartCheckDlg 대화 상자입니다.

class CLocalAlignPartCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CLocalAlignPartCheckDlg)

public:
	CLocalAlignPartCheckDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLocalAlignPartCheckDlg();

	void SetSelectedInspection(int iImageType,GTRegion *pSelectedROI, int iInspectionType);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);


// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOCAL_ALIGN_PART_CHECK_DLG };

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
	BOOL m_bCheckLocalAlignPartCheckUse;
	afx_msg void OnBnClickedButtonCreateMatchingMode();
};
