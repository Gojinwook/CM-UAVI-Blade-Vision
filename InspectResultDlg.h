#pragma once


// CInspectResultDlg 대화 상자입니다.

class CInspectResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectResultDlg)

public:
	CInspectResultDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInspectResultDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSPECT_RESULT_DLG };
	static CInspectResultDlg	*m_pInstance;

public:
	static	CInspectResultDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString LastLotID;
	CString CurrentLotID;
public:
	void InitStaticText();
	CFont* GetFont(LONG lfWidth, LONG lfHeight, LONG lfWeight);
	SYSTEMTIME FindingYesterDay(SYSTEMTIME Todaytime);


public:
	virtual BOOL OnInitDialog();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnEnChangeEditLotId();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonSetResetTime();
	int m_iResetTime;
};
