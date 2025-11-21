#pragma once


// COperator_Register 대화 상자입니다.

class COperator_RegisterDlg : public CDialog
{
	DECLARE_DYNAMIC(COperator_RegisterDlg)

public:
	COperator_RegisterDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~COperator_RegisterDlg();

	CString m_strLogin_Mode;
	CString m_strOperator;
	CString m_strPwd;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OPERATOR_REGISTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
