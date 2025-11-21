#pragma once

#define	SLD_MAX_LOG_SIZE     1000

// CLogDlg 대화 상자입니다.

class CLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLogDlg();

	CString m_sCurrentLotID;
	CString m_sCurrentTotalModuleMix;
	CString m_sCurrentTotalBarcodeShift;
	CString m_sCurrentTotalMatchingError;
	CString m_sCurrentTotalFAISpecialNG;

	CString m_sBeforeLotID;
	CString m_sBeforeTotalModuleMix;
	CString m_sBeforeTotalBarcodeShift;
	CString m_sBeforeTotalMatchingError;
	CString m_sBeforeTotalFAISpecialNG;

	CString m_sBefore2LotID;
	CString m_sBefore2TotalModuleMix;
	CString m_sBefore2TotalBarcodeShift;
	CString m_sBefore2TotalMatchingError;
	CString m_sBefore2TotalFAISpecialNG;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOG_DLG };
		static CLogDlg	*m_pInstance;

public:
	static	CLogDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void Show();

	void ClearView();
	void WriteLog(CString sLogText, int iColor = LOG_COLOR_BLACK, int iCharHeight = 200, BOOL bChangeBGColor = FALSE, BOOL bClearView = FALSE);

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	CRichEditCtrl	m_ListLog;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
