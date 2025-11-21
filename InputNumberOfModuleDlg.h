#pragma once


// CInputNumberOfModuleDlg 대화 상자입니다.

class CInputNumberOfModuleDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputNumberOfModuleDlg)

public:
	CInputNumberOfModuleDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInputNumberOfModuleDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };
	CString m_strNumber;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.


	
	CFont* GetFont(LONG lfWidth, LONG lfHeight, LONG lfWeight);
	void UpdateColorString();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButtonAccept();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
