#pragma once


// CResultViewSelectDlg 대화 상자입니다.

class CResultViewSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CResultViewSelectDlg)

public:
	CResultViewSelectDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CResultViewSelectDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_RESULT_VIEW_SELECT_DLG };

	int m_iSelectedResultView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonResultBsurface();
	afx_msg void OnBnClickedButtonResultBedge();
	afx_msg void OnBnClickedButtonResultLens();
	afx_msg void OnBnClickedButtonResultLens2();
	afx_msg void OnBnClickedButtonResultExtra1();
	afx_msg void OnBnClickedButtonResultExtra2();
	afx_msg void OnBnClickedButtonResultExtra3();
	afx_msg void OnBnClickedButtonResultExtra();
};
