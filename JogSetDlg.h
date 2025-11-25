#pragma once
#include "JogButtonStatic.h"

// CJogSetDlg 대화 상자입니다.

class CJogSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CJogSetDlg)

public:
	CJogSetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CJogSetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_JOG_SET_DLG };
		static CJogSetDlg	*m_pInstance;

public:
	static	CJogSetDlg* GetInstance(BOOL bShowFlag=FALSE);
	void	DeleteInstance();
	void Show();

	CRect			m_ScreenRect;
	CRect  GetPosition() { return m_ScreenRect; }
	void   SetPosition(int left, int top, int right, int bottom) { m_ScreenRect = CRect(left, top, right, bottom); }

	void LoadViewParam();

public:
	int m_iJogButtonStatus;
	int m_iStageNo_teaching;

	CJogButtonStatic m_pJogButtonStatic1,m_pJogButtonStatic2,m_pJogButtonStatic3,m_pJogButtonStatic4;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	BOOL m_bCheckMatchingImage[MAX_IMAGE_TAB];

	afx_msg void OnBnClickedButtonBarcodeSetting();
	afx_msg void OnBnClickedButtonSetMatchingImageNumber();
	afx_msg void OnBnClickedButtonSetFaiOption();
};
