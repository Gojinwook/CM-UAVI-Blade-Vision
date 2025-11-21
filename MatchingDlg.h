#pragma once

// CMatchingDlg 대화 상자입니다.

class CMatchingDlg : public CDialog
{
	DECLARE_DYNAMIC(CMatchingDlg)

public:
	CMatchingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMatchingDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MATCHING_DLG };
			static CMatchingDlg	*m_pInstance;

public:
	static CMatchingDlg* GetInstance(BOOL bShowFlag=FALSE);
	void DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	int m_iRefContRow;
	int m_iRefContCol;
	int m_iRefContRad1;
	int m_iRefContRad2;
	int m_iRefContRad3;
	int m_iRefContRad4;
	double m_dContRatio23;
	double m_dContRatio45;
	int m_iRadioBlackMatchingImageType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMakeMatchingFile();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonUniformity();
	afx_msg void OnBnClickedBnSetExtraparameter();
	afx_msg void OnBnClickedMatchBtnPresetContour();
	afx_msg void OnBnClickedMatchBtnSetRatio();
	afx_msg void OnBnClickedBnCreateDontcareRoi();
	afx_msg void OnBnClickedBnDeleteDontcareRoi();
	virtual BOOL OnInitDialog();

protected:
	CButtonCS m_bnCreateROI;
	CButtonCS m_bnDeleteROI;
	CButtonCS m_bnCreatePolygonROI;
	CButtonCS m_bnCreatePolygonEditROI;
	CButtonCS m_bnResetPolygonROI;
public:
	afx_msg void OnBnClickedRadioBlackMatchingImageBsurface();
	afx_msg void OnBnClickedRadioBlackMatchingImageLens1();
	BOOL m_bCheckUseMatchingAngle;
	int m_iEditMatchingAngleRange;
	afx_msg void OnBnClickedCheckUseMatchingAngle();
	BOOL m_bCheckUseMatchingScale;
	afx_msg void OnBnClickedCheckUseMatchingScale();
	afx_msg void OnBnClickedRadioBlackMatchingImageBedge();
	afx_msg void OnBnClickedRadioBlackMatchingImageLens2();
	afx_msg void OnBnClickedButtonSetContourContrast();
	int m_iEditMatchingScaleMax;
	int m_iEditMatchingScaleMin;
	CComboBox m_cbContourType;
	afx_msg void OnBnClickedBnCreatePolygonDontcareEditRoi();
	afx_msg void OnBnClickedBnCreatePolygonDontcareRoi();
	afx_msg void OnBnClickedBnResetPolygonDontcareRoi();
};
