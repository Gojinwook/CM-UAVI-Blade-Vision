#pragma once


// CAdminPasswordDlg 대화 상자입니다.

class CAdminPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CAdminPasswordDlg)

public:
	CAdminPasswordDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAdminPasswordDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ADMIN_PASSWORD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCancel();
	CString m_sUserID;
};
