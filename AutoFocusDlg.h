#pragma once

// CAutoFocusDlg 대화 상자입니다.

class CAutoFocusDlg : public CDialog
{
	DECLARE_DYNAMIC(CAutoFocusDlg)

public:
	CAutoFocusDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAutoFocusDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AUTO_FOCUS_DLG };
		static CAutoFocusDlg	*m_pInstance;

public:
	static	CAutoFocusDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void	Show();

	void UpdateViewParam();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

public:
	double m_dEditAMovePosition;
	double m_dEditJMovePosition;

	double m_dEditAFStartPosition;
	double m_dEditAFEndPosition;
	double m_dEditAFMoveInterval;

	double m_dCurZPosition;

protected:
	CButtonCS m_bnGetCurrentPos;
	CButtonCS m_bnAMove;
	CButtonCS m_bnJMovePlus;
	CButtonCS m_bnJMoveMinus;
	CButtonCS m_bnAFMakeROI;
	CButtonCS m_bnAFStart;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonGetCurrentPosition();
	afx_msg void OnBnClickedButtonAmove();
	afx_msg void OnBnClickedButtonJmovePlus();
	afx_msg void OnBnClickedButtonJmoveMinus();
	afx_msg void OnBnClickedButtonStartAf();
	afx_msg void OnBnClickedButtonRoiAf();
};
