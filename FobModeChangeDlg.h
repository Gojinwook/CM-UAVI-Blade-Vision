#pragma once


class CFobModeChangeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFobModeChangeDlg)

public:
	CFobModeChangeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFobModeChangeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FOB_MODE_CHANGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	TCHAR m_cPasswordChar;

public:
	int m_nRadioFobMode;
	afx_msg void OnBnClickedRadioFobModeOperator();
	afx_msg void OnBnClickedRadioFobModeAdmin();
	afx_msg void OnBnClickedOk();
};
