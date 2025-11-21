#if !defined(AFX_THRESHOLDHISTOGRAMDLG_H__D1728995_140C_4D12_95D6_598ADC3492E7__INCLUDED_)
#define AFX_THRESHOLDHISTOGRAMDLG_H__D1728995_140C_4D12_95D6_598ADC3492E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThresholdHistogramDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThresholdHistogramDlg dialog

#define CCSD_EDGE_X			(double)30.0
#define CCSD_EDGE_Y			(double)10.0
#define CCSD_DRAWWIDTH		(double)400.0
#define CCSD_DRAWHEIGHT		(double)200.0

class CThresholdHistogramDlg : public CDialog
{
// Construction
public:
	CThresholdHistogramDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CThresholdHistogramDlg)
	enum { IDD = IDD_THRESHOLD_HISTOGRAM_DIALOG };
	static CThresholdHistogramDlg	*m_pInstance;

public:
	static	CThresholdHistogramDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
public:
	int		m_nEditLower;
	int		m_nEditUpper;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThresholdHistogramDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CThresholdHistogramDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnChangeEditLower();
	afx_msg void OnChangeEditUpper();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CRect	m_BackRect;
	CRect	m_DrawRect;

	int		m_nLowerBar;			// Left Bar Position ( Height Image Level)
	int		m_nUpperBar;			// Right Bar Position ( Height Image Level)

	int		m_iHistogram[256];
	BOOL	m_bDrawBar;
	
	Hobject	m_HProcessingImage;
	
	int		m_nPeakHistoValue;
	int		m_nPeakThreshold;
	BOOL	m_bUseDarkArea;
	int		m_iSelectedLine;
	
	Hlong	m_lImageWidth;
	Hlong	m_lImageHeight;

protected:
	CButtonCS m_bnMakeRegionROI;
	CButtonCS m_bnTestRegionROI;
	
public:
	void UpdateEdit();
	void Updata_ThresholdData();
	void UpdateBar(int nLowerBar, int nUpperBar);

	void SetImage(Hobject HImage);
	void UpdateViewParam();

	void Set_LowerBar(int iBar) { m_nLowerBar = iBar; }
	void Set_UpperBar(int iBar) { m_nUpperBar = iBar; }

	void Set_UseDarkArea(BOOL bUse) { m_bUseDarkArea = bUse; }
	void RedrawRect();
	int Get_PeakToPeak();
	afx_msg void OnBnClickedButtonMakeRegionRoi();
	BOOL m_bCheckMinBlob;
	BOOL m_bCheckMaxArea;
	BOOL m_bCheckFillUp;
	BOOL m_bCheckConvex;
	BOOL m_bCheckRect;
	BOOL m_bCheckOpening;
	BOOL m_bCheckClosing;
	BOOL m_bCheckOpeningClosingInverse;
	BOOL m_bCheckDifference;
	int m_iEditMinBlobSize;
	int m_iEditOpeningSize;
	int m_iEditClosingSize;
	afx_msg void OnBnClickedButtonTestRegion();
	BOOL m_bCheckBoundary;
	int m_iEditBoundaryDilationSize;
	BOOL m_bCheckDilation;
	int m_iEditDilationSize;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THRESHOLDHISTOGRAMDLG_H__D1728995_140C_4D12_95D6_598ADC3492E7__INCLUDED_)
