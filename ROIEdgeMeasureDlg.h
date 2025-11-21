#pragma once


// CROIEdgeMeasureDlg 대화 상자입니다.

class CROIEdgeMeasureDlg : public CDialog
{
	DECLARE_DYNAMIC(CROIEdgeMeasureDlg)

public:
	CROIEdgeMeasureDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CROIEdgeMeasureDlg();

	void SetSelectedInspection(int iImageType, GTRegion *pSelectedROI, int iTabIndex);

	void SetParam(CAlgorithmParam AlgorithmParam);
	void GetParam(CAlgorithmParam *pAlgorithmParam);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEACH_ROI_EDGE_MEASURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	int			m_iSelectedImageType;
	int			m_iInspectionType;
	GTRegion	*m_pSelectedROI;


public:
	virtual BOOL OnInitDialog();
	BOOL m_bCheckEdgeMeasureUse;
	BOOL m_bCheckEdgeMeasureLocalAlignUse;
	int m_iEditEdgeMeasureMultiPointNumber;
	BOOL m_bCheckEdgeMeasureEndPoint;
	int m_iEditEdgeMeasureStartLength;
	int m_iEditEdgeMeasureEndLength;
	int m_iRadioEdgeMeasureDir;
	int m_iRadioEdgeMeasureGv;
	int m_iRadioEdgeMeasurePos;
	double m_dEditEdgeMeasureSmFactor;
	int m_iEditEdgeMeasureEdgeStr;
	double m_dEditEdgeMeasureRotateAngle;

	afx_msg void OnBnClickedButtonTestEdgeMeasure();
	afx_msg void OnBnClickedButtonRoiAffineTrans();
};
