#pragma once


// CTeachAlgorithmLinefitDlg 대화 상자입니다.

class CTeachAlgorithmLinefitDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeachAlgorithmLinefitDlg)

public:
	CTeachAlgorithmLinefitDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeachAlgorithmLinefitDlg();

	void SetImageType(int iImageType) { m_iSelectedImageType = iImageType; }
	void SetInspectionType(int iInspectionType) { m_iSelectedInspection = iInspectionType; }
	void SetSelectedROI(GTRegion* pSelectedROI) { m_pSelectedROI = pSelectedROI; }

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ALGORITHM_LINEFIT_DLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	int m_iEditEdgeMeasureMultiPointNumber;
	BOOL m_bCheckEdgeMeasureEndPoint;
	int m_iEditEdgeMeasureStartLength;
	int m_iEditEdgeMeasureEndLength;
	int m_iRadioEdgeMeasureDir;
	int m_iRadioEdgeMeasureGv;
	int m_iRadioEdgeMeasurePos;
	double m_dEditEdgeMeasureSmFactor;
	int m_iEditEdgeMeasureEdgeStr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	int m_iSelectedImageType;
	int m_iSelectedInspection;
	GTRegion* m_pSelectedROI;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
