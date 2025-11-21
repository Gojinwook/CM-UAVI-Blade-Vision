#pragma once


// CDefectPriorityDlg 대화 상자입니다.

class CDefectPriorityDlg : public CDialog
{
	DECLARE_DYNAMIC(CDefectPriorityDlg)

public:
	CDefectPriorityDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDefectPriorityDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PRIORITY };

	int		m_iPriority[MAX_DEFECT_NUMBER];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnSelchangeComboPriority_1();
	afx_msg void OnSelchangeComboPriority_2();
	afx_msg void OnSelchangeComboPriority_3();
	afx_msg void OnSelchangeComboPriority_4();
	afx_msg void OnSelchangeComboPriority_5();
	afx_msg void OnSelchangeComboPriority_6();
	afx_msg void OnSelchangeComboPriority_7();
	afx_msg void OnSelchangeComboPriority_8();

	afx_msg void OnClickedButtonReviewColor1();
	afx_msg void OnClickedButtonReviewColor2();
	afx_msg void OnClickedButtonReviewColor3();
	afx_msg void OnClickedButtonReviewColor4();
	afx_msg void OnClickedButtonReviewColor5();
	afx_msg void OnBnClickedButtonReviewColor6();
	afx_msg void OnBnClickedButtonReviewColor7();
	afx_msg void OnBnClickedButtonReviewColor8();

	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	long	m_lNGColor[MAX_DEFECT_NUMBER];
	COLORREF pUserColor[MAX_DEFECT_NUMBER];


	void ChangePriority(int iSelectedIndex);

	void RGBToHSV( double r, double g, double b, double *h, double *s, double *v);
	void HSVToRGB( double h, double s, double v, double *r, double *g, double *b);
	unsigned char ToRGB1(double rm1, double rm2, double rh);
	void DWORDToRGB( DWORD color, DWORD *r, DWORD *g, DWORD *b);
	void RGBToDWORD( DWORD r, DWORD g, DWORD b, DWORD *color);
};
